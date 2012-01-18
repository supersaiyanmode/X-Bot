#ifndef XBOT_WINDOW_HEADER
#define XBOT_WINDOW_HEADER

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <string>
#include <vector>
#include "Image.h"


namespace xbot{
    class Window{
        static std::vector<std::string> windowIds;
        std::string windowId;
        static std::string getNewWindowId();
    public:
        Window();
        void showImage(const xbot::Image&);
        void showImage(const cv::Mat&);
        static char waitKey(int=10);
        ~Window();
    };
}

#endif