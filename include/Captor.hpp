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
       @param  framerate  Running framerate.
    */
    Captor(
        const int& device, 
        const int& width,
        const int& height,
        const int& duration,
        bites::Mutexed <std::vector <float>>& framerate
        ):
        m_device        (device),
        m_width         (width),
        m_height        (height),
        m_duration      (duration),
        m_output_queues (),
        m_framerate (framerate)
        {/* Empty. */}

    /**
       Add an output queue for allocated frames.
    */
    void addOutput( bites::ConcurrentQueue <cv::Mat*>& );

private:
    int m_device;
    int m_width;
    int m_height;
    int m_duration;

    // The output queues and the associated access mutex.
    std::mutex m_output_queues_mutex;
    std::vector< bites::ConcurrentQueue <cv::Mat*>* > m_output_queues;

    bites::Mutexed <std::vector <float>>& m_framerate;
    void run();

    /**
       Push a frame onto all output queues.
    */
    void pushOutput( cv::Mat* frame );
};

}  // namespace sherlock.

#endif  // __CAPTOR_HPP__
