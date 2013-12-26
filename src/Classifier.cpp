// Include application headers.
#include "sherlock.hpp"

namespace sherlock {

void Classifier::run ()
{
    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    m_input_queue.wait_and_pop(frame);
    while(frame)
    {
        std::vector<cv::Rect> rects;
        cv::Size min_size (
            frame->size().width*m_min_size_ratio,
            frame->size().height*m_min_size_ratio);
        cv::Size max_size (
            frame->size().width*m_max_size_ratio,
            frame->size().height*m_max_size_ratio);
        m_cv_classifier.detectMultiScale(
            *frame,
            rects,
            m_scale_factor,
            m_min_neighbors,
            0,    // flags.
            min_size,
            max_size
            );

        // Add rectangles to the data queue.
        for(auto rect : rects) 
        {
            m_output_queue.push({ rect, m_color });
        }

        // Filter out excess images in the input queue.
        // Detection framerate is more likely (than not) to be slower than
        // capture framerate, hence the "lossy filtering" here.
        // Add all dropped frames and processed frames to "done" queue.
        int count = 0;
        auto prev_frame = frame;
        while(m_input_queue.try_pop(frame))
        {
            m_done_queue.push(prev_frame);
            prev_frame = frame;
            count++;
        }
        if(count == 0)
        {
            m_done_queue.push(prev_frame);
            m_input_queue.wait_and_pop(frame);
        }
    }
}

}  // namespace sherlock.
