// pdc_look.cpp

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "pdchess.h"


void undo_move( mod_data_type * mod_data ) {
	unsigned int i;
	army_type * owner = mod_data->pc[0]->owner;
	piece_type * cap_pc = mod_data->cap_pc, * pc;

	for( i = 0; i < 2; i++ ) {
		// Re-enable castling if ability lost
		// upon move being undone

		if( owner->castle_lost[i] == owner->sh->move_rec_idx ) {
			owner->castle_lost[i] = CAN_CASTLE_CONST;
		}

		if( (pc = mod_data->pc[i]) == NULL ) {
			continue;
		}

		*(pc->self_ptr) = NULL;
		*pc = mod_data->old_pc[i];
		*(pc->self_ptr) = pc;
	}

	if( cap_pc != NULL ) {
		cap_pc->captured = NOT_CAPTURED_CONST;
		*(cap_pc->self_ptr) = cap_pc;
	}

	owner->ep_pawn = mod_data->ep_pawn;
}


static bool add_move( move_type * move, move_assoc_list_type * move_alist, piece_type * cap_pc ) {

	unsigned int slot_num = (cap_pc != NULL) ? cap_pc->def->slot_num : ZERO_EFFECT_SLOT;
	slot_type * slot;

	assert( slot_num < NUM_ALIST_SLOTS );
	slot = &move_alist->slots[slot_num];

	if( slot_num == 0 ) {
		slot->len = 1;		// Causes shortcut in best_move()

		return false;
	}

	if( slot->len >= slot->size ) {
		slot->size += SLOT_SIZE_INC;
		printf( "Resizing slot %d to %d\n", slot_num, slot->size );
		slot->moves = (move_type *)realloc( slot->moves,
		slot->size * sizeof(move_type) );
	}

	slot->moves[slot->len++] = *move;

	return true;
}


static void all_promotions( move_type * move, move_assoc_list_type * move_alist, piece_type * cap_pc ) {

	move_type_type mvs[4] = { PROMOTE_TO_N, PROMOTE_TO_B, PROMOTE_TO_R, PROMOTE_TO_Q };
	unsigned int i;

	for( i = 0; i < 4; i++ ) {
		move->type = mvs[i];
		add_move( move, move_alist, cap_pc );
	}
}


static unsigned int all_valid_pawn_moves( piece_type * pc, move_assoc_list_type * move_alist ) {

	army_type * owner = pc->owner, * opponent = owner->opponent;
	unsigned int r1 = pc->row, c1 = pc->col, num_moves = 0, colour = owner->colour;
	int v = 2 * colour - 1;
	piece_type * ep = opponent->ep_pawn, * cap_pc, ** board_row = owner->sh->board.ptr[r1+v];
	move_type move;
	bool promote = (r1 == 5 * colour + 1) ? true : false;

	move.r1 = r1;
	move.c1 = c1;
	move.r2 = move.c2 = 0;
	move.type = REGULAR_MOVE;
	move.pc = pc;
	move.dstPP = NULL;

	if( is_square_clear( owner, r1 + v, c1 ) ) {
		move.r2 = r1 + v;
		move.c2 = c1;
		move.dstPP = &board_row[c1];

		if( promote ) {
			all_promotions( &move, move_alist, NULL );
			num_moves += 4;
		} else {
			add_move( &move, move_alist, NULL );
			num_moves++;

			if( r1 == 5*(1-owner->colour)+1  &&  is_square_clear( owner, r1 + 2*v, c1 ) ) {

				move.r2 += v;
				move.dstPP = &owner->sh->board.ptr[r1+2*v][c1];
				add_move( &move, move_alist, NULL );
				num_moves++;
			}
		}
	}

	cap_pc = read_board(owner, r1 + v, c1 - 1);

	if( c1 > 0
		&&  ((cap_pc != NULL //(((cap_pc = read_board(owner, r1 + v, c1 - 1)) != NULL
			&& cap_pc->owner == opponent)
		||  ((cap_pc = ep) != NULL
			&&  cap_pc == read_board(owner,r1,c1-1)))
	) {

		move.r2 = r1 + v;
		move.c2 = c1 - 1;
		move.dstPP = &board_row[c1-1];

		if( promote ) {
			all_promotions( &move, move_alist, cap_pc /* NULL */ );
			num_moves += 4;
		} else {
			add_move( &move, move_alist, cap_pc );
			num_moves++;
		}
	}

	if( c1 < BOARD_SIZE - 1
		&&  (((cap_pc = read_board(owner, r1 + v, c1+1 )) != NULL
			&& cap_pc->owner == opponent)
		||  ((cap_pc = ep) != NULL
			&&  cap_pc == read_board(owner,r1,c1+1)) )
	) {
		move.r2 = r1 + v;
		move.c2 = c1 + 1;
		move.dstPP = &board_row[c1+1];

		if( promote ) {
			all_promotions( &move, move_alist, cap_pc /* NULL */ );
			num_moves += 4;
		} else {
			add_move( &move, move_alist, cap_pc );
			num_moves++;
		}
	}

	return num_moves;
}


