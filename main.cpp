#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "CheckersBoard.h"
#include "Server.h"
#include "game/Game.h"

CheckersBoard board(1);
CheckersGame game;

std::string checkerStatus(){
    return board.str();
}

int main(int argc, char **argv){
    Server s(44101);
    s.setCallback(checkerStatus);
    s.start();
    board.startDetection();
    //game.display();
    int r1,c1,r2,c2;
    while (1){
        if (board.pollMove(r1,c1,r2,c2)){
            int from = 8*(r1/2) + c1/2, to = 8*(r2/2) + c2/2;
            std::cout<<"Making move: "<<from<<" -> "<<to<<std::endl;
            //game.nextMove(from,to);
            //game.display();
        }
        Thread<CheckersBoard,int>::sleep(500);
    }
    return 0;
}