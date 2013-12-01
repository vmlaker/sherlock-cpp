#ifndef __DETECTOR_HPP__
#define __DETECTOR_HPP__

// Include standard headers.
#include <vector>
#include <thread>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include "ConcurrentQueue.hpp"

// Include application headers.
#include "Captor.hpp"

namespace sherlock {

class Detector {

public:
    Detector(
        const int& device, 
        const int& width, 
        const int& height, 
        const int& duration);
    ~Detector();
    void run();

private:
    void capture();
    void detect(
        ConcurrentQueue<cv::Mat*>* detect_queue,
        cv::CascadeClassifier* classifier,
        const cv::Scalar& color
        );
    void display();
    void deallocate(const int& deallocate_trigger_count);

    // Video capture object.
    sherlock::Captor m_captor;

    // Classifiers and their respective colors.
    std::vector <cv::CascadeClassifier*> m_classifiers;

    std::vector <cv::Scalar> m_colors;


    // Collect a list of positive detection rectangles.
    struct RectColor{
        cv::Rect rect;
        cv::Scalar color;
    };

    // Shared queues.
    std::vector< ConcurrentQueue <cv::Mat*>* > m_detect_queues;
    ConcurrentQueue <cv::Mat*> m_display_queue;
    ConcurrentQueue <cv::Mat*> m_done_queue;
    ConcurrentQueue <RectColor> m_rect_colors;

    // Detector threads.
    std::list <std::thread*> m_detectors;
    std::thread* m_displayer;
    std::thread* m_deallocator;
};

}  // namespace sherlock.

#endif  //__DETECTOR_HPP__
