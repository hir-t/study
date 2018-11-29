/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief stockバッファに関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#include<stdio.h>

#include "topgun.h"
#include "topgunStock.h"
#include "topgunCompact.h" /* CC_PATTERN */
#include "topgunMemory.h"  /* malloc */
#include "topgunLine.h"
#include "topgunFsim.h"
#include "topgunFlist.h" /* FLIST */
#include "topgunTime.h"

/*
#include "topgun_atpg.h"
#include "topgun_time.h"
#include "topgun_error.h"
*/

static STOCK_PAT_HEAD Stock_pat_head = {
	NULL,                   /* *pat_stock     */
	0,                      /* total_size     */
	STOCK_SIZE,             /* stock_siez     */
	0,                      /* width          */
};

extern LINE_INFO Line_info;
extern FSIM_HEAD Fsim_head;
extern LINE      *Line_head;

/*
extern LINE       **Pi_head;
*/

/* original function list */
static STOCK_PAT *stock_make_new_stock_pat ( void ); /* 新しいstock_patを作成する */
static void stock_connect( STOCK_PAT * );  /* stock_patを接続する */
static void stock_copy_pattern( CC_PATTERN *, STOCK_PAT *, Ulong );
static void stock_make_target_fault( STOCK_INFO * );
static void stock_result( STOCK_INFO * );
static void stock_reverse( FSIM_INFO *, COMP_INFO *);
static void stock_pat_categorized_one_det( STOCK_INFO *);
static void stock_pat_categorized_redun( STOCK_INFO *, Ulong);
static Ulong stock_make_order_one_det( STOCK_INFO * );
static Ulong stock_make_order_reverse( STOCK_INFO * );
static Ulong stock_make_order_double_detect( STOCK_INFO *);
static void stock_fault_flag_clear( STOCK_INFO *);
static void stock_double_detect( FSIM_INFO *, COMP_INFO *);
static void stock_result_pattern( STOCK_INFO * );


extern void stock_enter_pattern( CC_PATTERN * ); /* stockにパタンを入力する */
extern void stock_initialize( void ); /* Stock_pat_headの初期化 */
extern void stock_make_info( STOCK_INFO * );/* stock_infoを作成する */
extern void stock_get_pattern( CC_PATTERN *, Ulong );
extern void fsim_stock_fault_init ( STOCK_MODE );
extern void fsim_stock_pat_id( FSIM_INFO *, COMP_INFO *, STOCK_INFO *, Ulong );

/* test function */
void test_null_variable( void *, TOPGUN_MEMORY_TYPE_ID, char *);

/********************************************************************************
 * << Function >>
 *		逆順故障シミュレーション
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *  ATPG_FINISH : ATPGが完了
 *  ???         : ATPGを打ち切り $$$ 機能追加時に追加 $$$
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/10/13
 *
 ********************************************************************************/

void stock_initialize
(
 void
 ){
	Stock_pat_head.width =  ( Line_info.n_pi / Fsim_head.bit_size ) + 1 ;
}

/********************************************************************************
 * << Function >>
 *		逆順故障シミュレーション
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *  ATPG_FINISH : ATPGが完了
 *  ???         : ATPGを打ち切り $$$ 機能追加時に追加 $$$
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/10/13
 *
 ********************************************************************************/

void stock_compact
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
 ){

	switch ( fsim_info->mode ) {
	case FSIM_STOCK:
		stock_double_detect( fsim_info, comp_info );
		break;
	default:
		stock_reverse( fsim_info, comp_info );
		break;
	}
}

/********************************************************************************
 * << Function >>
 *		Fault stock
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/06
 *
 ********************************************************************************/


