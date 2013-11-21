// Difference from running average.

#include <vector>
#include <sstream>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "RateTicker.hpp"
#include "Config.hpp"
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
    const char* title = "detect 1";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

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
            int rr, gg, bb;
            std::stringstream(config[fname]) >> rr >> gg >> bb;
            colors.push_back(cv::Scalar(rr, gg, bb));
        }
    }

    // Monitor framerates for the given seconds past.
    RateTicker framerate ({ 1, 5, 10 });

    // Run the loop for designated amount of time.
    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(DURATION);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Take a snapshot.
        cv::Mat frame;
        cap >> frame; 

        // Collect a list of positive detection rectangles.
        struct RectColor{
            cv::Rect rect;
            cv::Scalar color;
        };
        std::vector<RectColor> results;
        for(auto ii=0; ii!= classifiers.size(); ++ii){
            std::vector<cv::Rect> rects;
            classifiers[ii]->detectMultiScale(
                frame,
                rects,
                1.3,  // scale factor.
                3,    // min neighbors.
                0,    // flags.
                {frame.size().width/20, frame.size().height/20},  // min size.
                {frame.size().width/2, frame.size().height/2}     // max size.
                );
            for(auto jj=rects.begin(); jj!= rects.end(); ++jj){
                results.push_back({ *jj, colors[ii] });
            }
        }

        // Draw the rectangles.
        for(auto ii=results.begin(); ii!=results.end(); ++ii){
            auto rect = ii->rect;
            auto color = ii->color;
            cv::rectangle(
                frame,
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
        line << fps[0] << ", " << fps[1] << ", " << fps[2];
        std::list<std::string> lines;
        lines.push_back(line.str());
        sherlock::writeOSD(frame, lines, 0.04);

        // Display the snapshot.
        cv::imshow(title, frame); 
        cv::waitKey(1);
    }

}
