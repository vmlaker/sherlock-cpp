// Object detection.

// Include standard headers.
#include <vector>
#include <sstream>
#include <thread>

// Include 3rd party headers.
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "ConcurrentQueue.hpp"
#include "RateTicker.hpp"
#include "Config.hpp"

// Include application headers.
#include "util.hpp"


// Capture frames for given *duration* number of seconds,
// and push frames onto queues.
void capture(
    cv::VideoCapture* cap,
    const int& duration, 
    std::vector <ConcurrentQueue <cv::Mat*>* >* detect_queues,
    ConcurrentQueue <cv::Mat*>* display_queue
    )
{
    // Monitor framerates for the given seconds past.
    RateTicker framerate ({ 1, 5, 10 });

    // Run the loop for designated amount of time.
    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(duration);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Take a snapshot.
        auto frame = new cv::Mat;
        *cap >> *frame; 

        // Stamp the capture framerate on top of the image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2] << " (capture)";
        std::list<std::string> lines ({ line.str() });
        sherlock::writeOSD(*frame, lines, 0.04);

        // Push image onto all queues.
        for(auto proc_queue : *detect_queues)
        {
            proc_queue->push(frame);
        }
        display_queue->push(frame);
    }

    // Signal end-of-processing by pushing NULL onto all queues.
    for(auto proc_queue : *detect_queues)
    {
        proc_queue->push(NULL);
    }
    display_queue->push(NULL);
}


// Collect a list of positive detection rectangles.
struct RectColor{
    cv::Rect rect;
    cv::Scalar color;
};


// Do detection.
void detect(
    ConcurrentQueue <cv::Mat*>* detect_queue,
    cv::CascadeClassifier* classifier,
    const cv::Scalar& color,
    ConcurrentQueue <RectColor>* rect_colors,
    ConcurrentQueue <cv::Mat*>* done_queue
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
            rect_colors->push({ rect, color });
        }

        // Filter out excess images in the input queue.
        // Detection framerate is more likely (than not) to be slower than
        // capture framerate, hence the "lossy filtering" here.
        // Add all dropped frames and processed frames to "done" queue.
        int count = 0;
        auto prev_frame = frame;
        while(detect_queue->try_pop(frame))
        {
            done_queue->push(prev_frame);
            prev_frame = frame;
            count++;
        }
        if(count == 0)
        {
            done_queue->push(prev_frame);
            detect_queue->wait_and_pop(frame);
        }
    }
}


// Draw rectangles on queued frames, and display.
void display(
    ConcurrentQueue <cv::Mat*>* display_queue,
    ConcurrentQueue <RectColor>* rect_colors,
    ConcurrentQueue <cv::Mat*>* done_queue
    )
{
    // Create the output window.
    const char* title = "detect 2";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    // Monitor framerates for the given seconds past.
    RateTicker framerate ({ 1, 5, 10 });

    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    display_queue->wait_and_pop(frame);
    while(frame)
    {
        // Draw the rectangles.
        RectColor rect_color;
        while(rect_colors->try_pop(rect_color))
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
        line << fps[0] << ", " << fps[1] << ", " << fps[2] << " (display)";
        std::list<std::string> lines ({ "", line.str() });
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
        while(display_queue->try_pop(frame))
        {
            done_queue->push(prev_frame);
            prev_frame = frame;
            ++count;
        }
        if(count == 0)
        {
            done_queue->push(prev_frame);
            display_queue->wait_and_pop(frame);
        }
    }
}


// Deallocate frames in given queue.
// Deallocate a frame when its count reaches trigger threshold.
void deallocate(
    ConcurrentQueue <cv::Mat*>* done_queue,
    const int& deallocate_trigger_count
    )
{
    // Count the number of times each frame is encountered
    // in the "done" queue (to know when the count triggers
    // deallocation.)
    std::map <cv::Mat*, int> done_counts;
    
    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    done_queue->wait_and_pop(frame);
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
        done_queue->wait_and_pop(frame);
    }
}


int main(int argc, char** argv)
{
    // Parse command-line arguments.
    int DEVICE, WIDTH, HEIGHT, DURATION;
    std::istringstream(std::string(argv[1])) >> DEVICE;
    std::istringstream(std::string(argv[2])) >> WIDTH;
    std::istringstream(std::string(argv[3])) >> HEIGHT;
    std::istringstream(std::string(argv[4])) >> DURATION;

    // Initialize the classifiers and their respective colors.
    Config config ("conf/classifiers.conf");
    std::vector< cv::CascadeClassifier* > classifiers;
    std::vector< cv::Scalar > colors;
    for(auto fname : config.keys())
    {
        boost::filesystem::path file(fname + ".xml");
        for(auto dname : { "/usr/share/OpenCV/haarcascades", 
                    "/usr/share/OpenCV/lbpcascades" }
            )
        {
            boost::filesystem::path dir(dname);
            boost::filesystem::path full = dir / file;
            auto cfer = new cv::CascadeClassifier(full.string());
            if( cfer->empty() ) continue;
            classifiers.push_back(cfer);
            double rr, gg, bb;
            std::stringstream(config[fname]) >> rr >> gg >> bb;
            colors.push_back({ rr, gg, bb });
        }
    }

    // Create the shared queues.
    std::vector< ConcurrentQueue <cv::Mat*>* > detect_queues;
    ConcurrentQueue <cv::Mat*> display_queue;
    ConcurrentQueue <cv::Mat*> done_queue;
    ConcurrentQueue <RectColor> rect_colors;
    
    // Start up the detector threads.
    std::list <std::thread*> detectors;
    for(int ii=0; ii!= classifiers.size(); ++ii)
    {
        auto frames = new ConcurrentQueue<cv::Mat*>;
        detect_queues.push_back(frames);
        cv::CascadeClassifier* cfer = classifiers[ii];
        cv::Scalar color = colors[ii];
        auto tt = new std::thread(
            detect,
            frames,
            cfer,
            color,
            &rect_colors,
            &done_queue
            );
        detectors.push_back(tt);
    }

    // Create the OpenCV video capture object.
    cv::VideoCapture cap(DEVICE);
    cap.set(3, WIDTH);
    cap.set(4, HEIGHT);

    // Start up capture and display threads.
    std::thread capturer (capture, &cap, DURATION, &detect_queues, &display_queue);
    std::thread displayer (display, &display_queue, &rect_colors, &done_queue);

    // Start up the deallocator thread.
    auto deallocate_trigger_count = detectors.size() + 1;
    std::thread deallocator (
        deallocate, &done_queue, deallocate_trigger_count);
    
    // Join all threads.
    capturer.join();
    displayer.join();
    for(auto thread : detectors)
    {
        thread->join();
        delete thread;
    }

    // Signal deallocator thread to stop, and join it.
    done_queue.push(NULL);
    deallocator.join();
}
