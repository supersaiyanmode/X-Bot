#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "opencv/highgui.h"
#include "opencv/cv.h"

#include "Camera.h"
#include "Window.h"
#include "Image.h"
#include "Transformations.h"
#include "ChessBoard.h"

int main(int argc, char **argv){
    std::cout<<"Initiating..\n";
    xbot::Camera c;
    xbot::Window w;
    std::vector<cv::Point2f> corners;
    
    while (xbot::Window::waitKey(1500) != 27){
        cv::Mat m = c.grab().getImage();
        ChessBoard cb(m);
        cb.drawLines();
        //cb.drawCorners();
        std::cout<<"Drawing\n";
        w.showImage(m);
        //cv::waitKey(0);
    }
    return 0;
}
