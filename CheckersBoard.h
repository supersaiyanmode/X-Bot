#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <algorithm>
#include "opencv/cv.h"
#include "Window.h"
#include "Camera.h"
#include "CheckersLine.h"
#include "Cell.h"


/*
 * Struct MoveData
 * contains:
 *      r1, c1, r2, c2 - The indices of the cells containing the current move.
 *      pos1, pos2:     The centroid of the cells.
 */
struct MoveData {
    int r1, c1, r2, c2;
    cv::Point pos1, pos2;
};

/*
 * Class CheckersBoard
 * contains:
 *      camera - xbot::Camera object
 *      window - xbot::Window Object, a simple OpenCV window
 *      cells - 8x8 cells matrix
 *      analyse() - returns a stringised (64 bytes) form of the state
 *      state() - Calls analyse() many times, to return  a *stable* state
 *      getMove() - Compares two string states to return a moveData
 */

class CheckersBoard{
    static int totalDetected, total;
    xbot::Camera camera;
    xbot::Window window;
    std::vector<std::vector<Cell> > cells;//(8, std::vector<Cell>(8,Cell()));
    
    std::string analyse(cv::Mat);
public:
    CheckersBoard(int);
    std::string state();
    bool getMove(std::string, MoveData&);
    std::vector<std::vector<Cell> > getCells();
    void destroy();
};

#endif
