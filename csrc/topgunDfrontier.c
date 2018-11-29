/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief Dフロンティアに関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2005 / 01 / 20   initialize
		2007 / 06 / 01   imp_id_n & imp_id_f
		2016 / 12 / 12   initialize
*/

#include<stdio.h>
#include "topgun.h"
#include "topgunState.h" /* STATE_3 , STATE_9 */
#include "topgunLine.h"  /* LINE , LINE_TYPE, LINE_INFO */
#include "topgunAtpg.h"  /* LINE_LIST */
#include "topgunTm.h"    /* TM_XXX */
#include "topgunMemory.h" /* FMT_DFORNT */
#include "topgunError.h"  /* FEC_XXXX */

extern	GENE_HEAD	Gene_head;


/* function list */
extern void   geneg_init_dfront ( LINE *fault_line ); /* dfront treeの初期化 */
extern GENE_RESULT atpg_drive_dfront( void );
extern void atpg_ent_propa_dfront( ASS_LIST * );
extern void gene_reflesh_state_dfront( DFRONT * );
extern void gene_imp_trace_2_dfront( Ulong );


/* other function list */
void   atpg_reflesh_state( LINE *, int );
int    atpg_get_d_state9( STATE_9 );
GENE_RESULT atpg_make_dfront( DFRONT * );
ASS_LIST *utility_combine_ass_list( ASS_LIST *, ASS_LIST *);

/* print function */
void   topgun_print_mes_id_type_state9( LINE * );
void   topgun_print_mes( int, int );
void   topgun_print_mes_n();
void   topgun_print_mes_result_line( int, int, LINE * );
void   topgun_print_mes_id_pre_state9 ( LINE *, CONDITION );
void   topgun_print_mes_id_after_state9 ( LINE *, CONDITION );
void   topgun_print_mes_reflesh_normal( Ulong , Ulong);
void   topgun_print_mes_reflesh_failure( Ulong , Ulong);

/* test function */
void  test_line_null ( LINE *, char * );
void  test_ass_list ( ASS_LIST *, char * );
void  test_dfront ( DFRONT *, char  * );


/********************************************************************************
 * << Function >>
 *		enter D-frontier
 *
 * << Function ID >>
 *    	18-1
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line poiter of d-frontier
 *
 * << extern >>
 * name			(I/O)	type	    description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

void gene_init_dfront
(
 LINE *fault_line        
 ){

	Ulong     i = 0;             /* 信号線の出力数カウンタ */
	DFRONT 	  *dfront    = NULL; /* dfront用メモリポインタ */
	DFRONT    *child     = NULL; /* 出力先dfront用メモリポインタ */
	DFRONT    **children = NULL; /* 出力先dfront用メモリポインタのポインタ */
	
	char     *func_name = "gene_init_dfront"; /* 関数名 */

	topgun_print_mes_result_line( 23, 0, fault_line );

	test_line_null ( fault_line, func_name );

    /* D-frontier用topノードメモリ確保 */
	dfront = ( DFRONT * )topgunMalloc( FMT_DFRONT, sizeof( DFRONT ),
										1, func_name );

	/* dfront の初期化 */
	dfront->flag      = DF_ROOT;
    dfront->line      = fault_line;
    dfront->ass_list  = NULL;
	dfront->last_n    = 0;
	dfront->last_f    = 0;
	dfront->cnt_n     = 0;
	dfront->cnt_f     = 0;
	dfront->prev      = NULL;
	dfront->next      = NULL;


	/* 出力方向へ探索するD-Frontierのメモリを確保 */
	children = ( DFRONT ** )topgunMalloc( FMT_DFRONT_P, sizeof( DFRONT * ),
										   fault_line->n_out, func_name );

	for ( i = 0; i < fault_line->n_out; i++ ) {

		/* 子供用D-frontier用メモり確保 */
		child = ( DFRONT * )topgunMalloc( FMT_DFRONT, sizeof( DFRONT ),
										   1, func_name );

		/* child の初期化 */
		/* $$$ branchの順番が可観測費の順になってれば楽 $$$ */
		child->flag      = DF_NOCHECK;
		child->line      = fault_line->out[ i ];
		child->ass_list  = NULL;
		child->last_n    = 0;
		child->last_f    = 0;
		child->cnt_n     = 0;
		child->cnt_f     = 0;
		child->prev      = dfront;
		child->next      = NULL;

		children[ i ]   = child;
	}
	dfront->next = children;

	/* D-frontier リストに登録 */

#ifndef TOPGUN_TEST
    if ( Gene_head.dfront_t != NULL ) {
		topgun_error( FEC_PRG_ATPG_HEAD, func_name );
	}
