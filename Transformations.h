#ifndef XBOT_TRANSFORMATIONS_HEADER
#define XBOT_TRANSFORMATIONS_HEADER
#include "Image.h"

namespace xbot{
    xbot::Image toGrayScale(const xbot::Image&);
    xbot::Image medianFilter(const xbot::Image &,int);
    xbot::Image laplaceTransform(const xbot::Image &);    
}
#endif