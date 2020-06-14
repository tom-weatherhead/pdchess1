// pdchess.h - A component of the PD-Chess package
// By Tom Weatherhead - April 1, 1993
// (C) Copyright 1993 - 2020 by Tom Weatherhead

#define BOARD_SIZE              8
#define NUM_PIECE_DEFS          6
#define NUM_ALIST_SLOTS		      6
#define SLOT_SIZE_INC		        10
#define ZERO_EFFECT_SLOT	      5
#define MAX_ARMY_SIZE           16
#define MAX_NUM_GEN_MOVES       150
#define INIT_MOVE_REC_SIZE	    400
#define INIT_BEST_SLOT_SIZE	    60
#define INC_MOVE_REC_SIZE	      200
#define MAX_MAX_PLY		          25
#define O_WHITE                 0
#define O_BLACK                 1
#define KINGSIDE                0
#define QUEENSIDE               1
#define MAX_MOVE_REC_IDX	      32768
#define CAN_CASTLE_CONST	      MAX_MOVE_REC_IDX
// CAN_CASTLE_CONST > 4 * max possible # of moves
// in a legal chess game
#define CANT_CASTLE_CONST	      (MAX_MOVE_REC_IDX-1)
#define NOT_CAPTURED_CONST	    MAX_MOVE_REC_IDX
#define NOT_PROMOTED_CONST	    MAX_MOVE_REC_IDX
#define AUTO_INDEFINITELY	      MAX_MOVE_REC_IDX

#define PAWN_VALUE		          1
#define KNIGHT_VALUE		        3
#define BISHOP_VALUE	        	3
#define ROOK_VALUE		          5
#define QUEEN_VALUE		          9
#define KING_VALUE		          100
// KING_VALUE > 2 * sum of all 15 other pieces
#define VICTORY_VALUE		        (KING_VALUE/2)
#define DEFEAT_VALUE		        (-VICTORY_VALUE)

#define PAWN_TOKEN		'P'
#define KNIGHT_TOKEN		'N'
#define BISHOP_TOKEN		'B'
#define ROOK_TOKEN		'R'
#define QUEEN_TOKEN		'Q'
#define KING_TOKEN		'K'

/* Define a boolean type */

#ifndef false
#ifdef true
#undef true
#endif
typedef enum { false, true } bool;
#else
#ifndef true
#define true 1
#endif
typedef char bool;
#endif

// Macros
// #define read_board(o,x,y)       o->sh->board.ptr[x][y]
#define read_board(o,x,y)       ((x<0||x>=BOARD_SIZE||y<0||y>=BOARD_SIZE)?NULL:o->sh->board.ptr[x][y])
#define write_board(o,x,y,p)    o->sh->board.ptr[x][y]=p
#define is_square_clear(o,x,y)  (o->sh->board.ptr[x][y]==NULL)
#define havent_castled(o,s)     (o->castle_lost[s]==CAN_CASTLE_CONST)
#define disallow_castling(o,s)  if(!havent_castled(o,s)){o->castle_lost[s]=o->sh->move_rec_idx;}
#define is_visible(p)	  (p->captured==NOT_CAPTURED_CONST)
#define in_opening_book(s)  ((s)->open_move_ptr!=NULL)
#define close_open_book(s)  (s)->open_move_ptr=NULL

#define is_checkmate(o)	(best_move(NULL,o,1,0,0)<=DEFEAT_VALUE?true:false)

typedef enum {
	BS_CHOOSE_SRC,
	BS_CHOOSE_DST
} ButtonStateType;

typedef enum {
	PT_HUMAN,
	PT_COMPUTER
} PlayerTypeType;

typedef struct {
	int r, c;
} vector_type;

typedef struct {
	const char * name;
	char token;   // What it looks like when printed
	unsigned int points, slot_num, num_vectors;
	vector_type vectors[9];
	bool limited_moving;
} piece_def_type;

struct army_struct;

typedef struct piece_struct {
	struct army_struct * owner;	// Who owns it
	struct piece_struct ** self_ptr;	// ptr to board square
	piece_def_type * def;	// What it is
	unsigned int row, col,	// Where it is
		captured,			// When captured
		promoted;			// When promoted from pawn
} piece_type;

