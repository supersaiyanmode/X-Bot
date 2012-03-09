#include <cmath>
#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdio>
#include "CheckersBoard.h"
#include "Window.h"
#include "Thread.cpp"

#define DEBUG 0
//###########CHESSLINE################

ChessLine::ChessLine(const cv::Point& p, const cv::Point& q):p1(p), p2(q){
    slope_ = (p.y-q.y)/(double)(p.x-q.x);
    slopeI_ = (double)(p.x-q.x)/(double)(p.y-q.y);
    horiz = slope_ < 1 && slope_ > -1;
    intercept_ = horiz? (p1.y - slope_*p1.x) : (p1.x - p1.y/slope_);
}
bool ChessLine::horizontal() const{
    return horiz;
}
bool ChessLine::vertical() const{
    return !horiz;
}

cv::Point ChessLine::getPoint(int first){
    return first? p1 : p2;
}

double ChessLine::slope() const{
    return slope_;
}
double ChessLine::slopeI() const{
    return slopeI_;
}
int ChessLine::intercept() const{
    return intercept_;
}

bool operator<(const ChessLine& left, const ChessLine& right){
    return left.intercept()<right.intercept();
}

cv::Point operator^(const ChessLine& left, const ChessLine& right){
    //this crap assumes one line is horizontal and another is vertical
    if ((!!left.horizontal()) ^ (!!right.horizontal())){
        //y = <slopeHoriz> * x + interceptHoriz
        ChessLine horizLine(left.horizontal()? left:right);
        
        //x = y*slopeVert_I + interceptVert
        ChessLine vertLine(right.horizontal()? left:right);
        
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
    }
}

//############CELL###############
Cell::Cell(){
    corners.resize(4);
    subCorners.resize(4);
}

int Cell::area(){
    //use that crappy two triangle area method ..
    //MUST: pass in strict clockwise or anti ..
    //nahi toh you will get non-convex poly, formula fails..
    int ret = 0;
    ret += corners[0].x*corners[1].y - corners[1].x*corners[0].y;
    ret += corners[1].x*corners[2].y - corners[2].x*corners[1].y;
    ret += corners[2].x*corners[3].y - corners[3].x*corners[2].y;
    ret += corners[3].x*corners[0].y - corners[0].x*corners[3].y;
    return ret/2;
}
//############CHESSBOARD###############
#define ERR_BAD_DETECTION "BAD DETECTION"

int CheckersBoard::total=0;
int CheckersBoard::totalDetected=0;
CheckersBoard::CheckersBoard(int cam):camera(cam){
    
}

void CheckersBoard::drawCorners(){
    /*std::vector<cv::Point2f> corners;
    cv::findChessboardCorners(img,cv::Size(7,7), corners);
    for (int i=0; i<corners.size(); i++){
        cv::circle(img, cv::Point(corners[i].x,corners[i].y),4, cv::Scalar(0,255,0),-1);
    }*/
}

std::string CheckersBoard::state(){
    const static int RELIABLE_NUM = 2;
    std::vector<std::string> states;
    while (1){
        std::string res;
        cv::Mat img;
        do{
            img = camera.grab();
            res = analyse(img);
            window.showImage(img);
            window.waitKey(res.length()==64? 800:800);
        }while (res.length() != 64);
        
        states.push_back(res);
        if (states.size() > RELIABLE_NUM)
            states.erase(states.begin());
        
        bool same = true;
        for (int i=0; i<states.size() -1 && same; i++){
            if (states[i] != states[i+1]) 
                same = false;
        }
        if (same && states.size() == RELIABLE_NUM)
            return res;
    }
    return "";
}

