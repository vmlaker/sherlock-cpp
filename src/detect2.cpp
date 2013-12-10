/**
   Multi-threaded object detection.
*/

// Include standard headers.
#include <string>
#include <sstream>

// Include application headers.
#include "Detector.hpp"

int main(int argc, char** argv)
{
    // Parse command-line arguments.
    int DEVICE, WIDTH, HEIGHT, DURATION;
    std::string CONFIG_FNAME ("conf/classifiers.conf");
    std::istringstream(std::string(argv[1])) >> DEVICE;
    std::istringstream(std::string(argv[2])) >> WIDTH;
    std::istringstream(std::string(argv[3])) >> HEIGHT;
    std::istringstream(std::string(argv[4])) >> DURATION;
    if (argc > 5) std::istringstream(std::string(argv[5])) >> CONFIG_FNAME;

    // Run the detector.
    sherlock::Detector det (DEVICE, WIDTH, HEIGHT, DURATION, CONFIG_FNAME);
    det.run();
}
