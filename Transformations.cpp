#include "Transformations.h"

xbot::Image xbot::toGrayScale(const xbot::Image& img){
    cv::Mat m = img.getImage();
    cv::Mat res(img.width(), img.height(),8);
    cv::cvtColor(m,res,CV_RGB2GRAY);
    return xbot::Image(res);
}

xbot::Image xbot::medianFilter(const xbot::Image &img,int size){
    cv::Mat m = img.getImage();
    cv::Mat res = img.clone().getImage();
    cv::medianBlur(m,res,size);
    return xbot::Image(res);
}

xbot::Image xbot::laplaceTransform(const xbot::Image &img){
    cv::Mat m = img.getImage();
    cv::Mat res = img.clone().getImage();
    cv::Laplacian(m,res,img.depth());
    return xbot::Image(res);
}