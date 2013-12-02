/*!
  The Detector class encapsulates object detector functionality.
 */

// Include 3rd party headers.
#include <boost/filesystem.hpp>
#include "Config.hpp"

// Include application headers.
#include "Detector.hpp"
#include "util.hpp"

namespace sherlock {

Detector::Detector(
    const int& device,
    const int& width,
    const int& height,
    const int& duration
    ) :
    m_captor(device, width, height, duration, m_detect_queues, m_display_queue),
    m_displayer(m_display_queue, m_done_queue, m_rect_colors),
    m_deallocator(m_done_queue)
{
    Config config ("conf/classifiers.conf");
    for(auto fname : config.keys())
    {
        if(fname == "DIRS"){
            continue;
        }
        boost::filesystem::path file(fname + ".xml");
        std::string dir_name;
        std::stringstream dirs(config["DIRS"]);
        while(dirs >> dir_name){
            boost::filesystem::path dir (dir_name);
            boost::filesystem::path full = dir / file;
            auto cfer = new cv::CascadeClassifier(full.string());
            if(cfer->empty()){
                continue;
            }
            m_classifiers.push_back(cfer);
            int rr, gg, bb;
            std::stringstream(config[fname]) >> rr >> gg >> bb;
            m_colors.push_back(cv::Scalar(rr, gg, bb));
        }
    }
}


// Do detection.
void Detector::detect(
    ConcurrentQueue<cv::Mat*>* detect_queue,
    cv::CascadeClassifier* classifier,
    const cv::Scalar& color
    )
{
    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    detect_queue->wait_and_pop(frame);
    while(frame)
    {
        std::vector<cv::Rect> rects;
        classifier->detectMultiScale(
            *frame,
            rects,
            1.3,  // scale factor.
            3,    // min neighbors.
            0,    // flags.
            {frame->size().width/20, frame->size().height/20},  // min size.
            {frame->size().width/2, frame->size().height/2}     // max size.
            );

        // Add rectangles to the data queue.
        for(auto rect : rects) 
        {
            m_rect_colors.push({ rect, color });
        }

        // Filter out excess images in the input queue.
        // Detection framerate is more likely (than not) to be slower than
        // capture framerate, hence the "lossy filtering" here.
        // Add all dropped frames and processed frames to "done" queue.
        int count = 0;
        auto prev_frame = frame;
        while(detect_queue->try_pop(frame))
        {
            m_done_queue.push(prev_frame);
            prev_frame = frame;
            count++;
        }
        if(count == 0)
        {
            m_done_queue.push(prev_frame);
            detect_queue->wait_and_pop(frame);
        }
    }
}


void Detector::run()
{
    // Start up the detector threads.
    for(int ii=0; ii!= m_classifiers.size(); ++ii)
    {
        auto frames = new ConcurrentQueue<cv::Mat*>;
        m_detect_queues.push_back(frames);
        cv::CascadeClassifier* cfer = m_classifiers[ii];
        cv::Scalar color = m_colors[ii];
        auto tt = new std::thread(
            &Detector::detect,
            this,
            frames,
            cfer,
            color);
        m_detectors.push_back(tt);
    }

    // Start up capture and display threads.
    m_captor.start();
    m_displayer.start();

    // Start up the deallocator thread.
    m_deallocator.setTrigger( m_detectors.size() + 1 );
    m_deallocator.start();
}


Detector::~Detector()
{
    // Join all threads.
    for(auto thread : m_detectors)
    {
        thread->join();
        delete thread;
    }

    // Signal deallocator thread to stop, and join it.
    m_done_queue.push(NULL);
}

}  // namespace sherlock.
