// pdchess.cpp

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "pdchess.h"

void free_stuff( shared_type * sh ) {
	unsigned int i, j;
	move_assoc_list_type * alist;
	slot_type * slot;

	free( sh->move_rec );
	free( sh->best_move_slot.moves );

	//  printf( "Slot sizes:\n" );
	//  printf(
	//    "Ply       K       Q       R     B|N       P       0\n" );

	for( i = 0; i < MAX_MAX_PLY; i++ ) {
		//    printf( "%3d", i );
		alist = &sh->alists[i];

		for( j = 0; j < NUM_ALIST_SLOTS; j++ ) {
			slot = &alist->slots[j];
			free( slot->moves );
			//      printf( "%8d", slot->size );
		}

		//    printf( "\n" );
	}
} // free_stuff()

// This function is probably a bottleneck
// Inline it???

static piece_type * pc_at_coords( army_type * owner, unsigned int r, unsigned int c ) {
	piece_type * rtn = read_board(owner, r, c);

	// No need to check piece visibility
	return( (rtn != NULL  &&  rtn->owner == owner) ? rtn : NULL );
} // pc_at_coords()


// static bool primitive_move( army_type * owner,
// 	unsigned int r1, unsigned int c1,
// 	unsigned int r2, unsigned int c2 ) {
// 	piece_type * pc;

// 	if( (pc = pc_at_coords(owner,r1,c1)) == NULL ) {
// 		return false;
// 	}

// 	pc->row = r2;
// 	pc->col = c2;
// 	write_board(owner,r1,c1,NULL);
// 	write_board(owner,r2,c2,pc);

// 	return true;
// } // primitive_move()

unsigned int army_value( army_type * owner ) {
	unsigned int i, total = 0;
	piece_type * pc;

	for( i = 0; i < owner->num_pieces; i++ ) {
		pc = &owner->pieces[i];

		if( !is_visible(pc) ) {
			continue;
		}

		total += pc->def->points;
	}

	return total;
} // army_value()

// Is owner attacking square?

static bool is_attacked( army_type * owner, unsigned int row, unsigned int col ) {
	unsigned int i;
	piece_type * pc;
	/* is_valid_move doesn't use .dstPP (???) */
	move_type move;
	/* = { 0, 0, row, col, REGULAR_MOVE, NULL, NULL }; */
	/* &owner->sh->board.ptr[row][col] }; */

	move.r2 = row;
	move.c2 = col;
	move.type = REGULAR_MOVE;
	move.dstPP = NULL;

	for( i = 0; i < owner->num_pieces; i++ ) {
		pc = &owner->pieces[i];

		if( !is_visible(pc) ) {
			continue;
		}

		move.r1 = pc->row;
		move.c1 = pc->col;
		move.pc = pc;

		if( is_valid_move( owner, &move ) ) {
			return true;
		}
	}

	return false;
} // is_attacked()

bool is_castle_valid( army_type * owner, move_type_type type ) {
	unsigned int row = 7 * (1 - owner->colour), kc = 4, rc, i, k_dst, side = (type == CASTLE_KSIDE) ? 0 : 1;
	int inc;
	piece_type * kpc, * rpc, ** board_row = owner->sh->board.ptr[row];

	if( !havent_castled(owner,side) ) {
		return false;
	}

	if( type == CASTLE_KSIDE ) {
		rc = 7;
		k_dst = 6;
		inc = 1;
	} else {
		rc = 0;
		k_dst = 2;
		inc = -1;
	}

	kpc = pc_at_coords( owner, row, kc );
	rpc = pc_at_coords( owner, row, rc );

	if( kpc == NULL  ||  rpc == NULL
		||  kpc->def->token != KING_TOKEN
		||  rpc->def->token != ROOK_TOKEN ) {

		return false;
	}

	// Must be no pieces between K and R

	for( i = kc + inc; i != rc; i += inc ) {

		if( board_row[i] != NULL ) {
			return false;
		}
	}

	// No square from/through/to which king moves may be attacked

	for( i = kc; i != k_dst + inc; i += inc ) {

		if( is_attacked( owner->opponent, row, i ) ) {
			return false;
		}
	}

	return true;
} // is_castle_valid()


