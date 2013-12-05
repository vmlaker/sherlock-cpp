/**
  Utility functions.
*/
#include <list>
#include <string>
#include <boost/date_time.hpp>
#include <opencv2/opencv.hpp>

namespace sherlock {

/**
  Return alpha value based on given timestamp.
  The alpha value is in range [0.0, 1.0], scaled from the
  distance between *tstamp_prev* and now, the distance 
  maximized at *max_life* seconds.
  Set *tstamp_prev* to now.

      For example:  distance  max_life  alpha
                    --------  --------  -----
                        3         6      0.5
                        6         6      1.0
                        9         6      1.0

  @param  tstamp_prev  The previous timestamp with which to compute alpha.
  @param  max_life     Ceiling duration value.
*/
double getAlpha(
    boost::posix_time::ptime& tstamp_prev,
    double max_life);

/**
  Write text given in *lines* iterable,
  the height of each line determined by *size* as
  proportion of image height.

  @param  image  Destination image to write to.
  @param  lines  List of strings, each string representing a line of text.
  @param  size   Proportion of image height determining font height.
*/
void writeOSD(
    cv::Mat& image, 
    const std::list<std::string>& lines, 
    const double& size);

} // namespace sherlock.
