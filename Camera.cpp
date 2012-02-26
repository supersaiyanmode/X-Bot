#include "Camera.h"
using namespace xbot;
Camera::Camera(int n):capture(n){
    if (!capture.isOpened())
        throw 1;
}

cv::Mat Camera::grab(){
    cv::Mat frame;
    capture >> frame;   //that right shift operator overloaded to capture a frame .. :D
    return frame;
}

Camera::~Camera(){

}