#ifndef CHECKERS_LINE_H
#define CHECKERS_LINE_H
#include <opencv/cv.h>

/*
 * 2D coordinate geometry lines.
 */

class CheckersLine{
    bool horiz;
    double slope_,slopeI_;
    int intercept_;
    cv::Point p1,p2;
public:
    CheckersLine(const cv::Point&, const cv::Point&);
    bool horizontal() const;
    bool vertical() const;
    double slope() const;
    double slopeI() const;
    int intercept() const;
    cv::Point getPoint(int);
};

//used for std::sort(..)
bool operator<(const CheckersLine&, const CheckersLine&);

//For intersecting two non-parallel lines.
cv::Point operator^(const CheckersLine&, const CheckersLine&);
#endif