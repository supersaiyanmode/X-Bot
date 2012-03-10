#include "const.h"
#include "types.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string.h>

STATE                   move;                   // used to pass STATE to play() thread.
int                     search_depth;           // used to pass search depth to play() thread.

SUCESSOR_PTR            q;                      // play() thread returns alternative moves here.
STATE_PTR               free_STATE    = NULL;   // linked list of free STATEs.
SUCESSOR_PTR            free_SUCESSOR = NULL;   // linked list of free SUCESSORs.

STATE_PTR               prev_STATE    = NULL;   // linked list of STATEs for "Undo".
STATE_PTR               next_STATE    = NULL;   // linked list of STATEs for "Redo".

SUCESSOR_PTR            r             = NULL;   // currently displayed move alternatives.

STATE                   move_old;        // holds previous STATE for "Show Move" button. 
STATE                   move_new;        // holds currently displayed STATE.
STATE                   move_computer;   // holds move made by computer
STATE                   move_player;     // holds move made by player, esp. before kinging action..

STATE                   abort_restore;
STATE                   abort_restore_old;

STATE                   start;
STATE                   start_undo;

unsigned char           default_piece                = NO_PIECE;
unsigned char           search_depth_max             = 10;
unsigned char           mouse_over_square            = 32;
unsigned char           compulsory_jump_start_square = 32;

char                    text_value[16]    = "Value(0)";

bool                    first_move              = true;
bool                    game_over               = false;
bool                    compulsory_jump_found   = false;
bool                    avoid_suicide_called    = false;
bool                    computer_jumped         = false;


bool                    equal_STATE(STATE_PTR p, STATE_PTR q);

void                    new_game(void);
void                    push_STATE(STATE_PTR *r, STATE_PTR q);
void                    refresh_STATE(STATE_PTR p);
void                    mouse_move(unsigned char i);
void                    make_computer_play(void);
void                    display_computer_move(void);
void                    post_undo_redo(void);

extern void             play(void *dummy);
extern SUCESSOR_PTR     make_SUCESSOR(STATE_PTR p);
extern SUCESSOR_PTR     make_single_step_SUCESSOR(STATE_PTR p);
extern SUCESSOR_PTR     consider_jumps(STATE_PTR p);
extern STATE_PTR        new_STATE(void);
extern void             delete_SUCESSOR(SUCESSOR_PTR q);

extern void             copy_STATE(STATE_PTR p, STATE_PTR q);
extern unsigned int     copy_STATE_length;

extern bool             jump_found;
extern bool             jump_found_player;

bool computerJumped(){
    return computer_jumped;
}
char* getValue(){
    return text_value;
}
char* getState(STATE_PTR p){
    static char array[65] = {0};
    memset(&array[0], ' ', 64);
    array[64] = 0;
    
    for (int iter= 0; iter<32; ++iter) {
        int row = iter/4, col = 2*(3-iter%4) + (row%2==1?1:0);
        char cur='.';
        switch (p->board[iter]) {
        case PLAYER_PIECE:
            cur = 'p';
            break;
            
        case PLAYER_KING:
            cur = 'P';
            break;
        case COMPUTER_PIECE:
            cur = 'c';
            break;

        case COMPUTER_KING:
            cur = 'C';
            break;
        case NO_PIECE:
            break;
        }
        array[(7-row)*8 + col] = cur;
    }
    return array;
}
char* getPlayerState(){
    return getState(&move_player);
}
char* getComputerState(){
    return getState(&move_computer);
}


void playDone(){
    STATE_PTR p;
    if (!avoid_suicide_called &&
        q->num > 1 && q->ptr[0]->value < -1000 && search_depth != 2) {
        delete_SUCESSOR(q);
        avoid_suicide_called = true;
        search_depth = 2;    // Search Depth of 2 wont commit suicide.
        play((void*)0);
        playDone();
    } 
    else {
        if (avoid_suicide_called)
            search_depth = search_depth_max;
        display_computer_move();
    }
}

