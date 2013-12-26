// Include 3rd party headers.
#include <bites.hpp>

// Include application headers.
#include "sherlock.hpp"

namespace sherlock {

void Captor::addOutput( bites::ConcurrentQueue <cv::Mat*>& output )
{
    std::lock_guard <std::mutex> locker (m_output_queues_mutex);
    m_output_queues.push_back( &output );
}

void Captor::pushOutput( cv::Mat* frame ) 
{
    std::lock_guard <std::mutex> locker (m_output_queues_mutex);
    for (auto oqueue : m_output_queues)
    {
        oqueue->push (frame);
    }
}

std::vector <float> Captor::getFramerate ()
{
    return m_framerate.get();
}

void Captor::run ()
{
    // Create the OpenCV video capture object.
    cv::VideoCapture cap(m_device);
    cap.set(3, m_width);
    cap.set(4, m_height);

    // Monitor framerates for the given seconds past.
    bites::RateTicker ticker ({ 1, 5, 10 });

    // Compute interval needed to observe maximum FPS limit.
    float interval_float = 1 / m_max_fps;
    int interval_sec = (int) interval_float;
    auto min_interval = boost::posix_time::time_duration(
        0, // Hours.
        0, // Minutes.
        interval_sec, // Seconds.
        (interval_float - interval_sec) * 1000000  // Fractional seconds.
        );

    // Run the loop for designated amount of time.
    auto prev = boost::posix_time::microsec_clock::universal_time();
    auto end = prev + boost::posix_time::seconds(m_duration);
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
        // Insert delay to observe maximum framerate limit.
        auto elapsed = boost::posix_time::microsec_clock::universal_time() - prev;
        auto sleep_microsec = ( min_interval - elapsed ).total_microseconds();
	sleep_microsec = sleep_microsec < 0 ? 0 : sleep_microsec;
        usleep(sleep_microsec);
        prev = boost::posix_time::microsec_clock::universal_time();

        // Take a snapshot.
        auto frame = new cv::Mat;
        cap >> *frame; 

        // Set the framerate.
        m_framerate.set(ticker.tick());

        // Push image onto all output queues.
        pushOutput( frame );
    }

    // Signal end-of-processing by pushing NULL onto all output queues.
    pushOutput( NULL );
}

}  // namespace sherlock.
