// opening-book.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pdchess.h"


void open_open_book( shared_type * sh ) {
	static const char * aaaaa6_mv[] = { "f1 c4", "e7 e6", "c4 b3",
		"b7 b5", "O-O", "f8 e7", "d1 f3", "d8 c7", "f3 g3",
		NULL };
	static open_type aaaaa6 = { aaaaa6_mv,
		"Short-Kasparov 16, PCA 1993", 0, NULL };

	static const char * aaaab6_mv[] = { "c1 e3", "e7 e6", "f2 f3",
		"b7 b5", NULL };
	static open_type aaaab6 = { aaaab6_mv,
		"Sicilian-Scheveningen", 0, NULL };

	static const char * aaaac6_mv[] = { "f2 f4", NULL };
	static open_type aaaac6 = { aaaac6_mv, "Sicilian-Najdorf",
		0, NULL };

	static const char * aaaa5_mv[] = { "a7 a6", NULL };
	static open_type * aaaa5_ch[] = { &aaaaa6, &aaaab6, &aaaac6 };
	static open_type aaaa5 = { aaaa5_mv, NULL, 3, aaaa5_ch };

	static const char * aaab5_mv[] = { "b8 c6", "f1 e2", "e7 e5", NULL };
	static open_type aaab5 = { aaab5_mv, "Sicilian-Boleslavsky", 0, NULL };

	static const char * aaa4_mv[] = { "g1 f3", "d7 d6", "d2 d4",
		"c5 d4", "f3 d4", "g8 f6", "b1 c3", NULL };
	static open_type * aaa4_ch[] = { &aaaa5, &aaab5 };
	static open_type aaa4 = { aaa4_mv, NULL, 2, aaa4_ch };

	static const char * aab4_mv[] = { "g2 g3", "d7 d5", "e4 d5",
		"d8 d5", "g1 f3",
		"c8 g4", "f1 g2", "d5 e6", "e1 f1", "b8 c6", "h2 h3",
		"g4 h5", "b1 c3",
		"g8 f6", "d2 d3", "e6 d7", "g3 g4", "h5 g6", NULL };
	static open_type aab4 = { aab4_mv, "Nadyrhanov-Stefansson", 0, NULL };

	static const char * aa3_mv[] = { "c7 c5", NULL };
	static open_type * aa3_ch[] = { &aaa4, &aab4 };
	static open_type aa3 = { aa3_mv, "Sicilian Defence", 2, aa3_ch };

	static const char * aba4_mv[] = { "f1 c4", "g8 f6", "d2 d3",
		"c7 c6", "g1 f3",
		"d7 d5", "c4 b3", "a7 a5", "b1 c3", "f8 b4", "a2 a3",
		"b4 c3", "b2 c3",
		"b8 d7", "e4 d5", "f6 d5", "O-O", "O-O", "f1 e1",
		"f8 e8", NULL };
	static open_type aba4 = { aba4_mv, "Bishop's (Kasparov-Bareev)", 0, NULL };

	static const char * abb4_mv[] = { "g1 f3", "b8 c6", "f1 b5",
		"a7 a6", "b5 a4",
		"g8 f6", "O-O", "f8 e7", "f1 e1", "b7 b5", "a4 b3",
		"O-O", "a2 a4",
		"b5 b4", "d2 d3", "d7 d6", "a4 a5", "c8 e6", NULL };
	static open_type abb4 = { abb4_mv, "Kasparov-Short 1, PCA 1993", 0, NULL };

	static const char * ab3_mv[] = { "e7 e5", NULL };
	static open_type * ab3_ch[] = { &aba4, &abb4 };
	static open_type ab3 = { ab3_mv, NULL, 2, ab3_ch };

	static const char * a2_mv[] = { "e2 e4", NULL };
	// ThAW 2020-03-04 : Testing:
	static open_type * a2_ch[] = { &aa3, &ab3 };
	static open_type a2 = { a2_mv, NULL, 2, a2_ch };
	// static open_type * a2_ch[] = { &aa3 };
	// static open_type a2 = { a2_mv, NULL, 1, a2_ch };

	static const char * ba3_mv[] = { "d7 d6", "g1 f3", "c8 g4",
		"c2 c4", "b8 d7",
		"b1 c3", "e7 e5", "e2 e3", "c7 c6", "h2 h3", "g4 h5",
		"f1 e2", "f8 e7",
		"O-O", "f7 f5", NULL };
	static open_type ba3 = { ba3_mv, NULL, 0, NULL };

	static const char * bba4_mv[] = { "c2 c4", "c7 c6", "g1 f3",
		"g8 f6", "b1 c3",
		"e7 e6", "e2 e3", "b8 d7", "d1 c2", "f8 d6", "b2 b3",
		"O-O", "f1 e2",
		"d5 c4", "b3 c4", "e6 e5", "O-O", "f8 e8", "f1 d1",
		NULL };
	static open_type bba4 = { bba4_mv, NULL, 0, NULL };

	static const char * bbb4_mv[] = { "g1 f3", "g8 f6", "c2 c4",
		"d5 c4", "b1 c3",
		"c7 c6", "a2 a4", "c8 g4", "f3 e5", "g4 h5", "g2 g3",
		"e7 e6", "f1 g2",
		"f8 b4", "e5 c4", "f6 d5", "d1 b3", "b8 a6", "O-O",
		"O-O", NULL };
	static open_type bbb4 = { bbb4_mv, NULL, 0, NULL };

	static const char * bb3_mv[] = { "d7 d5", NULL };
	static open_type * bb3_ch[] = { &bba4, &bbb4 };
	static open_type bb3 = { bb3_mv, NULL, 2, bb3_ch };

	static const char * bcaa5_mv[] = { "c7 c5", "d4 d5", "b7 b5",
		"c4 b5", "a7 a6",
		"b5 b6", "d8 b6", "b1 c3", "e7 e6", "e2 e4", "e6 d5",
		"e4 d5", "d7 d6",
		"g1 f3", "f8 e7", "f1 e2", "c8 g4", "f3 d2", "g4 e2",
		"d1 e2", "O-O",
		"O-O", NULL };
	static open_type bcaa5 = { bcaa5_mv, NULL, 0, NULL };

	static const char * bcab5_mv[] = { "g7 g6", "b1 c3", "f8 g7",
		"e2 e4", "d7 d6",
		"g1 f3", "O-O", "f1 e2", "e7 e5", "O-O", "b8 c6",
		"d4 d5", "c6 e7",
		"f3 e1", "f6 d7", "e1 d3", "f7 f5", NULL };
	static open_type bcab5 = { bcab5_mv, NULL, 0, NULL };

	static const char * bca4_mv[] = { "c2 c4", NULL };
	static open_type * bca4_ch[] = { &bcaa5, &bcab5 };
	static open_type bca4 = { bca4_mv, NULL, 2, bca4_ch };

	static const char * bcb4_mv[] = { "g1 f3", "e7 e6", "g2 g3",
		"b7 b5", "f1 g2",
		"c8 b7", "O-O", "h7 h6", "a2 a4", "b5 b4", "c2 c4",
		"d7 d6", "a4 a5",
		"b8 a6", "b1 d2", "f8 e7", "f1 e1", "O-O", NULL };
	static open_type bcb4 = { bcb4_mv, NULL, 0, NULL };

	static const char * bc3_mv[] = { "g8 f6", NULL };
	static open_type * bc3_ch[] = { &bca4, &bcb4 };
	static open_type bc3 = { bc3_mv, NULL, 2, bc3_ch };

	static const char * b2_mv[] = { "d2 d4", NULL };
	static open_type * b2_ch[] = { &ba3, &bb3, &bc3 };
	static open_type b2 = { b2_mv, NULL, 3, b2_ch };

	static const char * ca3_mv[] = { "e7 e5", "b1 c3", "g8 f6",
		"g1 f3", "b8 c6",
		"g2 g3", "f8 c5", "f1 g2", "O-O", "O-O", "d7 d6",
		"d2 d3", "h7 h6",
		"a2 a3", "a7 a6", "b2 b4", "c5 a7", "c1 b2", "c8 g4",
		NULL };
	static open_type ca3 = { ca3_mv, NULL, 0, NULL };

	static const char * cb3_mv[] = { "b8 c6", "b1 c3", "e7 e5",
		"g2 g3", "g7 g6",
		"f1 g2", "f8 g7", "d2 d3", "d7 d6", "e2 e4", NULL };
	static open_type cb3 = { cb3_mv, "Botvinnik System", 0, NULL };

	static const char * c2_mv[] = { "c2 c4", NULL };
	static open_type * c2_ch[] = { &ca3, &cb3 };
	static open_type c2 = { c2_mv, "English opening", 2, c2_ch };

	static const char * da3_mv[] = { "d7 d5", "c2 c4", "e7 e6",
		"d2 d4", "g8 f6",
		"b1 c3", "d5 c4", "d1 a4", "b8 d7", "e2 e4", "a7 a6",
		"f1 c4", "c7 c6",
		"a4 d1", "f8 e7", "O-O", "O-O", "a2 a4", "b7 b6", NULL };
	static open_type da3 = { da3_mv, NULL, 0, NULL };

	static const char * db3_mv[] = { "g8 f6", "c2 c4", "g7 g6",
		"b2 b3", "f8 g7",
		"c1 b2", "d7 d6", "g2 g3", "e7 e5", "f1 g2", "O-O",
		"O-O", "f8 e8",
		"d2 d3", "h7 h6", "b1 c3", "c7 c6", "a1 c1", "b8 a6",
		NULL };
	static open_type db3 = { db3_mv, NULL, 0, NULL };

	static const char * dc3_mv[] = { "c7 c5", "c2 c4", "b8 c6",
		"b1 c3", "g8 f6",
		"d2 d4", "c5 d4", "f3 d4", "e7 e6", "a2 a3", "c6 d4",
		"d1 d4", "b7 b6",
		"c1 f4", "f8 c5", "d4 d2", "O-O", "a1 d1", "c8 b7",
		NULL };
	static open_type dc3 = { dc3_mv, NULL, 0, NULL };

	static const char * d2_mv[] = { "g1 f3", NULL };
	static open_type * d2_ch[] = { &da3, &db3, &dc3 };
	static open_type d2 = { d2_mv, NULL, 3, d2_ch };

	static const char * a1_mv[] = { NULL };
	static open_type * a1_ch[] = { &c2, &d2, &b2, &a2 };
	static open_type a1 = { a1_mv, NULL, 4, a1_ch };

	sh->open_type_ptr = &a1;
	sh->open_move_ptr = a1.moves;
} // open_open_book()


