#include "const.h"
#include "types.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>

void                    play(void *dummy);
int                     f(STATE_PTR p, int alpha, int beta, int depth);
SUCESSOR_PTR            make_SUCESSOR(STATE_PTR p);
void                    add_jumps(SUCESSOR_PTR q, STATE_PTR p, bool jumped);
SUCESSOR_PTR            consider_jumps(STATE_PTR p);
int                     value(STATE_PTR p);
STATE_PTR               new_STATE(void);
SUCESSOR_PTR            new_empty_SUCESSOR(void);
void                    delete_SUCESSOR(SUCESSOR_PTR q);

void                    end_game_tweek(void);
void                    set_clump_value(STATE_PTR p);

SUCESSOR_PTR            make_single_step_SUCESSOR(STATE_PTR p);

void                    copy_STATE(STATE_PTR p, STATE_PTR q);

extern STATE            move;                   // The state to consider
extern int              search_depth;           // The depth to search to

extern SUCESSOR_PTR     q;                      // Used to return alternative moves

extern STATE_PTR        free_STATE;
extern SUCESSOR_PTR     free_SUCESSOR;

extern unsigned char    num_moves[4][32];
extern unsigned char    moves[4][32][4];
extern unsigned char    jumps[4][32][4];

extern PIECE_NOS_PTR    t;
extern PIECE_NOS        computer_turn_nos;
extern PIECE_NOS        opponent_turn_nos;

extern bool             jump_found;
extern bool             jump_found_player;


unsigned int            copy_STATE_length = COPY_STATE_LENGTH_COMPLETE;


void copy_STATE(STATE_PTR p, STATE_PTR q){
    memcpy(q, p, copy_STATE_length);
}
    


void play(void *crap){
    SUCESSOR_PTR r;
    int max, t, i, j;

    copy_STATE_length = COPY_STATE_LENGTH_PLAY; // board[32], turn and piece
    q = new_empty_SUCESSOR();  // extern SUCESSOR_PTR q, returns alternative moves.
    r = make_SUCESSOR(&move);  // extern STATE move, holds STATE to play from.
    max = -4000;

    for (i = 0; i<r->num; ++i) {
            t = -f(r->ptr[i], -4000, 4000, search_depth-1);
            if (t==max) {
                    j = q->num;
                    //std::cout<<"Current q->num: "<<j<<std::endl;
                    q->ptr[j] = new_STATE();
                    copy_STATE(r->ptr[i], q->ptr[j]);
                    q->ptr[j]->value = max;
                    ++(q->num);
            } else if (t>max) {
                    max = t;
                    for (j = 0; j<q->num; ++j) {
                            q->ptr[j]->next = free_STATE;
                            free_STATE = q->ptr[j];
                    }
                    q->ptr[0] = new_STATE();
                    copy_STATE(r->ptr[i], q->ptr[0]);
                    q->ptr[0]->value = max;
                    q->num = 1;
            }
    }
    delete_SUCESSOR(r);
    end_game_tweek();
    copy_STATE_length = COPY_STATE_LENGTH_COMPLETE;
}


//--------------------------------------------------------------------------------
//    void end_game_tweek()
//--------------------------------------------------------------------------------
//
// This tweak deletes from the extern SUCUSSOR_PTR q all STATES that do not lead 
// to the kinging of a COMPUTER_PIECE.  If there are no COMPUTER_PIECEs, 
// COMPUTER_KINGs are encouraged to chase PLAYER_PIECEs or PLAYER_KINGs.
//
//--------------------------------------------------------------------------------


