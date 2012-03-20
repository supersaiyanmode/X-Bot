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

std::string translate(int r, int c){
    if (r==-1 && c==-1)
        return "0 380";
    static int offsetX = atoi(CONFIG["BOARD_OFFSET_X"].c_str());
    static int offsetY = atoi(CONFIG["BOARD_OFFSET_Y"].c_str());
    std::stringstream s;
    std::cout<<"TRANSLATE r"<<r<<" c"<<c<<std::endl;
    
    //bloody origin is on the right! Damn .. why didnt this strike me earlier!..
    s<<((7-c)*25+offsetX)<<" "<<(r*25+offsetY);
    std::cout<<"Result: "<<s.str()<<std::endl;
    return s.str();
}
void tempMove(int r1, int c1, int r2, int c2, std::vector<std::vector<Cell> >& cells){
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
            printf("ERROR connecting");
            return;
        }
    }
    
    std::string ret;
    std::cout<<"Translate: ("<<r1<<","<<c1<<") -> "<<translate(r1,c1)<<std::endl;
    std::getchar();
    ::writeLine(clientSocket,translate(r1,c1));
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
    
    ::writeLine(clientSocket,"DOWN");
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
    
    ::writeLine(clientSocket,"MAGNETON");
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
    
    ::writeLine(clientSocket,"UP");
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
    
    ::writeLine(clientSocket,translate(r2,c2));
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
    
    ::writeLine(clientSocket,"DOWN");
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
    
    ::writeLine(clientSocket, "MAGNETOFF");
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
    
    ::writeLine(clientSocket, "UP");
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;

    ::writeLine(clientSocket, "0 380");
    if (!::readLine(clientSocket,ret)){
        std::cout<<"Socket error!"<<std::endl;
        return;
    }
    std::cout<<ret<<std::endl;
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
            /*while (game.state() != board.state()){
                std::cout<<"Hammer <enter> after doing robot's move as above!: ";
                std::getchar();
            }*/
            
            std::vector<PieceMove> moves(game.moves());
            std::vector<std::vector<Cell> > cells(board.getCells());
            for (std::vector<PieceMove>::iterator it=moves.begin(); it!=moves.end(); it++){
                std::cout<<it->description<<std::endl;
                tempMove(it->r1, it->c1, it->r2, it->c2,cells);
            }
        }
        Thread<CheckersBoard,int>::sleep(sleepTime);
    }
    return 0;
}