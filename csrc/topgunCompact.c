/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 圧縮バッファに関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#include<stdio.h>

#include "topgun.h"
#include "topgunLine.h"
#include "topgunFsim.h"
#include "topgunCompact.h"
#include "topgunMemory.h"
#include "topgunTime.h"
#include "topgunError.h"

static COMP_HEAD Comp_head = {
	NULL,                   /* **buf_a        */
	NULL,                   /* **buf_b        */
	NULL,                   /* *num_x_state   */
	0,                      /* flag           */
	0,                      /* width          */
	COMP_BUF_SIZE,          /* buf_size       */
	0,                      /* min_n_x_state  */
	0,                      /* min_n_x_buf_id */
};

static CC_PATTERN tmp_pat;  /* 圧縮バッファで入れ換える時に用いる */

extern LINE_INFO Line_info;
extern FSIM_HEAD Fsim_head;
extern LINE       **Pi_head;


/* original function list */
extern void compact ( COMP_INFO * );
extern void  compact_get_pattern_with_id ( CC_PATTERN *, Ulong );
extern COMP_CHECK_ALL_X compact_check_pattern_all_x ( CC_PATTERN * );
extern void  comp_env_init ( void );
extern void compact_buffer_initialize( void );
extern void compact_cc_pat_initialize( CC_PATTERN * );
extern void compact_info_initialize( COMP_INFO * );
extern Ulong compact_get_buf_size( void );
extern void comp_env_init( void );
extern COMP_ON_CHECK comp_on_check ( void );
 
static void  compact_enter_pattern_buffer ( Ulong, CC_PATTERN * );
static Ulong compact_check_possibility ( Ulong, CC_PATTERN * );
static void  compact_overwrite_pattern ( Ulong, CC_PATTERN * );
static void  compact_search_min_x_state( void );
static Ulong compact_count_x_state_with_buffer_id ( Ulong );
static Ulong compact_count_x_state ( Ulong *,  Ulong * );
static void  compact_min_x_state_recalc ( Ulong );
static void compact_trial( CC_PATTERN *, CC_PATTERN *);


extern void pattern_get_from_generate( CC_PATTERN * );

/* topgun_state.c */
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9値から正常値を取り出す */

/* topgun_uty.c */
Ulong utility_count_0_bit ( Ulong );
void  utility_renew_parameter_with_env ( char *, Ulong *, Ulong );

/* print function */
void  topgun_print_mes_compact_pattern( Ulong *, Ulong *, Ulong, Ulong );
void  topgun_print_mes_compact_posibbility( Ulong );
void  topgun_print_mes_n_compact( void );
void  topgun_print_mes_min_check ( Ulong, Ulong );
void  topgun_print_mes_compact_count_x_in_buf ( Ulong buffer_id, Ulong num_x_state);
void  topgun_print_mes_compact_count_x_in_pat ( Ulong num_x_state);
	
/* test_function */
void test_null_variable( void *, TOPGUN_MEMORY_TYPE_ID, char *);

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