void stock_double_detect
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
 ){

	Ulong i = 0;
	Ulong n_pat = 0;
	STOCK_INFO stock_info;
	
	// Fsimの時間計測開始
	topgun_2_utility_enter_start_time( &Time_head.sim );
	topgun_2_utility_enter_start_time( &Time_head.stock_sim );

	// まずは1パタンずつ
	fsim_info->n_cc_pat = 1;
	
	stock_make_info ( &stock_info );
	fsim_stock_fault_init( STOCK_DDET ) ; // fsimの関数設定(double detect mode)

	n_pat = stock_info.total_size;
	

	while ( n_pat ) {

		/* まず順々でdouble detectをする */
		for ( i = 0 ; i < stock_info.total_size ; i++ ) {
			//printf("Double Pat %lu\n",i );
			fsim_stock_pat_id( fsim_info, comp_info, &stock_info, stock_info.pat_order[ i ] );
		}

	
		/* 必須パタンと未確定にわける */
		stock_pat_categorized_one_det( &stock_info );
	
		/* one_detのパタンをセットする */
		n_pat = stock_make_order_one_det( &stock_info );
	
		/* 故障フラグをリセット */
		stock_fault_flag_clear( &stock_info );

		fsim_stock_fault_init( STOCK_ONE_DET ) ; // fsimの関数設定( one detect mode)

		/* 必須ベクトルを1detでする */
		for ( i = 0 ; i < n_pat ; i++ ) {
			//printf("Onedet Pat %lu\n",stock_info.pat_order[ i ] );
			fsim_stock_pat_id( fsim_info, comp_info, &stock_info, stock_info.pat_order[ i ] );
		}

		//stock_result( &stock_info );
	
		// reverseのパタンをセットする
		n_pat = stock_make_order_reverse( &stock_info );
	
		fsim_stock_fault_init( STOCK_REVERSE ) ; // fsimの関数設定( reverse mode)

		/* 残りを逆順でする */
		for ( i = 0; i < n_pat ; i++ ) {
			//printf("Reverse Pat %lu\n",stock_info.pat_order[ i ] );
			fsim_stock_pat_id( fsim_info, comp_info, &stock_info, stock_info.pat_order[ i ] );
		}

		/* 冗長パタンを探す */
		stock_pat_categorized_redun( &stock_info, n_pat );

		
		// 次のdouble detect用に順々のパタンをセットする
		n_pat = stock_make_order_double_detect( &stock_info );
	}

	
	stock_result( &stock_info );

	/* Fsimの時間計測終了 */
	topgun_2_utility_enter_end_time( &Time_head.stock_sim );
	topgun_2_utility_enter_end_time( &Time_head.sim );


}


/********************************************************************************
 * << Function >>
 *		逆順故障シミュレーション
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *  ATPG_FINISH : ATPGが完了
 *  ???         : ATPGを打ち切り $$$ 機能追加時に追加 $$$
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/10/13
 *
 ********************************************************************************/

