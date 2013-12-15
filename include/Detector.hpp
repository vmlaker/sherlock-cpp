#ifndef __DETECTOR_HPP__
#define __DETECTOR_HPP__

// Include standard headers.
#include <vector>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include <bites.hpp>

// Include application headers.
#include "Captor.hpp"
#include "Classifier.hpp"
#include "Deallocator.hpp"
#include "Displayer.hpp"

namespace sherlock {

/**
   The top-level object detection class.
*/
class Detector {

public:

    /**
       Initialize the object detector with configuration parameters.

       @param  device        Device index.
       @param  width         Width of video.
       @param  height        Height of video.
       @param  duration      Duration of detection (in seconds.)
       @param  config_fname  Classifier configuration file.
    */
    Detector(
        const int& device, 
        const int& width, 
        const int& height, 
        const int& duration,
        const std::string& config_fname);
    ~Detector();

    /**
       Start detection.
    */
    void run();

private:
    // Video capture object.
    sherlock::Captor m_captor;

    // Video display object.
    sherlock::Displayer m_displayer;

    // Memory deallocate object.
    sherlock::Deallocator m_deallocator;

    // List of classifier objects.
    std::list <sherlock::Classifier*> m_classifiers;

    // Shared queues.
    std::vector< bites::ConcurrentQueue <cv::Mat*>* > m_classifier_inputs;
    bites::ConcurrentQueue <cv::Mat*> m_display_queue;
    bites::ConcurrentQueue <cv::Mat*> m_done_queue;
    bites::ConcurrentQueue <Classifier::RectColor> m_rect_colors;
};

}  // namespace sherlock.

#endif  //__DETECTOR_HPP__