#endif /* TOPGUN_TEST */

	Gene_head.dfront_t  = dfront;
	Gene_head.dfront_c  = dfront;
	/* 普通は不要のはず */
	Gene_head.last_n    = 0;
	Gene_head.last_f    = 0;
	Gene_head.cnt_n     = 0;
	Gene_head.cnt_f     = 0;

	/* back trace tree に登録 */
	/* dfrontierのassignの登録方法を変更したため不要 2005/04/07
	if(atpg_ent_dfront_btree(dfront) == ERROR){
		return(ERROR);
	}
	*/
}

/********************************************************************************
 * << Function >>
 *		dfrontier root check
 *
 * << Function ID >>
 *      18-2
 *
 * << Return >>
 *	DRIVE_CONT	: D-Drive cotinue
 *	DRIVE_END	: D-Drive end
 *	DRIVE_IMPOS : D-Drive impossible
 *
 * << Argument >>
 * args			(I/O)	type			description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/25
 *
 ********************************************************************************/

GENE_RESULT atpg_drive_dfront
(
 void
){

	GENE_RESULT result; /* 故障伝搬の結果 */
	DFRONT      *df;    /* dforntier */

	char    *func_name = "atpg_drive_dfront"; /* 関数名 */

	topgun_print_mes( 23, 2 );

	/* dfrontierリストの作成 */
	if ( Gene_head.dfront_c == NULL ) {
		/* dfrontierが消滅 */
		/* 冗長もしくはuntest */
		return( DRIVE_IMPOS );
	}

	while ( 1 ) {
		
		df = Gene_head.dfront_c;

		topgun_print_mes_id_type_state9( df->line );

		/* 現在のdfrontierのstateを調べる */

		result = atpg_get_d_state9( df->line->state9 );

		if ( result == (GENE_RESULT) DRIVE_POINT ) {
			/* df->lineにはDが伝搬している */
			/* 伝搬経路を追って、Dが一番進んでいるところを探す */

			topgun_print_mes( 23, 3 );

			/* $$$ チェックする順番には課題あり $$$ */
			if ( df->line->n_out == 0 ) {
				/* dfrontierが消滅 */
				if ( df->line->type == TOPGUN_PO ) {
					/* df(dfrontier)が外部出力に到達 = 故障伝搬完了 */
					Gene_head.dfront_c->line = df->line;
					Gene_head.propa_line = df->line;
					Gene_head.flag |= ATPG_PROPA_E;
					return ( DRIVE_END );
				}
				else if ( df->line->type == TOPGUN_BLKO ) {
					return( DRIVE_IMPOS );
				}
				/* $$$ 別のエラーの可能性あり $$$ */
				topgun_print_mes( 23, 4 );
				return( DRIVE_IMPOS );
			}
			else if( df->line->n_out > 1 ){

				/* 複数ある時 */
				topgun_print_mes( 23, 6 );

				/* dfront->nextから選択し前進させる */
				result = atpg_make_dfront( df );
				if ( result == (GENE_RESULT) DRIVE_IMPOS ) {
					return( DRIVE_IMPOS );
				}
			}
			else if( df->line->n_out == 1 ) {

				test_line_null ( df->line->out[0], func_name );

				/* 単方向の場合 */
				topgun_print_mes( 23, 5 );

				/* 次へDfrontierを進める */
				result = atpg_make_dfront( df );
				if ( result == (GENE_RESULT)DRIVE_IMPOS ) {
					return( DRIVE_IMPOS ) ;
				}
				topgun_print_mes_id_type_state9( df->line );
			}
			else{
				/* 0でも1以上でも1でもない場合 */
				topgun_error( FEC_CIRCUIT_NO_LINE, func_name );
			}
		}
		else{
		    /* result ==DRIVE_CONT
			   df->lineにはDが伝搬してない
			   次伝搬させる箇所そのもの */
			/* result ==DRIVE_IMPOS
			   既に値が固定、伝搬できない */
			return( result );
		}
	}
}

/********************************************************************************
 * << Function >>
 *		make new D-frontier data structure
 *
 * << Function ID >>
 *    	18-4
 *
 * << Return >>
 *	DRIVE_IMPOS : D-Drive impossible
 *	DRIVE_CONT	: D-Drive cotinue
 *
 * << Argument >>
 * args			(I/O)	type			description
 * dfront		 I		DFRONT *		making dfrontier point
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/28
 *
 ********************************************************************************/