void stock_reverse
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
 ){

	Ulong i;
	Ulong j;
	STOCK_INFO stock_info;
	
	/* Fsimの時間計測開始 */
	topgun_2_utility_enter_start_time( &Time_head.sim );
	topgun_2_utility_enter_start_time( &Time_head.stock_sim );

	stock_make_info ( &stock_info );
	fsim_stock_fault_init( 1 ) ; /* 1 detect */

	/* まずは1パタンずつ */
	fsim_info->n_cc_pat = 1;
	
	for ( i = 0, j = stock_info.total_size - 1 ; i < stock_info.total_size ; i++, j-- ) {

		fsim_stock_pat_id( fsim_info, comp_info, &stock_info, j );
	}

	/* Fsimの時間計測終了 */
	topgun_2_utility_enter_end_time( &Time_head.stock_sim );
	topgun_2_utility_enter_end_time( &Time_head.sim );


	stock_result( &stock_info );
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファマネージャ
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *  ATPG_FINISH : ATPGが完了
 *  ???         : ATPGを打ち切り $$$ 機能追加時に追加 $$$
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_enter_pattern
(
 CC_PATTERN *cc_pat
 ){
	Ulong i              = 0;
	Ulong cnt            = Stock_pat_head.total_size / Stock_pat_head.stock_size;
	Ulong buffer_id      = Stock_pat_head.total_size % Stock_pat_head.stock_size;
	STOCK_PAT *stock_pat = NULL;

	if ( buffer_id == 0 ) {
		/* 新たなstock_patを追加する */
		stock_pat = stock_make_new_stock_pat();

		stock_connect ( stock_pat );
		
	}
	else {
		/* 何番目のstock_patかを求める */
		stock_pat = Stock_pat_head.stock_pat;
		for ( i = 0 ; i < cnt ; i++ ) {
			stock_pat = stock_pat->next;
		}
	}
	
	/* パターンをstockバッファに入力する */
	stock_copy_pattern( cc_pat, stock_pat, buffer_id );

	/* 新たなパタンの分、1加算する */
	Stock_pat_head.total_size++; 
	
}

/********************************************************************************
 * << Function >>
 *		新しいstock_patのメモリ領域の確保と初期化をする
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

STOCK_PAT *stock_make_new_stock_pat
(
 void
 ){

	STOCK_PAT *new_stock_pat = NULL;

	Ulong i = 0; /* 圧縮バッファサイズ */
	Ulong j = 0; /* 圧縮バッファの幅 */
	
	char *func_name = "stock_make_new_stock_pat";

	new_stock_pat = ( STOCK_PAT *)topgunMalloc
		( FMT_STOCK_PAT, sizeof( STOCK_PAT ), 1, func_name );

	/* とりあえず固定バッファサイズで作る */
	new_stock_pat->buf_a = ( Ulong **)topgunMalloc
		( FMT_Ulong_P, sizeof(  Ulong *), Stock_pat_head.stock_size, func_name );
	new_stock_pat->buf_b = ( Ulong **)topgunMalloc
		( FMT_Ulong_P, sizeof(  Ulong *), Stock_pat_head.stock_size, func_name );

	new_stock_pat->flag = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Stock_pat_head.stock_size, func_name );

	new_stock_pat->next = NULL;

	/* とりあえず　全部 "X" に初期化 */

	for ( i = 0 ; i < Stock_pat_head.stock_size ; i++ ) {

		new_stock_pat->buf_a[ i ] = ( Ulong *)topgunMalloc
			( FMT_Ulong, sizeof(  Ulong ), Stock_pat_head.width, func_name );
		new_stock_pat->buf_b[ i ] = ( Ulong *)topgunMalloc
			( FMT_Ulong, sizeof(  Ulong ), Stock_pat_head.width, func_name );

		for ( j = 0 ; j < Stock_pat_head.width ; j++ ) {

			/* X(a = 0, b = 0) で初期化する */
			new_stock_pat->buf_a[ i ][ j ] = 0;
			new_stock_pat->buf_b[ i ][ j ] = 0;
		}

		new_stock_pat->flag[ i ] = SP_NO_DEFINE;
	}

	return ( new_stock_pat );
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファの初期化
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_connect
(
 STOCK_PAT *new_stock_pat
 ){

	Ulong cnt = 0;
	Ulong i =  0;
	STOCK_PAT *stock_pat = NULL;
	
	if ( Stock_pat_head.total_size == 0 ) {
		Stock_pat_head.stock_pat = new_stock_pat;
	}
	else {
		cnt = Stock_pat_head.total_size / Stock_pat_head.stock_size;

		stock_pat = Stock_pat_head.stock_pat;
		
		for ( i = 1 ; i < cnt ; i++ ) {
			
			stock_pat = stock_pat->next;
		}
		stock_pat->next = new_stock_pat;
	}
}


/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_copy_pattern
(
 CC_PATTERN *cc_pat,
 STOCK_PAT *stock_pat,
 Ulong buffer_id
 ){
	Ulong *pattern_a = NULL;
	Ulong *pattern_b = NULL;

	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */

	char *func_name = "stock_copy_pattern_val2";
	
	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );

	/* 前提条件「すでに圧縮可能であることは確認済み」 */

	for ( i = 0 ; i < Stock_pat_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		ここで想定される各bit演算
		   X | X ( a : 0 | 0 -> 0 , b : 0 | 0 -> 0 )
		   X | 0 ( a : 0 | 0 -> 0 , b : 0 | 1 -> 1 )
		   X | 1 ( a : 0 | 1 -> 1 , b : 0 | 0 -> 0 )
		   0 | 0 ( a : 0 | 0 -> 0 , b : 1 | 1 -> 1 )
		   1 | 1 ( a : 1 | 1 -> 1 , b : 0 | 0 -> 0 )

		*/

		stock_pat->buf_a[ buffer_id ][ i ] = pattern_a[ i ];
		stock_pat->buf_b[ buffer_id ][ i ] = pattern_b[ i ];
	}
}