void end_game_tweek(){
    long temp, clump_max;
    int i, j;
    
    if (q->num < 2)    
        return;

    for ( i = 0; i<q->num; ++i )
        set_clump_value( q->ptr[i] );
    
    clump_max = q->ptr[0]->clump;

    for ( i = 1; i<q->num; ++i)
        if ((temp = q->ptr[i]->clump) > clump_max)
            clump_max = temp;

    for ( i = 0, j = 0; i<q->num; ++i ) {
        if ( q->ptr[i]->clump < clump_max ) {
            q->ptr[i]->next = free_STATE;
            free_STATE = q->ptr[i];
        } else {
            if (j<i)
                    q->ptr[j] = q->ptr[i];
            ++j;
        }
    }
    q->num = j;
}


//--------------------------------------------------------------------------------
//    void set_clump_value(STATE_PTR p)
//--------------------------------------------------------------------------------
//
// Computes a value and stores it in p->clump so that the highest value is for the 
// kinging of a COMPUTER_PIECE and then for COMPUTER_KINGs that chase the main 
// clump of PLAYER_PIECEs or PLAYER_KINGs.
//
//--------------------------------------------------------------------------------


void set_clump_value(STATE_PTR p){
    int i, j, row1, col1, row2, col2, row_dist, col_dist;
    p->clump = 0;

    for ( i = 0; i < 32; ++i ) {
        if ( p->board[i] == COMPUTER_PIECE || p->board[i] == COMPUTER_KING ) {
            row1 = i/4;
            col1 = ( i%4 )*2 + 1 - row1 + ( row1/2 )*2;
            for ( j = 0; j<32; ++j ) {
                if (p->board[j] == PLAYER_PIECE || p->board[j] == PLAYER_KING) { 
                    if ( p->board[i] == COMPUTER_PIECE )
                        p->clump -= row1 * 16;  // First priority to king piece

                    else {
                        row2 = j/4;
                        col2 = ( j%4 )*2 + 1 - row2 + ( row2/2 )*2;
                
                        if (( row_dist = (row1 - row2)) < 0 )
                                row_dist = -row_dist;
                        if (( col_dist = (col1 - col2)) < 0 )
                                col_dist = -col_dist;
                        
                        p->clump += 16 - row_dist - col_dist;        // Kings chase pieces
                    }
                }
            }
        }
    }
}


//--------------------------------------------------------------------------------
//    int f(STATE_PTR p, int alpha, int beta, unsigned int depth)
//--------------------------------------------------------------------------------
//
// Recursive "alpha/beta" game tree trimming search routine.
// 
// inputs:-    
// ======
//        p        The pointer to STATE to process.
//        aplha    The highest game value to the player the state player can force.
//        beta    The lowest game value to the player the state opponent can force.
//        depth    The search depth to look ahead.
//
//        Note:-    When alpha < beta the game tree is trimmed.
//        ====
//
//        abort_pressed        Terminates play() thread when TRUE.  
//                            Set by "Checkers.cpp" thread.
//
//    outputs:-    
//    =======
//        Returns the highest game value the player can force so far.
//
//
//    Note:-    Value to player equals negative value to opponent.
//    ====
//
//--------------------------------------------------------------------------------


int f(STATE_PTR p, int alpha, int beta, int depth){
    SUCESSOR_PTR q;
    int i, t;
    
    if (depth==0)
        return(value(p));

    q = make_SUCESSOR(p);
    
    if (q->num==0  && (alpha = value(p)) > -1000 && alpha < 1000) {
        if (!STALEMATE_LEGAL)
            alpha -= 2000;                      // lose when can't move
        else if (alpha < -16)
            alpha += 16;                        // seek or avoid stalemate
        else if (alpha >  16)
            alpha -= 16;                        // seek or avoid stalemate
    }

    for (i = 0; i<q->num && alpha<beta; ++i) {
        t = -f(q->ptr[i], -beta, -alpha, depth-1);
        if (t>alpha)
            alpha = t;
    }
    delete_SUCESSOR(q);
    return(alpha);
}


//--------------------------------------------------------------------------------
//    SUCESSOR_PTR make_SUCESSOR(STATE_PTR p)
//--------------------------------------------------------------------------------
//
//    Return a pointer to a SUCESSOR that contains all possible moves or jumps 
//    from STATE pointed to by p.
//
//--------------------------------------------------------------------------------


