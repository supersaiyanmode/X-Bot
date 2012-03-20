#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <algorithm>
#include "opencv/cv.h"
#include "Window.h"
#include "Camera.h"
#include "CheckersLine.h"
#include "Cell.h"

struct MoveData {
    int r1, c1, r2, c2;
    cv::Point pos1, pos2;
};

class CheckersBoard{
    static int totalDetected, total;
    xbot::Camera camera;
    xbot::Window window;
    std::vector<std::vector<Cell> > cells;//(8, std::vector<Cell>(8,Cell()));
    
    std::string analyse(cv::Mat);
public:
    CheckersBoard(int);
    std::string state();
    void drawCorners();
    bool getMove(std::string, MoveData&);
    std::vector<std::vector<Cell> > getCells();
    void destroy();
};

#endif
