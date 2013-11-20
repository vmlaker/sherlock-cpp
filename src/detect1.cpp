// Difference from running average.

#include <vector>
#include <sstream>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "RateTicker.hpp"
#include "util.hpp"
#include "cascade.hpp"

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
    const char* title = "detect 1";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    std::vector< cv::CascadeClassifier* > classifiers;
    std::vector< cv::Scalar > colors;
    for(auto ii=sherlock::classify::specs.begin(); ii!=sherlock::classify::specs.end(); ++ii)
    {
        boost::filesystem::path file(ii->first);
        boost::filesystem::path dir(ii->second.dir);
        boost::filesystem::path full = dir / file;
        auto cfer = new cv::CascadeClassifier(full.string());
        classifiers.push_back(cfer);
        auto color = cv::Scalar(
            ii->second.color[0],
            ii->second.color[1],
            ii->second.color[2]
            );
        colors.push_back(color);
    } 

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

        struct RectColor{
            cv::Rect rect;
            cv::Scalar color;
        };

        std::vector<RectColor> results;
        for(auto ii=0; ii!= classifiers.size(); ++ii){
            auto classi = classifiers[ii];
            std::vector<cv::Rect> rects;
            classi->detectMultiScale(
                frame,
                rects,
                1.3,  // scale factor.
                3,    // min neighbors.
                0,    // flags.
                cv::Size(frame.size().width/20, frame.size().height/20),     // min size.
                cv::Size(frame.size().width/2, frame.size().height/2)     // max size.
                );
            for(auto jj=rects.begin(); jj!= rects.end(); ++jj){
                auto rect = *jj;
                auto color = colors[ii];
                RectColor rrcc = { rect, color };
                results.push_back(rrcc);
            }
        }
        for(auto ii=results.begin(); ii!=results.end(); ++ii){
            auto rrcc = *ii;
            auto rect = rrcc.rect;
            auto color = rrcc.color;
            cv::rectangle(
                frame,
                cv::Point(rect.x, rect.y),
                cv::Point(rect.x + rect.width, 
                          rect.y + rect.height),
                color,
                2  // thickness.                
                );
        }

        // Display the snapshot.
        cv::imshow(title, frame); 
        cv::waitKey(1);
    }
}