// Return current opening move and advance pointer, if possible

const char * open_get_move( shared_type * sh ) {
	const char * rtn;
	const char ** open_move_ptr = sh->open_move_ptr;
	unsigned int i;
	open_type * open_type_ptr = sh->open_type_ptr;

	// if (open_type_ptr != NULL && open_type_ptr->name != NULL) {
	// 	printf( "Foo: Opening: %s\n", open_type_ptr->name );
	// }

	if( open_move_ptr == NULL ) {
		return NULL;
	}

	if( *open_move_ptr != NULL ) {
		rtn = *open_move_ptr;
		sh->open_move_ptr++;

		if (sh->open_move_ptr == NULL && open_type_ptr->name != NULL) {
			printf( "Opening: %s\n", open_type_ptr->name );
		}

		printf("Returning move: %s\n", rtn);

		return rtn;

		//	return( *sh->open_move_ptr++ );
	}

#if 0
	if( open_type_ptr->name != NULL ) {
		printf( "Opening: %s\n", open_type_ptr->name );
	}
#endif

	if( open_type_ptr->num_child == 0 ) {
		sh->open_move_ptr = NULL;
		printf( "open_get_move: childless node; book closed\n" );

		return NULL;
	}

	i = ((unsigned int)rand()) % open_type_ptr->num_child;
	open_type_ptr = sh->open_type_ptr = open_type_ptr->child[i];
	open_move_ptr = sh->open_move_ptr = open_type_ptr->moves;
	rtn = *open_move_ptr;
	sh->open_move_ptr++;

	if (open_type_ptr != NULL && open_type_ptr->name != NULL) {
		printf( "Foo 2: Opening: %s\n", open_type_ptr->name );
	}

	return rtn;
} // open_get_move()