void compact
(
 COMP_INFO *comp_info
 ){
	/* パターンをバッファに入力する */
	/* -> 入った(圧縮できた) */
	/* -> 入った(圧縮できず、空バッファに入った) */
	/* -> 入らない(圧縮できず、空バッファもない) */
	/* ->　何かのパターンを放り返す */

	Ulong    num_x_state = 0;
	Ulong    i = 0; /* 圧縮バッファサイズのカウンタ */
	Ulong    comp_buf_id = 0; /* 生成したパタンを入れた圧縮バッファのID */

	topgun_2_utility_enter_start_time ( &Time_head.compact );
	
	topgun_print_mes_n_compact();
	
	for ( i = 0 ; i < Comp_head.buf_size ; i++ ) {

		/* バッファiの圧縮可能性をしらべる */
		comp_info->result = compact_check_possibility ( i, comp_info->cc_pat ) ;

		topgun_print_mes_compact_pattern( comp_info->cc_pat->pattern_a
										  , comp_info->cc_pat->pattern_b, Comp_head.buf_size, Comp_head.buf_size );
		topgun_print_mes_n_compact();

		topgun_print_mes_compact_pattern
			( Comp_head.buf_a[ i ], Comp_head.buf_b[ i ], i, Comp_head.buf_size );
		topgun_print_mes_compact_posibbility( comp_info->result );
		
		if ( comp_info->result == COMPACT_OK ) {

			comp_buf_id = i;
			
			/* 圧縮する */
			compact_enter_pattern_buffer ( comp_buf_id, comp_info->cc_pat ) ;

			/* iのXの数を計算する */
			compact_min_x_state_recalc ( comp_buf_id );
			
			/* min_x_bufを再計算する */
			compact_search_min_x_state();
			
			topgun_print_mes_compact_pattern
				( Comp_head.buf_a[ comp_buf_id ], Comp_head.buf_b[ comp_buf_id ], (Comp_head.buf_size + 1)  , Comp_head.buf_size );
			topgun_print_mes_n_compact();
			
			break;
		}
	}

	if ( comp_info->result == COMPACT_NG ) {
		/* すべてのにバッファで圧縮できない */
		
		/* 一番Xの少ないパターンを放出する */

		/* 生成したパターンのXの数をもとめる */
		num_x_state = compact_count_x_state ( comp_info->cc_pat->pattern_a,
											  comp_info->cc_pat->pattern_b );

		topgun_print_mes_min_check ( num_x_state, Comp_head.min_n_x_state );

		
		if ( Comp_head.min_n_x_state < num_x_state ) {
			/* 圧縮バッファに入ってる方がXが少い */

			/* パターンを取ってくる */
			compact_get_pattern_with_id ( &tmp_pat, Comp_head.min_x_buf_id );

			topgun_print_mes_compact_pattern
				( tmp_pat.pattern_a, tmp_pat.pattern_b, i, Comp_head.buf_size);
			topgun_print_mes_n_compact();
		
			/* 空いたバッファに上書きする */
			compact_overwrite_pattern
				( Comp_head.min_x_buf_id, comp_info->cc_pat ) ;

			topgun_print_mes_compact_pattern
				( Comp_head.buf_a[ Comp_head.min_x_buf_id ],
				  Comp_head.buf_b[ Comp_head.min_x_buf_id ], ( Comp_head.buf_size+1), Comp_head.buf_size);
			topgun_print_mes_n_compact();

			/* min_x_bufを再計算する */
			compact_search_min_x_state();
		}
		else {
			/* パターンの方がXが少い */
		}
	}
	else if ( comp_info->result == COMPACT_OK ) {
		/* Fsim3を起動するか */
		if ( FSIM_VAL3 == 1  ) {
			/* Fsim3を起動する場合のパタンを入れる */
			/* i番目に入っている */
			/* パターンを取ってくる */
			compact_get_pattern_with_id ( comp_info->cc_pat, comp_buf_id );
		}
	}

	topgun_2_utility_enter_end_time ( &Time_head.compact );
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

void compact_mini_x
(
 COMP_INFO *comp_info
 ){

	/* パターンをバッファに入力する */
	/* -> 入った(圧縮できた) */
	/* -> 入った(圧縮できず、空バッファに入った) */
	/* -> 入らない(圧縮できず、空バッファもない) */
	/* ->　何かのパターンを放り返す */


	Ulong    mini_x_state = FL_ULMAX;  
	Ulong    mini_x_state_buf_id = FL_ULMAX;

	Ulong    current_x_state = 0;

	Ulong    current_result = COMPACT_NG;
	
	Ulong    num_x_state = 0;
	Ulong    i = 0; /* 圧縮バッファサイズのカウンタ */
	Ulong    comp_buf_id = 0; /* 生成したパタンを入れた圧縮バッファのID */

	topgun_2_utility_enter_start_time ( &Time_head.compact );
	
	topgun_print_mes_n_compact();

	comp_info->result = COMPACT_NG;
	
	//printf("COMP EXE ");

	for ( i = 0 ; i < Comp_head.buf_size ; i++ ) {

		/* バッファiの圧縮可能性をしらべる */
		current_result = compact_check_possibility ( i, comp_info->cc_pat ) ;

		topgun_print_mes_compact_pattern( comp_info->cc_pat->pattern_a
										  , comp_info->cc_pat->pattern_b, Comp_head.buf_size, Comp_head.buf_size );
		topgun_print_mes_n_compact();

		topgun_print_mes_compact_pattern
			( Comp_head.buf_a[ i ], Comp_head.buf_b[ i ], i, Comp_head.buf_size );
		topgun_print_mes_compact_posibbility( comp_info->result );
		
		if ( current_result == COMPACT_OK ) {

			comp_info->result = COMPACT_OK;

			comp_buf_id = i;

			//printf(" o%3lu",i);

			/* まずtmp_patにコピーする */
			compact_get_pattern_with_id ( &tmp_pat, comp_buf_id );
			
			/* 仮圧縮する */
			compact_trial ( &tmp_pat, comp_info->cc_pat ) ;

			/* Xの数を計算する */
			num_x_state = compact_count_x_state ( tmp_pat.pattern_a,
												  tmp_pat.pattern_b  );

			/* Xの増分を求める */
			current_x_state = Comp_head.num_x_state[ comp_buf_id ] - num_x_state;

			if ( mini_x_state > current_x_state ) {
				mini_x_state = current_x_state ;
				mini_x_state_buf_id = comp_buf_id;
			}
			if ( mini_x_state == 0 ) {
				break;
			}
		}
		else {
			//printf(" x%3lu",i);
		}
	}
	//printf("\n");

	if ( comp_info->result == COMPACT_NG ) {
		/* すべてのにバッファで圧縮できない */
		//printf("COMP_NG\n");
		/* 一番Xの少ないパターンを放出する */

		/* 生成したパターンのXの数をもとめる */
		num_x_state = compact_count_x_state ( comp_info->cc_pat->pattern_a,
											  comp_info->cc_pat->pattern_b );

		topgun_print_mes_min_check ( num_x_state, Comp_head.min_n_x_state );

		
		if ( Comp_head.min_n_x_state < num_x_state ) {
			/* 圧縮バッファに入ってる方がXが少い */

			/* パターンを取ってくる */
			compact_get_pattern_with_id ( &tmp_pat, Comp_head.min_x_buf_id );

			topgun_print_mes_compact_pattern
				( tmp_pat.pattern_a, tmp_pat.pattern_b, i, Comp_head.buf_size);
			topgun_print_mes_n_compact();
		
			/* 空いたバッファに上書きする */
			compact_overwrite_pattern
				( Comp_head.min_x_buf_id, comp_info->cc_pat ) ;

			topgun_print_mes_compact_pattern
				( Comp_head.buf_a[ Comp_head.min_x_buf_id ],
				  Comp_head.buf_b[ Comp_head.min_x_buf_id ], ( Comp_head.buf_size+1), Comp_head.buf_size);
			topgun_print_mes_n_compact();

			/* min_x_bufを再計算する */
			compact_search_min_x_state();
		}
		else {
			/* パターンの方がXが少い */
		}
	}
	else if ( comp_info->result == COMPACT_OK ) {

		comp_buf_id = mini_x_state_buf_id;

		//printf("COMP_IN  : %5lu\n",comp_buf_id);
			
		/* 圧縮する */
		compact_enter_pattern_buffer ( comp_buf_id, comp_info->cc_pat ) ;

		/* iのXの数を計算する */
		compact_min_x_state_recalc ( comp_buf_id );
			
		/* min_x_bufを再計算する */
		compact_search_min_x_state();
			
		topgun_print_mes_compact_pattern
			( Comp_head.buf_a[ comp_buf_id ], Comp_head.buf_b[ comp_buf_id ], (Comp_head.buf_size + 1)  , Comp_head.buf_size );
		topgun_print_mes_n_compact();
		
		/* Fsim3を起動するか */
		if ( FSIM_VAL3 == 1  ) {
			/* Fsim3を起動する場合のパタンを入れる */
			/* i番目に入っている */
			/* パターンを取ってくる */
			compact_get_pattern_with_id ( comp_info->cc_pat, comp_buf_id );
		}
	}

	topgun_2_utility_enter_end_time ( &Time_head.compact );
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファからパターンを抽出する
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

void compact_get_pattern_with_id
(
 CC_PATTERN *return_pattern,
 Ulong buf_id
 ){
	Ulong i = 0; /* 圧縮バッファサイズ */

	/* reterun_patternに値をコピーする */
	char *func_name = "compact_get_patetrn";

	test_null_variable ( return_pattern, FMT_CC_PATTERN_P, func_name );

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		return_pattern->pattern_a[ i ] = Comp_head.buf_a[ buf_id ][ i ];
		return_pattern->pattern_b[ i ] = Comp_head.buf_b[ buf_id ][ i ];

	}
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

void compact_buffer_initialize
(
 void
 ){
	Ulong i = 0; /* 圧縮バッファサイズ */
	Ulong j = 0; /* 圧縮バッファ幅 */

	/* たてにバッファ数、横にPIを取る (メモリは多い あまりに対して、PIを掛けるため) */
	/* パターンごとの処理(圧縮可/不可判断、圧縮、初期化)が容易なため */

	char *func_name = "compact_buffer_initialize";

	/* 1パターン 2bit(buf_aとbuf_b)とする */
	Comp_head.width = ( Line_info.n_pi / Fsim_head.bit_size ) + 1 ;


	//printf("width %ld\n", Comp_head.width);
	//printf("Comp_head.buf_size %ld\n", Comp_head.buf_size );

	
	/* とりあえず固定バッファサイズで作る */
	Comp_head.buf_a = ( Ulong **)topgunMalloc
		( FMT_Ulong_P, sizeof(  Ulong *), Comp_head.buf_size, func_name );
	Comp_head.buf_b = ( Ulong **)topgunMalloc
		( FMT_Ulong_P, sizeof(  Ulong *), Comp_head.buf_size, func_name );


	Comp_head.num_x_state = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof( Ulong ), Comp_head.buf_size, func_name );
	
	/* とりあえず　全部 "X" に初期化 */

	for ( i = 0 ; i < Comp_head.buf_size ; i++ ) {

		Comp_head.buf_a[ i ] = ( Ulong *)topgunMalloc
			( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );
		Comp_head.buf_b[ i ] = ( Ulong *)topgunMalloc
			( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );

		for ( j = 0 ; j < Comp_head.width ; j++ ) {

			/* X(a = 0, b = 0) で初期化する */
			Comp_head.buf_a[ i ][ j ] = 0;
			Comp_head.buf_b[ i ][ j ] = 0;
		}

		Comp_head.num_x_state[ i ] = 0;
	}

	/* 圧縮バッファから溢れて一つ取る場合につかうパターンの初期化 */
	tmp_pat.pattern_a = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );
	tmp_pat.pattern_b = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );

	for ( j = 0 ; j < Comp_head.width ; j++ ) {

			/* X(a = 0, b = 0) で初期化する */
			tmp_pat.pattern_a[ j ] = 0;
			tmp_pat.pattern_b[ j ] = 0;
	}
	Comp_head.min_x_buf_id = 0;

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

