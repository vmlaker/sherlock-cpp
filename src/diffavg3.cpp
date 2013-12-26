// Difference from running average, with multiprocessing.

// Include standard headers.
#include <vector>
#include <sstream>

// Include 3rd party headers.
#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>
#include <bites.hpp>

// Include application headers.
#include "sherlock.hpp"


// Capture frames for given *duration* number of seconds,
// and push frames and alpha values onto their respective queues.
void capture(
    cv::VideoCapture* cap,
    const int& duration, 
    bites::ConcurrentQueue <cv::Mat*>* captures,
    bites::ConcurrentQueue <float>* alphas
    )
{
    // Keep track of previous iteration's timestamp.
    boost::posix_time::ptime tstamp_prev;

    // Run the loop for designated amount of time.
    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(duration);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Capture the snapshot.
        auto frame = new cv::Mat;
        *cap >> *frame; 

        // Compute alpha value.
        auto alpha = sherlock::getAlpha(tstamp_prev, 1.0);

        // Push the frame and alpha value onto respective queues.
        captures->push(frame);
        alphas->push(alpha);
    }

    // Signal end-of-processing by pushing NULL value onto the queue.
    captures->push(NULL);
}


// Compute Difference from Average of the frames,
// taking into account previously computed alpha values,
// and push resulting diff frames onto the queue.
void diff_average(
    bites::ConcurrentQueue <cv::Mat*>* captures,
    bites::ConcurrentQueue <cv::Mat*>* diffs,
    bites::ConcurrentQueue <float>* alphas
    )
{
    // Monitor framerates for the given seconds past.
    std::vector<float> periods = { 1, 5, 10 };
    bites::RateTicker framerate (periods);

    // Maintain accumulation of differences.
    cv::Mat image_acc;

    // Pull from the queue while there are valid frames.
    cv::Mat* frame;
    captures->wait_and_pop(frame);
    while(frame){

        const auto RTYPE = CV_32FC3;

        // Initalize accumulation if so indicated.
        if (image_acc.empty()){
            image_acc = cv::Mat::zeros(frame->size(), frame->type());
            image_acc.convertTo(image_acc, RTYPE);
        }

        // Compute difference.
        cv::Mat converted;
        frame->convertTo(converted, RTYPE);
        auto diff = new cv::Mat;
        cv::absdiff(
            image_acc, 
            converted,
            *diff
            );

        // Retrieve the alpha value.
        float alpha;
        alphas->wait_and_pop(alpha);

        // Accumulate the difference.
        cv::accumulateWeighted(
            converted,
            image_acc,
            alpha
            );

        // Convert the diff image.
        diff->convertTo(*diff, frame->type());

        // Original frame is no longer needed, so deallocate it.
        delete frame;

        // Write the processing framerate on top of the diff image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2] << " (processing)";
        std::list<std::string> lines ({ line.str() });
        sherlock::writeOSD(*diff, lines, 0.04);

        // Push diff image onto queue.
        diffs->push(diff);

        // Retrieve the next frame from queue.
        captures->wait_and_pop(frame);
    }

    // Signal end-of-processing by pushing NULL value onto the queue.
    diffs->push(NULL);
}


// Display queued frames.
void display(
    bites::ConcurrentQueue <cv::Mat*>* diffs
    )
{
    // Create the output window.
    const char* title = "diff average 2";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    // Monitor framerates for the given seconds past.
    std::vector<float> periods = { 1, 5, 10 };
    bites::RateTicker framerate (periods);

    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    diffs->wait_and_pop(frame);
    while(frame){

        // Write the display framerate on top of the image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2] << " (display)";
        // Show the display framerate *below* the processing framerate
        // by first adding a "carriage return" (empty line) to the list.
        std::list<std::string> lines({ "", line.str() });
        sherlock::writeOSD(*frame, lines, 0.04);

        // Display the snapshot.
        cv::imshow(title, *frame);

        // Allow HighGUI to process event.
        cv::waitKey(1);

        // Deallocate the current image and retrieve the next,
        // while filtering out excess (intermediate) images in the queue.
        // If display hardware is not fast enough, 
        // showing intermediate images introduces (incremental) lag.
        // Hence the "lossy filter" by dropping excess frames.
        cv::Mat* prev = frame;
        int count = 0;
        while(diffs->try_pop(frame))
        {
            // Since we popped the next image, we can delete the previous.
            delete prev; 
            prev = frame; 
            count++;
        }
        // If there weren't any successful try-pops above,
        // then do an absolute wait-and-pop here.
        if(count == 0)
        {
            diffs->wait_and_pop(frame);
            delete prev;
        }
    }
}


// Launch Capture and Diff threads, and display results to output window.
int main(int argc, char** argv)
{
    // Parse command-line arguments.
    int DEVICE, WIDTH, HEIGHT;
    float DURATION;
    std::istringstream(std::string(argv[1])) >> DEVICE;
    std::istringstream(std::string(argv[2])) >> WIDTH;
    std::istringstream(std::string(argv[3])) >> HEIGHT;
    std::istringstream(std::string(argv[4])) >> DURATION;

    // Create the OpenCV video capture object.
    cv::VideoCapture cap(DEVICE);
    cap.set(3, WIDTH);
    cap.set(4, HEIGHT);

    // Create the shared queues.
    bites::ConcurrentQueue <cv::Mat*> captures;
    bites::ConcurrentQueue <float> alphas;
    bites::ConcurrentQueue <cv::Mat*> diffs;

    // Start up the threads.
    std::thread capturer (capture, &cap, DURATION, &captures, &alphas);
    std::thread diff_averager (diff_average, &captures, &diffs, &alphas);
    std::thread displayer (display, &diffs);

    // Join the threads.
    capturer.join();
    diff_averager.join();
    displayer.join();
}
