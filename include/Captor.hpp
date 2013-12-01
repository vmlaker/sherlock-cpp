#ifndef __CAPTOR_HPP__
#define __CAPTOR_HPP__

// Include standard headers.
#include <vector>
#include <thread>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include "ConcurrentQueue.hpp"

namespace sherlock {

class Captor {

public:
    Captor(
        const int& device, 
        const int& width,
        const int& height,
        const int& duration,
        std::vector< ConcurrentQueue <cv::Mat*>* >& detect_queues,
        ConcurrentQueue <cv::Mat*>& display_queue);
    ~Captor();
    void start();

private:
    int m_device;
    int m_width;
    int m_height;
    int m_duration;
    std::vector< ConcurrentQueue <cv::Mat*>* >& m_detect_queues;
    ConcurrentQueue <cv::Mat*>& m_display_queue;
    std::thread* m_thread = NULL;

    void run();
};

}  // namespace sherlock.

#endif  // __CAPTOR_HPP__