void setPlayerMouseDown(int i){
    int j;
    
    i = (i/4)*4+(3-(i%4)); // i is board position clicked in. See "tables.cpp"

    
    switch(move_new.board[i]) {
    case PLAYER_PIECE:
    case PLAYER_KING:
        if (default_piece != NO_PIECE ) {
            std::cout<<"Error 1: Default piece is not NO_PIECE"<<std::endl;
        }
        if (!compulsory_jump_found) {
            mouse_move(i);
            if (move_new.board[i] == PLAYER_PIECE ||
                move_new.board[i] == PLAYER_KING) {
                std::cout<<"Playing chord.wav"<<std::endl;
                break;
            }
        } else {
            std::cout<<"Error 2: Possibly entering non-compulsory jump.\n";
            std::cout<<"Playing chord.wav"<<std::endl;
            break;
        }
    case HI_LIGHTED_PLAYER_PIECE:
    case HI_LIGHTED_PLAYER_KING:
        default_piece = move_new.board[i];
        move_new.board[i] = HI_LIGHTED_NO_PIECE;
        break;
    case HI_LIGHTED_END_SQUARE:
        default_piece = NO_PIECE;
        for (j=0; j<(int)r->num; ++j) {
            if (r->ptr[j]->piece == i) {
                copy_STATE(r->ptr[j], &move_new);
                copy_STATE(&move_new, &move_player);
                break;
            }
        }
        make_computer_play();
        break;
    case HI_LIGHTED_MID_SQUARE:
        default_piece = NO_PIECE;
        for (j=0; j<(int)r->num; ++j) {
            if (r->ptr[j]->piece - 128 == i) {
                copy_STATE(r->ptr[j], &move_new);
                break;
            }
        }
        mouse_move(i);
        playDone();
        break;
                            
    case HI_LIGHTED_NO_PIECE:
        move_new.board[i] = default_piece;
        default_piece = NO_PIECE;
        break;

    default:
        std::cout<<"Error 3: Invalid move. No piece!!"<<std::endl;
        break;
    }
}

bool equal_STATE(STATE_PTR p, STATE_PTR q){
    for (int i = 0; i<32; ++i)
            if (p->board[i] != q->board[i])
                return false;
    return true;
}

void new_game(void){
    srand((unsigned)time(NULL));
    
    int i;
    STATE_PTR p;search_depth_max = 10;

    for (i = 0; i<12; ++i)
        move.board[i] = PLAYER_PIECE;
    for (i = 12; i<20; ++i)
        move.board[i] = NO_PIECE;
    for (i = 20; i<32; ++i)
        move.board[i] = COMPUTER_PIECE;

    move.turn = PLAYER_PIECE;
    move.result = NONE;
    move.value = 0;
    move.jump_start_square = 32;
    
    copy_STATE_length = COPY_STATE_LENGTH_COMPLETE;

    copy_STATE(&move, &move_new);
    copy_STATE(&move, &move_old);
    copy_STATE(&move, &abort_restore);
    copy_STATE(&move, &abort_restore_old);
    copy_STATE(&move, &start);
    copy_STATE(&move, &start_undo);
    copy_STATE(&move, &move_computer);

    while (prev_STATE != NULL) {
        p = prev_STATE;
        prev_STATE = prev_STATE->next;
        delete(p);
    }

    push_STATE(&prev_STATE, &start_undo);

    search_depth = search_depth_max;
    
    strcpy(text_value, "Value(0)");
    
    first_move = true;
    default_piece = NO_PIECE;
    game_over = false;
    jump_found_player = false;
    compulsory_jump_found = false;
    
    if (r != NULL)
        delete_SUCESSOR(r);
    r = NULL;
}


//--------------------------------------------------------------------------------
//    void refresh_STATE(STATE_PTR p)        get state ready for play() thread
//--------------------------------------------------------------------------------

