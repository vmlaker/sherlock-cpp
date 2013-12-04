#ifndef __CAPTOR_HPP__
#define __CAPTOR_HPP__

// Include standard headers.
#include <vector>
#include <thread>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include <bites.hpp>

// Include application headers.
#include "Worker.hpp"

namespace sherlock {

class Captor : public Worker {

public:
    Captor(
        const int& device, 
        const int& width,
        const int& height,
        const int& duration,
        std::vector< bites::ConcurrentQueue <cv::Mat*>* >& classifier_queues,
        bites::ConcurrentQueue <cv::Mat*>& display_queue
        ):
        m_device        (device),
        m_width         (width),
        m_height        (height),
        m_duration      (duration),
        m_classifier_queues (classifier_queues),
        m_display_queue (display_queue)
        { /* Empty. */ }

private:
    int m_device;
    int m_width;
    int m_height;
    int m_duration;
    std::vector< bites::ConcurrentQueue <cv::Mat*>* >& m_classifier_queues;
    bites::ConcurrentQueue <cv::Mat*>& m_display_queue;
    void run();
};

}  // namespace sherlock.

#endif  // __CAPTOR_HPP__
