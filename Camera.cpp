#include "Camera.h"
using namespace xbot;

/* 
    int n -- the camera number, /dev/video1 => n = 1
*/
Camera::Camera(int n):capture(n){
    if (!capture.isOpened())
        throw 1;
}

/*
 * Grabs an image from the camera and returns it
 */
cv::Mat Camera::grab(){
    cv::Mat frame;
    capture >> frame;   //that right shift operator overloaded to capture a frame .. :D
    return frame;
}

Camera::~Camera(){

}