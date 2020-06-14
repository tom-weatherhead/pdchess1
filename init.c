// pdc_init.cpp

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "pdchess.h"


bool create_piece( army_type * owner, unsigned int r, unsigned int c, unsigned int def_idx ) {

	piece_type * pc;

	if( r >= BOARD_SIZE  ||  c >= BOARD_SIZE ) {
		printf("Cannot create piece at %d,%d: invalid coordinates\n", r, c );

		return false;
	}

	if( !is_square_clear( owner, r, c ) ) {
		printf("Cannot create piece: %d,%d already occupied\n", r, c );

		return false;
	}

	pc = &owner->pieces[owner->num_pieces++];
	pc->owner = owner;
	pc->row = r;
	pc->col = c;
	pc->def = &owner->sh->pc_def[def_idx];
	pc->captured = NOT_CAPTURED_CONST;
	pc->promoted = NOT_PROMOTED_CONST;
	pc->self_ptr = &owner->sh->board.ptr[r][c];
	*(pc->self_ptr) = pc;	// write_board(owner,r,c,pc);

	return true;
} // create_piece()


void init_computer_army( army_type * owner ) {
	unsigned int max_ply = 0, max_ply_cap = 0;

	do {
		// char buf[64];
		// max_ply = 0;
		printf( "Max search depth for %s (1-%d): ",
		owner->name, MAX_MAX_PLY );
		fflush( stdin ); // Ensure that the input stream buffer is empty
		scanf( "%d", &max_ply );
		// fgets(buf, sizeof buf, stdin);
		// max_ply = atoi(buf);
	} while( max_ply < 1  ||  max_ply > MAX_MAX_PLY );

	owner->max_ply = max_ply;

	do {
		printf( "Extended max search depth for %s (%d-%d): ",
		owner->name, max_ply, MAX_MAX_PLY );
		fflush( stdin );
		scanf( "%d", &max_ply_cap );
	} while( max_ply_cap < max_ply  ||  max_ply_cap > MAX_MAX_PLY );

	owner->max_ply_cap = max_ply_cap;
} // init_computer_army()


static void init_army( army_type * owner, unsigned int colour ) {
	owner->colour = colour;
	owner->name = (colour == O_WHITE) ? "White" : "Black";
	owner->castle_lost[KINGSIDE] = owner->castle_lost[QUEENSIDE] = CAN_CASTLE_CONST;
	owner->num_pieces = 0;
	owner->ep_pawn = NULL;
} // init_army()


static void init_army_pieces( army_type * owner ) {
	unsigned int row = 7 * (1 - owner->colour);

	// piece[0].Init( this, row, 3, 4 ); ...
	create_piece( owner, row, 3, 4 );           // Queen
	create_piece( owner, row, 0, 3 );           // Rooks
	create_piece( owner, row, 7, 3 );
	create_piece( owner, row, 2, 2 );           // Bishops
	create_piece( owner, row, 5, 2 );
	create_piece( owner, row, 1, 1 );           // Knights
	create_piece( owner, row, 6, 1 );
	create_piece( owner, row, 4, 5 );           // King
	row = 5 * (1 - owner->colour)+1;
	create_piece( owner, row, 0, 0 );           // Pawns
	create_piece( owner, row, 1, 0 );
	create_piece( owner, row, 2, 0 );
	create_piece( owner, row, 3, 0 );
	create_piece( owner, row, 4, 0 );
	create_piece( owner, row, 5, 0 );
	create_piece( owner, row, 6, 0 );
	create_piece( owner, row, 7, 0 );
} // init_army_pieces()


#if 1
static void construct_study( army_type army[] ) {
	char str[10];
	unsigned int i, row, col, pc_num, colour;
	army_type * owner;
	piece_type * pc;

	close_open_book( army->sh );

	// Automatically create the 2 kings,
	// prompting the user for coords

	for( i = 0; i < 2; i++ ) {
		owner = &army[i];

		do {
			printf( "Coordinates of %s king: ", owner->name );
			scanf( "%s", str );
		} while( !coord_ext_to_int( str, &row, &col )
			||  !create_piece( owner, row, col, 5 ) );

		if( row != 7*(1-i)  ||  col != 4 ) {
			printf( "Castling disallowed for %s\n", owner->name );
			owner->castle_lost[0] = owner->castle_lost[1] = 0;
		}
	}

	/* Allow creation of extra pieces */

	for( ; ; ) {
		print_board( army );

		for( i = 0; i < NUM_PIECE_DEFS; i++ ) {
			printf( "%d: %s  ", i, army->sh->pc_def[i].name );
		}

		printf( "\nCreate which piece (-1 to exit): " );
		scanf( "%d", &pc_num );

		if( pc_num >= NUM_PIECE_DEFS ) {
			break;
		}

		do {
			printf( "Colour (white=%d, black=%d): ", O_WHITE, O_BLACK );
			scanf( "%d", &colour );
		} while( colour >= 2 );

		owner = &army[colour];

		if( owner->num_pieces >= MAX_ARMY_SIZE ) {
			printf( "%s already has %d pieces\n", owner->name, owner->num_pieces );
			continue;
		}

		do {
			printf( "Coordinates: " );
			scanf( "%s", str );
		} while( !coord_ext_to_int( str, &row, &col )
			||  !create_piece( owner, row, col, pc_num ) );
	}

	// Check rooks for castling

	for( colour = 0; colour <= 1; colour++ ) {	// Army num
		owner = &army[colour];

		for( i = 0; i <= 1; i++ ) {		// Castle type

			if( owner->castle_lost[i] == 0 ) {
				continue;
			}

			pc = read_board(owner,7*(1-colour),7*(1-i));

			if( pc == NULL  ||  pc->owner != owner  ||  pc->def->token != ROOK_TOKEN ) {

				printf( "%sside castling disallowed for %s\n",
					(i == 0) ? "King" : "Queen", owner->name );
				owner->castle_lost[i] = 0;
			}
		}
	}
} // construct_study()
#endif


