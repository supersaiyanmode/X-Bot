#ifndef XBOT_CAMERA_HEADER
#define XBOT_CAMERA_HEADER


#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <queue>

/*
 * Class Camera: A tiny wrapper over cv::VideoCapture
 */
namespace xbot{
    class Camera{
        cv::VideoCapture capture;
    public:
        Camera(int);
        cv::Mat grab();
        ~Camera();
    };
}

#endif