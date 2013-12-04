#ifndef __CLASSIFIER_HPP__
#define __CLASSIFIER_HPP__

// Include standard headers.
#include <string>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include <bites.hpp>

// Include application headers.
#include "Sherlock.hpp"

namespace sherlock {

/*!
  Haar cascade classifier thread.
*/
class Classifier : public bites::Thread
{
public:
    Classifier(
        const std::string& fname,
        const cv::Scalar& color,
        bites::ConcurrentQueue <cv::Mat*>& input_queue,
        bites::ConcurrentQueue <sherlock::RectColor>& output_queue,
        bites::ConcurrentQueue <cv::Mat*>& done_queue
        );
private:
    const cv::Scalar m_color;
    bites::ConcurrentQueue <cv::Mat*>& m_input_queue;
    bites::ConcurrentQueue <sherlock::RectColor>& m_output_queue;
    bites::ConcurrentQueue <cv::Mat*>& m_done_queue;
    cv::CascadeClassifier m_cv_classifier;
    void run();
};

}  // namespace sherlock.

#endif  // __CLASSIFIER_HPP__
