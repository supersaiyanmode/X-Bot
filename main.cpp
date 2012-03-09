#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include "CheckersBoard.h"
#include "Server.h"
#include "game/Game.h"

CheckersBoard board(1);
CheckersGame game;

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
    Server s(44101);
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
    while (1){
        if (board.getMove(game.state(), r1,c1,r2,c2)){
            std::cout<<"Params: "<<r1<<" "<<c1<<" "<<r2<<" "<<c2<<std::endl;
            //5 2 4 3
            r1 = 7-r1;
            r2 = 7-r2;
            int from = 8*((r1+1)/2) + c1/2, to = 8*((r2+1)/2) + c2/2;
            std::cout<<"Making move: "<<from<<" -> "<<to<<std::endl;
            game.nextMove(from,to);
            game.display();
            while (game.state() != board.state()){
                std::cout<<"Hammer <enter> after doing robot's move as abov!: ";
                std::getchar();
            }
        }
        Thread<CheckersBoard,int>::sleep(500);
    }
    return 0;
}