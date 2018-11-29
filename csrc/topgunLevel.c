/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief レベルに関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h>
#include "topgun.h"
#include "topgunState.h"  /* STATE_3, STATE_9 */
#include "topgunLine.h"   /* LINE, LINE_TYPE, LINE_INFO */
#include "topgunAtpg.h"   /* JUST_INFO */
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h"  /* FEC_XX */

extern LINE_INFO  Line_info;
extern LINE       **Pi_head;
extern LINE       **Po_head;
extern LINE       **BBPi_head;
extern LINE       **BBPo_head;
extern LINE       *Line_head;
extern LINE       ***Lv_pi_head;
extern JUST_INFO  **Lv_head;

/* function list */
extern void level_init ( void );
static void level_pi_calc ( void );
static void level_po_calc ( void );
static void lv_head ( void );
static void level_pi_head ( void );


/* print function */
void  topgun_print_mes_lv_calc_result ( LINE * );
void  topgun_print_mes_n_lv_calc_result ( Ulong );

/* test function */
void  test_line_in_null ( LINE *, char * );
void  test_line_out_null ( LINE *, char * );
void  test_lv_pi_head ( Ulong *, char * );



/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-1
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Line_info   I/O    HEADER  main infomation
 * Pi_head       I/O    LINE**  PrimaryInput header
 * Line_head     I      LINE*   Line Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_init ( void ) {
    /* line.levelの計算 */
    level_pi_calc();

	/* lv_po */
	level_po_calc();

	/* $$$ どっちかに統一すべき $$$ */
	/* level_pi_headの作成 */
    level_pi_head();

	/* lv_headの作成 */
    lv_head();
}

/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-1
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Line_info   I/O    HEADER  main infomation
 * Pi_head       I/O    LINE**  PrimaryInput header
 * Line_head     I      LINE*   Line Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_pi_calc
