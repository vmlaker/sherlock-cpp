#ifndef SHERLOCK_DEALLOCATOR_HPP_INCLUDED
#define SHERLOCK_DEALLOCATOR_HPP_INCLUDED

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include <bites.hpp>

namespace sherlock {

/*!
  Memory deallocation thread.
*/  
class Deallocator : public bites::Thread 
{
public:
    Deallocator( bites::ConcurrentQueue <cv::Mat*>& done_queue ) 
        : m_done_queue(done_queue) {/* Empty. */}
    void setTrigger(const int& value) { m_trigger = value; }
private:
    bites::ConcurrentQueue <cv::Mat*>& m_done_queue;
    int m_trigger = -1;
    void run();
};

}  // namespace sherlock.

#endif  // SHERLOCK_DEALLOCATOR_HPP_INCLUDED