GENE_RESULT atpg_make_dfront
(
 DFRONT *dfront
 ){

	Ulong  min_tm_co; /* branchのなかで最小の可観測費 */
	Ulong  min_cnt_propa; /* branchのなかで最小の伝搬回数 */
	Ulong  br;        /* branchの順番 */
	Ulong  i;        /* buranchの数 */

	DFRONT *d_next; /* 未処理のDfrontierへのポインタ */
	DFRONT *child;     /* 子供のdfrontへのポインタ */
	DFRONT **children; /* 子供のdfrontを束ねるポインタ */

	char   *func_name = "atpg_make_dfront"; /*  関数名                          */

	min_tm_co	  = TM_MAX;
	min_cnt_propa = FL_ULMAX;
	br 			  = FL_ULMAX;

	/* branchの順番がco順になれば必要ない */
	/* ここから */

#ifdef PROPA_COUNT
	// カウント付きdfrontモード(バックトラックが少いと役に立たない)
	for ( i = 0; i < dfront->line->n_out ; i++){

		d_next = dfront->next[ i ];

		/* D-frontierが未調査か */
		if ( d_next->flag & DF_NOCHECK ) {
			/* TM的に観測できる可能性があるか? */
			if ( d_next->line->tm_co != TM_INF ) {
				if ( d_next->line->cnt_propa < min_cnt_propa ) {
					min_cnt_propa = d_next->line->cnt_propa;
					min_tm_co      = d_next->line->tm_co;
					br = i;
				}
				else {
					if ( d_next->line->cnt_propa == min_cnt_propa ) {
						if ( d_next->line->tm_co < min_tm_co ) {
							/* 最小の可観測費で未調査flagを持つ枝を探索 */
							min_tm_co = d_next->line->tm_co;
							br        = i;
						}
					}
				}
			}
		}
	}
	if ( br != FL_ULMAX ) {
		dfront->line->out[ br ]->cnt_propa++;
	}
#else
	// TMが安いところに行かせる
	for ( i = 0; i < dfront->line->n_out ; i++){

		d_next = dfront->next[ i ];

		/* D-frontierが未調査か */
		if ( d_next->flag & DF_NOCHECK ) {

			if ( d_next->line->tm_co < min_tm_co ) {
				/* 最小の可観測費で未調査flagを持つ枝を探索 */
				min_tm_co = d_next->line->tm_co;
				br        = i;

			}
		}
	}
#endif /* PROPA_NORMAL */
	

	if ( br == FL_ULMAX ) {
		/* 可観測費が無限大以外の枝がない */
		/* Dfrontがない場合 */
		/* 単純に故障を検出できないだけかも */
		/* $$$ 要backtrack $$$ */
		return( DRIVE_IMPOS );
	}
	/* ここまで */



	
	/* dfront_cの更新 */
	dfront = dfront->next[ br ];

	/* 2005/11/8 削除対象 */
	/*
	Gene_head.dfront_c->imp_trace = Gene_head.imp_trace;
	Gene_head.imp_trace           = NULL;
	   2005/11/18 ここまで */

	Gene_head.dfront_c = dfront;

	dfront->flag |= DF_ROOT;


	/* 新しいD-Frontierの出力側へのデータ作成 */
	children = ( DFRONT ** )topgunMalloc( FMT_DFRONT_P, sizeof( DFRONT * ),
										   dfront->line->n_out, func_name );

	for ( i = 0 ; i < dfront->line->n_out ; i++ ) {
    	/* 子供用D-frontier用メモり確保 */
		child = ( DFRONT * )topgunMalloc( FMT_DFRONT, sizeof( DFRONT ),
										   1, func_name);

		/* child の初期化 */
		/* $$$ branchの順番が可観測費の順になってれば楽 $$$ */
		child->flag      = DF_NOCHECK;
		child->line      = dfront->line->out[ i ];
		child->ass_list  = NULL;
#ifndef OLD_IMP2
		child->last_n    = 0;
		child->last_f    = 0;
		child->cnt_n     = 0;
		child->cnt_f     = 0;
#else		
		child->imp_trace = NULL;
#endif /* OLD_IMP2 */
		child->prev      = dfront;
		child->next      = NULL;

		children[ i ] = child;
	}
	dfront->next = children;

	return( DRIVE_CONT );
}


/********************************************************************************
 * << Function >>
 *		entry propagate list on dfrontier tree
 *
 * << Function ID >>
 *    	18-3
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type			description
 * propa_list	 I		ASS_LIST *		assignment for propagating fault
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/7
 *
 ********************************************************************************/

void atpg_ent_propa_dfront
(
 ASS_LIST *propa_list
 ){

	char *func_name = "atpg_ent_propa_dfront"; /* 関数名 */

	test_ass_list ( propa_list, func_name );

	/* propa_list をdfront_cのass_listの最後尾に足す */
	if ( Gene_head.dfront_c->ass_list == NULL ) {
		Gene_head.dfront_c->ass_list = propa_list;
	}
	else{
		utility_combine_ass_list ( Gene_head.dfront_c->ass_list, propa_list ) ;
		/*
		propa_list->next = Gene_head.dfront_c->ass_list;
		Gene_head.dfront_c->ass_list = propa_list;
		*/
	}
}

