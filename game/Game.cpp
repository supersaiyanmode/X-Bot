#include <iostream>
#include <vector>
#include <cmath>
#include "types.h"
#include "Game.h"

int setPlayerMouseDown(int);
char* getComputerState(); //State after computer plays, or kings player piece.
char* getPlayerState(); //State before the above..
void new_game();
char* getValue();
bool computerJumped();

std::ostream& operator<<(std::ostream& o, const std::vector<PieceMove>& moves){
    o<<"Moves--\n";
    for (std::vector<PieceMove>::const_iterator it=moves.begin(); it!= moves.end(); it++){
        o<<it->description<<" : ("<<it->r1<<","<<it->c1<<") -> ("<<it->r2<<","
            <<it->c2<<")"<<std::endl;
    }
    o<<"--\n";
    return o;
}

void CheckersGame::stateStringToArray(const std::string& s, char **arr){
    int cur=0;
    for (int i=0; i<8; i++)
        for (int j=0; j<8; j++)
            arr[i][j] = s[cur++];
}

bool CheckersGame::getNeighbour(Neighbour n, int row, int col, int& dest_r, int& dest_c){
    switch (n){
        case NORTH_WEST:
            if (row<7 && col>0){
                dest_r = row+1;
                dest_c = col-1;
                return true;
            }
            return false;
        case NORTH_EAST:
            if (row<7 && col<7){
                dest_r = row+1;
                dest_c = col+1;
                return true;
            }
            return false;
        case SOUTH_EAST:
            if (row>0 && col<7){
                dest_r = row-1;
                dest_c = col+1;
                return true;
            }
            return false;
        case SOUTH_WEST:
            if (row>0 && col>0){
                dest_r = row-1;
                dest_c = col-1;
                return true;
            }
            return false;
    }
    return false;
}

CheckersGame::CheckersGame(){
    new_game();
    curState = getComputerState();
}

std::string CheckersGame::state(){
    return curState;
}

std::string CheckersGame::value(){
    return getValue();
}

void CheckersGame::display(){
    int cur=0;
    for (int i=0; i<8; i++){
        for (int j=0; j<8; j++){
            std::cout<<curState[cur++]<<" ";
        }
        std::cout<<std::endl;
    }
}

int CheckersGame::nextMove(int from, int to){
    if (setPlayerMouseDown(from))
        return 0;
    if (setPlayerMouseDown(to))
        return 0;
    prevState = getPlayerState();
    curState = getComputerState();
    
    return 1;
}

std::vector<PieceMove> CheckersGame::moves(){
    std::vector<PieceMove> ret;
    
    //find any player coin to be kinged..
    for (int i=0; i<8; i++){
        //check the top row, 0th row
        if (curState[i] == 'P' && prevState[i] == 'p'){
            PieceMove pOut, pIn;
            pOut.piece = 'p';
            pOut.r1 = 0;
            pOut.c1 = i;
            pOut.r2 = -1;
            pOut.c2 = -1;
            pOut.description = "Player kinging - out";
            ret.push_back(pOut);
            
            pOut.piece = 'P';
            pOut.r1 = -1;
            pOut.c1 = -1;
            pOut.r2 = 0;
            pOut.c2 = i;
            pOut.description = "Player kinging - in";
            ret.push_back(pIn);
            break;
        }
    }
    
    //find the computer-coin that has changed
    int coin_initial_r=-1,coin_initial_c=-1;
    int coin_final_r=-1,coin_final_c=-1;
    char coin_initial;
    
    //find coin that disappeared..
    for (int i=0; i<64; i++){
        if (curState[i] == '.' && (prevState[i] == 'c' || prevState[i] == 'C')){
            coin_initial = prevState[i];
            coin_initial_r = i/8;
            coin_initial_c = i%8;
            break;
        }
    }
    if (coin_initial_r == -1 && coin_initial_c == -1)
        return ret;
    
    //find coin that appeared..
    for (int i=0; i<64; i++){
        if (prevState[i] == '.' && (curState[i] == 'c' || curState[i] == 'C')){
            coin_final_r = i/8;
            coin_final_c = i%8;
            break;
        }
    }
    if (coin_final_r == -1 && coin_final_c == -1)
        return ret;
    
    int temp_r, temp_c;
    while (coin_final_c != coin_initial_c && coin_final_r != coin_initial_r){
        //Non-Jump move?
        if (std::abs(coin_final_c - coin_initial_c) == 1 && 
                std::abs(coin_final_r - coin_initial_r) == 1){
            PieceMove p;
            p.r1 = coin_initial_r;
            p.c1 = coin_initial_c;
            p.r2 = coin_final_r;
            p.c2 = coin_final_c;
            p.piece = coin_initial;
            p.description = "Piece move";
            ret.push_back(p);
            coin_initial_r = coin_final_r;
            coin_initial_c = coin_final_c;
        }else{
            PieceMove pOut, pMove;
            int mid_r, mid_c, jump_r, jump_c;
            bool found = false;
            //check north-west
            Neighbour array[] = {NORTH_WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST};
            
            for (int i=0; i<4 && !found; i++){
                if (getNeighbour(array[i], coin_initial_r, coin_initial_c, temp_r, temp_c)
                        && prevState[temp_r*8 + temp_c] != curState[temp_r*8 + temp_c]){
                    mid_r = temp_r;
                    mid_c = temp_c;
                    if (getNeighbour(NORTH_WEST, temp_r,temp_c, temp_r, temp_c)){
                        jump_c = temp_c;
                        jump_r = temp_r;
                        found = true;
                    }
                }
            }
                                
            pOut.r1 = mid_r;
            pOut.c1 = mid_c;
            pOut.r2 = -1;
            pOut.piece = prevState[mid_r*8 + mid_c];
            pOut.c2 = -1;
            pOut.description = "Piece capture";
            ret.push_back(pOut);
            
            pMove.piece = coin_initial;
            pMove.r1 = coin_initial_r;
            pMove.c1 = coin_initial_c;
            pMove.r2 = jump_r;
            pMove.c2 = jump_c;
            pMove.description = "Piece Move";
            ret.push_back(pMove);
            
            coin_initial_c = jump_c;
            coin_initial_r = jump_r;
        }
        //check for kinging of piece on initial variables..
        //check the bottom row, 7th row
        if (coin_initial_r == 7 && coin_initial == 'c'){
            PieceMove pOut, pIn;
            pOut.piece = 'c';
            pOut.r1 = 7;
            pOut.c1 = coin_initial_c;
            pOut.r2 = -1;
            pOut.c2 = -1;
            pOut.description = "Computer kinging - out";
            ret.push_back(pOut);
            
            pOut.piece = 'C';
            pOut.r1 = -1;
            pOut.c1 = -1;
            pOut.r2 = 7;
            pOut.c2 = coin_initial_c;
            pOut.description = "Computer kinging - in";
            ret.push_back(pIn);
            
            coin_initial = 'C';
        }
    }
    return ret;        
}

void CheckersGame::destroy(){
    
}