void compact_enter_pattern_buffer
(
 Ulong buffer_id,
 CC_PATTERN *cc_pat
 ){
	Ulong *pattern_a = NULL;
	Ulong *pattern_b = NULL;

	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */

	 char *func_name = "compact_buffer_enter_pattern";
	
	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );

	/* 前提条件「すでに圧縮可能であることは確認済み」 */

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		ここで想定される各bit演算
		   X | X ( a : 0 | 0 -> 0 , b : 0 | 0 -> 0 )
		   X | 0 ( a : 0 | 0 -> 0 , b : 0 | 1 -> 1 )
		   X | 1 ( a : 0 | 1 -> 1 , b : 0 | 0 -> 0 )
		   0 | 0 ( a : 0 | 0 -> 0 , b : 1 | 1 -> 1 )
		   1 | 1 ( a : 1 | 1 -> 1 , b : 0 | 0 -> 0 )

		*/

		Comp_head.buf_a[ buffer_id ][ i ] |= pattern_a[ i ];
		Comp_head.buf_b[ buffer_id ][ i ] |= pattern_b[ i ];
	}
}

/********************************************************************************
 * << Function >>
 *		仮のパターン圧縮
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
 *		2006/09/18
 *
 ********************************************************************************/

void compact_trial
(
 CC_PATTERN *tmp_pat,
 CC_PATTERN *cc_pat
 ){
	Ulong *tmp_pat_a = NULL;
	Ulong *tmp_pat_b = NULL;
	
	Ulong *pattern_a = NULL;
	Ulong *pattern_b = NULL;

	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */

	 char *func_name = "compact_buffer_enter_pattern";
	
	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	tmp_pat_a = tmp_pat->pattern_a;
	tmp_pat_b = tmp_pat->pattern_b;

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );

	/* 前提条件「すでに圧縮可能であることは確認済み」 */

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		ここで想定される各bit演算
		   X | X ( a : 0 | 0 -> 0 , b : 0 | 0 -> 0 )
		   X | 0 ( a : 0 | 0 -> 0 , b : 0 | 1 -> 1 )
		   X | 1 ( a : 0 | 1 -> 1 , b : 0 | 0 -> 0 )
		   0 | 0 ( a : 0 | 0 -> 0 , b : 1 | 1 -> 1 )
		   1 | 1 ( a : 1 | 1 -> 1 , b : 0 | 0 -> 0 )

		*/

		tmp_pat_a[ i ] |= pattern_a[ i ];
		tmp_pat_b[ i ] |= pattern_b[ i ];
	}
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターンの上書き
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

