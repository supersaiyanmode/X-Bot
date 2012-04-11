#ifndef CELL_H
#define CELL_H
#include <opencv/cv.h>


/*
 * struct Cell
 * Contains:
 *      row, column: The place where this cell appears on the checkersboard
 *      type: Type of the cell.
 *      image: The subsection of the image that corresponds to this cell
 *      corners: The 4 corners of the cell as vector<cv::Point>
 *      subCorners: The 4 corners of the cell wrt to this->image
 *      area(): returns area of the cell
 */

struct Cell{
    int row, column;
    enum CellType{BLACK=' ', WHITE='.', RED='R', GREEN='G', BLUE='B'} type;
    
    cv::Mat image;
    std::vector<cv::Point> corners,subCorners;
    
    Cell();
    cv::Point centroid();
    int area();
};


#endif