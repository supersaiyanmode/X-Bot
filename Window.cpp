#include <algorithm>
#include "Window.h"

using namespace xbot;

std::vector<std::string> Window::windowIds = std::vector<std::string>();

Window::Window():windowId(Window::getNewWindowId()){
    windowIds.push_back(windowId);
    cv::namedWindow(windowId,1);
}

void Window::showImage(const cv::Mat& m){
    cv::imshow(windowId, m);
}

std::string Window::getNewWindowId(){
    std::string res;
    do {
        res = "";
        for (int i=10; i--; res += 'A' + rand()%26);
    }while(std::find(Window::windowIds.begin(), Window::windowIds.end(), res) != Window::windowIds.end());
    return res;
}

Window::~Window(){
    //windowIds.erase(std::find(windowIds.begin(),windowIds.end(),windowId));
    cvDestroyWindow(windowId.c_str());
}

char Window::waitKey(int time){
    return cv::waitKey(time);
}