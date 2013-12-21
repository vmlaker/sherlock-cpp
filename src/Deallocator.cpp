// Include application headers.
#include "Deallocator.hpp"

namespace sherlock {

// Deallocate frames in given queue.
// Deallocate a frame when its count reaches trigger threshold.
void Deallocator::run ()
{
    // Count the number of times each frame is encountered
    // in the "done" queue (to know when the count triggers
    // deallocation.)
    std::map <cv::Mat*, int> done_counts;
    
    // Pull from the queue while there are valid matrices.
    cv::Mat* frame;
    m_done_queue.wait_and_pop(frame);
    while(frame)
    {
        // If this is the first time frame is encountered,
        // initialize it's count to 1 (one.)
        // Otherwise, increment the frame's existing count,
        if(done_counts.find(frame) == done_counts.end())
        {
            done_counts.insert({ frame, 1 });
        }else{
            done_counts[frame]++;
        }
        
        // Perform deallocation, if triggered.
        if(done_counts[frame] == m_trigger)
        {
            delete frame;
            done_counts.erase(frame);
        }
        
        // Retrieve the next frame.
        m_done_queue.wait_and_pop(frame);
    }
}

}  // namespace sherlock.
