#ifndef CHECKERS_TYPES_H
#define CHECKERS_TYPES_H
#include "const.h"

typedef struct state_block {

    unsigned char            board[32];                // Array of pieces, see "tables.cpp" and "const.h"
    int                        piece;                    // The piece that is being considered by a Function.
    int                        turn;                    // The player whose turn it is to move.    
                                                    // Above 3 feilds must be first for copy_STATE().!!!
    int                        value;                    // Used by play() to hold best value move so far.
    long                    clump;                    // Used by end_game_tweak().

    unsigned char            result;                    // Undo/Redo Game Result (NONE, WIN, LOSE or DRAW)
    unsigned char            jump_start_square;        // Undo/Redo compulsory jump start square
    bool                    jump_found;                // Undo/Redo jump found flag
    bool                    game_over;                // Undo/Redo game over flag

    struct state_block       *next;                    // Pointer that is used to chain free list.
                                                    // Must be last feild in STATE for
} STATE, *STATE_PTR;                                // copy_STATE() to work !!!

#define  COPY_STATE_LENGTH_COMPLETE   sizeof(STATE) - sizeof(STATE_PTR)
#define  COPY_STATE_LENGTH_PLAY          sizeof(unsigned char) * 32 + sizeof(int) + sizeof(int)


typedef struct sucessor_block {
    
    STATE_PTR                ptr[STATE_MAX];            // Array of pointers to STATE.
    unsigned char            num;                    // Number of STATES in SUCESSOR.

    struct sucessor_block  *next;                    // Pointer that is used to chain free list.

} SUCESSOR, *SUCESSOR_PTR;


typedef struct piece_block {                        // Optimization Variables. See "tables.cpp"
    
    unsigned char   mover_piece;
    unsigned char   mover_king;
    unsigned char   opponent_piece;
    unsigned char   opponent_king;

} PIECE_NOS, *PIECE_NOS_PTR;


#endif