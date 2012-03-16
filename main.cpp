#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <signal.h>
#include "CheckersBoard.h"
#include "Server.h"
#include "Arm.h"
#include "game/Game.h"
#include "Config.h"
#include "SocketUtils.h"
#include <cstdlib>
#include <cstring>

Config CONFIG("CONFIG");
CheckersBoard board(atoi(CONFIG["CAMERA_NUM"].c_str()));
CheckersGame game;
Server server(atoi(CONFIG["SERVER_PORT"].c_str()));
Arm arm;

void tempMove(int x, int y){
    static int clientSocket = -1;
    if (clientSocket == -1){
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serv_addr;
        std::memset((char*)&serv_addr, 0, sizeof(serv_addr));
        
        hostent *s= gethostbyname("192.168.1.100");
        if (s == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            return;
        }
        serv_addr.sin_family = AF_INET;
        std::memcpy((char *)&serv_addr.sin_addr.s_addr,(char *)s->h_addr, s->h_length);
        serv_addr.sin_port = htons(21739);
        if (::connect(clientSocket,(sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
            printf("RROR connecting");
            return;
        }
    }
    
    ::writeLine(clientSocket,"motor1 0 motor2 0");
}

void exit(){
    //destroy explicitly (writes to CONFIG etc etc ..)
    board.destroy();
    game.destroy();
    arm.destroy();
    CONFIG.destroy();
    exit(0);
}
void sigInterrupt(int sig){
    std::cout<<"Caught Signal: "<<sig<<std::endl;
    exit();
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
    tempMove(0,0);
    
    signal(SIGINT, sigInterrupt);
    
    server.setCallback(checkerStatus);
    server.start();
    
    //hope the server starts up.. dont want mixed crap on stdout
    Thread<CheckersBoard,int>::sleep(100); 
    
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
    int sleepTime = atoi(CONFIG["USER_MOVE_POLL_SLEEP_TIME"].c_str());
    while (1){
        MoveData move;
        if (board.getMove(game.state(), move)){
            std::cout<<"Params: "<<move.r1<<" "<<move.c1<<" "<<move.r2<<" "<<move.c2<<std::endl;
            //5 2 4 3
            move.r1 = 7-move.r1;
            move.r2 = 7-move.r2;
            int from = 4*move.r1 + move.c1/2, to = 4*move.r2 + move.c2/2;
            std::cout<<"Making move: "<<from<<" -> "<<to<<std::endl;
            if (!game.nextMove(from,to)){
                std::cout<<"Error move!!!\n";
            }
            std::cout<<"State after my move..\n";
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