bool is_valid_move( army_type * owner, move_type * move ) {
	int drow, dcol, v_pawn, rowdiff, coldiff;
	unsigned int i, row, col, r1 = move->r1, c1, r2, c2, num_vectors;
	piece_type * pc, * ep_pawn, * other_pc;
	piece_def_type * def;
	vector_type * vectors;
	army_type * opponent;

	if( move->type == CASTLE_KSIDE ) {
		return is_castle_valid( owner, CASTLE_KSIDE );
	} else if( move->type == CASTLE_QSIDE ) {
		return is_castle_valid( owner, CASTLE_QSIDE );
	}

	c1 = move->c1;
	r2 = move->r2;
	c2 = move->c2;

	if( ( r1 == r2  &&  c1 == c2 )
		||  r1 >= BOARD_SIZE  ||  c1 >= BOARD_SIZE
		||  r2 >= BOARD_SIZE  ||  c2 >= BOARD_SIZE
	) {
		return false;
	}

	if( (pc = pc_at_coords( owner, r1, c1 )) == NULL ) {
		return false;
	}

	/* => pc = move->pc */

	def = pc->def;

	if( def->token == PAWN_TOKEN ) {       /* Pawn */
		/* is_square_clear( owner, r1+v_pawn, c1 ) not stored
		 * in var because it is calculated at most once in the
		 * following expression:
		 */
		opponent = owner->opponent;
		v_pawn = 2 * owner->colour - 1; /* -1 for white, 1 for black */

		return( (
			( r1 == 5*(1-owner->colour)+1  &&  c2 == c1
			&&  r2 == r1 + 2*v_pawn
			&& is_square_clear( owner, r1+v_pawn, c1 )
			&& is_square_clear( owner, r2, c1 ) )
			|| ( r2 == r1 + v_pawn  &&
			( ( c1 == c2  &&  is_square_clear( owner, r2, c2 ) )
			/* ... ie. move ahead 1 */
			|| ( abs( (int)(c2 - c1) ) == 1  &&
			( pc_at_coords( opponent, r2, c2 ) != NULL
			|| ((ep_pawn = opponent->ep_pawn) != NULL /* En Passant */
			&&  ep_pawn == pc_at_coords( opponent, r1, c2 ))))))
		) ? true : false );
	}

	/* Moves that get this far are neither castling nor pawn moves */
	rowdiff = r2 - r1;
	coldiff = c2 - c1;
	num_vectors = def->num_vectors;
	vectors = def->vectors;

	for( i = 0; i < num_vectors; i++ ) {
		drow = vectors[i].r;
		dcol = vectors[i].c;

		if( rowdiff*drow < 0  ||  coldiff*dcol < 0  ||  rowdiff*dcol != coldiff*drow
		) {
			continue; /* Wrong direction */
		}

		row = r1;
		col = c1;

		for( ; ; ) {
			row += drow;
			col += dcol;

			if( row >= BOARD_SIZE  ||  col >= BOARD_SIZE ) {
				break;
			}

			other_pc = read_board( owner, row, col );

			/* Can't capture own piece */

			if( other_pc != NULL  &&  other_pc->owner == owner ) {
		        break;
			}

			if( row == r2  &&  col == c2 ) {
				return true;
			}

			if( other_pc != NULL  // Can't go thru opponent's piece
				||  pc->def->limited_moving
			) {
				break;
			}
		}
	}

	return false;
} // is_valid_move()


bool is_check( army_type * owner ) {
	unsigned int i;
	piece_type * pc;

	/* Use pointer to king piece in army_type ? */

	for( i = 0; i < owner->num_pieces; i++ ) {
		pc = &owner->pieces[i];

		if( pc->def->token == KING_TOKEN ) {
			return is_attacked( owner->opponent, pc->row, pc->col );
		}
	}

	printf( "Error in is_check: No king found\n" );

	return false;
} // is_check()