SUCESSOR_PTR make_SUCESSOR(STATE_PTR p)
{
    SUCESSOR_PTR q;
    STATE_PTR new_state;
    int c, i, j, k, l;
    
    q = new_empty_SUCESSOR();
    if (p->turn==COMPUTER_PIECE)
        t = &computer_turn_nos;
    else
        t = &opponent_turn_nos;
    jump_found = false;
    for (i = 0; i<32; ++i) {
        c = p->board[i];
        if (c==t->mover_piece || c==t->mover_king) {
            p->piece = i;
            add_jumps(q, p, false);
            if (!jump_found) {
                j = num_moves[c][i];
                for (k = 0; k<j; ++k) {
                    l = moves[c][i][k];
                    if (p->board[--l]==NO_PIECE) {
                        new_state = new_STATE();
                        copy_STATE(p, new_state);
                        new_state->board[i] = NO_PIECE;
                        if (l<4 || l>27)
                            new_state->board[l] = t->mover_king;
                        else
                            new_state->board[l] = c;
                        new_state->turn = t->opponent_piece;
                        q->ptr[q->num] = new_state;
                        ++(q->num);
                    }
                }
            }
        }
    }
    return(q);
}    


//--------------------------------------------------------------------------------
//    void add_jumps(SUCESSOR_PTR q, STATE_PTR p, bool jumped)
//--------------------------------------------------------------------------------
//
//    Recursive function that return in SUCESSOR_PTR q all jumps that are legal for 
//    STATE pointed to by p.  If it finds a jump all previous non-jump moves are
//    deleted out of q and jump_found is set to TRUE.
//
//    inputs:-    q            pointer to SUCESSOR where jumps are put.
//    ======        p            pointer to STATE from which jumps are made.
//                jumped        TRUE when p is half way through a multiple jump.
//
//    Note:-        p->piece    is position of piece to make jumps from.
//    ====
//--------------------------------------------------------------------------------


void add_jumps(SUCESSOR_PTR q, STATE_PTR p, bool jumped)
{
    SUCESSOR_PTR r;
    int i;
    
    r = consider_jumps(p);
    if (r==NULL) { 
        if (jumped) {
            if (!jump_found) {
                for (i = 0; i<q->num; ++i) {
                    q->ptr[i]->next = free_STATE;
                    free_STATE = q->ptr[i];
                }
                q->num = 0;
                jump_found = true;
            }
            p->turn = t->opponent_piece;
            q->ptr[q->num] = p;
            ++(q->num);
        }
    } else {
        for (i = 0; i<r->num; ++i)
            add_jumps(q, r->ptr[i], true);
        r->next = free_SUCESSOR;
        free_SUCESSOR = r;
        if (jumped) {
            p->next = free_STATE;
            free_STATE = p;
        }
    }
}


//--------------------------------------------------------------------------------
//    SUCESSOR_PTR consider_jumps(STATE_PTR p)
//--------------------------------------------------------------------------------
//
//    Return a SUCESSOR_PTR that contains all single jumps for STATE 
//    pointed to by p or NULL.
//
//    inputs:-    p            pointer to STATE from which jumps are made.
//
//    Note:-        p->piece    is position of piece to make jumps from.  
//    ====                    If p->piece < 0 then p->piece + 64 has kinged through
//                            jumping and can jump no further.
//
//--------------------------------------------------------------------------------


