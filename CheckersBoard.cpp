#include <cmath>
#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdio>
#include "CheckersBoard.h"
#include "Window.h"
#include "Thread.cpp"
#include "Config.h"

#define DEBUG 0
//############CHESSBOARD###############
#define ERR_BAD_DETECTION "BAD DETECTION"

int CheckersBoard::total=0;
int CheckersBoard::totalDetected=0;

CheckersBoard::CheckersBoard(int cam):camera(cam){
    
}

/*
 * Returns the state as a string **STABLE/RELIABLE**
 * 
 * NOTE: Modify this function to have a separate thread for detection.
 * And may be a query for the states can check to see if last NUM captures
 * are same and return it, else wait till they are same.
 */

std::string CheckersBoard::state(){
    static size_t RELIABLE_NUM = atoi(CONFIG["RELIABLE_DETECTION_COUNT"].c_str());
    static int sleepTimeOk = atoi(CONFIG["CAPTURE_OK_SLEEP_TIME"].c_str());
    static int sleepTimeErr = atoi(CONFIG["CAPTURE_ERROR_SLEEP_TIME"].c_str());
    static int lTrim = atoi(CONFIG["IMAGE_LTRIM"].c_str());
    static int rTrim = atoi(CONFIG["IMAGE_RTRIM"].c_str());
    static int tTrim = atoi(CONFIG["IMAGE_TTRIM"].c_str());
    static int bTrim = atoi(CONFIG["IMAGE_BTRIM"].c_str());
    
    std::vector<std::string> states;
    while (1){
        std::string res;
        cv::Mat img;
        do{
            img = camera.grab();
            img = img(cv::Range(tTrim, img.size().height - bTrim), //row Range
                        cv::Range(lTrim,img.size().width - rTrim)   //col Range
                    );
            
            res = analyse(img);
            window.showImage(img);
            
            int sleep=res.length()==64? sleepTimeOk:sleepTimeErr;
            //window.waitKey(sleep);
            while (sleep > 0){ //new busy-capture-wait
                //window.showImage(camera.grab());
                camera.grab();
                window.waitKey(10);
                sleep -= 10;
            }
        }while (res.length() != 64);
        
        //add to the bag
        states.push_back(res);
        if (states.size() > (unsigned)RELIABLE_NUM)
            states.erase(states.begin());
        
        //check for the similarity
        bool same = true;
        for (size_t i=0; i<states.size() -1 && same; i++){
            if (states[i] != states[i+1]) 
                same = false;
        }
        if (same && states.size() == RELIABLE_NUM)
            return res;
    }
    return "";
}

/*
 * bool getMove(curState, [out] move)
 * Figures out what the user has moved by comparing 
 *      curState        and     this->state()
 */

bool CheckersBoard::getMove(std::string curState, MoveData& move){
    std::cout<<"Checking for player move...";
    std::string newState = state();
    if (newState.length() != 64){
        std::cout<<"[Bad new state]\n";
        return false;
    }
    
    size_t pos=-1;
    move.r1 = -1;
    
    //polls changes for pP coins ..
    while ((pos=curState.find_first_of("pP",pos+1))!=std::string::npos){
        if (curState[pos] != newState[pos]){
            std::cout<<"Found inequality at: "<<pos<<std::endl;
            move.r1 = pos/8;
            move.c1 = pos%8;
            move.pos1 = cells[move.r1][move.c1].centroid();
            break;
        }
    }
    if (move.r1 == -1){
        std::cout<<"No change!\n";
        return false;
    }
    pos = -1;
    while ((pos=curState.find_first_of(".",pos+1))!=std::string::npos){
        if (curState[pos] != newState[pos]){
            std::cout<<"Found inequality at: "<<pos<<std::endl;
            move.r2 = pos/8;
            move.c2 = pos%8;
            move.pos2 = cells[move.r1][move.c1].centroid();
            break;
        }
    }
    if (move.r2 == -1){
        std::cout<<"Cant find piece that appeared.\n";
        return false;
    }
    std::cout<<"[Got!]\n";
    return true;
}


/*
 * Heart of image processing...
 * Takes an image, processes the lines, and returns the stringised form
 * 
 * Steps:
 *      1. Convert to Gray
 *      2. Apply Canny
 *      3. HoughLines Transform
 *      4. Ignore noisy lines on the slope criteria
 *      5. Group similiar lines separately as horizontal and vertical lines
 *      6. Average out the lines in the group
 *      7. Check if there are 9 horiz and 9 vert lines
 *      8. Make out 81 corners
 *      9. Using that make out 64 cells.
 *      10. For each cell:
 *              i.      Apply hough circle transform
 *              ii.     If circle exists:
 *                      - Draw with intensity '1' a circle with the radius and center found
 *                      - Take dot product and divide by pi*r*r to get avg. intensity
 *                      - Compare this intensity
 *              ii.     Else:
 *                      - Draw a polygon of the corners of the cell.
 *                      - Dot it with original image and divide by area
 *                      - Compare for black/white cell
 *      11. Make the string
 * 
 */