bool is_draw( army_type * owner ) {
	unsigned int cycle_len, move_rec_idx = owner->sh->move_rec_idx;
	char * move_rec = owner->sh->move_rec, * move_rec_ptr = move_rec + move_rec_idx;

	if( owner->sh->move_rec_idx >= owner->sh->capture_by_or_draw ) {
		printf("Game is drawn due to lack of captures for 50 moves\n\n");

		return true;
	}

	/* Search for a cycle of moves repeated 3 times.
	* Cycles are at least 8 bytes (2 moves) long.
	* cycle_len <= 100 (ie. 25 moves) because no captures can
	* occur in 2nd and 3rd iterations (see Thm. prf.).  Longer
	* cycles would be caught by
	* the (>= 50 moves wo. capture => draw) provision above.
	*/

	for(cycle_len = 8; cycle_len <= 100  &&  3 * cycle_len <= move_rec_idx; cycle_len += 4 ) {

		if( !memcmp( move_rec_ptr - cycle_len, move_rec_ptr - 2 * cycle_len, cycle_len )
			&&  !memcmp( move_rec_ptr - cycle_len, move_rec_ptr - 3 * cycle_len, cycle_len )
		) {
			printf( "Game is drawn due to move repetition\n\n" );

			return true;
		}
	}

	return false;
} // is_draw()


static void record_move( army_type * owner, move_type * move ) {
	char * move_rec;
	unsigned int move_rec_idx = owner->sh->move_rec_idx;

	if( move_rec_idx >= owner->sh->move_rec_size ) {
		owner->sh->move_rec = (char *)realloc( owner->sh->move_rec,
		owner->sh->move_rec_size += INC_MOVE_REC_SIZE );
	}

	move_rec = owner->sh->move_rec;

	if( move->type == CASTLE_KSIDE ) {
		move_rec[move_rec_idx] = 64;
	} else if( move->type == CASTLE_QSIDE ) {
		move_rec[move_rec_idx] = 72;
	} else {
		move_rec[move_rec_idx] = move->r1 * 8 + move->c1;
		move_rec[move_rec_idx + 1] = move->r2 * 8 + move->c2;
	}

	owner->sh->move_rec_idx += 2;
} // record_move()


static void castle( army_type * owner, move_type * move, mod_data_type * mod_data
) {
	unsigned int row = 7*(1-owner->colour), kc = 4, rc, k_dst, r_dst;
	piece_type * kpc, * rpc, ** board_row;

	if( move->type == CASTLE_KSIDE ) {
		rc = 7;
		k_dst = 6;
		r_dst = 5;
	} else {
		rc = 0;
		k_dst = 2;
		r_dst = 3;
	}

	kpc = pc_at_coords( owner, row, kc );
	rpc = pc_at_coords( owner, row, rc );

	if( mod_data != NULL ) {
		mod_data->pc[0] = kpc;
		mod_data->old_pc[0] = *kpc;
		mod_data->pc[1] = rpc;
		mod_data->old_pc[1] = *rpc;
		mod_data->cap_pc = NULL;
		mod_data->ep_pawn = owner->ep_pawn;
	}

	owner->ep_pawn = NULL;

#if 1
	*(kpc->self_ptr) = *(rpc->self_ptr) = NULL;
	board_row = owner->sh->board.ptr[row];
	kpc->self_ptr = &board_row[k_dst];
	rpc->self_ptr = &board_row[r_dst];
	*(kpc->self_ptr) = kpc;
	*(rpc->self_ptr) = rpc;
#else
	write_board(owner,row,kc,NULL);
	write_board(owner,row,rc,NULL);
	write_board(owner,row,k_dst,kpc);
	write_board(owner,row,r_dst,rpc);
#endif
	kpc->col = k_dst;
	rpc->col = r_dst;

	owner->castle_lost[KINGSIDE] = owner->castle_lost[QUEENSIDE] = owner->sh->move_rec_idx;

	if( mod_data == NULL /* ie. !virtual_capture */ ) {
		record_move( owner, move );
	}
} // castle()


