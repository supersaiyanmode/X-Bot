#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "opencv/highgui.h"
#include "opencv/cv.h"

#include "Camera.h"
#include "Window.h"
#include "ChessBoard.h"
#include "CheckersServer/Server.h"

std::string checkersStr="#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.";
std::string checkerStatus(){
    return checkersStr;
}

int main(int argc, char **argv){
    Server s(44101);
    s.setCallback(checkerStatus);
    s.start();
    std::cout<<"Initiating image processing..\n";
    xbot::Camera c(1);
    xbot::Window w;
    std::vector<cv::Point2f> corners;
    
    while (xbot::Window::waitKey(1500) != 27){
        cv::Mat m = c.grab();
        //static cv::Mat m = cv::imread("pic.jpg");
        ChessBoard cb(m);
        cb.drawLines();
        if (cb.good())
            checkersStr = cb.str();
        //cb.drawCorners();
        w.showImage(m);
        //cv::waitKey(0);
    }
    return 0;
}
