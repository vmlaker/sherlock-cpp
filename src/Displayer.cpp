// Include 3rd party headers.
#include <bites.hpp>

// Include application headers.
#include "sherlock.hpp"

namespace sherlock {

// Draw rectangles on queued frames, and display.
void Displayer::run ()
{
    // Create the output window.
    const char* title = "Sherlock";
    cv::namedWindow(title, CV_WINDOW_NORMAL);

    // Monitor framerates for the given seconds past.
    bites::RateTicker ticker ({ 1, 5, 10 });

    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    m_display_queue.wait_and_pop(frame);
    while(frame)
    {
        // Draw the rectangles.
        Classifier::RectColor rect_color;
        while(m_rect_colors.try_pop(rect_color))
        {
            auto rect = rect_color.rect;
            auto color = rect_color.color;
            cv::rectangle(
                *frame,
                cv::Point(rect.x, rect.y),
                cv::Point(rect.x + rect.width, rect.y + rect.height),
                color,
                2  // thickness.                
                );
        }

        // Write the on-screen-display information.
        std::ostringstream line1, line2, line3;
        line1 << frame->cols << "x" << frame->rows;
        line2 << std::fixed << std::setprecision(2);
        auto fps = m_get_capture_fps();
        line2 << fps[0] << ", " << fps[1] << ", " << fps[2] << " (FPS capture)";
        fps = ticker.tick();
        line3 << std::fixed << std::setprecision(2);
        line3 << fps[0] << ", " << fps[1] << ", " << fps[2] << " (FPS display)";
        std::list<std::string> lines ({ line1.str(), line2.str(), line3.str() });
        sherlock::writeOSD(*frame, lines, 0.04);

        // Display the snapshot.
        cv::imshow(title, *frame); 
        cv::waitKey(1);
        
        // Filter out excess images in the queue.
        // If display hardware is not fast enough, showing these 
        // images introduces (incremental) lag, hence the "lossy filter."
        // Add all dropped and processed frames to "done" queue.
        int count = 0;
        auto prev_frame = frame;
        while(m_display_queue.try_pop(frame))
        {
            m_done_queue.push(prev_frame);
            prev_frame = frame;
            ++count;
        }
        if(count == 0)
        {
            m_done_queue.push(prev_frame);
            m_display_queue.wait_and_pop(frame);
        }
    }
}

}  // namespace sherlock.