bool CheckersBoard::getMove(std::string curState, int& r1, int& c1, int& r2, int& c2){
    std::cout<<"Checking for player move...";
    std::string newState = state();
    if (newState.length() != 64){
        std::cout<<"[Bad new state]\n";
        return false;
    }
    
    size_t pos=-1;
    r1 = -1;
    //polls changes for pP coins ..
    while ((pos=curState.find_first_of("pP",pos+1))!=std::string::npos){
        if (curState[pos] != newState[pos]){
            std::cout<<"Found inequality at: "<<pos<<std::endl;
            r1 = pos/8;
            c1 = pos%8;
            break;
        }
    }
    if (r1 == -1){
        std::cout<<"No change!\n";
        return false;
    }
    pos = -1;
    while ((pos=curState.find_first_of(".",pos+1))!=std::string::npos){
        if (curState[pos] != newState[pos]){
            std::cout<<"Found inequality at: "<<pos<<std::endl;
            r2 = pos/8;
            c2 = pos%8;
            break;
        }
    }
    if (r2 == -1){
        std::cout<<"Cant find piece that appeared.\n";
        return false;
    }
    std::cout<<"[Got!]\n";
    return true;
}

std::string CheckersBoard::analyse(cv::Mat img){
    cv::Mat mainImage = img.clone();
    
    cv::Mat bwImg(img.size(),8);
    cv::cvtColor(img,bwImg,CV_BGR2GRAY);
    
    
    cv::Mat m = bwImg.clone();
    
    //cv::erode(m,m,cv::Mat());
    //cv::dilate(m,m,cv::Mat());
    
    cv::Canny(m,m,50,200);
    //cv::GaussianBlur(m,m,cv::Size(5,5),2);
    //cv::threshold(m,m,50,255,cv::THRESH_BINARY);
    
    cv::dilate(m,m,cv::Mat());
    
    static xbot::Window w;
    w.showImage(m);
    w.waitKey(1);
    //img = m;
    //return 0;
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(m,   //the image
                    lines, //out - vector of vec4i ..
                    0.5, //linear accuracy of the accumulator
                    CV_PI/180.0, //radial accuracy
                    20,  //max parallel distance of lines to be considered as one..
                    300, //min length of the line -- condition
                    30 //max breakage in the line .. along the line ..
                );
    
    for (int i=0, len=lines.size();i<len; i++){
        //draw the effing line!
        cv::Point p1(lines[i][0],lines[i][1]),p2(lines[i][2],lines[i][3]);
        cv::line(img, p1, p2, cv::Scalar(255,0,255),2,8);
    }
    
    std::vector<ChessLine> horizLines, vertLines, horizLinesPruned, vertLinesPruned;
    for (int i=0, len=lines.size(); i<len; i++){
        cv::Point p1(lines[i][0],lines[i][1]),p2(lines[i][2],lines[i][3]);
        ChessLine cline(p1,p2);
        (cline.horizontal()?horizLines:vertLines).push_back(cline);
    }
    
    std::sort(horizLines.begin(), horizLines.end());
    std::sort(vertLines.begin(), vertLines.end());
    
    int horizThreshold = m.size().height/32; //height for threshold of horizontal lines 
    int vertThreshold = m.size().width/32; // and similarly for vertical ones..
    
    //combine similar horizontal lines..
    for (int i=0, len=horizLines.size(); 0 && i<len-1; i++){
        cv::line(img, horizLines[i].getPoint(0), horizLines[i].getPoint(1),
                 cv::Scalar(0,255,255),2,8);
        //std::cout<<"("<<horizLines[i].intercept()<<", "<<horizLines[i].slope()<<"), ";
    }
    std::cout.flush();
    
    for (int i=0, len=horizLines.size(); i<len; i++){
        int fromIndex = i, toIndex = i-1;
        int minIntercept=horizLines[i].intercept(), maxIntercept=horizLines[i].intercept();
        
        while (maxIntercept - minIntercept < horizThreshold){ //threshold
            toIndex++;
            i++;
            if (i >= len)
                break;
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
        
        
        ChessLine newCBLine(cv::Point(0,avgIntercept),
                            cv::Point(img.size().width, avgSlope*img.size().width + avgIntercept));
        horizLinesPruned.push_back(newCBLine);
    }
#if DEBUG
    std::cout<<"Horizontal lines count: "<<horizLinesPruned.size()<<std::endl;
#endif    
    //draw the effing Pruned lines..
    for (int i=0, len=horizLinesPruned.size(); i<len; i++){
        cv::line(img, horizLinesPruned[i].getPoint(0), horizLinesPruned[i].getPoint(1),
                 cv::Scalar(0,255,0),2,8);
    }
    
    //###############################################################################//
    //combine similar vertical lines..
    for (int i=0, len=vertLines.size(); 0 && i<len-1; i++){
        cv::line(img, vertLines[i].getPoint(0), vertLines[i].getPoint(1),
                 cv::Scalar(255,0,255),2,8);
        //std::cout<<"("<<horizLines[i].intercept()<<", "<<horizLines[i].slope()<<"), ";
    }
    std::cout.flush();
    
    for (int i=0, len=vertLines.size(); i<len; i++){
        int fromIndex = i, toIndex = i-1;
        int minIntercept=vertLines[i].intercept(), maxIntercept=vertLines[i].intercept();
        
        while (maxIntercept - minIntercept < vertThreshold){ //threshold
            toIndex++;
            i++;
            if (i >= len)
                break;
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
        
        ChessLine newCBLine(cv::Point(avgIntercept,0),
                            cv::Point(avgSlopeI*img.size().height + avgIntercept,img.size().height));
        vertLinesPruned.push_back(newCBLine);
    }
    
#if DEBUG   
    std::cout<<"Vertical lines count: "<<vertLinesPruned.size()<<std::endl;
#endif
    //draw the effing line!
    for (int i=0, len=vertLinesPruned.size(); i<len; i++){
        cv::line(img, vertLinesPruned[i].getPoint(0), vertLinesPruned[i].getPoint(1),
                 cv::Scalar(255,0,0),2,8);
    }
    
    total++;
    if (horizLinesPruned.size() != 9 || vertLinesPruned.size() != 9){
#if DEBUG
        std::cout<<"Bad detection..\n";
        std::cout<<"Accuracy as of now: "<<totalDetected/double(total)<<std::endl;
#endif
        return ERR_BAD_DETECTION;
    }
    
    //Now awesome cr@p begins ..
    std::vector<std::vector<Cell> > cells(8, std::vector<Cell>(8,Cell()));
    
    //the 9*9 corners..
    cv::Point corners[9][9];
    for (int i=0, lenH=horizLinesPruned.size(); i<lenH; i++){
        for (int j=0, lenV=vertLinesPruned.size(); j<lenV; j++){
            cv::Point p =  horizLinesPruned[i] ^ vertLinesPruned[j];
            corners[i][j] = p;
            cv::circle(img, p,4, cv::Scalar(0,255,0),-1);
            //fill up the this->cells
            if (i > 0 && j > 0){
                cells[i-1][j-1].corners[0] = corners[i-1][j-1];
                cells[i-1][j-1].corners[1] = corners[i-1][j];
                cells[i-1][j-1].corners[2] = corners[i][j];
                cells[i-1][j-1].corners[3] = corners[i][j-1];
                
                int left = std::min(corners[i-1][j-1].x,corners[i][j-1].x),
                    right = std::max(corners[i-1][j].x,corners[i][j].x),
                    top = std::min(corners[i-1][j-1].y,corners[i-1][j].y),
                    bottom = std::max(corners[i][j-1].y,corners[i][j].y);
                //the subsection of the image ..
                try{
                    cells[i-1][j-1].image = mainImage(
                        cv::Range(top, bottom+1), //row Range
                        cv::Range(left,right+1)   //col Range
                    );
                }catch(...){
                    return ERR_BAD_DETECTION;
                }
                //subcorners == corners wrt to subimage
                cells[i-1][j-1].subCorners[0] = cv::Point(
                    corners[i-1][j-1].x - left,
                    corners[i-1][j-1].y - top
                );
                cells[i-1][j-1].subCorners[1] = cv::Point(
                    corners[i-1][j].x - left,
                    corners[i-1][j].y - top
                );
                cells[i-1][j-1].subCorners[2] = cv::Point(
                    corners[i][j].x - left,
                    corners[i][j].y - top
                );
                cells[i-1][j-1].subCorners[3] = cv::Point(
                    corners[i][j-1].x - left,
                    corners[i][j-1].y - top
                );
            }
        }
    }
    
    /////////////////
    /*
    in the for loop below .. check if circles exist ..
    if so .. check the channel which is brightest ..
    else .. the cell is empty .. check whther it is b/w..
    */
    //////////////////
    
    //perform detection of color ..
    
    for (int i=0; i<8; i++){
        for (int j=0; j<8; j++){
            cv::Mat bwCircleImg(cells[i][j].image.size(),8);
            cv::cvtColor(cells[i][j].image,bwCircleImg,CV_BGR2GRAY);
            
            //check if circles exist..
            std::vector<cv::Vec3f> circles;
            cv::HoughCircles(bwCircleImg, circles, CV_HOUGH_GRADIENT,
                        3, 200, 200, 100);
            
            if (circles.size()){ //circles exist
                if (circles.size() != 1){
                    std::cout<<"More than 1 circles found at cell: "
                        <<i<<","<<j<<std::endl; 
                    return ERR_BAD_DETECTION;
                }
                //draw all circles..
                /*for (std::vector<cv::Vec3f>::iterator it=circles.begin();
                        it != circles.end(); it++){
                    xbot::Window w;
                    cv::circle(cells[i][j].image, 
                            cv::Point((*it)[0],(*it)[1]),
                            4, cv::Scalar(0,255,0),(*it)[2]);
                    w.showImage(cells[i][j].image);
                    w.waitKey(0);
                }*/
                
                //use the intensity of two channels R and B to settle this..
                enum {CHANNEL_BLUE, CHANNEL_GREEN, CHANNEL_RED};
                
                //prepare mask..
                cv::Mat mask = cv::Mat::zeros(bwCircleImg.size(), CV_8UC1);
                int radius = circles[0][2];
                cv::circle(mask, cv::Point(circles[0][0],circles[0][1]),radius,1,-1);
                
                //split into channels
                std::vector<cv::Mat> channels(3);
                cv::split(cells[i][j].image, channels);
                
                //checking RED Channel
                int resultRed = mask.dot(channels[CHANNEL_RED])/(3.14159*radius*radius);
                
                //checking GREEN Channel
                int resultGreen = mask.dot(channels[CHANNEL_GREEN])/(3.14159*radius*radius);
                
                cells[i][j].type = resultRed > resultGreen? Cell::RED : Cell::GREEN;
            }else{
                //(black n white)
                const int BW_THRESH = 100;
                cv::Mat mask = cv::Mat::zeros(bwCircleImg.size(), CV_8UC1);
                cv::fillConvexPoly(mask, &(cells[i][j].subCorners[0]),4,1);
                
                int result = mask.dot(bwCircleImg)/cells[i][j].area();
                cells[i][j].type = (result <= BW_THRESH)? Cell::BLACK : Cell::WHITE;
            }
        }
    }
    totalDetected++;
    //current hack .. one green coin less that we have..
    cells[7][6].type = Cell::GREEN;
    std::string s;
    for (int i=0; i<8; i++){
        for (int j=0; j<8; j++){
            s.append(1,cells[i][j].type);
            //std::cout<<(char)cells[i][j].type<<" ";
        }
        //std::cout<<std::endl;
    }
    //std::cout<<s<<std::endl;
    (std::cout<<".").flush();
    return s;
}