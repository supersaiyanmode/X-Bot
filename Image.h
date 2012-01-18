#ifndef XBOT_IMAGE_HEADER
#define XBOT_IMAGE_HEADER
#include "opencv/cv.h"
#include "opencv/highgui.h"

namespace xbot{
    class Image{
        cv::Mat img;
    public:
        Image();
        Image(const cv::Mat&);
        void fromFile(std::string);
        Image clone() const;
        
        cv::Mat getImage() const;
        int height() const;
        int width() const;
        int depth() const;
        ~Image();
    };
}

#endif