void refresh_STATE(STATE_PTR p)
{
    
    int i;
    
    for (i=0; i<32; ++i) {
        if (p->board[i] > HIGHEST_VALID_PIECE) {
            switch(p->board[i]) {

            case HI_LIGHTED_PLAYER_PIECE:
                
                p->board[i] = PLAYER_PIECE;
                break;
            
            case HI_LIGHTED_PLAYER_KING:
                
                p->board[i] = PLAYER_KING;
                break;
            
            case HI_LIGHTED_NO_PIECE:
            case HI_LIGHTED_END_SQUARE:
            case HI_LIGHTED_MID_SQUARE:
            case COMPUTER_JUMPED_SQUARE:
            case COMPUTER_START_SQUARE:
                
                p->board[i] = NO_PIECE;
                break;
            
            case COMPUTER_END_SQUARE_PIECE:
                
                p->board[i] = COMPUTER_PIECE;
                break;
            
            case COMPUTER_END_SQUARE_KING:
                
                p->board[i] = COMPUTER_KING;
                break;
            
            default:
                
                exit(1);
            }
        }
    }
}


//--------------------------------------------------------------------------------
//    void mouse_move(unsigned int)
//--------------------------------------------------------------------------------
//
//    Inputs:        STATE            move_new;    The state the mouse is over
//    ======        unsigned int    i;            The squaure in state the mouse is over
//
//    Outputs:    STATE            move_new;    board[] contains the mouse over info for i.
//    =======                                    piece is set to i.
//                SUCESSOR_PTR    r;            r->ptr[x] is single step move when mouse
//                                            is clicked over end or mid jump square x.
//                                            r is set for WM_LBUTTONDOWN code.
//
//--------------------------------------------------------------------------------

void mouse_move(unsigned char i)
{
    unsigned char j, k;

    refresh_STATE(&move_new);

    if (i<32) {         
                        
        switch(move_new.board[i]) {
        
        case PLAYER_PIECE:
        case PLAYER_KING:
        
            move_new.piece = i;
            
            if (r != NULL)
                delete_SUCESSOR(r);
            r = make_single_step_SUCESSOR(&move_new);        // r is Global Variable
            
            if (r != NULL) {
                for ( j=0; j<r->num; ++j)
                    if ((k = r->ptr[j]->piece) > 127)
                        move_new.board[k-128] = HI_LIGHTED_MID_SQUARE;
                    else 
                        move_new.board[k] = HI_LIGHTED_END_SQUARE;
                                    
                if (move_new.board[i] == PLAYER_PIECE)
                    move_new.board[i] = HI_LIGHTED_PLAYER_PIECE;
                else if (move_new.board[i] == PLAYER_KING)
                    move_new.board[i] = HI_LIGHTED_PLAYER_KING;
            }
                            
        }
    
    }
        
}

void make_computer_play(){
    int i;
    STATE_PTR p, newp;
    
    refresh_STATE(&move);
    q = make_SUCESSOR(&move);
    
    if (first_move) {
        newp = new_STATE();
        copy_STATE(&move, newp);
        q->ptr[q->num] = newp;
        ++(q->num);
    }
    for (i = 0; i<(int)q->num; ++i)
        if (equal_STATE(q->ptr[i], &move_new))
            break;
    if (i<(int)q->num) {    // Legal move entered
            
        delete_SUCESSOR(q);

        avoid_suicide_called = false;
        game_over = false;
            
        copy_STATE(&move, &move_old);
        copy_STATE(&move_new, &move);

        move.turn = COMPUTER_PIECE;

        while (next_STATE != NULL) {
            p = next_STATE;
            next_STATE = next_STATE->next;
            delete(p);
        }
        
        push_STATE(&prev_STATE, &move_new);

        std::cout<<"making computer play..."<<std::endl;
        play((void*)0);
        playDone();
    } else {    // illegal move entered, 
                // can only be when first_move == true and Play button pressed
                // and the user has selected a PIECE and not clicked an end square.
            
        delete_SUCESSOR(q);
        copy_STATE(&move, &move_new);

        std::cout<<"Playing Chord.wav"<<std::endl;
    }
}


//--------------------------------------------------------------------------------
//    void display_computer_move(void)        code called after computer has moved.
//--------------------------------------------------------------------------------

