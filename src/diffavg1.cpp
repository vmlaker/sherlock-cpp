// Difference from running average.

#include <vector>
#include <sstream>
#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>
#include "RateTicker.hpp"
#include "util.hpp"

int main(int argc, char** argv)
{
    // Parse command-line arguments.
    int DEVICE, WIDTH, HEIGHT, DURATION;
    std::istringstream(std::string(argv[1])) >> DEVICE;
    std::istringstream(std::string(argv[2])) >> WIDTH;
    std::istringstream(std::string(argv[3])) >> HEIGHT;
    std::istringstream(std::string(argv[4])) >> DURATION;

    // Create the OpenCV video capture object.
    cv::VideoCapture cap(DEVICE);
    cap.set(3, WIDTH);
    cap.set(4, HEIGHT);

    // Create the output window.
    const char* title = "diff average 1";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    // Maintain accumulation of differences.
    cv::Mat image_acc;

    // Keep track of previous iteration's timestamp.
    boost::posix_time::ptime tstamp_prev;

    // Monitor framerates for the given seconds past.
    std::vector<float> periods = { 1, 5, 10 };
    RateTicker framerate (periods);

    // Run the loop for designated amount of time.
    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(DURATION);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Take a snapshot.
        cv::Mat frame;
        cap >> frame; 

        const auto RTYPE = CV_32FC3;

        // Compute alpha value.
        auto alpha = sherlock::getAlpha(tstamp_prev, 1.0);

        // Initalize accumulation if so indicated.
        if (image_acc.empty()){
            image_acc = cv::Mat::ones(frame.size(), frame.type());
            image_acc.convertTo(image_acc, RTYPE);
        }

        // Compute difference.
        cv::Mat converted;
        frame.convertTo(converted, RTYPE);
        cv::Mat image_diff;
        cv::absdiff(
            image_acc, 
            converted,
            image_diff
            );

        // Accumulate.
        cv::accumulateWeighted(
            converted,
            image_acc,
            alpha
            );

        image_diff.convertTo(image_diff, frame.type());

        // Write the framerate on top of the image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2];
        std::list<std::string> lines;
        lines.push_back(line.str());
        sherlock::writeOSD(image_diff, lines, 0.04);
        
        // Display the snapshot.
        cv::imshow(title, image_diff);

        // Allow HighGUI to process event.
        cv::waitKey(1);
    }
}
