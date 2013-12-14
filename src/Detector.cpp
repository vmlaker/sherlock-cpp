/**
   The Detector class encapsulates object detector functionality.
*/

// Include 3rd party headers.
#include <boost/filesystem.hpp>
#include <bites.hpp>

// Include application headers.
#include "Detector.hpp"
#include "util.hpp"

namespace sherlock {

Detector::Detector(
    const int& device,
    const int& width,
    const int& height,
    const int& duration,
    const std::string& config_fname
    ) :
    m_captor(device, width, height, duration, m_capture_framerate),
    m_displayer(
        m_display_queue, 
        m_done_queue, 
        m_rect_colors,
        m_capture_framerate),
    m_deallocator(m_done_queue)
{
    // Add display queue as video capture output.
    m_captor.addOutput (m_display_queue);

    // Load the configuration file.
    bites::Config config (config_fname);

    // Iterate the configuration entries.
    for(auto fname : config.keys())
    {
        // Skip the DIRS setting 
        // (only remainder of file is actual classifier listing.)
        if(fname == "DIRS")
        {
            continue;
        }

        // Process each classifier XML file in the configuration.
        boost::filesystem::path file(fname + ".xml");
        std::string dir_name;
        std::stringstream dirs(config["DIRS"]);
        while(dirs >> dir_name){

            // Assemble the filename.
            boost::filesystem::path dir (dir_name);
            boost::filesystem::path full = dir / file;

            // Check validity of the XML file by creating
            // OpenCV cascade classifier and testing its validity
            // (skip if invalid.)
            cv::CascadeClassifier test (full.string());
            if(test.empty())
            {
                continue;
            }

            // Assemble the color object.
            int rr, gg, bb;
            std::stringstream(config[fname]) >> rr >> gg >> bb;
            cv::Scalar color(rr, gg, bb);

            // Create the classifier input queue.
            auto input_queue = new bites::ConcurrentQueue<cv::Mat*>;
            m_classifier_inputs.push_back(input_queue);

            // Add the classifier input queue as video capture output.
            m_captor.addOutput(*input_queue);

            // Create the classifier worker.
            auto cfer = new sherlock::Classifier(
                full.string(),
                color,
                *input_queue,
                m_rect_colors,
                m_done_queue
                );
            m_classifiers.push_back(cfer);
        }
    }

    // Dump a warning in case of no classifiers.
    if (m_classifiers.size() == 0)
    {
        std::cout << "Warning: Not using any classifiers." << std::endl;
    }
}


void Detector::run()
{
    // Start up the classifier threads.
    for(auto classifier : m_classifiers)
    {
        classifier->start();
    }

    // Start up capture and display threads.
    m_captor.start();
    m_displayer.start();

    // Start up the deallocator thread.
    m_deallocator.setTrigger( m_classifiers.size() + 1 );
    m_deallocator.start();
}


Detector::~Detector()
{
    // Signal deallocator thread to stop.
    m_done_queue.push(NULL);

    // Join all threads.
    for (auto classifier : m_classifiers)
    {
        classifier->join();
        delete classifier;
    }
    m_captor.join();
    m_displayer.join();
    m_deallocator.join();

    for (auto cinput : m_classifier_inputs)
    {
        delete cinput;
    }
}

}  // namespace sherlock.
