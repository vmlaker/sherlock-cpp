#ifndef __DISPLAYER_HPP__
#define __DISPLAYER_HPP__

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include <bites.hpp>

// Include application headers.
#include "Classifier.hpp"
#include "Captor.hpp"

namespace sherlock {

/**
   Display thread.
*/
class Displayer : public bites::Thread 
{
public:
    Displayer(
        bites::ConcurrentQueue <cv::Mat*>& display_queue,
        bites::ConcurrentQueue <cv::Mat*>& done_queue,
        bites::ConcurrentQueue <Classifier::RectColor>& rect_colors,
        sherlock::Captor& captor
        ):
        m_display_queue (display_queue),
        m_done_queue    (done_queue),
        m_rect_colors   (rect_colors),
        m_captor        (captor)
        {/* Empty. */}
private:
    bites::ConcurrentQueue <cv::Mat*>& m_display_queue;
    bites::ConcurrentQueue <cv::Mat*>& m_done_queue;
    bites::ConcurrentQueue <Classifier::RectColor>& m_rect_colors;
    sherlock::Captor& m_captor;
    void run();
};

}  // namespace sherlock.

#endif  // __DISPLAYER_HPP__
