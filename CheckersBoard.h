#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <algorithm>
#include "opencv/cv.h"
#include "Window.h"
#include "Camera.h"

class ChessLine{
    bool horiz;
    double slope_,slopeI_;
    int intercept_;
    cv::Point p1,p2;
public:
    ChessLine(const cv::Point&, const cv::Point&);
    bool horizontal() const;
    bool vertical() const;
    double slope() const;
    double slopeI() const;
    int intercept() const;
    cv::Point getPoint(int);
};

struct Cell{
    int row, column;
    enum CellType{BLACK=' ', WHITE='.', RED='c', GREEN='p', BLUE='B'} type;
    
    cv::Mat image;
    std::vector<cv::Point> corners,subCorners;
    
    Cell();
    int area();
};

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
};

#endif

