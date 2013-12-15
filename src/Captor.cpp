// Include 3rd party headers.
#include <bites.hpp>

// Include application headers.
#include "Captor.hpp"

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

    // Run the loop for designated amount of time.
    auto now = boost::posix_time::microsec_clock::universal_time();
    auto dur = boost::posix_time::seconds(m_duration);
    auto end = now + dur;
    while (end > boost::posix_time::microsec_clock::universal_time())
    {
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