void compact_overwrite_pattern
(
 Ulong buffer_id,
 CC_PATTERN *cc_pat
 ){

	Ulong *pattern_a = NULL;
	Ulong *pattern_b = NULL;

	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */

	char *func_name = "compact_buffer_enter_pattern";

	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );	

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		ここで想定される各bit演算
		 old   new
		   X | X ( a : 0 | 0 -> 0 , b : 0 | 0 -> 0 ) (1)
		   X | 0 ( a : 0 | 0 -> 0 , b : 0 | 1 -> 1 ) (2)
		   X | 1 ( a : 0 | 1 -> 1 , b : 0 | 0 -> 0 ) (3)
		   0 | X ( a : 0 | 0 -> 0 , b : 1 | 0 -> 0 ) (4)
		   0 | 0 ( a : 0 | 0 -> 0 , b : 1 | 1 -> 1 ) (5)
		   0 | 1 ( a : 0 | 1 -> 1 , b : 1 | 0 -> 0 ) (6)
		   1 | X ( a : 1 | 0 -> 0 , b : 0 | 0 -> 0 ) (7)
		   1 | 0 ( a : 1 | 0 -> 0 , b : 0 | 1 -> 1 ) (8)
		   1 | 1 ( a : 1 | 1 -> 1 , b : 0 | 0 -> 0 ) (9)

		*/

		Comp_head.buf_a[ buffer_id ][ i ] = pattern_a[ i ];
		Comp_head.buf_b[ buffer_id ][ i ] = pattern_b[ i ];

		/* 退避していたものをコピーしてかえす */
		pattern_a[ i ] = tmp_pat.pattern_a [ i ];
		pattern_b[ i ] = tmp_pat.pattern_b [ i ];
	}

	compact_min_x_state_recalc ( buffer_id );
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファ中の任意パターンに存在するXの数を求める
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       パターン中のXの数を数える
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

