#include "Cell.h"

//CTOR: resize the vectors to 4.
Cell::Cell(){
    corners.resize(4);
    subCorners.resize(4);
}

//Calculate the area by dividing into triangles.
int Cell::area(){
    //use that crappy two triangle area method ..
    //MUST: pass in strict clockwise or anti ..
    //Else you will get non-convex poly, formula fails..
    int ret = 0;
    ret += corners[0].x*corners[1].y - corners[1].x*corners[0].y;
    ret += corners[1].x*corners[2].y - corners[2].x*corners[1].y;
    ret += corners[2].x*corners[3].y - corners[3].x*corners[2].y;
    ret += corners[3].x*corners[0].y - corners[0].x*corners[3].y;
    return ret/2;
}


//Calculate the centroid
cv::Point Cell::centroid(){
    return cv::Point((corners[0].x+corners[1].x+corners[2].x+corners[3].x)/4,
                     (corners[0].y+corners[1].y+corners[2].y+corners[3].y)/4);
}