(
 void
){

    Ulong      i       = 0; /* 外部入力数 */
    Ulong      j       = 0; /* 最大分岐数 */
    Ulong      max_lv  = 0; /* 最大レベル */
	Ulong      tmp_lv  = 0; /* レベルカウンタ */
    LINE       *lp     = NULL; /* レベルを求める信号線 */
	LINE       *lp_out = NULL; /* 対象信号線の出力側信号線 */
    LINE_STACK *head   = NULL; /* stackの先頭 */
	LINE_STACK *tail   = NULL; /* stackの最後尾 */
	LINE_STACK *ls_tmp = NULL; /* メモリ確保用ポインタ */
	LINE_STACK *ls_ptr = NULL; /* 探索用テンポラリ */

    char       *func_name = "level_pi_calc"; /* 関数名 */

    head = NULL;
    tail = NULL;

	/* TMの計算用に入力側の最多段数を求める MIN = 1, 0は未設定 */
    /* 入力値にXがある場合は切れている入力を足す必要あり */

    for ( i = 0; i < Line_info.n_pi; i++ ) {

		/* PIを1に設定 */
		lp = Pi_head[ i ];
		lp->lv_pi = 1;


		topgun_print_mes_lv_calc_result ( lp );

		/* 外部入力の出力信号線をstackにつむ */
		for ( j = 0 ; j < lp->n_out ; j++ ) {
			lp_out = lp->out[ j ];
			if( ( lp_out->flag & LINE_CHK ) == 0 ) {

				lp_out->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
														   sizeof( LINE_STACK ),
														   1, func_name );
				ls_tmp->line = lp_out;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

	/* BBpi */
    for ( i = 0; i < Line_info.n_bbpi; i++ ) {

		/* BBpiを1に設定 */
		lp = BBPi_head[ i ];
		lp->lv_pi = 1;

		topgun_print_mes_lv_calc_result ( lp );

		/* 外部入力の出力信号線をstackにつむ */
		for ( j = 0 ; j < lp->n_out ; j++ ) {
			lp_out = lp->out[ j ];
			if( ( lp_out->flag & LINE_CHK ) == 0 ) {
				lp_out->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
													   sizeof( LINE_STACK ),
													   1, func_name );

				ls_tmp->line = lp_out;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

    Line_info.max_lv_pi = 1;
    ls_tmp = head;

    /* initailze head & tail */
    while ( ls_tmp ) {
		ls_ptr = ls_tmp;
		ls_tmp = ls_tmp->next;
    }
    tail       = ls_ptr;

	/* main部分 */

	while ( head ) {

		ls_ptr = head;
		head   = head->next;

		if ( head  == NULL ) {
			tail = NULL;
		}

		/* lpのlv_piを决める */
		lp     = ls_ptr->line;
		test_line_in_null ( lp , func_name ) ;

		max_lv = 0;

		for ( i = 0 ; i < lp->n_in ; i++ ) {

			/* lpの入力レベルを確認 */
			tmp_lv = lp->in[ i ]->lv_pi;

			if ( tmp_lv == 0 ) {
				break; /* 未設定の入力が存在する */
			}

			if ( max_lv < tmp_lv ) {
				/* 最大値を更新 */
				max_lv = tmp_lv;
			}
		}

		/* ls_ptrのチェックは完了 */
		/* -> ls_ptrの出力を積む or stackの最下段に積む */

		/* lv_piの未設定の入力が一つでもある */
		/* stackの最終段に再設定 */
		if ( tmp_lv == 0 ){
			ls_ptr->next = NULL;
			if ( tail == NULL ) {
				head         = ls_ptr;
				tail         = ls_ptr;
			}
			else {
				tail->next   = ls_ptr;
				tail         = ls_ptr;
			}
		}
		/* lv_piがすべて設定ずみ
		   ls_ptrは解放*/
		else{

			/* 入力のlevelの最大値に1を足す */
			lp->lv_pi = ( max_lv + 1 );

			/* Line_info.max_lv の更新 */
			if ( Line_info.max_lv_pi < lp->lv_pi ) {
				Line_info.max_lv_pi = lp->lv_pi;
			}

			topgun_print_mes_lv_calc_result ( lp );

			test_line_out_null ( lp , func_name );

			/* lpの出力をhead/tailに積む */
			for ( j = 0; j < lp->n_out; j++ ) {

				lp_out = lp->out[ j ];

				/* LINE_CHK flagが立っていなるか */
				if ( ( lp_out->flag & LINE_CHK ) == 0 ) {
					/* flagが立っていなかったら */

					/* flagを立てて、head/tailに積む */
					lp_out->flag |= LINE_CHK;

					ls_tmp = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK,
										sizeof( LINE_STACK ),
										1, func_name);

					ls_tmp->line = lp_out;

					ls_tmp->next = NULL;
					if( tail != NULL ){
						tail->next   = ls_tmp;
					}
					else {
						head     = ls_tmp;
					}
					tail         = ls_tmp;
				}

			}
			topgunFree( ls_ptr, FMT_LINE_STACK, 1, func_name );
		}

		/* 保険 */
		if( head == (LINE_STACK *)NULL ){

			if(tail != NULL){
				head = tail;
				tail = tail->next;
			}
		}
    }

    /* level 0(PI/PPI)分を加算 */
    Line_info.max_lv_pi++;

    for ( i = 0; i < Line_info.n_line; i++ ) {
		Line_head[ i ].flag &= ~LINE_CHK;
	}
}

/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-1
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Line_info   I/O    HEADER  main infomation
 * Pi_head       I/O    LINE**  PrimaryInput header
 * Line_head     I      LINE*   Line Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_po_calc
(
 void
){

    Ulong      i       = 0; /* 外部入力数 */
    Ulong      j       = 0; /* 最大分岐数 */
    Ulong      max_lv  = 0; /* 最大レベル */
	Ulong      tmp_lv  = 0; /* レベルカウンタ */
    LINE       *lp     = NULL; /* レベルを求める信号線 */
	LINE       *lp_in  = NULL; /* 対象信号線の出力側信号線 */
    LINE_STACK *head   = NULL; /* stackの先頭 */
	LINE_STACK *tail   = NULL; /* stackの最後尾 */
	LINE_STACK *ls_tmp = NULL; /* メモリ確保用ポインタ */
	LINE_STACK *ls_ptr = NULL; /* 探索用テンポラリ */

    char       *func_name = "level_po_calc"; /* 関数名 */

    head = NULL;
    tail = NULL;

	/* TMの計算用に入力側の最多段数を求める MIN = 1, 0は未設定 */
    /* 入力値にXがある場合は切れている入力を足す必要あり */

    for ( i = 0; i < Line_info.n_po; i++ ) {

		/* PIを1に設定 */
		lp = Po_head[ i ];
		lp->lv_po = 1;


		topgun_print_mes_lv_calc_result ( lp );

		/* 外部入力の出力信号線をstackにつむ */
		for ( j = 0 ; j < lp->n_in ; j++ ) {
			lp_in = lp->in[ j ];
			if( ( lp_in->flag & LINE_CHK ) == 0 ) {

				lp_in->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
														   sizeof( LINE_STACK ),
														   1, func_name );
				ls_tmp->line = lp_in;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

	/* BBpo */
    for ( i = 0; i < Line_info.n_bbpo; i++ ) {

		/* BBpiを1に設定 */
		lp = BBPo_head[ i ];
		lp->lv_po = 1;
		
		topgun_print_mes_lv_calc_result ( lp );

		/* 外部入力の出力信号線をstackにつむ */
		for ( j = 0 ; j < lp->n_in ; j++ ) {
			lp_in = lp->in[ j ];
			if( ( lp_in->flag & LINE_CHK ) == 0 ) {
				lp_in->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
													   sizeof( LINE_STACK ),
													   1, func_name );

				ls_tmp->line = lp_in;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

    Line_info.max_lv_po = 1;
    ls_tmp = head;

    /* initailze head & tail */
    while ( ls_tmp ) {
		ls_ptr = ls_tmp;
		ls_tmp = ls_tmp->next;
    }
    tail       = ls_ptr;

	/* main部分 */

	while ( head ) {

		ls_ptr = head;
		head   = head->next;

		if ( head  == NULL ) {
			tail = NULL;
		}

		/* lpのlv_piを决める */
		lp     = ls_ptr->line;
		test_line_in_null ( lp , func_name ) ;

		max_lv = 0;

		for ( i = 0 ; i < lp->n_out ; i++ ) {

			/* lpの入力レベルを確認 */
			tmp_lv = lp->out[ i ]->lv_po;

			if ( tmp_lv == 0 ) {
				break; /* 未設定の入力が存在する */
			}

			if ( max_lv < tmp_lv ) {
				/* 最大値を更新 */
				max_lv = tmp_lv;
			}
		}

		/* ls_ptrのチェックは完了 */
		/* -> ls_ptrの出力を積む or stackの最下段に積む */

		/* lv_piの未設定の入力が一つでもある */
		/* stackの最終段に再設定 */
		if ( tmp_lv == 0 ){
			ls_ptr->next = NULL;
			if ( tail == NULL ) {
				head         = ls_ptr;
				tail         = ls_ptr;
			}
			else {
				tail->next   = ls_ptr;
				tail         = ls_ptr;
			}
		}
		/* lv_piがすべて設定ずみ
		   ls_ptrは解放*/
		else{

			/* 入力のlevelの最大値に1を足す */
			lp->lv_po = ( max_lv + 1 );

			/* Line_info.max_lv の更新 */
			if ( Line_info.max_lv_po < lp->lv_po ) {
				Line_info.max_lv_po = lp->lv_po;
			}

			topgun_print_mes_lv_calc_result ( lp );

			test_line_out_null ( lp , func_name );

			/* lpの出力をhead/tailに積む */
			for ( j = 0; j < lp->n_in; j++ ) {

				lp_in = lp->in[ j ];

				/* LINE_CHK flagが立っていなるか */
				if ( ( lp_in->flag & LINE_CHK ) == 0 ) {
					/* flagが立っていなかったら */

					/* flagを立てて、head/tailに積む */
					lp_in->flag |= LINE_CHK;

					ls_tmp = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK,
										sizeof( LINE_STACK ),
										1, func_name);

					ls_tmp->line = lp_in;

					ls_tmp->next = NULL;
					if( tail != NULL ){
						tail->next   = ls_tmp;
					}
					else {
						head     = ls_tmp;
					}
					tail         = ls_tmp;
				}

			}
			topgunFree( ls_ptr, FMT_LINE_STACK, 1, func_name );
		}

		/* 保険 */
		if( head == (LINE_STACK *)NULL ){

			if(tail != NULL){
				head = tail;
				tail = tail->next;
			}
		}
    }

    /* level 0(PI/PPI)分を加算 */
    Line_info.max_lv_po++;

    for ( i = 0; i < Line_info.n_line; i++ ) {
		Line_head[ i ].flag &= ~LINE_CHK;
	}
}

/********************************************************************************
 * << Function >>
 *		make Lv_head
 *
 * << Function ID >>
 *	   	3-?
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/09/07
 *
 ********************************************************************************/

void lv_head
(
 void
){
    Ulong   i;

	char    *func_name = "lv_head"; /* 関数名 */

    Lv_head = ( JUST_INFO **)topgunMalloc( FMT_JUST_INFO_P, sizeof( JUST_INFO *),
											 Line_info.max_lv_pi, func_name);

	/* n_lv_pi[ i ]と cnt[ i ]の初期化 */
    for ( i = 0; i < Line_info.max_lv_pi; i++ ) {
		Lv_head[i] = NULL;
    }

}

/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-2
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_pi_head
(
 void
){

    Ulong   i,j,lv_pi, *cnt_lv;

	char    *func_name = "level_pi_head";	/* 関数名  */

    Line_info.n_lv_pi = (Ulong *)topgunMalloc( FMT_Ulong, sizeof( Ulong ),
												  Line_info.max_lv_pi, func_name);

	cnt_lv = (Ulong *)topgunMalloc( FMT_Ulong, sizeof( Ulong ),
									 Line_info.max_lv_pi, func_name);

	/* n_lv_pi[ i ]と cnt[ i ]の初期化 */
    for ( i = 0; i < Line_info.max_lv_pi; i++ ) {
		Line_info.n_lv_pi[ i ] = 0;
		cnt_lv[ i ] = 0;
    }

	/* n_lv_pi[ i ]の計算 */
    for ( i = 0; i < Line_info.n_line; i++ ) {
		if ( ( &( Line_head[ i ] ) ) != NULL ) {
			Line_info.n_lv_pi[ Line_head[ i ].lv_pi ]++;
		}
    }

	/* print */
    for( i = 0; i < Line_info.max_lv_pi; i++ ) {
		topgun_print_mes_n_lv_calc_result ( i );
    }


    Lv_pi_head = ( LINE *** )topgunMalloc( FMT_LINE_PP, sizeof( LINE ** ),
											Line_info.max_lv_pi, func_name);

    for (i = 0; i < Line_info.max_lv_pi; i++){

		Lv_pi_head[ i ] = (LINE **)topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
												  Line_info.n_lv_pi[ i ],
												  func_name);
		for ( j = 0; j < Line_info.n_lv_pi[ i ]; j++) {
			Lv_pi_head[ i ][ j ] = NULL;
		}
    }

    for ( i = 0; i < Line_info.n_line; i++) {
		lv_pi = Line_head[ i ].lv_pi;
		Lv_pi_head[ lv_pi ][ cnt_lv[ lv_pi ] ] = &( Line_head[ i ] );
		cnt_lv[ lv_pi ]++;
    }

	test_lv_pi_head ( cnt_lv, func_name );

	topgunFree( cnt_lv, FMT_Ulong, Line_info.max_lv_pi, func_name);

}

/* End of File */