static unsigned int all_valid_moves( piece_type * pc, move_assoc_list_type * move_alist
) {
	int dr, dc;
	unsigned int num_moves = 0, r1 = pc->row, c1 = pc->col, i, row, col, num_vectors = pc->def->num_vectors;
	vector_type * vector, * vectors = pc->def->vectors;
	piece_type * other_pc;
	army_type * owner = pc->owner;
	move_type move;
	bool limited_moving = pc->def->limited_moving;
	board_type * board = &owner->sh->board;

	/* Explicit initialization of move */
	move.r1 = r1;
	move.c1 = c1;
	move.r2 = move.c2 = 0;
	move.type = REGULAR_MOVE;
	move.pc = pc;
	move.dstPP = NULL;

	for( i = 0; i < num_vectors; i++ ) {
		vector = &vectors[i];
		dr = vector->r;
		dc = vector->c;
		row = r1;
		col = c1;

		for( ; ; ) {
			row += dr;
			col += dc;

			if( row >= BOARD_SIZE  ||  col >= BOARD_SIZE ) {
				break;
			}

			other_pc = read_board( owner, row, col );

			if( other_pc != NULL  &&  other_pc->owner == owner ) {
				break;
			}

			move.r2 = row;
			move.c2 = col;
			move.dstPP = (other_pc != NULL) ? other_pc->self_ptr : &board->ptr[row][col];
			/* Will break out if king captured */

			if( !add_move( &move, move_alist, other_pc ) ) {
		        return num_moves;
			}

			num_moves++;

			/* Can capture opponent's piece; can't pass thru it */

			if( limited_moving  ||  other_pc != NULL ) {
				break;
			}
		}
	}

	return num_moves;
}


int best_move( army_type * root_owner, army_type * owner, unsigned int ply, int best_sibling, int prev_move_val
	/* Last 2 args for A-B pruning */
) {
	int effect, max_effect = -2*KING_VALUE;
	unsigned int i = 0, j, slot_value, * move_rec_idx,
		* nmm_ptr = &owner->sh->num_moves_made,
		* nmg_ptr = &owner->sh->num_moves_gend;
	static const int slot_values[NUM_ALIST_SLOTS] = { KING_VALUE,
		QUEEN_VALUE, ROOK_VALUE, BISHOP_VALUE, PAWN_VALUE, 0 };
	move_type * move, castle_move = { 0, 0, 0, 0 };
	move_assoc_list_type * alist = &owner->sh->alists[ply - 1];
	slot_type * slot, * best_move_slot = NULL;
	mod_data_type mod_data;
	piece_type * pc;

	/* init move_alist */

	for( i = 0; i < NUM_ALIST_SLOTS; i++ ) {
		alist->slots[i].len = 0;
	}

  // Construct prioritized association list
  // of possible moves

	if( ply == 1 ) {
		best_move_slot = &owner->sh->best_move_slot;
		best_move_slot->len = 0;
	}

	for( i = 0; i < owner->num_pieces; i++ ) {
		pc = &owner->pieces[i];

		if( !is_visible(pc) ) {
			continue;
		}

		*nmg_ptr += (*((pc->def->token == PAWN_TOKEN)
			? &all_valid_pawn_moves
			: &all_valid_moves))( pc, alist );

		if( alist->slots[0].len != 0 ) { /* Can capture king */

			if( ply <= 2 ) {
				i = -1;
			}

			return KING_VALUE;
		}
	} /* for each piece */

	/* Add castling moves to list */
	/* Can't capture during castling */
	// printf("best_move() : Add castling moves\n");

	if( is_castle_valid( owner, CASTLE_KSIDE ) ) {
		castle_move.type = CASTLE_KSIDE;
		add_move( &castle_move, alist, NULL );
	}

	if( is_castle_valid( owner, CASTLE_QSIDE ) ) {
		castle_move.type = CASTLE_QSIDE;
		add_move( &castle_move, alist, NULL );
	}

	move_rec_idx = &owner->sh->move_rec_idx;

	for( i = 1; i < NUM_ALIST_SLOTS; i++ ) {
		slot = &alist->slots[i];
		slot_value = slot_values[i];

		for( j = 0; j < slot->len; j++ ) {
			move = &slot->moves[j];

			/* Move */
			++*nmm_ptr;	/* Increment the number of moves made */
			move_piece( owner, move, &mod_data );
			effect = slot_value;

#if 0
			if( effect >= KING_VALUE ) {
				/* Do nothing */
			} else
#endif
			if( root_owner != NULL  &&  ply >= root_owner->max_ply_cap ) {
				// Counter the horizon effect.
				effect = (effect + 1) / 2;
			} else if( (root_owner == NULL && ply < 2 ) ||
				(root_owner != NULL && (ply < root_owner->max_ply ||
				effect > 0 ))) {

				/* if( opponent's king hasn't been taken )
				 * and ((mate search
				 * with ply < 2) or (real search and ply bounds OK))
				 */
				*move_rec_idx += 2;
				effect -= best_move( root_owner, owner->opponent, ply + 1, max_effect, effect );
				*move_rec_idx -= 2;
			}

			undo_move( &mod_data );

			if( effect > max_effect ) {
				max_effect = effect;

				if( ply == 1 ) {
					best_move_slot->len = 1;
					best_move_slot->moves[0] = *move;

					if( effect >= KING_VALUE/2 ) {
						break;
					}
				} else if( prev_move_val - effect < best_sibling
					/* ie. alpha-beta pruning */
					||  effect >= KING_VALUE/2 ) {

					return max_effect;
				}
			} else if( effect == max_effect  &&  ply == 1 ) {

				if( best_move_slot->len >= best_move_slot->size ) {
					best_move_slot->size += SLOT_SIZE_INC;
					best_move_slot->moves = (move_type *)realloc(
						best_move_slot->moves,
						best_move_slot->size * sizeof(move_type) );
				}

				best_move_slot->moves[best_move_slot->len++] = *move;
			}
		} /* for each move in slot */
	} /* for each slot */

	if( ply == 1  &&  root_owner != NULL ) {
		assert( best_move_slot->len > 0 );
		root_owner->opt_move = best_move_slot->moves[((unsigned int)rand()) % best_move_slot->len];
	}

	return( max_effect );
}


// **** End of pdc_look.cpp ****
