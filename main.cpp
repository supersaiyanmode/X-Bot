#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <signal.h>
#include "CheckersBoard.h"
#include "Server.h"
#include "game/Game.h"
#include "Config.h"

CheckersBoard board(atoi(CONFIG["CAMERA_NUM"]));
CheckersGame game;

void sigInterrupt(int sig){
    std::cout<<"Signal: "<<sig<<std::endl;
    exit(0);
}

void displayState(std::string s){
    if (s.length() != 64)
        std::cout<<"<BAD STATE!>"<<std::endl;
    else{
        int pos = 0;
        for (int i=0; i<8; i++){
            for (int j=0; j<8; j++){
                std::cout<<s[pos++]<<" ";
            }
            std::cout<<std::endl;
        }
    }
}
std::string checkerStatus(){
    return board.state();
}

int main(int argc, char **argv){
    Server s(atoi(CONFIG["SERVER_PORT"]));
    s.setCallback(checkerStatus);
    s.start();
    
    std::string boardState;
    
    std::cout<<"Game State:\n";
    displayState(game.state());
    std::cout<<"BoardState:\n";
    displayState(boardState = board.state());
    //assert game and camera have same initial state
    while (game.state() != boardState){
        std::cout<<"Different states.\n";
        std::cout<<"Game State: "<<game.state()<<std::endl;
        std::cout<<"BoardState: "<<boardState<<std::endl;
        std::cout<<"Hit <enter> (with a hammer) after changing!: ";
        std::getchar();
        boardState = board.state();
    }
    //game.display();
    int r1,c1,r2,c2;
    int sleepTime = atoi(CONFIG["USER_MOVE_POLL_SLEEP_TIME"]);
    while (1){
        if (board.getMove(game.state(), r1,c1,r2,c2)){
            std::cout<<"Params: "<<r1<<" "<<c1<<" "<<r2<<" "<<c2<<std::endl;
            //5 2 4 3
            r1 = 7-r1;
            r2 = 7-r2;
            int from = 4*r1 + c1/2, to = 4*r2 + c2/2;
            std::cout<<"Making move: "<<from<<" -> "<<to<<std::endl;
            game.nextMove(from,to);
            game.display();
            while (game.state() != board.state()){
                std::cout<<"Hammer <enter> after doing robot's move as above!: ";
                std::getchar();
            }
        }
        Thread<CheckersBoard,int>::sleep(sleepTime);
    }
    return 0;
}