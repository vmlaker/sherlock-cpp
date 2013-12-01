/*!
  The Detector class encapsulates object detector functionality.
 */

// Include 3rd party headers.
#include <boost/filesystem.hpp>
#include "RateTicker.hpp"
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
    m_captor(device, width, height, duration, m_detect_queues, m_display_queue)
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
    std::cout << "constructor end" << std::endl;
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


// Draw rectangles on queued frames, and display.
void Detector::display()
{
    // Create the output window.
    const char* title = "detect 2";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    // Monitor framerates for the given seconds past.
    RateTicker framerate ({ 1, 5, 10 });

    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    m_display_queue.wait_and_pop(frame);
    while(frame)
    {
        // Draw the rectangles.
        RectColor rect_color;
        while(m_rect_colors.try_pop(rect_color))
        {
            auto rect = rect_color.rect;
            auto color = rect_color.color;
            cv::rectangle(
                *frame,
                cv::Point(rect.x, rect.y),
                cv::Point(rect.x + rect.width, rect.y + rect.height),
                color,
                2  // thickness.                
                );
        }

        // Write the display framerate on top of the image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2] << " (FPS display)";
        std::list<std::string> lines ({ "", "", line.str() });
        sherlock::writeOSD(*frame, lines, 0.04);

        // Display the snapshot.
        cv::imshow(title, *frame); 
        cv::waitKey(1);
        
        // Filter out excess images in the queue.
        // If display hardware is not fast enough, showing these 
        // images introduces (incremental) lag, hence the "lossy filter."
        // Add all dropped and processed frames to "done" queue.
        int count = 0;
        auto prev_frame = frame;
        while(m_display_queue.try_pop(frame))
        {
            m_done_queue.push(prev_frame);
            prev_frame = frame;
            ++count;
        }
        if(count == 0)
        {
            m_done_queue.push(prev_frame);
            m_display_queue.wait_and_pop(frame);
        }
    }
}


// Deallocate frames in given queue.
// Deallocate a frame when its count reaches trigger threshold.
void Detector::deallocate(
    const int& deallocate_trigger_count
    )
{
    // Count the number of times each frame is encountered
    // in the "done" queue (to know when the count triggers
    // deallocation.)
    std::map <cv::Mat*, int> done_counts;
    
    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    m_done_queue.wait_and_pop(frame);
    while(frame)
    {
        // If this is the first time frame is encountered,
        // initialize it's count to 1 (one.)
        if(done_counts.find(frame) == done_counts.end())
        {
            done_counts.insert({ frame, 1 });
        }
        // Otherwise, increment the frame's existing count,
        // and perform deallocation, if triggered.
        else
        {
            done_counts[frame]++;
            if(done_counts[frame] == deallocate_trigger_count)
            {
                delete frame;
                done_counts.erase(frame);
            }
        }
        
        // Retrieve the next frame.
        m_done_queue.wait_and_pop(frame);
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
    m_displayer = new std::thread(&Detector::display, this);

    // Start up the deallocator thread.
    auto deallocate_trigger_count = m_detectors.size() + 1;
    m_deallocator = new std::thread(
        &Detector::deallocate, this, deallocate_trigger_count);
}


Detector::~Detector()
{
    // Join all threads.
    m_displayer->join();
    delete m_displayer;
    for(auto thread : m_detectors)
    {
        thread->join();
        delete thread;
    }

    // Signal deallocator thread to stop, and join it.
    m_done_queue.push(NULL);
    m_deallocator->join();
    delete m_deallocator;
}

}  // namespace sherlock.
