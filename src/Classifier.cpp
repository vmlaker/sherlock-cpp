// Include application headers.
#include "Classifier.hpp"
#include "util.hpp"

namespace sherlock {

void Classifier::run ()
{
    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    m_input_queue.wait_and_pop(frame);
    while(frame)
    {
        std::vector<cv::Rect> rects;
        m_cv_classifier.detectMultiScale(
            *frame,
            rects,
            1.3,  // scale factor.
            3,    // min neighbors.
            0,    // flags.
            {frame->size().width/20, frame->size().height/20},  // min size.
            {frame->size().width/2, frame->size().height/2}     // max size.
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