Ulong compact_count_x_state_with_buffer_id
(
 Ulong buffer_id
 ){

	/* 上限を設定するのも一つの手 */

	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */
	Ulong num_x_state = 0; /* Xの数 */

	Ulong tmp_pattern = 0;

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 ) */
		tmp_pattern = ( Comp_head.buf_a[ buffer_id ][ i ] |
						Comp_head.buf_b[ buffer_id ][ i ] );

		/* $$$ bitあまりの0は上げ底として一旦放置　$$$ */
		/* $$$ たぶん初期処理でヘッダに覚えさえておいてあとで引けばよさげ) */
		num_x_state += utility_count_0_bit ( tmp_pattern );
	}

	topgun_print_mes_compact_count_x_in_buf ( buffer_id, num_x_state);
	
	return ( num_x_state );

}

/********************************************************************************
 * << Function >>
 *		圧縮バッファ中の任意パターンに存在するXの数を求める
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       パターン中のXの数を数える
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

Ulong compact_count_x_state
(
 Ulong *pattern_a,
 Ulong *pattern_b
 ){

	/* 上限を設定するのも一つの手 */

	Ulong i = 0; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */
	Ulong num_x_state = 0; /* Xの数 */

	Ulong tmp_pattern = 0;

	char *func_name = "compact_count_x_state";

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );
	
	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 ) */
		tmp_pattern = ( pattern_a[ i ] | pattern_b[ i ] );

		/* $$$ bitあまりの0は上げ底として一旦放置　$$$ */
		/* $$$ たぶん初期処理でヘッダに覚えさえておいてあとで引けばよさげ) */
		num_x_state += utility_count_0_bit ( tmp_pattern );
	}

	topgun_print_mes_compact_count_x_in_pat ( num_x_state) ;

	return ( num_x_state );

}
	 
/********************************************************************************
 * << Function >>
 *		圧縮バッファ中の任意パターンに存在するXの数を求める
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       パターン中のXの数を数える
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

void compact_min_x_state_recalc
(
 Ulong buffer_id
 ){
	Ulong num_x_state;
	
	num_x_state = compact_count_x_state_with_buffer_id ( buffer_id );
	
	Comp_head.num_x_state[ buffer_id ] = num_x_state;

	if ( Comp_head.min_n_x_state > num_x_state ) {
		/* Xが少ないところの情報の更新 */
		Comp_head.min_n_x_state  = num_x_state;
		Comp_head.min_x_buf_id = buffer_id;
	}
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファへのパターン圧縮の可否(のみ)確認する
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * COMPACT_NG  : 圧縮不可能 ( == UL_MAX )
 * COMPPRES_OK  : 圧縮可能 ( == 0 )
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

Ulong  compact_check_possibility
(
 Ulong buffer_id,
 CC_PATTERN *cc_pat
 ){

	Ulong *pattern_a = NULL;
	Ulong *pattern_b = NULL;
	Ulong i = 0; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */

	char *func_name = "compact_buffer_enter_pattern";

	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );
	
	
	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		ここで想定される各bit演算<>
		   X <> X ( a : 0 <> 0 , b : 0 <> 0 → OK )
		   X <> 0 ( a : 0 <> 0 , b : 0 <> 1 → OK )
		   X <> 1 ( a : 0 <> 1 , b : 0 <> 0 → OK )
		   0 <> X ( a : 0 <> 1 , b : 1 <> 0 → OK )
		   0 <> 0 ( a : 0 <> 0 , b : 1 <> 1 → OK )
		   0 <> 1 ( a : 0 <> 1 , b : 1 <> 0 → NG )
		   1 <> X ( a : 1 <> 0 , b : 0 <> 0 → OK )
		   1 <> 0 ( a : 1 <> 0 , b : 0 <> 1 → NG )
		   1 <> 1 ( a : 1 <> 1 , b : 0 <> 0 → OK )

		*/

		if ( ( Comp_head.buf_a[ buffer_id ][ i ] & pattern_b[ i ] ) ||
			 ( Comp_head.buf_b[ buffer_id ][ i ] & pattern_a[ i ] ) ) {
			/* 衝突発生, 圧縮できない */
			return ( COMPACT_NG );
		}
	}
	/* どこも衝突が発生していない -> 圧縮OK */
	return ( COMPACT_OK );
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファ中のXの最小値パターンを求める
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       パターン中のXの数を数える
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

