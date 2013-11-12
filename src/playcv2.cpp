// Live playback with OpenCV.

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

    // Monitor framerates for the given seconds past.
    std::vector<float> periods = { 1.0, 5, 10 };
    RateTicker framerate (periods);

    // Create the OpenCV video capture object.
    cv::VideoCapture cap(DEVICE);
    cap.set(3, WIDTH);
    cap.set(4, HEIGHT);

    // Create the display window.
    const char* title = "playing OpenCV capture";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(DURATION);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Take a snapshot.
        cv::Mat frame;
        cap >> frame; 
        
        // Stamp framerate onto image.
        auto fps = framerate.tick();
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << fps[0] << ", " << fps[1] << ", " << fps[2];
        std::list<std::string> lines;
        lines.push_back(line.str());
        sherlock::writeOSD(frame, lines, 0.04);

        // Display the snapshot.
        cv::imshow(title, frame); 
        cv::waitKey(1);
    }
}
