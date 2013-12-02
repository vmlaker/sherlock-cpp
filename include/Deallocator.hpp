#ifndef __DEALLOCATOR_HPP__
#define __DEALLOCATOR_HPP__

// Include standard headers.
#include <thread>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include "ConcurrentQueue.hpp"

namespace sherlock {

class Deallocator {

public:
    Deallocator( ConcurrentQueue <cv::Mat*>& done_queue );
    ~Deallocator();
    void setTrigger(const int& value) { m_trigger = value; }
    void start();

private:
    ConcurrentQueue <cv::Mat*>& m_done_queue;
    std::thread* m_thread = NULL;
    int m_trigger = -1;
    void run();
};

}  // namespace sherlock.

#endif  // __DEALLOCATOR_HPP__