static bool moves_match( const char * str, move_type * match_to_move ) {
	move_type local_move, match_to_move2 = *match_to_move;

	move_ext_to_int( str, &local_move );
	match_to_move2.pc = NULL;
	match_to_move2.dstPP = NULL;

	return memcmp( &local_move, &match_to_move2, sizeof( move_type ) ) ? false : true;
} // moves_match()


// This function sets open_move_ptr to point to the next move
// if 'move' matches, and to NULL otherwise.

void open_match_move( shared_type * sh, move_type * move_to_match ) {
	unsigned int i;
	const char ** open_move_ptr = sh->open_move_ptr;
	open_type * open_type_ptr = sh->open_type_ptr;

	if( open_move_ptr == NULL ) {
		return;
	}

	if( *open_move_ptr != NULL ) {

		if( moves_match( *open_move_ptr, move_to_match ) ) {
			sh->open_move_ptr++;
		} else {
			printf( "Move not matched; opening book closed\n" );
			sh->open_move_ptr = NULL;
		}

		return;
	}

#if 1
	if( open_type_ptr->name != NULL ) {
		printf( "Opening: %s\n", open_type_ptr->name );
	}
#endif

	for( i = 0; i < open_type_ptr->num_child; i++ ) {

		if( moves_match( open_type_ptr->child[i]->moves[0], move_to_match)) {

			open_type_ptr = sh->open_type_ptr = open_type_ptr->child[i];
			sh->open_move_ptr = open_type_ptr->moves + 1;

			return;
		}
	}

	printf( "Move not matched; opening book closed\n" );
	sh->open_move_ptr = NULL;
} // open_match_move()


// ******** End of pdc_open.cpp ********
