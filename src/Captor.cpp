// Include 3rd party headers.
#include <boost/date_time.hpp>
#include "RateTicker.hpp"

// Include application headers.
#include "Captor.hpp"
#include "util.hpp"

namespace sherlock {

void Captor::run ()
{
    // Create the OpenCV video capture object.
    cv::VideoCapture cap(m_device);
    cap.set(3, m_width);
    cap.set(4, m_height);

    // Monitor framerates for the given seconds past.
    RateTicker framerate ({ 1, 5, 10 });

    // Run the loop for designated amount of time.
    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(m_duration);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Take a snapshot.
        auto frame = new cv::Mat;
        cap >> *frame; 

        // Stamp the capture framerate on top of the image.
        auto fps = framerate.tick();
        std::ostringstream line1, line2;
        line1 << m_width << "x" << m_height;
        line2 << std::fixed << std::setprecision(2);
        line2 << fps[0] << ", " << fps[1] << ", " << fps[2] << " (FPS capture)";
        std::list<std::string> lines ({ line1.str(), line2.str() });
        sherlock::writeOSD(*frame, lines, 0.04);

        // Push image onto all queues.
        for(auto proc_queue : m_detect_queues)
        {
            proc_queue->push(frame);
        }
        m_display_queue.push(frame);
    }

    // Signal end-of-processing by pushing NULL onto all queues.
    for(auto proc_queue : m_detect_queues)
    {
        proc_queue->push(NULL);
    }
    m_display_queue.push(NULL);
}

}  // namespace sherlock.
