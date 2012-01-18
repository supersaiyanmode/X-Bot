#include "Camera.h"
using namespace xbot;
Camera::Camera():capture(1){
    if (!capture.isOpened())
        throw 1;
}

Image Camera::grab(){
    cv::Mat frame;
    capture >> frame;   //that right shift operator overloaded to capture a frame .. :D
    return Image(frame);
}

Camera::~Camera(){

}