void compact_search_min_x_state
(
 void
 ){

	Ulong i; /* 圧縮バッファの幅(Comp_head.width)のカウンタ */

	/* char *func_name = "compact_search_x_state"; */

	Comp_head.min_x_buf_id  = 0; /* 初期値 */
	Comp_head.min_n_x_state = Comp_head.num_x_state[ 0 ]; /* 初期値 */

	/* 前提条件 : すべてbuufferごとのXの数は求まっている */
	for ( i = 1 ; i < Comp_head.buf_size ; i++ ) {
		if ( Comp_head.min_n_x_state > Comp_head.num_x_state[ i ] ) {
			/* 更新する */
			Comp_head.min_n_x_state  = Comp_head.num_x_state[ i ];
			Comp_head.min_x_buf_id = i;
		}
	}
}

/********************************************************************************
 * << Function >>
 *		ATPGで生成したパターンを得る
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cc_pat        I/O    CC_PATTERN * 組合せ回路パターン
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I      COMP_HEAD   compact buffer header
 * Pi_head       I      LINE **     Primary Input header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void pattern_get_from_generate
(
 CC_PATTERN *cc_pat
 ){

	Ulong   *pat_a = NULL;
	Ulong   *pat_b = NULL;
	Ulong   width_count = 0;
	Ulong   shift_count = 0;
	Ulong   i; /* 外部入力数のカウンタ */
	LINE    *pi = NULL; /* 外部入力のLINEポインタ */
	STATE_3 state3 = STATE3_C;

	char *func_name = "pattern_get_from_generate";

	test_null_variable ( cc_pat, FMT_CC_PATTERN_P, func_name );

	pat_a = cc_pat->pattern_a;
	pat_b = cc_pat->pattern_b;

	/* all Xにリセット */
	pat_a[ width_count ] = 0;
	pat_b[ width_count ] = 0;

	for ( i = 0 ; i < Line_info.n_pi ; i++, shift_count++ ) {

		pi = Pi_head[ i ];

		/* PIの正常値を求める */
		state3 = atpg_get_state9_2_normal_state3 ( pi->state9 );

		if ( shift_count == Fsim_head.bit_size ) {
			shift_count = 0;
			width_count++;

			if ( width_count > Comp_head.width ) {
				printf("width is bad width_count %ld Comp_head.width %ld\n",
					   width_count, Comp_head.width);
			}

			/* all Xにリセット */
			pat_a[ width_count ] = 0;
			pat_b[ width_count ] = 0;
		}


		/*
		  各変数とパターンの関係

		  pi_count(i) 0123 4567 ...                        32 33        ...
		  shift_count 0123 4567 ...                        32 0123 4567 ...
		  width_count 0000      ...                      0000 1111 1111 ...

		        pat_a 0100 0010 0100 0000 0000 0100 0000 0000 0000 0010 ...
				pat_b 0010 0100 0001 0110 0001 0000 0100 0100 0000 0100 ...

			PI state3 X10X X01X X1X0 X00X XXX0 X1XX X0XX X0XX XXXX X01X ...

		*/

		/* $$$ 関数のポインタ化が必要かも $$$ */
		switch ( state3 ) {
		case STATE3_0:
			/* defaultがX(00)なので、0(01)になるように、bだけbitを変化させる */
			pat_b[ width_count ] |= ( BIT_1_ON << shift_count ) ;
			break;
		case STATE3_1:
			/* defaultがX(00)なので、1(01)になるように、aだけbitを変化させる */
			pat_a[ width_count ] |= ( BIT_1_ON << shift_count ) ;
			break;
		case STATE3_X:
		case STATE3_U: /* 単純な入力のUではなく、観測できないPIの場合、Uになる */
			break;
			/* defaultがX(00)なのでなにもしない */
		default:
			topgun_error( FEC_PRG_LINE_STAT, func_name );
		}
	}
}

/********************************************************************************
 * << Function >>
 *		ATPGで生成したパターンを得る
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cc_pat        I/O    CC_PATTERN * 組合せ回路パターン
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I      COMP_HEAD   compact buffer header
 * Pi_head       I      LINE **     Primary Input header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/12
 *
 ********************************************************************************/

