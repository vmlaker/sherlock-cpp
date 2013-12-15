#ifndef __CAPTOR_HPP__
#define __CAPTOR_HPP__

// Include standard headers.
#include <vector>
#include <thread>
#include <mutex>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include <bites.hpp>

namespace sherlock {

/**
   Video capture thread.
*/
class Captor : public bites::Thread 
{
public:
    /**
       Initialize the video capture thread with parameters.

       @param  device     Device index.
       @param  width      Width of video.
       @param  height     Height of video.
       @param  duration   Duration of detection (in seconds.)
    */
    Captor(
        const int& device, 
        const int& width,
        const int& height,
        const int& duration
        ):
        m_device        (device),
        m_width         (width),
        m_height        (height),
        m_duration      (duration)
        {/* Empty. */}

    /**
       Add an output queue for allocated frames.
    */
    void addOutput( bites::ConcurrentQueue <cv::Mat*>& );

    /**
       Retrieve the current capture framerate.
    */
    std::vector <float> getFramerate ();

private:
    int m_device;
    int m_width;
    int m_height;
    int m_duration;

    // The output queues and the associated access mutex.
    std::mutex m_output_queues_mutex;
    std::vector< bites::ConcurrentQueue <cv::Mat*>* > m_output_queues;

    // The current running framerate.
    bites::Mutexed <std::vector <float>> m_framerate;

    /**
       Push a frame onto all output queues.
    */
    void pushOutput( cv::Mat* frame );

    /**
       The threaded function.
    */
    void run();
};

}  // namespace sherlock.

#endif  // __CAPTOR_HPP__