void display_computer_move(void)
{
    int i, result;
    
    result = false;
    compulsory_jump_found = false;        
    first_move = false;
            
    copy_STATE(&move_new, &move_old);
    if (q->num > 0) {
        copy_STATE(q->ptr[rand() % q->num], &move);
        move.turn = PLAYER_PIECE;
        move.game_over = false;
        delete_SUCESSOR(q);
        
        if (avoid_suicide_called) {
            strcpy(text_value, "You Win !!!");
            move.result = LOSE;
            result = true;
        } else if (move.value > 1000) {
            search_depth -= 2;
            strcpy(text_value, "I Win !!!");
            move.result = WIN;
            result = true;
        } else if (move.value < -1000) {
            strcpy(text_value, "You Win !!!");
            move.result = LOSE;
            result = true;
        } else {
            sprintf(text_value, "Value(%d)", -move.value);
            move.result = NONE;
        }
                
        q = make_SUCESSOR(&move);
        if (q->num == 0) {
            if(!result) {
                strcpy(text_value, "Draw !!!");
                move.result = DRAW;
            }
            move.game_over = true;
            game_over = true;
        }
    } else {
        if (!(strcmp(text_value, "You Win !!!") == 0)) {
            strcpy(text_value, "Draw !!!");
        }
        move.game_over = true;
        game_over = true;
    }
    std::cout<<text_value<<std::endl;
    copy_STATE(&move, &move_computer);
    
    delete_SUCESSOR(q);
    copy_STATE(&move, &move_new);
    move_old.turn = COMPUTER_PIECE;
    q = make_SUCESSOR(&move_old);        // Sets jump_found global
    if (jump_found)
        std::cout<<"Jumped!!"<<std::endl;
    else if (q->num != 0)
        std::cout<<"Done!"<<std::endl;
    computer_jumped = jump_found;

    
    delete_SUCESSOR(q);
    q = make_SUCESSOR(&move_new);        // Sets Global Variable jump_found
    delete_SUCESSOR(q);
    
    jump_found_player = jump_found;
    move_new.jump_found = jump_found;
    compulsory_jump_found = false;
    compulsory_jump_start_square = 32;

    if (jump_found_player) {
        for (i=0; i<32; ++i) {
            if (r != NULL) {
                delete_SUCESSOR(r);
                r = NULL;
            }
            mouse_move(i);                // Sets Global Variable r
            
            if (r != NULL) {
                delete_SUCESSOR(r);
                r = NULL;
                if (!compulsory_jump_found) {
                    compulsory_jump_found = true;
                    compulsory_jump_start_square = i;
                } else {
                    compulsory_jump_found = false;
                    compulsory_jump_start_square = 32;
                    refresh_STATE(&move_new);
                    break;
                }
            }
        }
        
        if (compulsory_jump_found)
            mouse_move(compulsory_jump_start_square);    // Sets Global Variable r
    }
    
    move_new.jump_start_square = compulsory_jump_start_square;
    if (!compulsory_jump_found) {
        for (i=0; i<32; ++i) {
            if (move_old.board[i] != move_new.board[i]) {
                if (move_old.board[i] == COMPUTER_PIECE || 
                            move_old.board[i] == COMPUTER_KING )
                    move_new.board[i] = COMPUTER_START_SQUARE;
                else if (move_old.board[i] == PLAYER_PIECE || 
                            move_old.board[i] == PLAYER_KING )
                    move_new.board[i] = COMPUTER_JUMPED_SQUARE;
                else if (move_old.board[i] == NO_PIECE) {
                    if (move_new.board[i] == COMPUTER_PIECE)
                        move_new.board[i] = COMPUTER_END_SQUARE_PIECE;
                    else if (move_new.board[i] == COMPUTER_KING)
                        move_new.board[i] = COMPUTER_END_SQUARE_KING;
                }
            }
        }
    }

    copy_STATE(&move_new, &move);
    copy_STATE(&move_old, &abort_restore_old);
    copy_STATE(&move_new, &abort_restore);
    push_STATE(&prev_STATE, &move_new);
}    

//--------------------------------------------------------------------------------
//    void    push_STATE(STATE_PTR *r, STATE_PTR q);
//--------------------------------------------------------------------------------

void push_STATE(STATE_PTR *r, STATE_PTR q)
{
    STATE_PTR p;

    p = new_STATE();
    copy_STATE(q, p);
    refresh_STATE(p);
    p->next = *r;
    *r = p;
}
