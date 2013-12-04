#ifndef __CLASSIFIER_HPP__
#define __CLASSIFIER_HPP__

// Include standard headers.
#include <string>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include "ConcurrentQueue.hpp"

// Include application headers.
#include "Worker.hpp"
#include "Sherlock.hpp"

namespace sherlock {

class Classifier : public Worker {

public:
    Classifier(
        const std::string& fname,
        const cv::Scalar& color,
        ConcurrentQueue <cv::Mat*>& input_queue,
        ConcurrentQueue <sherlock::RectColor>& output_queue,
        ConcurrentQueue <cv::Mat*>& done_queue
        );
private:
    const cv::Scalar m_color;
    ConcurrentQueue <cv::Mat*>& m_input_queue;
    ConcurrentQueue <sherlock::RectColor>& m_output_queue;
    ConcurrentQueue <cv::Mat*>& m_done_queue;
    cv::CascadeClassifier m_cv_classifier;
    void run();
};

}  // namespace sherlock.

#endif  // __CLASSIFIER_HPP__
