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
      @param  fname           Name of XML file.
      @param  color           Color associated with the classifier.
      @param  scale_factor    Amount to reduce image at each scale.
      @param  min_neighbors   Number of neighbors each candidate rectangle retains.
      @param  min_size_ratio  Ratio of image size for minimum possible object size.
      @param  max_size_ratio  Ratio of image size for maximum possible object size.
      @param  input_queue     Input queue of incoming frames.
      @param  output_queue    Output queue of resulting RectColor objects.
      @param  done_queue      Output queue of processed (or skipped) frames.
    */
    Classifier(
        const std::string& fname,
        const cv::Scalar& color,
        const float& scale_factor,
        const int& min_neighbors,
        const float& min_size_ratio,
        const float& max_size_ratio,
        bites::ConcurrentQueue <cv::Mat*>& input_queue,
        bites::ConcurrentQueue <Classifier::RectColor>& output_queue,
        bites::ConcurrentQueue <cv::Mat*>& done_queue
        ):
        m_color(color),
        m_scale_factor(scale_factor),
        m_min_neighbors(min_neighbors),
        m_min_size_ratio(min_size_ratio),
        m_max_size_ratio(max_size_ratio),
        m_input_queue(input_queue),
        m_output_queue(output_queue),
        m_cv_classifier(fname),
        m_done_queue(done_queue)
        {/* Empty. */}

private:
    const cv::Scalar m_color;
    const float m_scale_factor;
    const int m_min_neighbors;
    const float m_min_size_ratio;
    const float m_max_size_ratio;
    bites::ConcurrentQueue <cv::Mat*>& m_input_queue;
    bites::ConcurrentQueue <Classifier::RectColor>& m_output_queue;
    bites::ConcurrentQueue <cv::Mat*>& m_done_queue;
    cv::CascadeClassifier m_cv_classifier;
    void run();
};

}  // namespace sherlock.

#endif  // __CLASSIFIER_HPP__