void init_board( army_type * army ) {
	static piece_def_type pc_def[NUM_PIECE_DEFS] = {
		{ "Pawn", PAWN_TOKEN, PAWN_VALUE, 4, 0,
			{ {0,0} /* {1,0},{1,1},{2,0} */ },
			true
		},
		{ "Knight", KNIGHT_TOKEN, KNIGHT_VALUE, 3, 8,
			{ {2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2},{1,-2},{2,-1},{0,0} },
			true
		},
		{ "Bishop", BISHOP_TOKEN, BISHOP_VALUE, 3, 4,
			{ {1,1},{-1,1},{-1,-1},{1,-1},{0,0} },
			false
		},
		{ "Rook", ROOK_TOKEN, ROOK_VALUE, 2, 4,
			{ {0,1},{1,0},{0,-1},{-1,0},{0,0} },
			false
		},
		{ "Queen", QUEEN_TOKEN, QUEEN_VALUE, 1, 8,
			{ {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{0,0} },
			false
		},
		{ "King", KING_TOKEN, KING_VALUE, 0, 8,
			{ {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{0,0} },
			true
		}
	};
	static shared_type shared_data;
	shared_type * sh = &shared_data;
	army_type * owner;
	unsigned int i, j, row, col,
		init_slot_size[NUM_ALIST_SLOTS] = {10, 10, 10, 20, 40, 60};
	char str[10], c;
	move_assoc_list_type * alist;
	slot_type * slot;

	sh->pc_def = pc_def;

	sh->best_move_slot.size = INIT_BEST_SLOT_SIZE;
	sh->best_move_slot.moves = (move_type *)malloc(INIT_BEST_SLOT_SIZE * sizeof(move_type));

	for(i = 0; i < MAX_MAX_PLY; i++) {
		alist = &sh->alists[i];

		for(j = 0; j < NUM_ALIST_SLOTS; j++) {
			slot = &alist->slots[j];
			slot->len = 0;
			slot->size = init_slot_size[j];
			slot->moves = (move_type *)malloc(slot->size * sizeof(move_type));
		}
	}

	sh->move_rec = (char *)malloc( INIT_MOVE_REC_SIZE );
	sh->move_rec_idx = 0;
	sh->move_rec_size = INIT_MOVE_REC_SIZE;
	sh->capture_by_or_draw = 200;
	army[O_BLACK].sh = army[O_WHITE].sh = sh;
	army[O_BLACK].opponent = &army[O_WHITE];
	army[O_WHITE].opponent = &army[O_BLACK];

	// Set all squares to empty

	for( row = 0; row < BOARD_SIZE; row++ ) {

		for( col = 0; col < BOARD_SIZE; col++ ) {
			sh->board.ptr[row][col] = NULL;
		}
	}

	init_army( &army[O_WHITE], O_WHITE );
	init_army( &army[O_BLACK], O_BLACK );

	do {
		printf( "Game or study? (g/s): " );
		scanf( "%s", str );
		c = *(eat_spaces( str ));
		c = tolower( c );
	} while( c != 'g'  &&  c != 's' );

	if( c == 'g' ) {    // Play a normal game
		open_open_book( sh );
		init_army_pieces( &army[O_WHITE] );
		init_army_pieces( &army[O_BLACK] );
	} else {                    // Construct and play a study
		construct_study( army );
	}

	for( i = 0; i < 2; i++ ) {
		owner = &army[i];

		do {
			printf( "\n%s: human or computer (h/c): ",
			owner->name );
			scanf( "%s", str );
			c = *(eat_spaces( str ));
			c = tolower( c );
		} while( c != 'h'  &&  c != 'c' );

		if( c == 'c' ) {
			owner->auto_until = AUTO_INDEFINITELY;
			owner->move_proc = &computer_move;
			owner->type = PT_COMPUTER;
			init_computer_army( owner );
			// owner->max_ply = 3;
			// owner->max_ply_cap = 5;
		} else {
			owner->auto_until = 0;
			owner->move_proc = &human_move;
			owner->type = PT_HUMAN;
		}
	}

#if 0
	sh->resign_thresh = 8;
#else
	do {
		printf( "Resign threshold (3-%d): ", KING_VALUE );
		scanf( "%d", &sh->resign_thresh );
	} while( sh->resign_thresh < 3  ||  sh->resign_thresh > KING_VALUE );
#endif
} // init_board()


// ******** End of pdc_init.cpp ********
