// Utility functions.

#include <string>
#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

namespace sherlock {


// Return alpha value based on given timestamp.
// The alpha value is in range [0.0, 1.0], scaled from the
// distance between *tstamp_prev* and now, the distance 
// maximized at *max_life* seconds.
// Set *tstamp_prev* to now.
// For example:  distance  max_life  alpha
//               --------  --------  -----
//                    3         6      0.5
//                    6         6      1.0
//                    9         6      1.0
//
double getAlpha(
    boost::posix_time::ptime& tstamp_prev,
    double max_life
    )
{
    auto now = boost::posix_time::microsec_clock::universal_time();
    double alpha = 1.0;  // Default is 100% opacity.
    if (!tstamp_prev.is_not_a_date_time()) 
    {
        // alpha = min {delta_t, max_life} / max_life
        auto tdelta = now - tstamp_prev;
        alpha = tdelta.total_nanoseconds()/1000000000.;
        alpha = std::min(alpha, max_life);
        alpha /= max_life;
    }
    tstamp_prev = now;
    return alpha;
}


// Write text given in *lines* iterable,
// the height of each line determined by *size* as
// proportion of image height.
void writeOSD(
    cv::Mat& image, 
    const std::list<std::string>& lines, 
    const double& size)
{
    // Compute row height at scale 1.0 first.
    int baseline;
    cv::Size textSize = cv::getTextSize(
        "I",                       // Text.
        cv::FONT_HERSHEY_SIMPLEX,  // Font.
        1.0,                       // Font scale.
        1,                         // Thickness.
        &baseline
        );
     
    // Compute actual scale to match desired height.
    double line_height = image.rows * size;
    double scale = line_height / textSize.height;

    // Deterimine base thickness, based on scale.
    int thickness = scale * 4;

    // Increase line height, to account for thickness.
    line_height += thickness * 3;

    // Iterate the lines of text, and draw them.
    int xoffset = textSize.width * scale;
    int yoffset = line_height;
    for(auto ii=lines.begin(); ii!=lines.end(); ++ii){

        auto line = *ii;
 
        // Draw the drop shadow.
        cv::putText( 
            image,
            line,
            cv::Point(
                xoffset + std::max(1, thickness/2), 
                yoffset + std::max(1, thickness/2)
                ),
            cv::FONT_HERSHEY_SIMPLEX,
            scale,
            cv::Scalar(0, 0, 0),
            thickness
            );

        // Draw the text body.
        cv::putText( 
            image,
            line,
            cv::Point(xoffset, yoffset),
            cv::FONT_HERSHEY_SIMPLEX,
            scale,
            cv::Scalar(215, 215, 70),
            thickness
            );

        // Draw the highlight.
        cv::putText( 
            image,
            line,
            cv::Point(
                xoffset - std::max(1, thickness/3), 
                yoffset - std::max(1, thickness/3)
                ),
            cv::FONT_HERSHEY_SIMPLEX,
            scale,
            cv::Scalar(245, 255, 200),
            thickness/3
            );
        
        yoffset += line_height;
    }

}

} // namespace sherlock.
