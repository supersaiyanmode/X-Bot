#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <algorithm>
#include "opencv/cv.h"

class ChessLine{
    bool horiz;
    int lineNo;
    double slope_,slopeI_;
    int intercept_;
    cv::Point p1,p2;
public:
    int intercept() const;
    ChessLine(const cv::Point&, const cv::Point&);
    bool horizontal();
    bool vertical();
    double slope();
    double slopeI();
    cv::Point getPoint(int);
    static std::pair<std::vector<ChessLine>, std::vector<ChessLine> > findLines(
            const std::vector<cv::Vec4i>&);
};

class Cell{
    int row, column;
    int color;
    int coin;
    std::vector<cv::Point> cellCorners;
public:

};

class ChessBoard{
    cv::Mat& img;
    std::vector<std::vector<cv::Point> > corners;
public:
    ChessBoard(cv::Mat&);
    
    void drawCorners();
    void drawLines();
};

#endif