/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_make_info
(
 STOCK_INFO *stock_info
 ){
	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */

	char *func_name = "stock_make_info";

	/* メモリ確保 */
	stock_info->total_size = Stock_pat_head.total_size;
	stock_info->pat_order = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), stock_info->total_size, func_name );

	/* 初期化 */
	/* 順々(のちのち別関数化が必要) */
	for ( i = 0 ; i < stock_info->total_size ; i++) {
		stock_info->pat_order[ i ] = i;
	}

	stock_make_target_fault ( stock_info );
	
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/10/16
 *
 ********************************************************************************/

void stock_get_pattern
(
 CC_PATTERN *cc_pat,
 Ulong position
 ){
	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */
	STOCK_PAT *stock_pat;

	Ulong cnt            = position / Stock_pat_head.stock_size;
	Ulong buffer_id      = position % Stock_pat_head.stock_size;
	
	stock_pat = Stock_pat_head.stock_pat;

	for ( i = 0 ; i < cnt ; i++ ) {
		stock_pat = stock_pat->next;
	}

	
	cc_pat->pattern_a = stock_pat->buf_a[ buffer_id ];
	cc_pat->pattern_b = stock_pat->buf_b[ buffer_id ];
	
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/10/16
 *
 ********************************************************************************/

Ulong *stock_get_pattern_flag
(
 Ulong position
 ){
	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */
	STOCK_PAT *stock_pat;

	Ulong cnt            = position / Stock_pat_head.stock_size;
	Ulong buffer_id      = position % Stock_pat_head.stock_size;
	
	stock_pat = Stock_pat_head.stock_pat;

	for ( i = 0 ; i < cnt ; i++ ) {
		stock_pat = stock_pat->next;
	}
	return ( &(stock_pat->flag[ buffer_id ]) );
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_make_target_fault
(
 STOCK_INFO *stock_info
 ){

	Ulong i;
	Ulong all_fault = 0;
	Ulong count = 0;
	Ulong select_flag = SELECT_DEFAULT;

	STOCK_FAULT *stock_fault = NULL;
	FLIST  *flist_0 = NULL;
	FLIST  *flist_1 = NULL;
	
	char *func_name = "stock_make_target_fault";

	/* 故障シミュレータで検出された故障をもつ信号線の数を数える */
	/* randomも加える(randomdetもdoubledetectの対象とするため) 2006/11/09 */
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		flist_0 = Line_head[ i ].flist[ FSIM_SA0 ];

		if ( flist_0 != NULL ) {
			if ( ( flist_0->info & TOPGUN_DTS ) ||
				 ( flist_0->info & TOPGUN_DTR ) ) {
				select_flag = 1;
			}
		}
		flist_1 = Line_head[ i ].flist[ FSIM_SA1 ];
		if ( flist_1 != NULL ) {
			if ( ( flist_1->info & TOPGUN_DTS ) ||
				 ( flist_1->info & TOPGUN_DTR ) ) {
				select_flag = 1;
			}
		}
		if ( select_flag == 1 ) {
			all_fault++;
			select_flag = SELECT_DEFAULT;
		}
	}
	stock_info->n_fault = all_fault;

	/* メモリ確保 */
	stock_info->all_fault = ( STOCK_FAULT **)topgunMalloc
		( FMT_STOCK_FAULT_P, sizeof( STOCK_FAULT * ), all_fault, func_name );

	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		flist_0 = Line_head[ i ].flist[ FSIM_SA0 ];

		if ( flist_0 != NULL ) {
			if ( (  flist_0->info & TOPGUN_DTS ) ||
				 (  flist_0->info & TOPGUN_DTR ) ){
				/* この信号線ではsa0が選択できる */
				select_flag |= SELECT_ON_SA0;
			}
		}
		flist_1 = Line_head[ i ].flist[ FSIM_SA1 ];
				
		if ( flist_1 != NULL ) {
			if ( ( flist_1->info & TOPGUN_DTS ) ||
				 ( flist_1->info & TOPGUN_DTR ) ) {
				/* この信号線ではsa1が選択できる */
				select_flag |= SELECT_ON_SA1;
			}
		}
		if ( select_flag != SELECT_DEFAULT ) {
			/* この信号線の故障が選択できる */
			
			stock_info->all_fault[ count ] = ( STOCK_FAULT *)topgunMalloc
				( FMT_STOCK_FAULT, sizeof(  STOCK_FAULT ), 1, func_name );
				
			stock_info->all_fault[ count ]->line  = &(Line_head[ i ]);
			stock_info->all_fault[ count ]->no_pat_0 = STOCK_F_N_DET;
			stock_info->all_fault[ count ]->no_pat_1 = STOCK_F_N_DET;
			stock_info->all_fault[ count ]->flag     = select_flag;
			stock_info->all_fault[ count ]->next     = stock_fault;
			stock_fault  = stock_info->all_fault[ count ];

			count++;
			select_flag = SELECT_DEFAULT;
		}
	}
	/* リストの先頭を渡す */
	stock_info->stock_fault = stock_fault;
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_result
(
 STOCK_INFO *stock_info
  ){
	//故障の分類(すべてENDになるはず)
	//stock_result_fault ( stock_info );
	stock_result_pattern ( stock_info );
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_result_fault
(
 STOCK_INFO *stock_info
 ){
	
	Ulong i;
	STOCK_FAULT **fault_list = stock_info->all_fault;

	for ( i = 0; i < stock_info->n_fault ; i++ ) {

		if ( fault_list[ i ]->flag &  SELECT_ON_SA0 ) {
			
			printf("FLT_DET %7lu_0 ",fault_list[ i ]->line->line_id ); 

			if ( fault_list[ i ]->no_pat_0 == STOCK_F_D_DET ) {
				printf("DD\n");
			}
			else if ( fault_list[ i ]->no_pat_0 == STOCK_F_END ) {
				printf("END\n");
			}
			else if ( fault_list[ i ]->no_pat_0 == STOCK_F_N_DET ) {
				printf("NO\n");
			}
			else {
				printf("%lu\n", fault_list[ i ]->no_pat_0 );
			}
		}
		if ( fault_list[ i ]->flag &  SELECT_ON_SA1 ) {
			
			printf("FLT_DET %7lu_1 ",fault_list[ i ]->line->line_id ); 

			if ( fault_list[ i ]->no_pat_1 == STOCK_F_D_DET ) {
				printf("DD\n");
			}
			else if ( fault_list[ i ]->no_pat_1 == STOCK_F_END ) {
				printf("END\n");
			}
			else if ( fault_list[ i ]->no_pat_1 == STOCK_F_N_DET ) {
				printf("NO\n");
			}
			else {
				printf("%lu\n", fault_list[ i ]->no_pat_1 );
			}
		}
	}
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_result_pattern
(
 STOCK_INFO *stock_info
 ){

	Ulong     i = 0;
	Ulong     j = 0;
	Ulong     one_det = 0;
	Ulong     redun = 0;
	Ulong     other = 0;
	STOCK_PAT *stock_pat = NULL;

	stock_pat = Stock_pat_head.stock_pat;
	
	for ( i = 0, j = 0 ; i < Stock_pat_head.total_size ; i++, j++ ) {

		if ( j == Stock_pat_head.stock_size ){
			stock_pat = stock_pat->next;
			j = 0;
		}
		
		if( stock_pat->flag[ j ] & SP_ONE_DET ) {
			one_det++;
		}
		else if ( stock_pat->flag[ j ] & SP_REDUN ) {
			redun++;
		}
		else {
			other++; //ないはず
		}
	}

	printf("DD  %5lu  ( %5lu ) / total %5lu\n",one_det, redun, one_det+redun+other);
}

/********************************************************************************
 * << Function >>
 *		必須パタンのフラグをたてる
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_pat_categorized_one_det
(
 STOCK_INFO *stock_info
 ){

	Ulong i;
	Ulong *flag;
	STOCK_FAULT **fault_list = stock_info->all_fault;

	for ( i = 0; i < stock_info->n_fault ; i++ ) {

		if ( fault_list[ i ]->flag &  SELECT_ON_SA0 ) {
			
			//printf("FLT_DET %7lu_0 ",fault_list[ i ]->line->line_id ); 

			switch ( fault_list[ i ]->no_pat_0 ) {
			case STOCK_F_D_DET:
			case STOCK_F_END:
				//printf("DD\n");
				break;
			case STOCK_F_N_DET:
				//printf("AB\n"); //こないはず
				break;
			default:
				flag = stock_get_pattern_flag( fault_list[ i ]->no_pat_0 );
				*flag |= SP_ONE_DET;
				//printf("%lu\n", fault_list[ i ]->no_pat_0 );
				break;
			}
		}
		if ( fault_list[ i ]->flag &  SELECT_ON_SA1 ) {
			
			//printf("FLT_DET %7lu_1 ",fault_list[ i ]->line->line_id ); 

			switch ( fault_list[ i ]->no_pat_1 ) {
			case STOCK_F_D_DET:
			case STOCK_F_END:
				//printf("DD\n");
				break;
			case STOCK_F_N_DET:
				//printf("AB\n"); //こないはず
				break;
			default:
				flag = stock_get_pattern_flag( fault_list[ i ]->no_pat_1 );
				*flag |= SP_ONE_DET;
				//printf("%lu\n", fault_list[ i ]->no_pat_1 );
				break;
			}
		}
	}
}

/********************************************************************************
 * << Function >>
 *		必須パタンのフラグをたてる
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_pat_categorized_redun
(
 STOCK_INFO *stock_info,
 Ulong n_pat
 ){

	Ulong i;
	Ulong *flag;
	STOCK_FAULT **fault_list = stock_info->all_fault;


	// まずNO_DEFのパタンにすべてREDUNのフラグを付ける

	for ( i = 0 ; i < n_pat ; i++ ) {
		flag = stock_get_pattern_flag( stock_info->pat_order[ i ] );
		*flag |= SP_REDUN;
		*flag |= SP_COMPLETE;
	}


	// つぎに検出した故障があるパタンのフラグを元にもどす
	for ( i = 0; i < stock_info->n_fault ; i++ ) {

		if ( fault_list[ i ]->flag &  SELECT_ON_SA0 ) {
			
			//printf("FLT_DET %7lu_0 ",fault_list[ i ]->line->line_id ); 

			switch ( fault_list[ i ]->no_pat_0 ) {
			case STOCK_F_D_DET:
			case STOCK_F_END:
				//printf("DD\n");
				break;
			case STOCK_F_N_DET:
				//printf("AB\n"); //こないはず
				break;
			default:
				flag = stock_get_pattern_flag( fault_list[ i ]->no_pat_0 );
				*flag &= ~SP_REDUN;
				*flag &= ~SP_COMPLETE;
				//printf("%lu\n", fault_list[ i ]->no_pat_0 );
				break;
			}
		}
		if ( fault_list[ i ]->flag &  SELECT_ON_SA1 ) {
			
			//printf("FLT_DET %7lu_1 ",fault_list[ i ]->line->line_id ); 

			switch ( fault_list[ i ]->no_pat_1 ) {
			case STOCK_F_D_DET:
			case STOCK_F_END:
				//printf("DD\n");
				break;
			case STOCK_F_N_DET:
				//printf("AB\n"); //こないはず
				break;
			default:
				flag = stock_get_pattern_flag( fault_list[ i ]->no_pat_1 );
				*flag &= ~SP_REDUN;
				*flag &= ~SP_COMPLETE;
				//printf("%lu\n", fault_list[ i ]->no_pat_1 );
				break;
			}
		}
	}
}

/********************************************************************************
 * << Function >>
 *		必須パタンのフラグをたてる
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

Ulong stock_make_order_double_detect
(
 STOCK_INFO *stock_info
 ){
	Ulong     i = 0;
	Ulong     j = 0;
	Ulong     count = 0;
	STOCK_PAT *stock_pat = NULL;

	stock_pat = Stock_pat_head.stock_pat;
	
	for ( i = 0, j = 0 ; i < Stock_pat_head.total_size ; i++, j++ ) {

		if ( j == Stock_pat_head.stock_size ){
			stock_pat = stock_pat->next;
			j = 0;
		}
		
		if ( ! ( stock_pat->flag[ j ] & SP_COMPLETE ) ) {
			/* not complieteパタンのみを対象とする */
			//printf("DD pattern %lu / %ld\n", i, Stock_pat_head.total_size) ;
			stock_info->pat_order[ count ] = i;
			count++;
		}
	}
	return ( count );
}