SUCESSOR_PTR consider_jumps(STATE_PTR p)
{
    SUCESSOR_PTR q;
    STATE_PTR new_state;
    int i, j, k, l, m, n, o;
    
    q = NULL;
    if (p->piece < 0)
        return(q);
    i = p->piece;
    j = p->board[i];
    k = num_moves[j][i];
    for (l = 0; l<k; ++l) {
        m = moves[j][i][l];
        n = p->board[--m];
        if (n==t->opponent_piece || n==t->opponent_king) {
            o = jumps[j][i][l];
            if (o!=0 && p->board[--o]==NO_PIECE) {
                new_state = new_STATE();
                copy_STATE(p, new_state);
                new_state->board[i] = NO_PIECE;
                new_state->board[m] = NO_PIECE;
                if (o<4 || o>27) {
                    new_state->board[o] = t->mover_king;
                    if (j == t->mover_piece)
                        new_state->piece = o - 64;
                    else
                        new_state->piece = o;
                } else {
                    new_state->board[o] = j;
                    new_state->piece = o;
                } 
                if (q==NULL)
                    q = new_empty_SUCESSOR();
                q->ptr[q->num] = new_state;
                ++(q->num);
            }
        }
    }
    return(q);
}    


//--------------------------------------------------------------------------------
//    STATE_PTR new_STATE(void)
//--------------------------------------------------------------------------------
//
//    Return a pointer to a free STATE
//
//--------------------------------------------------------------------------------


STATE_PTR new_STATE(void)
{
    STATE_PTR p;
    
    if (free_STATE == NULL) {
        p = new(STATE);
        if (p == NULL)
            exit(1);
    } else {
        p = free_STATE;
        free_STATE = free_STATE->next;
    }
    return(p);
}


//--------------------------------------------------------------------------------
//    SUCESSOR_PTR new_empty_SUCESSOR(void)
//--------------------------------------------------------------------------------
//
//    Return a pointer to a free empty SUCESSOR
//
//--------------------------------------------------------------------------------


SUCESSOR_PTR new_empty_SUCESSOR(void)
{
    SUCESSOR_PTR q;
    
    if (free_SUCESSOR == NULL) {
        q = new(SUCESSOR);
        if (q == NULL)
            exit(1);
    } else {
        q = free_SUCESSOR;
        free_SUCESSOR = free_SUCESSOR->next;
    }
    q->num = 0;    
    return(q);    
}


//--------------------------------------------------------------------------------
//    void delete_SUCESSOR(SUCESSOR_PTR q)
//--------------------------------------------------------------------------------
//
//    Frees SUCESSOR pointed to by q and all STATEs in it.
//
//--------------------------------------------------------------------------------


void delete_SUCESSOR(SUCESSOR_PTR q)
{
    int i;
    
    for (i = 0; i<q->num; ++i) {
        q->ptr[i]->next = free_STATE;
        free_STATE = q->ptr[i];
    }
    q->next = free_SUCESSOR;
    free_SUCESSOR = q;
}    


//--------------------------------------------------------------------------------
//    int value(STATE_PTR p)
//--------------------------------------------------------------------------------
//
//    Return the value of STATE pointed to by p.
//
//    Note:-        p->turn indicates who the value is for.
//    ====        Blocking kinging on back line is encouraged.
//                Kings on back lines is discouraged.
//                Value to player is negative value to opponent.
//
//--------------------------------------------------------------------------------


int value(STATE_PTR p)
{
    int i, x1, x2, x3, x4;
    
    if (p->board[0] == PLAYER_PIECE && p->board[2] == PLAYER_PIECE)
        x1 = 2;
    else
        x1 = 0;
    
    if (p->board[29] == COMPUTER_PIECE && p->board[31] == COMPUTER_PIECE)
        x3 = 2;
    else
        x3 = 0;
    
    x2 = x4 = 0;
    
    for (i = 0; i<4; ++i)
        switch(p->board[i]) {
        case PLAYER_PIECE:
            x1 += 8;
            break;
        case PLAYER_KING:
            x2 += 11;
            break;
        case COMPUTER_KING:
            x4 += 11;
        }
    for (i = 4; i<28; ++i)
        switch(p->board[i]) {
        case PLAYER_PIECE:
            x1 += 8;
            break;
        case PLAYER_KING:
            x2 += 12;
            break;
        case COMPUTER_PIECE:
            x3 += 8;
            break;
        case COMPUTER_KING:
            x4 += 12;
        }
    for (i = 28; i<32; ++i)
        switch(p->board[i]) {
        case PLAYER_KING:
            x2 += 11;
            break;
        case COMPUTER_PIECE:
            x3 += 8;
            break;
        case COMPUTER_KING:
            x4 += 11;
        }
    
    if (!x1 && !x2) {

        if (p->turn == PLAYER_PIECE)
            return( -2000 - x3 - x4 );
        else
            return(  2000 + x3 + x4 );
    
    } else if (!x3 && !x4) {
        
        if (p->turn == PLAYER_PIECE)
            return(  2000 + x1 + x2 );
        else
            return( -2000 - x1 - x2 );
    
    } else if (p->turn == PLAYER_PIECE)
    
        return( x1 + x2 - x3 - x4 );
    
    else
    
        return( x3 + x4 - x1 - x2 );

}