typedef struct {
	piece_type * ptr[BOARD_SIZE][BOARD_SIZE];
} board_type;

typedef enum {
	REGULAR_MOVE,
	PROMOTE_TO_N,	// indices of promotion typed here
	PROMOTE_TO_B,	// match indices of corresp. piece defs
	PROMOTE_TO_R,
	PROMOTE_TO_Q,
	CASTLE_KSIDE,
	CASTLE_QSIDE,
	EN_PASSANT
} move_type_type;

typedef struct move_struct {
	unsigned int r1, c1, r2, c2;
	move_type_type type;
	piece_type * pc, ** dstPP;
} move_type;

typedef struct {
	move_type * moves;
	unsigned int len, size;
} slot_type;

typedef struct {
	slot_type slots[NUM_ALIST_SLOTS];
	// unsigned int best_slot;
} move_assoc_list_type;

typedef struct open_struct {
	const char ** moves;			// NULL - terminated
	const char * name;
	unsigned short num_child;
	struct open_struct ** child;
} open_type;

typedef unsigned short num_type;

typedef struct {
	char c[2];
} omt_move_type;

typedef struct omt_node_struct {
	unsigned short num_moves, num_children, move_buf_size;
	omt_move_type * moves;
	struct omt_node_struct ** self_ptr, ** child;
} omt_node_type;

typedef struct {
	unsigned int move_rec_idx, move_rec_size,
		resign_thresh, num_moves_made,
		num_moves_gend;
	char * move_rec;

	open_type * open_type_ptr;
	const char ** open_move_ptr;  	// NULL when out of book
	omt_node_type * omt_root;	// Opening Move Tree root
	piece_def_type * pc_def;
	slot_type best_move_slot;
	move_assoc_list_type alists[MAX_MAX_PLY];
	board_type board;

	unsigned int capture_by_or_draw;
	char CaptureMsg[50];
} shared_type;

typedef struct army_struct {
	const char * name;
	unsigned int colour, num_pieces, max_ply,
		// Max depth of search while captures occur
		max_ply_cap,
		// Number of moves made in the course of a search
		num_moves_made,
		auto_until;	// Automated until move_rec_idx == this
	piece_type pieces[MAX_ARMY_SIZE], * ep_pawn;
	bool (*move_proc)( struct army_struct ** );
	PlayerTypeType type;
	unsigned int castle_lost[2]; // When lost ability to castle
	struct army_struct * opponent;
	move_type opt_move;
	shared_type * sh;
} army_type;

typedef struct {
	piece_type * pc[2], old_pc[2], * cap_pc, * ep_pawn;
} mod_data_type;


// Prototypes

// pdchess.cpp
void free_stuff( shared_type * );
unsigned int army_value( army_type * );
bool is_castle_valid( army_type *, move_type_type );
void move_piece( army_type *, move_type *, mod_data_type * );
void init_computer_army( army_type * );
bool is_check( army_type * );
void print_board( army_type * );
void backup( army_type * );
bool is_valid_move( army_type *, move_type * );
bool is_draw( army_type * );

// pdcmoves.cpp
bool human_move( army_type ** );
bool computer_move( army_type ** );

// pdc_look.cpp
void undo_move( mod_data_type * );
int best_move( army_type *, army_type *, unsigned int, int, int);

// pdc_open.cpp
void open_open_book( shared_type * );
const char * open_get_move( shared_type * );
void open_match_move( shared_type *, move_type * );

// pdc_disk.cpp
bool load_game( army_type [], army_type ** );
bool save_game( army_type [] );

// pdcmvcnv.cpp
const char * eat_spaces( const char * );
bool coord_ext_to_int( const char *, unsigned int *, unsigned int * );
bool move_ext_to_int( const char *, move_type * );
const char * move_int_to_ext( move_type * );
void print_move_rec( shared_type * );

// pdc_init.cpp
void init_board( army_type * );

// ******** End of pdchess.h ********
