#ifndef XBOT_WINDOW_HEADER
#define XBOT_WINDOW_HEADER

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <string>
#include <vector>

/*
 * class Window:
 *      A simple wrapper over OpenCV/Highui's cv::namedWindow.
 *      It generates random string for window id, for each instance.
 */

namespace xbot{
    class Window{
        static std::vector<std::string> windowIds;
        std::string windowId;
        static std::string getNewWindowId();
    public:
        Window();
        void showImage(const cv::Mat&);
        static char waitKey(int=10);
        ~Window();
    };
}

#endif