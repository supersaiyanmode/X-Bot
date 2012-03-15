#ifndef CELL_H
#define CELL_H
#include <opencv/cv.h>

struct Cell{
    int row, column;
    enum CellType{BLACK=' ', WHITE='.', RED='c', GREEN='p', BLUE='B'} type;
    
    cv::Mat image;
    std::vector<cv::Point> corners,subCorners;
    
    Cell();
    cv::Point centroid();
    int area();
};


#endif