void pattern_get_from_ramdom_generate
(
 CC_PATTERN *cc_pat,
 Ulong bit_1_set
 ){

	Ulong   *pat_a = NULL;
	Ulong   *pat_b = NULL;
	Ulong   width_count = 0;
	Ulong   shift_count = 0;
	Ulong   i; /* 外部入力数のカウンタ */
	LINE    *pi = NULL; /* 外部入力のLINEポインタ */

	char *func_name = "pattern_get_from_generate";

	Ulong check_bit = PARA_V1_BIT & bit_1_set;

	test_null_variable ( cc_pat, FMT_CC_PATTERN_P, func_name );

	pat_a = cc_pat->pattern_a;
	pat_b = cc_pat->pattern_b;

	/* all Xにリセット */
	pat_a[ width_count ] = 0;
	pat_b[ width_count ] = 0;

	//printf("get_pat  ");	

	for ( i = 0 ; i < Line_info.n_pi ; i++, shift_count++ ) {

		pi = Pi_head[ i ];


		if ( shift_count == Fsim_head.bit_size ) {
			shift_count = 0;
			width_count++;

			if ( width_count > Comp_head.width ) {
				printf("width is bad width_count %ld Comp_head.width %ld\n",
					   width_count, Comp_head.width);
			}

			/* all Xにリセット */
			pat_a[ width_count ] = 0;
			pat_b[ width_count ] = 0;
		}


		/*
		  各変数とパターンの関係

		  pi_count(i) 0123 4567 ...                        32 33        ...
		  shift_count 0123 4567 ...                        32 0123 4567 ...
		  width_count 0000      ...                      0000 1111 1111 ...

		        pat_a 0100 0010 0100 0000 0000 0100 0000 0000 0000 0010 ...
				pat_b 0010 0100 0001 0110 0001 0000 0100 0100 0000 0100 ...

			PI state3 X10X X01X X1X0 X00X XXX0 X1XX X0XX X0XX XXXX X01X ...

		*/

		/* $$$ 関数のポインタ化が必要かも $$$ */
		/* PIの正常値を求める */
		if ( pi->n_val_a & check_bit ) {
			//STATE3_1:
			//printf("1");
			
			/* defaultがX(00)なので、1(01)になるように、aだけbitを変化させる */
			pat_a[ width_count ] |= ( BIT_1_ON << shift_count ) ;
		}
		else {
			//STATE3_0:
			//printf("0");
			/* defaultがX(00)なので、0(01)になるように、bだけbitを変化させる */
			pat_b[ width_count ] |= ( BIT_1_ON << shift_count ) ;
		}
	}
	//printf("\n");
}

/********************************************************************************
 * << Function >>
 *		組合せパターンの初期化
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cc_pat        I/O    CC_PATERN * メモリ確保/初期化する
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

void compact_cc_pat_initialize
(
 CC_PATTERN *cc_pat
 ){
	Ulong i; /* 圧縮バッファ幅 */

	char *func_name = "compact_cc_pat_initialize";

	test_null_variable( cc_pat, FMT_CC_PATTERN, func_name );
	
	/* とりあえず　全部 "X" に初期化 */
	cc_pat->pattern_a = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );
	cc_pat->pattern_b = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X(a = 0, b = 0) で初期化する */
		cc_pat->pattern_a[ i ] = 0;
		cc_pat->pattern_b[ i ] = 0;
	}
}


/********************************************************************************
 * << Function >>
 *		組合せパターンの初期化
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * comp_info     I/O    COMP_INFO * 圧縮バッファに関する初期化
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

void compact_info_initialize
(
 COMP_INFO *comp_info
 ){

	Ulong i; /* bit_sizeカウンタ */
	
	char *func_name = "compact_info_initialize";

	test_null_variable ( comp_info, FMT_GENE_INFO, func_name );

	/* comp_info */
	comp_info->mode     = COMP_NO_INFO;
	comp_info->n_cc_pat = Fsim_head.bit_size;

	/* パターン並列を使うとき */

	comp_info->cc_pat_wodge  = ( CC_PATTERN ** )topgunMalloc
		( FMT_CC_PATTERN_P,	sizeof( CC_PATTERN * ), Fsim_head.bit_size, func_name );

	for ( i = 0 ; i < Fsim_head.bit_size ; i++ ) {

		comp_info->cc_pat_wodge[ i ]  = ( CC_PATTERN * )topgunMalloc
			( FMT_CC_PATTERN, sizeof( CC_PATTERN ), 1, func_name );
		
		compact_cc_pat_initialize ( comp_info->cc_pat_wodge[ i ] );
	}
}

/********************************************************************************
 * << Function >>
 *		組合せパターンがすべてXかチェックする
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      COMP_PAT_ALL_X     : パターンがすべてXである
 *      COMP_PAT_NOT_X : パターンがすべてXでない
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cc_pattern    I      cc_pattern* 組合せ回路パターン
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/07
 *
 ********************************************************************************/

