/**
   Multi-threaded object detection.
*/

// Include standard headers.
#include <string>
#include <sstream>

// Include application headers.
#include "sherlock.hpp"

int main(int argc, char** argv)
{
    // Parse command-line arguments.
    int DEVICE, WIDTH, HEIGHT, DURATION;
    float MAX_FPS;
    std::string CONFIG_FNAME ("conf/classifiers.conf");
    std::istringstream(std::string(argv[1])) >> DEVICE;
    std::istringstream(std::string(argv[2])) >> WIDTH;
    std::istringstream(std::string(argv[3])) >> HEIGHT;
    std::istringstream(std::string(argv[4])) >> DURATION;
    if (argc > 5) std::istringstream(std::string(argv[5])) >> MAX_FPS;
    if (argc > 6) std::istringstream(std::string(argv[6])) >> CONFIG_FNAME;

    // Run the detector.
    sherlock::Detector det (DEVICE, WIDTH, HEIGHT, DURATION, MAX_FPS, CONFIG_FNAME);
    det.run();
}