//--------------------------------------------------------------------------------
//    SUCESSOR_PTR make_single_step_SUCESSOR(STATE_PTR p)
//--------------------------------------------------------------------------------
//
//    Return a pointer to a SUCESSOR that contains all possible moves or single
//    step jumps from STATE pointed to by p. Please note that you call this 
//    function again with any STATEs that are mid jump to get the next possible 
//    jumps. It's good for "mouse over showing" because the piece feild in all
//    the SUCESSOR STATES gives the squares that are possible and 
//    whether they are mid jump or not.
//
//
//    inputs:-    STATE_PTR        p                        ( Parameter )
//    ======
//                bool            jump_found_player        ( Global Variable )
//
//
//    note:-    p->piece is the square being considered.
//    ====    and when > 127 it means that we are in the
//            middle of a jump and "piece - 128" 
//            is true square
//
//            STATEs in SUCESSOR are the STATEs after the move or single
//            jump has been made.
//
//            piece field in these STATEs are square to show Yellow or Green.
//            if piece is > 127 then piece is in mid jump and real piece 
//            square is "piece - 128" and should be shown Green.  Otherwize it
//            should be shown Yellow.
//
//            Always sets Global Variable t = &opponent_turn_nos
//
//--------------------------------------------------------------------------------



SUCESSOR_PTR make_single_step_SUCESSOR(STATE_PTR p)
{
    SUCESSOR_PTR q, r;
    STATE_PTR new_state;
    int c, i, j, k, l;
    
    q = NULL;
    p->turn = PLAYER_PIECE;
    t = &opponent_turn_nos;                                // Needed for consider_jumps()

    if ((i = p->piece) > 127) {
        i -= 128;
    }
    c = p->board[i];
    if (c==PLAYER_PIECE || c==PLAYER_KING) {
        if ((q = consider_jumps(p)) != NULL) {
            for (j = 0; j<q->num; ++j) {
                if (q->ptr[j]->piece < 0)                 // Piece has Kinged through jumping
                    q->ptr[j]->piece += 64;                // Set correct square, mark as END
                else if ((r = consider_jumps(q->ptr[j])) != NULL) {
                    q->ptr[j]->piece += 128;            // Mark as MID Square
                    delete_SUCESSOR(r);
                }
            }
        } else if (!jump_found_player) {                // Global Variable
            j = num_moves[c][i];
            for (k = 0; k<j; ++k) {
                l = moves[c][i][k];
                if (p->board[--l]==NO_PIECE) {
                    new_state = new_STATE();
                    copy_STATE(p, new_state);
                    new_state->board[i] = NO_PIECE;
                    if (l>27)
                        new_state->board[l] = PLAYER_KING;
                    else
                        new_state->board[l] = c;
                    new_state->piece = l;                    // Mark as END Square
                    if (q==NULL)
                        q = new_empty_SUCESSOR();
                    q->ptr[q->num] = new_state;
                    ++(q->num);
                }
            }
        }
    }
    return(q);
}    