void move_piece( army_type * owner, move_type * move, mod_data_type * mod_data
) {
	unsigned int r1 = move->r1, c1, r2, c2, colour;
	piece_type * pc, * cap_pc;
	char token;
	shared_type * sh = owner->sh;

	if( move->type == CASTLE_KSIDE ||  move->type == CASTLE_QSIDE ) {
		castle( owner, move, mod_data );

		return;
	}

	c1 = move->c1;
	r2 = move->r2;
	c2 = move->c2;
	pc = move->pc;      /* read_board(owner,r1,c1); */
	token = pc->def->token;

	assert( move->dstPP != NULL );

	if( (cap_pc = *(move->dstPP)) == NULL
		&&  token == PAWN_TOKEN  &&  c1 != c2
		&&  (cap_pc = read_board(owner,r1,c2)) != NULL
	) {
		move->type = EN_PASSANT;
		// ep_capture = true;
	}

	if( mod_data != NULL ) {
		mod_data->pc[0] = pc;
		mod_data->old_pc[0] = *pc;
		mod_data->pc[1] = NULL;
		mod_data->cap_pc = cap_pc;
		mod_data->ep_pawn = owner->ep_pawn;
	}

	pc->row = r2;
	pc->col = c2;
	*(pc->self_ptr) = NULL;
	*(pc->self_ptr = move->dstPP) = pc;

	if( move->type == EN_PASSANT ) {
		*(cap_pc->self_ptr) = NULL;
	}

	owner->ep_pawn = NULL;

	switch( token ) {
		case KING_TOKEN:
			disallow_castling(owner,KINGSIDE);
			disallow_castling(owner,QUEENSIDE);
			break;

		case ROOK_TOKEN:

			if( r1 != 7*(1-owner->colour)) {
				break;
			}

			if( c1 == 0 ) {
				disallow_castling(owner,QUEENSIDE);
			} else if( c1 == BOARD_SIZE - 1 ) {
				disallow_castling(owner,KINGSIDE);
			}

			break;

		case PAWN_TOKEN:
			colour = owner->colour;

			if( r2 == 7*colour ) {
				/* Promote pawn.
				* move type indices must match pc def indices (N,B,R,Q):
				*/
				pc->def = &sh->pc_def[(unsigned int)(move->type)];
				pc->promoted = sh->move_rec_idx;
			} else if( r1 == 5*(1-colour)+1  &&  r2 == 4 - colour ) {
				owner->ep_pawn = pc;
			}

			break;
	} // switch

	if( cap_pc != NULL ) {      // Hide captured piece, if any
		cap_pc->captured = sh->move_rec_idx;

		if( mod_data == NULL  /* ie. !virtual_capture */ ) {
			sprintf( sh->CaptureMsg, "%s's %s takes %s's %s",
			owner->name, pc->def->name,
			cap_pc->owner->name, cap_pc->def->name );
			sh->capture_by_or_draw = sh->move_rec_idx + 202;
		}
	} else if( mod_data == NULL ) {
		// ie. no capture during non-virtual move
		sh->CaptureMsg[0] = '\0';
	}

	if( mod_data == NULL ) {
		record_move( owner, move );
	}
} // move_piece()


