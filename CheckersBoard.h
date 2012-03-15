#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <algorithm>
#include "opencv/cv.h"
#include "Window.h"
#include "Camera.h"
#include "CheckersLine.h"
#include "Cell.h"

class CheckersBoard{
    static int totalDetected, total;
    xbot::Camera camera;
    xbot::Window window;
    std::string analyse(cv::Mat);
public:
    CheckersBoard(int);
    std::string state();
    void drawCorners();
    bool getMove(std::string, int&, int&, int&, int&);
    void destroy();
};

#endif

