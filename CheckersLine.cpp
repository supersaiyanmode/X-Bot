#include <iostream>
#include "CheckersLine.h"

//Construct using the two points, calculate the slope, and the inverse.
CheckersLine::CheckersLine(const cv::Point& p, const cv::Point& q):p1(p), p2(q){
    slope_ = (p.y-q.y)/(double)(p.x-q.x);
    slopeI_ = (double)(p.x-q.x)/(double)(p.y-q.y);
    horiz = slope_ < 1 && slope_ > -1;
    intercept_ = horiz? (p1.y - slope_*p1.x) : (p1.x - p1.y/slope_);
}
bool CheckersLine::horizontal() const{
    return horiz;
}
bool CheckersLine::vertical() const{
    return !horiz;
}

//Return the point that made up this line
cv::Point CheckersLine::getPoint(int first){
    return first? p1 : p2;
}

double CheckersLine::slope() const{
    return slope_;
}

//Inverse slope == slopeI, to make calculations stay within computation domain.
//and not touch inifinty for vertical lines.
double CheckersLine::slopeI() const{
    return slopeI_;
}
int CheckersLine::intercept() const{
    return intercept_;
}

//Overloaded < for sorting the lines.
bool operator<(const CheckersLine& left, const CheckersLine& right){
    return left.intercept()<right.intercept();
}

//Returns the intersection of the lines. Constraint: One is horiz other is vert.
cv::Point operator^(const CheckersLine& left, const CheckersLine& right){
    //this crap assumes one line is horizontal and another is vertical
    if ((!!left.horizontal()) ^ (!!right.horizontal())){
        //y = <slopeHoriz> * x + interceptHoriz
        CheckersLine horizLine(left.horizontal()? left:right);
        
        //x = y*slopeVert_I + interceptVert
        CheckersLine vertLine(right.horizontal()? left:right);
        
        //y = <slopeHoriz> * (y * <slopeVert_I> + interceptVert) + interceptHoriz
        //y(1 - slopeHoriz * slopeVert_I) = slopeHoriz*interceptVert + interceptHoriz
        //y = (slopeHoriz*interceptVert + interceptHoriz)/(1 - slopeHoriz*slopeVert_I)
        int y = horizLine.slope() * vertLine.intercept() + horizLine.intercept();
        y /= (1 - horizLine.slope() * vertLine.slopeI());
        
        //x =  y * slopeVert_I + interceptVert
        int x = y * vertLine.slopeI() + vertLine.intercept();
        return cv::Point(x,y);
    }else{
        std::cout<<"WHAT?!?!?!?! Intersect (probably) parallel lines?"<<std::endl;
        return cv::Point();
    }
}
