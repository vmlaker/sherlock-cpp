#ifndef __DISPLAYER_HPP__
#define __DISPLAYER_HPP__

// Include standard headers.
#include <thread>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include "ConcurrentQueue.hpp"

// Include application headers.
#include "Worker.hpp"
#include "Sherlock.hpp"

namespace sherlock {

class Displayer : public Worker {

public:
    Displayer(
        ConcurrentQueue <cv::Mat*>& display_queue,
        ConcurrentQueue <cv::Mat*>& done_queue,
        ConcurrentQueue <RectColor>& rect_colors
        ):
        m_display_queue (display_queue),
        m_done_queue (done_queue),
        m_rect_colors (rect_colors)
        { /* Empty. */ }

private:
    ConcurrentQueue <cv::Mat*>& m_display_queue;
    ConcurrentQueue <cv::Mat*>& m_done_queue;
    ConcurrentQueue <RectColor>& m_rect_colors;
    void run();
};

}  // namespace sherlock.

#endif  // __DISPLAYER_HPP__
