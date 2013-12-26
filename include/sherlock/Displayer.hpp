#ifndef SHERLOCK_DISPLAYER_HPP_INCLUDED
#define SHERLOCK_DISPLAYER_HPP_INCLUDED

// Include standard headers.
#include <functional>

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
    /**
       Initialize the display object.
       
       @param  display_queue    Input queue.
       @param  done_queue       Output queue for processed (or skipped) frames.
       @param  rect_colors      Input queue of detection perimeters.
       @param  get_capture_fps  Callback to retrieve capture framerate.
    */
    Displayer(
        bites::ConcurrentQueue <cv::Mat*>& display_queue,
        bites::ConcurrentQueue <cv::Mat*>& done_queue,
        bites::ConcurrentQueue <Classifier::RectColor>& rect_colors,
        std::function <std::vector <float> (void)> get_capture_fps
        ):
        m_display_queue   (display_queue),
        m_done_queue      (done_queue),
        m_rect_colors     (rect_colors),
        m_get_capture_fps (get_capture_fps)
        {/* Empty. */}
private:
    bites::ConcurrentQueue <cv::Mat*>& m_display_queue;
    bites::ConcurrentQueue <cv::Mat*>& m_done_queue;
    bites::ConcurrentQueue <Classifier::RectColor>& m_rect_colors;
    std::function <std::vector <float> (void)> m_get_capture_fps;
    void run();
};

}  // namespace sherlock.

#endif  // SHERLOCK_DISPLAYER_HPP_INCLUDED
