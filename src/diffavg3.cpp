// Difference from running average, with multiprocessing.

#include <vector>
#include <sstream>
#include <thread>
#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>
#include "RateTicker.hpp"
#include "ConcurrentQueue.hpp"
#include "util.hpp"

// Capture frames for given *duration* number of seconds,
// and push frames and alpha values onto their respective queues.
void step1(
    cv::VideoCapture* cap,
    const int& duration, 
    ConcurrentQueue< cv::Mat* >* frames,
    ConcurrentQueue< float >* alphas
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
        // Take a snapshot.
        auto frame = new cv::Mat;
        *cap >> *frame; 

        // Compute alpha value.
        auto alpha = sherlock::getAlpha(tstamp_prev, 1.0);

        // Push the frame and alpha value onto respective queues.
        frames->push(frame);
        alphas->push(alpha);
    }

    // Signal end-of-processing by pushing NULL value onto the queue.
    frames->push(NULL);
}


// Compute Difference from Average of the frames,
// taking into account previously computed alpha values,
// and push resulting diff frames onto the queue.
void step2(
    ConcurrentQueue< cv::Mat* >* frames,
    ConcurrentQueue< cv::Mat* >* diffs,
    ConcurrentQueue< float >* alphas
    )
{
    // Monitor framerates for the given seconds past.
    std::vector<float> periods = { 1, 5, 10 };
    RateTicker framerate (periods);

    // Maintain accumulation of differences.
    cv::Mat image_acc;

    // Pull from the queue while there are valid frames.
    cv::Mat* frame;
    frames->wait_and_pop(frame);
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

        // Write the processing framerate on top of the diff image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2] << " (processing)";
        std::list<std::string> lines;
        lines.push_back(line.str());
        sherlock::writeOSD(*diff, lines, 0.04);

        // Push diff image onto queue.
        diffs->push(diff);

        // Original frame is no longer needed, so deallocate it.
        delete frame;

        // Retrieve the next frame from queue.
        frames->wait_and_pop(frame);
    }

    // Signal end-of-processing by pushing NULL value onto the queue.
    diffs->push(NULL);
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

    // Create the output window.
    const char* title = "diff average 2";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    // Monitor framerates for the given seconds past.
    std::vector<float> periods = { 1, 5, 10 };
    RateTicker framerate (periods);

    // Create the shared queues.
    ConcurrentQueue <cv::Mat*> frames;
    ConcurrentQueue <float> alphas;
    ConcurrentQueue <cv::Mat*> diffs;

    // Start up the threads.
    std::thread thread1 (step1, &cap, DURATION, &frames, &alphas);
    std::thread thread2 (step2, &frames, &diffs, &alphas);

    // Pull from the queue while there are valid matrices.
    cv::Mat* diff;
    diffs.wait_and_pop(diff);
    while(diff){

        // Write the display framerate on top of the image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2] << " (display)";
        std::list<std::string> lines;
        //
        // Show the display framerate *below* the processing framerate
        // by first adding a "carriage return" (empty line) to the list.
        lines.push_back("");
        //
        lines.push_back(line.str());
        sherlock::writeOSD(*diff, lines, 0.04);

        // Display the snapshot.
        cv::imshow(title, *diff);

        // Allow HighGUI to process event.
        cv::waitKey(1);
        
        // Deallocate the current image.
        delete diff;

        // Filter out excess images in the queue.
        // If display hardware is not fast enough, showing these images 
        // would introduce (incremental) lag, therefore we add
        // lossy filtering here by dropping any excess frames.
        while(diffs.size()>1){
            diffs.wait_and_pop(diff);
            delete diff;
        }

        // Finally, pop off the next image.
        diffs.wait_and_pop(diff);
    }

    // Join the two forked threads.
    thread1.join();
    thread2.join();
}
