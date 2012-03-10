#ifndef GAME_H
#define GAME_H

enum Neighbour {NORTH_WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST};

struct PieceMove{
    int r1,c1,r2,c2;
    char piece; //p P c C
    std::string description;
};

std::ostream& operator<<(std::ostream&, const std::vector<PieceMove>&);

class CheckersGame{
    std::string curState,prevState;
    void stateStringToArray(const std::string&, char **);
    
    bool getNeighbour(Neighbour, int, int, int&, int&);
public:
    CheckersGame();
    std::string state();
    std::string value();
    void display();
    
    void nextMove(int, int);
    
    std::vector<PieceMove> moves();
};

#endif