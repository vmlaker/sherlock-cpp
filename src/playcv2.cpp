// Live playback with OpenCV.

#include <opencv2/opencv.hpp>
using namespace cv;

int main(int, char**)
{
    VideoCapture cap;
    cap.open(-1);
    namedWindow("video", 1);
    for(;;)
    {
        Mat frame;
        cap >> frame; 
        if(!frame.data)
            break;
        imshow("video", frame); 
        if(waitKey(10) >= 0) 
            break;
    }
}
