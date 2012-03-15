#ifndef CHECKERS_LINE_H
#define CHECKERS_LINE_H
#include <opencv/cv.h>

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
bool operator<(const CheckersLine&, const CheckersLine&);
cv::Point operator^(const CheckersLine&, const CheckersLine&);
#endif