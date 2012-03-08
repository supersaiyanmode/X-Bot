#ifndef CHECKERS_CONST
#define CHECKERS_CONST

#define  MIN_SQUARE_SIZE	25		// Minimum board square size to display.

#define  STALEMATE_LEGAL    false	// When "false" you lose when you can not move
									// When "true"  stalemates are considered a draw

#define  DEPTH_MAX			16	    // Maximum legal search depth.

#define  STATE_MAX			32	    // Maximum numbers of STATE in SUCESSOR.

#define  PLAYER_PIECE		0		// Used to specify piece type in board[].
#define  PLAYER_KING		1		// Also used as indexes to 
#define  COMPUTER_PIECE		2		// num_moves[x][] moves[x][][] jumps[x][][].
#define  COMPUTER_KING		3

#define  NO_PIECE			4

#define  HIGHEST_VALID_PIECE         4		// Only values in board[i] feild of  
											// STATE less than or equal to this 
											// are valid for play() thread.

#define  HI_LIGHTED_PLAYER_PIECE     5		// Used as board[i] values in STATE 
#define  HI_LIGHTED_PLAYER_KING      6		// when painting the game board.
#define  HI_LIGHTED_NO_PIECE         7
#define  HI_LIGHTED_END_SQUARE       8
#define  HI_LIGHTED_MID_SQUARE       9
#define  COMPUTER_START_SQUARE      10
#define  COMPUTER_JUMPED_SQUARE     11
#define  COMPUTER_END_SQUARE_PIECE  12
#define  COMPUTER_END_SQUARE_KING   13

#define  WIN	0			// Used as values in result feild of STATE
#define  LOSE	1
#define  DRAW	2
#define  NONE	3

#ifndef	 NULL
#define  NULL	0L
#endif

#endif
