// Utility functions.

#include <list>
#include <string>
#include <opencv2/opencv.hpp>

namespace sherlock {

// Write text given in *lines* iterable,
// the height of each line determined by *size* as
// proportion of image height.
void writeOSD(
    cv::Mat& image, 
    const std::list<std::string>& lines, 
    const double& size
    );

} // namespace sherlock.
