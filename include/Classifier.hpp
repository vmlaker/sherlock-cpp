#ifndef __CLASSIFIER_HPP__
#define __CLASSIFIER_HPP__

// Include standard headers.
#include <string>

// Include 3rd party headers.
#include <opencv2/opencv.hpp>
#include <bites.hpp>

namespace sherlock {

/**
  Haar cascade classifier thread.
*/
class Classifier : public bites::Thread
{
public:

    /**
      The resulting datum of the classifier,
      a rectangle and associated color.
    */
    struct RectColor{
        cv::Rect rect;     /**< the rectangle object */
        cv::Scalar color;  /**< the associated color */
    };

    /**
      Initialize the classifer with filename, color and I/O queues.
      @param fname         Name of XML file.
      @param color         Color associated with the classifier.
      @param input_queue   Input queue of incoming frames.
      @param output_queue  Output queue of resulting RectColor objects.
      @param done_queue    Output queue of processed (or skipped) frames.
    */
    Classifier(
        const std::string& fname,
        const cv::Scalar& color,
        bites::ConcurrentQueue <cv::Mat*>& input_queue,
        bites::ConcurrentQueue <Classifier::RectColor>& output_queue,
        bites::ConcurrentQueue <cv::Mat*>& done_queue
        ):
        m_color(color),
        m_input_queue(input_queue),
        m_output_queue(output_queue),
        m_cv_classifier(fname),
        m_done_queue(done_queue)
        {/* Empty. */}

private:
    const cv::Scalar m_color;
    bites::ConcurrentQueue <cv::Mat*>& m_input_queue;
    bites::ConcurrentQueue <Classifier::RectColor>& m_output_queue;
    bites::ConcurrentQueue <cv::Mat*>& m_done_queue;
    cv::CascadeClassifier m_cv_classifier;
    void run();
};

}  // namespace sherlock.

#endif  // __CLASSIFIER_HPP__