std::string CheckersBoard::analyse(cv::Mat img){
    //static xbot::Window w;
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
    
    //img = m;
    //return 0;
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(m,   //the image
                    lines, //out - vector of vec4i ..
                    0.5, //linear accuracy of the accumulator
                    CV_PI/180.0, //radial accuracy
                    20,  //threshold
                    200, //min line gap
                    30 //max breakage in the line .. along the line .. to join them
                );
    
    std::vector<CheckersLine> horizLines, vertLines, horizLinesPruned, vertLinesPruned;
    for (int i=0, len=lines.size(); i<len; i++){
        cv::Point p1(lines[i][0],lines[i][1]),p2(lines[i][2],lines[i][3]);
        CheckersLine cline(p1,p2);
        //(cline.horizontal()?horizLines:vertLines).push_back(cline);
        
        //A cleaner fail proof method. Avoids random noisy lines..
        if (cline.slope() > -0.5 && cline.slope()<0.5)
            horizLines.push_back(cline);
        else if (cline.slopeI() > -0.5 && cline.slopeI() < 0.5)
            vertLines.push_back(cline);
        //else
            //std::cout<<"Too much noise .. :-| "<<std::endl;
    }
    
    for (int i=0, len=horizLines.size();i<len; i++){
        //draw the effing line!
        cv::Point p1(horizLines[i].getPoint(0)),p2(horizLines[i].getPoint(1));
        cv::line(img, p1, p2, cv::Scalar(255,0,255),2,8);
    }
    for (int i=0, len=vertLines.size();i<len; i++){
        //draw the effing line!
        cv::Point p1(vertLines[i].getPoint(0)),p2(vertLines[i].getPoint(1));
        cv::line(img, p1, p2, cv::Scalar(255,0,255),2,8);
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
        
        
        CheckersLine newCBLine(cv::Point(0,avgIntercept),
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
        
        CheckersLine newCBLine(cv::Point(avgIntercept,0),
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
    cells = std::vector<std::vector<Cell> >(8, std::vector<Cell>(8,Cell()));
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
                
                cells[i-1][j-1].row = i-1;
                cells[i-1][j-1].column = j-1;
                
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
                //checking BLUE Channel
                int resultBlue = mask.dot(channels[CHANNEL_BLUE])/(3.14159*radius*radius);
                
                int maxColor = std::max(std::max(resultBlue,resultGreen),resultRed);
                
                if (maxColor == resultRed)              cells[i][j].type =  Cell::RED;
                else if (maxColor == resultGreen)       cells[i][j].type =  Cell::GREEN;
                else if (maxColor == resultBlue)        cells[i][j].type =  Cell::BLUE;
                
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
    
    static std::map<Cell::CellType, char> tempMap;
    
    if (!tempMap.size()){
        tempMap[Cell::BLACK] = ' ';
        tempMap[Cell::WHITE] = '.';
        
        if (CONFIG["COLOR_PLAYER"]=="RED") tempMap[Cell::RED] = 'p';
        else if (CONFIG["COLOR_PLAYER"]=="GREEN") tempMap[Cell::GREEN] = 'p';
        else if (CONFIG["COLOR_PLAYER"]=="BLUE") tempMap[Cell::BLUE] = 'p';

        if (CONFIG["COLOR_COMPUTER"]=="RED") tempMap[Cell::RED] = 'c';
        else if (CONFIG["COLOR_COMPUTER"]=="GREEN") tempMap[Cell::GREEN] = 'c';
        else if (CONFIG["COLOR_COMPUTER"]=="BLUE") tempMap[Cell::BLUE] = 'c';
    }
    std::string s;
    for (int i=0; i<8; i++){
        for (int j=0; j<8; j++){
            char cur = tempMap[cells[i][j].type];
            if (cur == 'p' || cur == 'c' || cur == ' ' || cur == '.')
                s.append(1,cur);
            else{
                std::cout<<"Got cell type: "<<(char)cells[i][j].type<<std::endl;
                std::cout<<"In map: "<<tempMap[cells[i][j].type]<<std::endl;
                std::cout<<"Invalid state encountered. Color recognition failed."<<std::endl;
                return "";
            }
        }
    }
    (std::cout<<".").flush();
    return s;
}

std::vector<std::vector<Cell> > CheckersBoard::getCells(){
    return cells;
}

void CheckersBoard::destroy(){
    //check for cv::Mat memory leak.
}