/********************************************************************************
 * << Function >>
 *		必須パタンのフラグをたてる
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

Ulong stock_make_order_one_det
(
 STOCK_INFO *stock_info
 ){
	Ulong     i = 0;
	Ulong     j = 0;
	Ulong     count = 0;
	STOCK_PAT *stock_pat = NULL;

	stock_pat = Stock_pat_head.stock_pat;
	
	for ( i = 0, j = 0 ; i < Stock_pat_head.total_size ; i++, j++ ) {

		if ( j == Stock_pat_head.stock_size ){
			stock_pat = stock_pat->next;
			j = 0;
		}
		
		if( stock_pat->flag[ j ] & SP_ONE_DET ) {
			/* one_detパターン*/
			if ( ! ( stock_pat->flag[ j ] & SP_COMPLETE ) ){
				/* one_det確定後のシミュレーションが終了していない */
				//printf("ONE pattern %lu / %ld\n", i, Stock_pat_head.total_size) ;
				stock_pat->flag[ j ] |= SP_COMPLETE;
				stock_info->pat_order[ count ] = i;
				count++;
			}
		}
	}
	return ( count );
}

/********************************************************************************
 * << Function >>
 *		必須パタンのフラグをたてる
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

Ulong stock_make_order_reverse
(
 STOCK_INFO *stock_info
 ){

	Ulong     jyo = Stock_pat_head.total_size / Stock_pat_head.stock_size ; // 剰
	Ulong     yo  = Stock_pat_head.total_size % Stock_pat_head.stock_size ; // 余

	Ulong     i = 0; //カウンタ
	Ulong     j = 0; //カウンタ
	Ulong     count = 0; //登録したパタン数
	Ulong     pat_n = Stock_pat_head.total_size - 1;
	
	STOCK_PAT *stock_pat = Stock_pat_head.stock_pat;

	//printf("reverse\n");
	
	// stock_patの設定
	for ( i = 0 ; i < jyo ; i++ ) {
		stock_pat = stock_pat->next;
	}
	for ( i = yo - 1 , j = 0 ; j < yo ; i--, j++, pat_n-- ) {
		if ( ! ( stock_pat->flag[ i ] & SP_ONE_DET ) ) {
			// one_detでないパターン
			if ( ! ( stock_pat->flag[ i ] & SP_COMPLETE ) ){
				// one_det確定後のシミュレーションが終了していない 
				//printf("pattern %lu / %ld\n", pat_n, Stock_pat_head.total_size );
				stock_info->pat_order[ count ] = pat_n;
				count++;
			}
		}
	}

	while ( jyo ) {
		jyo--;

		stock_pat = Stock_pat_head.stock_pat;
		for ( i = 0 ; i < jyo ; i++ ) {
			stock_pat = stock_pat->next;
		}
		for ( i = Stock_pat_head.stock_size - 1 , j = 0 ; j < Stock_pat_head.stock_size ;  i--, j++, pat_n-- ) {
			if ( ! ( stock_pat->flag[ i ] & SP_ONE_DET ) ) {
				// one_detでないパターン
				if ( ! ( stock_pat->flag[ i ] & SP_COMPLETE ) ) {
					// one_det確定後のシミュレーションが終了していない 
					//printf("pattern %lu / %ld\n", pat_n, Stock_pat_head.total_size) ;
					stock_info->pat_order[ count ] = pat_n;
					count++;
				}
			}
		}
	}
	return ( count );
}

/********************************************************************************
 * << Function >>
 *		必須パタンのフラグをたてる
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void stock_fault_flag_clear
(
 STOCK_INFO *stock_info
 ){

	STOCK_FAULT *stock_fault = NULL;

	stock_fault = stock_info->stock_fault;

	while ( stock_fault ) {

		if ( stock_fault->no_pat_0 != STOCK_F_END ) {
			stock_fault->no_pat_0 = STOCK_F_N_DET;
		}
		if ( stock_fault->no_pat_1 != STOCK_F_END ) {
			stock_fault->no_pat_1 = STOCK_F_N_DET;
		}
		stock_fault = stock_fault->next;
	}
}


/* End of File */