COMP_CHECK_ALL_X compact_check_pattern_all_x
(
 CC_PATTERN *cc_pattern
 ){

	Ulong i = 0; /* widthカウンタ */
	Ulong pattern = 0; 
	
	char *func_name = "compact_check_pattern_all_x";

	test_null_variable ( cc_pattern, FMT_CC_PATTERN, func_name );
	
	for ( i = 0 ; i < Comp_head.width ; i++ ) {
		/* Xならばすべて0になる */
		pattern |=  ( cc_pattern->pattern_a[ i ] | cc_pattern->pattern_b[ i ] );
	}

	if ( pattern == 0 ) {
		return ( COMP_PAT_ALL_X );
	}
	else {
		return ( COMP_PAT_NOT_X );
	}
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファサイズを返す
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      buf_size : 圧縮バッファサイズ( Ulong )
 *
 * << Argument >>
 * args			(I/O)	type	    description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   compact buffer header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/07
 *
 ********************************************************************************/

Ulong compact_get_buf_size
(
 void
 ){

	return ( Comp_head.buf_size );
}

/********************************************************************************
 * << Function >>
 *		Comp_head実行パラメータの設定
 *
 * << Function ID >>
 *	   	2-?
 *
 * << Return >>
 *      None
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Comp_head     I/O    COMP_HEAD   
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/08
 *
 ********************************************************************************/

void comp_env_init
(
 void
 ){
	/* $$$ 100はデフォルト値 $$$ */
	Comp_head.buf_size = ( 31 );
	utility_renew_parameter_with_env ( "TOPGUN_COM_BUF_SIZE", &( Comp_head.buf_size ), ( 31 ) );
	//printf("Comp_head.buf_size %ld\n", Comp_head.buf_size ) ;
	
}

/********************************************************************************
 * << Function >>
 *		Compactの実行開始フラグ
 *
 * << Function ID >>
 *	   	2-?
 *
 * << Return >>
 * COMP_ON_START : 圧縮開始
 * COMP_ON_STOP  : 圧縮開始せず
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Line_info     I      LINE_INFO
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/08
 *
 ********************************************************************************/

COMP_ON_CHECK comp_on_check
(
 void
 ){
	Ulong static count = 0;

	if ( ( count > 3 ) &&
		 ( ( Line_info.n_fault_atpg - Line_info.n_fault_atpg_count ) * 100 < ( count * 4 * Line_info.n_fault_atpg ) ) ) {

		//printf("pat count : %2ld, nokori %8ld, total %8ld\n",
		//count, Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );
		
		return ( COMP_ON_START );
	}
	count++;

	return ( COMP_ON_STOP ) ;
}

/********************************************************************************
 * << Function >>
 *		圧縮バッファからパラレル故障シミュレーション用のパタンを設定する
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *  
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

void compact_get_parallel_pattern
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
  ){
	
	Ulong *from_pattern_a = NULL;
	Ulong *from_pattern_b = NULL;
	Ulong *to_pattern_a = NULL;
	Ulong *to_pattern_b = NULL;

	Ulong mini_x_id = 0;   
	
	Ulong    i, j; /* カウンタ */

	topgun_2_utility_enter_start_time ( &Time_head.compact );

	fsim_info->n_cc_pat = Fsim_head.bit_size;
	
	/* 圧縮バッファから出たパタンのコピー */
	from_pattern_a = fsim_info->cc_pat->pattern_a;
	from_pattern_b = fsim_info->cc_pat->pattern_b;
	
	to_pattern_a   = comp_info->cc_pat_wodge[ 0 ]->pattern_a;
	to_pattern_b   = comp_info->cc_pat_wodge[ 0 ]->pattern_b;

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		to_pattern_a[ i ] = from_pattern_a[ i ];
		to_pattern_b[ i ] = from_pattern_b[ i ];

		from_pattern_a[ i ] = 0;
		from_pattern_b[ i ] = 0;
	}
	
	//printf("COMP_OUT : ");
	for ( i = 1 ; i < fsim_info->n_cc_pat ; i++ ) {
		/* 一番Xの少ないパターンを放出する */
		/* パターンを取ってくる */


		mini_x_id = Comp_head.min_x_buf_id;

		//printf(" %5lu", mini_x_id );
		
		compact_get_pattern_with_id ( comp_info->cc_pat_wodge[ i ], mini_x_id );

		topgun_print_mes_compact_pattern
			( tmp_pat.pattern_a, tmp_pat.pattern_b, i, Comp_head.buf_size);
		topgun_print_mes_n_compact();
		
		/* 空いたバッファはすべてXに戻す */
		for ( j = 0 ; j < Comp_head.width ; j++ ) {
			Comp_head.buf_a[ mini_x_id ][ j ] = 0;
			Comp_head.buf_b[ mini_x_id ][ j ] = 0;
		}

		/* min_x_bufを再計算する */
		compact_min_x_state_recalc ( mini_x_id ) ;
			
		/* 新しいmin_x_buf_idを再計算する */
		compact_search_min_x_state();
	}
	//printf("\n");
	
	topgun_2_utility_enter_end_time ( &Time_head.compact );
}

/* End of File */
