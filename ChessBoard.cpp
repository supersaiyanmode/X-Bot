#include <cmath>
#include <iostream>
#include <cstdio>
#include "ChessBoard.h"
#include "Transformations.h"
//###########CHESSLINE################

ChessLine::ChessLine(const cv::Point& p, const cv::Point& q):p1(p), p2(q){
    slope_ = (p.y-q.y)/(double)(p.x-q.x);
    slopeI_ = (double)(p.x-q.x)/(double)(p.y-q.y);
    horiz = slope_ < 1 && slope_ > -1;
    intercept_ = horiz? (p1.y - slope_*p1.x) : (p1.x - p1.y/slope_);
}
bool ChessLine::horizontal(){
    return horiz;
}
bool ChessLine::vertical(){
    return !horiz;
}

cv::Point ChessLine::getPoint(int first){
    return first? p1 : p2;
}

double ChessLine::slope(){
    return slope_;
}
double ChessLine::slopeI(){
    return slopeI_;
}
int ChessLine::intercept() const{
    return intercept_;
}

bool operator<(const ChessLine& left, const ChessLine& right){
    return left.intercept()<right.intercept();
}



//############CHESSBOARD###############

ChessBoard::ChessBoard(cv::Mat& m):img(m){
    
}
    
void ChessBoard::drawCorners(){
    std::vector<cv::Point2f> corners;
    cv::findChessboardCorners(img,cv::Size(7,7), corners);
    for (int i=0; i<corners.size(); i++){
        cv::circle(img, cv::Point(corners[i].x,corners[i].y),4, cv::Scalar(0,255,0),-1);
    }
}

void ChessBoard::drawLines(){
    //std::cout<<"Waiting...\n";
    //std::getchar();
    cv::Mat m = xbot::toGrayScale(xbot::Image(img)).getImage();
    cv::Canny(m,m,50,200);
    cv::GaussianBlur(m,m,cv::Size(3,3),2);
    cv::threshold(m,m,50,255,cv::THRESH_BINARY);
    
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(m, lines, 1, CV_PI/180.0,20,300,30);
    
    for (int i=0, len=lines.size(); 0 && i<len; i++){
        //draw the effing line!
        cv::Point p1(lines[i][0],lines[i][1]),p2(lines[i][2],lines[i][3]);
        cv::line(img, p1, p2, cv::Scalar(255,0,0),2,8);
    }
    
    std::vector<ChessLine> horizLines, vertLines, horizLinesPruned, vertLinesPruned;
    for (int i=0, len=lines.size(); i<len; i++){
        cv::Point p1(lines[i][0],lines[i][1]),p2(lines[i][2],lines[i][3]);
        ChessLine cline(p1,p2);
        (cline.horizontal()?horizLines:vertLines).push_back(cline);
    }
    
    std::sort(horizLines.begin(), horizLines.end());
    std::sort(vertLines.begin(), vertLines.end());
    
    
    
    //combine similar horizontal lines..
    std::cout<<"HorizIntercepts:\n";
    for (int i=0, len=horizLines.size(); 0 && i<len-1; i++){
        cv::line(img, horizLines[i].getPoint(0), horizLines[i].getPoint(1),
                 cv::Scalar(0,255,255),2,8);
        //std::cout<<"("<<horizLines[i].intercept()<<", "<<horizLines[i].slope()<<"), ";
    }
    std::cout.flush();
    
    for (int i=0, len=horizLines.size(); i<len-1; i++){
        int fromIndex = i, toIndex = i-1;
        int minIntercept=horizLines[i].intercept(), maxIntercept=horizLines[i].intercept();
        
        while (maxIntercept - minIntercept < 20){ //threshold
            toIndex++;
            i++;
            minIntercept = std::min(minIntercept, horizLines[i].intercept());
            maxIntercept = std::max(maxIntercept, horizLines[i].intercept());
        }
        i--;
        int totalIntercepts = 0, avgIntercept;
        double totalSlope=0.0, avgSlope;
        for (int j=fromIndex; j<=toIndex; j++){
            totalIntercepts += horizLines[j].intercept();
            totalSlope += horizLines[j].slope();
        }
        std::cout.flush();
        int total = toIndex - fromIndex + 1;
        avgIntercept = (double)totalIntercepts/double(total);
        avgSlope = (double)totalSlope/double(total);
        
        std::cout<<"Average("<<total<<"): ["<<avgIntercept<<", "<<avgSlope<<"]"<<std::endl;
        
        ChessLine newCBLine(cv::Point(0,avgIntercept),
                            cv::Point(img.size().width, avgSlope*img.size().width + avgIntercept));
        horizLinesPruned.push_back(newCBLine);
    }
    
    //draw Pruned lines..
    for (int i=0, len=horizLinesPruned.size(); i<len; i++){
        //draw the effing line!
        cv::line(img, horizLinesPruned[i].getPoint(0), horizLinesPruned[i].getPoint(1),
                 cv::Scalar(0,255,0),2,8);
    }
    
    //###############################################################################//
    //combine similar vertical lines..
    std::cout<<"VertIntercepts:\n";
    for (int i=0, len=vertLines.size(); 0 && i<len-1; i++){
        cv::line(img, vertLines[i].getPoint(0), vertLines[i].getPoint(1),
                 cv::Scalar(255,0,255),2,8);
        //std::cout<<"("<<horizLines[i].intercept()<<", "<<horizLines[i].slope()<<"), ";
    }
    std::cout.flush();
    
    for (int i=0, len=vertLines.size(); i<len-1; i++){
        int fromIndex = i, toIndex = i-1;
        int minIntercept=vertLines[i].intercept(), maxIntercept=vertLines[i].intercept();
        
        while (maxIntercept - minIntercept < 20){ //threshold
            toIndex++;
            i++;
            minIntercept = std::min(minIntercept, vertLines[i].intercept());
            maxIntercept = std::max(maxIntercept, vertLines[i].intercept());
        }
        i--;
        int totalIntercepts = 0, avgIntercept;
        double totalSlope=0.0, avgSlopeI;
        for (int j=fromIndex; j<=toIndex; j++){
            totalIntercepts += vertLines[j].intercept();
            totalSlope += vertLines[j].slopeI();
        }
        std::cout.flush();
        int total = toIndex - fromIndex + 1;
        avgIntercept = (double)totalIntercepts/double(total);
        avgSlopeI = (double)totalSlope/double(total);
        
        std::cout<<"Average("<<total<<"): ["<<avgIntercept<<", "<<avgSlopeI<<"]"<<std::endl;
        
        ChessLine newCBLine(cv::Point(avgIntercept,0),
                            cv::Point(avgSlopeI*img.size().height + avgIntercept,img.size().height));
        vertLinesPruned.push_back(newCBLine);
    }
    
    //draw Pruned lines..
    for (int i=0, len=horizLinesPruned.size(); i<len; i++){
        //draw the effing line!
        cv::line(img, vertLinesPruned[i].getPoint(0), vertLinesPruned[i].getPoint(1),
                 cv::Scalar(255,0,0),2,8);
    }
}