#if 0
void backup( army_type * owner ) {
	char * rec_ptr;
	unsigned int i, move_rec_idx, r1, c1, r2, c2, base_row;
	piece_type * pc, * opponent_pieces;
	army_type * opponent = owner->opponent;

	if( owner->sh->move_rec_idx < 2 ) {
		return;
	}

	// printf( "backup(): begin\n" );
	move_rec_idx = owner->sh->move_rec_idx -= 2;
	rec_ptr = owner->sh->move_rec + move_rec_idx;
	// printf( "Restoring castling capabilities...\n" );

	for( i = 0; i < 2; i++ ) {

		if( owner->castle_lost[i] == move_rec_idx ) {
			owner->castle_lost[i] = CAN_CASTLE_CONST;
		}
	}

	// printf( "Getting move coords...\n" );
	r1 = rec_ptr[0] / 8;
	c1 = rec_ptr[0] % 8;
	r2 = rec_ptr[1] / 8;
	c2 = rec_ptr[1] % 8;
	base_row = 7 * (1 - owner->colour );

	if( r1 == BOARD_SIZE ) {
		// printf( "Undo a kingside castle...\n" );
		primitive_move(owner,base_row,6,base_row,4);
		primitive_move(owner,base_row,5,base_row,7);
	} else if( r1 == BOARD_SIZE + 1 ) {
		// printf( "Undo a queenside castle...\n" );
		primitive_move(owner,base_row,2,base_row,4);
		primitive_move(owner,base_row,3,base_row,0);
	} else {
		// printf( "Undo a non-castle move...\n" );
		primitive_move(owner,r2,c2,r1,c1);      /* Move piece */

		pc = read_board(owner,r1,c1);

		if( pc->promoted == move_rec_idx ) {
			// printf( "Undo a promotion\n" );
			pc->promoted = NOT_PROMOTED_CONST;
			pc->def = &owner->sh->pc_def[0];
		}

		opponent_pieces = owner->opponent->pieces;

		/* Restore any captured piece */
		// printf( "Restore any captured piece...\n" );

		for( i = 0; i < owner->opponent->num_pieces; i++ ) {
			pc = &opponent_pieces[i];

			if( pc->captured == move_rec_idx ) {
				// printf( "Restoring a captured piece...\n" );
				pc->captured = NOT_CAPTURED_CONST;
				write_board(owner,r2,c2,pc);
				break;
			}
		}
	}

	/* Restore opponent's ep_pawn */

	if( move_rec_idx < 2 ) {
		opponent->ep_pawn = NULL;
	} else {
		// printf( "Restore opponent's ep_pawn...\n" );
		r1 = (*(rec_ptr - 2)) / 8;
		r2 = (*(rec_ptr - 1)) / 8;
		c2 = (*(rec_ptr - 1)) % 8;
		pc = pc_at_coords(opponent,r2,c2);
		opponent->ep_pawn = (pc != NULL
			&&  pc->def->token == PAWN_TOKEN
			&&  r1 == 5 *(1-opponent->colour)+1
			&&  r2 == 4 - opponent->colour)
			? pc : NULL;
	}

	// printf( "backup(): done\n" );
} // backup()
#endif


void print_board( army_type * owner ) {
	char ch;
	int row, col;
	piece_type * pc;

	putchar( '\n' );

	for( row = 0; row < BOARD_SIZE; row++ ) {

		for( col = 0; col < BOARD_SIZE; col++ ) {
			pc = owner->sh->board.ptr[row][col];

			if( pc == NULL  ||  pc->owner == NULL ) {
				ch = ((row + col) & 1) ? '+' : '-';
			} else {
		        ch = pc->def->token;

		        if( pc->owner->colour == O_BLACK ) {
					ch = _tolower( ch );
				}
			}

			putchar( ch );
		}

		putchar( '\n' );
	}

	putchar( '\n' );
} // print_board()


int main( void ) {
	const char * format_str;
	bool check, mate;
	static army_type army[2];
	army_type * owner = &army[0];

	init_board( army );

	for( ; ; ) {          /* Main loop */
		print_board( owner );

		if( is_check( owner->opponent ) ) {
			printf( "Semantic error: %s could capture %s's king\n\n", owner->name, owner->opponent->name );
			break;
		}

		printf( "Move #%d\n", owner->sh->move_rec_idx / 4 + 1 );

		if( !(*(owner->move_proc))( &owner )  ||  is_draw( owner ) ) {
			break;
		}

		check = is_check( owner->opponent );
		mate = is_checkmate( owner->opponent );
		format_str = (check)
			? ((mate)
				? "%s has been checkmated\n"
				: "%s is in check\n")
			: ((mate)
				? "%s has been stalemated\n" : NULL);

		if( format_str != NULL ) {
			printf( format_str, owner->opponent->name );

			if( mate ) {
				print_board( owner );
				break;
			}
		}

		owner = owner->opponent;
	}

	print_move_rec( owner->sh );
	free_stuff( owner->sh );
	printf( "Game complete\n" );

	// MSVC: Wait for ENTER before closing window.
	// fflush( stdin );
	// getchar();

	return( 0 /* Or whatever is interpreted as "OK" */ );
} // main()


// ******** End of pdchess.cpp ********
