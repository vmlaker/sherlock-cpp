// Object detection.

// Include standard headers.
#include <string>
#include <sstream>

// Include application headers.
#include "Detector.hpp"

int main(int argc, char** argv)
{
    // Parse command-line arguments.
    int DEVICE, WIDTH, HEIGHT, DURATION;
    std::istringstream(std::string(argv[1])) >> DEVICE;
    std::istringstream(std::string(argv[2])) >> WIDTH;
    std::istringstream(std::string(argv[3])) >> HEIGHT;
    std::istringstream(std::string(argv[4])) >> DURATION;

    // Run the detector.
    sherlock::Detector det (DEVICE, WIDTH, HEIGHT, DURATION);
    det.run();
}
