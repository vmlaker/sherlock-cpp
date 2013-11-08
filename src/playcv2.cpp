// Live playback with OpenCV.

#include <sstream>
#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

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

    // Create the display window.
    const char* title = "playing OpenCV capture";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(DURATION);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Take a snapshot and display it.
        cv::Mat frame;
        cap >> frame; 
        cv::imshow(title, frame); 
        cv::waitKey(1);
    }
}