/********************************************************************************
 * << Function >>
 *		reflesh state on dfront
 *
 * << Function ID >>
 *    	18-5
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 * dfront        I      DFRONT *    dfrontier infomation
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/04/14
 *		2007/06/01  imp_id_n & imp_id_f Masayoshi Yoshimura (Kyushu-u)
 *
 ********************************************************************************/

void gene_reflesh_state_dfront
(
 DFRONT *dfront
 ){

	ASS_LIST    *ass_list;
	ASS_LIST    *tmp_ass_list;
	Ulong       i;
	Ulong       pos;

	char *func_name = "gene_reflesh_stat_dfront"; // 関数名

	topgun_print_mes( 21, 1 );
	topgun_print_mes( 21, 4 );

	// test_dfront ( dfront, func_name );
	topgun_print_mes( 21 ,5 );

	// dfrontierを進めるための値割り当てに伴う、含意操作の結果を元に戻す
	if ( dfront->cnt_n != 0 ) {
		
		topgun_print_mes_reflesh_normal( dfront->last_n, dfront->cnt_n);
		
		for ( pos = dfront->last_n, i = 0 ; i < dfront->cnt_n; i++, pos-- ) {

			topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL );

			Gene_head.imp_list_n[ pos ]->state9 &= STATE9_XU;
			Gene_head.imp_list_n[ pos ]->imp_id_n = 0; // reflesh ( 0 = initial value )
			
			topgun_print_mes_id_after_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL );
		}
		Gene_head.last_n  = pos;
		
		dfront->last_n  = 0;
		dfront->cnt_n   = 0;
	}
	if ( dfront->cnt_f != 0 ) {

		topgun_print_mes_reflesh_failure( dfront->last_f, dfront->cnt_f);
		
		for ( pos = dfront->last_f, i = 0 ; i < dfront->cnt_f; i++, pos-- ) {

			topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );
			
			Gene_head.imp_list_f[ pos ]->state9 &= STATE9_UX;
			Gene_head.imp_list_n[ pos ]->imp_id_f = 0; // reflesh
			
			topgun_print_mes_id_after_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );

		}
		Gene_head.last_f = pos;

		dfront->last_f  = 0;
		dfront->cnt_f   = 0;
	}

	/* 2005/09/12 implication->assignmentに順番変更 */

	/* dfrontierを進めるために割当た値を元に戻す */
	ass_list = dfront->ass_list;

	test_ass_list ( ass_list, func_name ); 
					
	while ( ass_list ) {

		/* 信号線のstateを元に戻す */
		/* 全部imp_traceにはいるので不要 2006/05/25 */
		//atpg_reflesh_state( ass_list->line, ass_list->condition );

		/* 次の値の準備 */
		tmp_ass_list = ass_list;
		ass_list     = ass_list->next;

		topgunFree( tmp_ass_list, FMT_ASS_LIST, 1, func_name );
	}
	dfront->ass_list = NULL;
}

/********************************************************************************
 * << Function >>
 *		imp_trace move to D-Frontier Tree
 *
 * << Function ID >>
 *	    ?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 *     None
 *
 * << extern >>
 * name			(I/O)	type	description
 * Gene_head	I/O		ATPG_HEAD	aptg head infomation
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/09
 *      2007/06/05      enter imp_id
 *
 ********************************************************************************/

void gene_imp_trace_2_dfront
(
 Ulong imp_id
 ){
	Ulong pos = 0;
	Ulong i = 0;

	char *func_name = "gene_imp_trace_2_dfront"; /* 関数名 */

	if ( Gene_head.dfront_c == NULL ) {
		topgun_error( FEC_PRG_DFRONT, func_name );
	}
	// imp_id を登録する
	// noraml state
	for ( pos = Gene_head.last_n , i = 0 ; i < Gene_head.cnt_n ; i++, pos-- ) {
		Gene_head.imp_list_n[ pos ]->imp_id_f = imp_id;
	}
	// failure state
	for ( pos = Gene_head.last_f , i = 0 ; i < Gene_head.cnt_f ; i++, pos-- ) {
		Gene_head.imp_list_f[ pos ]->imp_id_f = imp_id;
	}
	
	/* 含意操作結果を正当化ツリーへの引渡し */
	Gene_head.dfront_c->last_n    = Gene_head.last_n;
	Gene_head.dfront_c->cnt_n     = Gene_head.cnt_n;
	Gene_head.dfront_c->last_f    = Gene_head.last_f;
	Gene_head.dfront_c->cnt_f     = Gene_head.cnt_f;

	/* Gene_head.last_x(最後に登録した場所)はそのまま */
	/* Gene_head.cnt_x カウンタを0に戻す */
	Gene_head.cnt_n = 0;
	Gene_head.cnt_f = 0;
}
/* End of File */

