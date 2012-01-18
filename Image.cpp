#include "Image.h"

using namespace xbot;

Image::Image(){}

Image::Image(const cv::Mat &m):img(m){}

void Image::fromFile(std::string f){
    img = cv::imread(f.c_str(),1);
}
Image Image::clone() const{
    cv::Mat m = img.clone();
    return Image(m);
}
cv::Mat Image::getImage() const{
    return img;
}

int Image::height() const{
    return img.size().height;
}
int Image::width() const{
    return img.size().width;
}
int Image::depth() const{
    return img.type();
}
Image::~Image(){}