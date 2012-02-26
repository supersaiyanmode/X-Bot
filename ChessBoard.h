#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <algorithm>
#include "opencv/cv.h"

class ChessLine{
    bool horiz;
    double slope_,slopeI_;
    int intercept_;
    cv::Point p1,p2;
public:
    int intercept() const;
    ChessLine(const cv::Point&, const cv::Point&);
    bool horizontal() const;
    bool vertical() const;
    double slope() const;
    double slopeI() const;
    cv::Point getPoint(int);
    static std::pair<std::vector<ChessLine>, std::vector<ChessLine> > findLines(
            const std::vector<cv::Vec4i>&);
};

struct Cell{
    int row, column;
    enum CellType{BLACK='#', WHITE='.', RED='R', GREEN='G', BLUE='B'} type;
    
    cv::Mat image;
    std::vector<cv::Point> corners,subCorners;
    
    Cell();
    int area();
};

class ChessBoard{
    cv::Mat& img;
    std::vector<std::vector<Cell> > cells;
public:
    ChessBoard(cv::Mat&);
    
    void drawCorners();
    int drawLines();
};

#endif

