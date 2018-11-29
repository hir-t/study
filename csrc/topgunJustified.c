/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 正当化に関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/



#include <stdio.h>

#include "topgun.h"
#include "topgunState.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunTime.h"
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h"  /* FEC_XX */

extern GENE_HEAD	Gene_head;
extern JUST_INFO    **Lv_head;

JUST_METHOD just_method  = 1;


JUST_RESULT (* Switch_cand_justified[ TOPGUN_NUM_PRIM] )( JUST_INFO * );
ASS_LIST* (* Switch_line_justified[ TOPGUN_NUM_PRIM ])( JUST_INFO * );

JUST_INFO* (* Switch_justified_method[ NUM_JUST_METHOD ])( JUST_INFO * );

Ulong (* Switch_get_justified_stat )( Ulong );

/* 正当化する際に割当る値 */
static STATE_3 Justified_line_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  未使用 */
		STATE3_C, /* PO  未使用 */
		STATE3_C, /* BR  未使用 */
		STATE3_C, /* INV 未使用 */
		STATE3_C, /* BUF 未使用 */
		STATE3_0,  /* AND        */
		STATE3_0,  /* NAND       */
		STATE3_1,  /* OR         */
		STATE3_1,  /* NOR        */
		STATE3_C, /* XOR  00 or 11のため*/
		STATE3_C, /* XNOR 00 or 11のため*/
		STATE3_C, /* BLKI 未使用 */
		STATE3_C, /* BLKO 未使用 */
		STATE3_C, /* UNK  未使用 */
};


/* 正当化する信号線の値 */
static STATE_3 Justify_line_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  未使用 */
		STATE3_C, /* PO  未使用 */
		STATE3_C, /* BR  未使用 */
		STATE3_C, /* INV 未使用 */
		STATE3_C, /* BUF 未使用 */
		STATE3_0,  /* AND        */
		STATE3_1,  /* NAND       */
		STATE3_1,  /* OR         */
		STATE3_0,  /* NOR        */
		STATE3_C, /* XOR  00 or 11のため*/
		STATE3_C, /* XNOR 00 or 11のため*/
		STATE3_C, /* BLK 未使用 */
		STATE3_C, /* UNK 未使用 */
};

/* 正当化の探索の際を全ての入力を正当化する値 */
static STATE_3 Justified_allin_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  未使用 */
		STATE3_C, /* PO  未使用 */
		STATE3_C, /* BR  未使用 */
		STATE3_C, /* INV 未使用 */
		STATE3_C, /* BUF 未使用 */
		STATE3_1,  /* AND        */
		STATE3_0,  /* NAND       */
		STATE3_0,  /* OR         */
		STATE3_1,  /* NOR        */
		STATE3_C, /* XOR  00 or 11のため*/
		STATE3_C, /* XNOR 00 or 11のため*/
		STATE3_C, /* BLK 未使用 */
		STATE3_C, /* UNK 未使用 */
};

/* 正当化の探索の際を一つ入力を正当化 */
static STATE_3 Justified_single_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  未使用 */
		STATE3_C, /* PO  未使用 */
		STATE3_C, /* BR  未使用 */
		STATE3_C, /* INV 未使用 */
		STATE3_C, /* BUF 未使用 */
		STATE3_0,  /* AND        */
		STATE3_1,  /* NAND       */
		STATE3_1,  /* OR         */
		STATE3_0,  /* NOR        */
		STATE3_C, /* XOR  00 or 11のため*/
		STATE3_C, /* XNOR 00 or 11のため*/
		STATE3_C, /* BLK 未使用 */
		STATE3_C, /* UNK 未使用 */
};


/* function list */
GENE_RESULT atpg_justified ( Ulong ); /* 正当化する信号線を一つ選び、正当化を行う */
static void atpg_enter_justified ( JUST_INFO * ); /* 正当化する信号線をリストへ登録する */
GENE_RESULT atpg_line_justified ( JUST_INFO *, Ulong ); /* 正当化する信号線に値を設定し、含意操作する */
Ulong     atpg_startlist_justified ( void ); /* 最初に正当化が必要な信号線(伝搬したPO)を登録する */
void      atpg_search_justified ( JUST_INFO * ); /* 正当化が必要な信号線を探索する */
static void atpg_enter_line_cand_justified ( LINE *, Uint condition ); /* 正当化が必要な信号線の候補を登録する */
int       atpg_check_finish_justified ( void ); /* 正当化が終了したかをチェックする */
JUST_INFO *atpg_select_line_justified ( void ); /* 正当化する信号線をリストから選択する */
extern void atpg_justified_init ( void ); /* 正当化関連の関数のポインタの初期化をする */
STATE_3   atpg_check_reverse_state_btree ( BTREE *btree ); /* back trace時に逆の値を取れるか判定をする */
void      atpg_enter_reverse_state ( STATE_3 r_state3, BTREE *btree, ASS_LIST *ass_list ); /* 逆の値を設定する */
int       atpg_justified_cand_update ( LINE *line, Ulong cand_in, Ulong ass_in );  /* どの入力を正当化するかを決定する */
Ulong     atpg_justified_get_cand_value ( LINE  *line ); /* 正当化する信号線の評価値を求める */
int       atpg_justified_judge ( Ulong cand_value, Ulong ass_value ); /* 正当化する信号線の評価値の良否を求める */
void      atpg_btree_just_info_reflesh ( JUST_INFO * ); /* just_infoを消去する */
void 	  atpg_line_justified_single_path_trace( ASS_LIST * );
void 	  atpg_line_justified_single_path_trace_with_implying_node( ASS_LIST * );

GENE_BACK gene_back_track_justified ( Ulong ); /* 正当化時に発生したback trackの処理をする */

static Ulong atpg_get_just_info ( void );
JUST_INFO *atpg_make_just_info ( LINE *line, Uint condition );
void      atpg_enter_lv_head_just_info ( JUST_INFO *just_info );


/* switch function */
/* 各gateで正当化する信号線および候補を求める */
static JUST_RESULT atpg_check_cand_justified_in1( JUST_INFO * ); /* po, br, inv */
static JUST_RESULT atpg_check_cand_justified_in0( JUST_INFO * ); /* pi, blk     */
static JUST_RESULT atpg_check_cand_justified_and_nand_or_nor( JUST_INFO * );

#ifdef TOPGUN_XOR_ON
int      atpg_check_cand_justified_xor( JUST_INFO * );
int      atpg_check_cand_justified_xnor( JUST_INFO * );
#endif /* TOPGUN_XOR_ON */

/* 各gateの信号線を正当化する値を求める */
ASS_LIST *atpg_line_justified_and_nand_or_nor( JUST_INFO * ); /* and, nand, or, nor */
ASS_LIST *atpg_line_justified_in1( JUST_INFO * ); /* po, br, inv */
ASS_LIST *atpg_line_justified_in0( JUST_INFO * ); /* pi, blk */

#ifdef TOPGUN_XOR_ON
ASS_LIST *atpg_line_justified_xor( JUST_INFO * );  /* xor  */
ASS_LIST *atpg_line_justified_xnor( JUST_INFO * ); /* xnor */
#endif /* TOPGUN_XOR_ON */


/* other file function list */
/* topgun_state.c */
IMP_RESULT gene_enter_state( ASS_LIST *, Ulong );
STATE_3  atpg_get_state9_2_normal_state3 ( STATE_9 );
STATE_3  atpg_get_state9_2_failure_state3 ( STATE_9 );
Uint     atpg_reverse_condition ( Uint );

/* topgun_state.hのマクロ */
//STATE_3  atpg_get_state9_with_condition_2_state3 ( STATE_9, Uint );

/* topgun_btree.c */
void     atpg_ent_just_btree( JUST_INFO * );
void     gene_ent_btree( ASS_LIST *);
void     atpg_make_btree( void );
void	 gene_imp_trace_2_btree( Ulong );
extern void atpg_btree_enter_just_flag ( JUST_INFO *);

/* topgun_reflesh.c */
void     atpg_btree_state_reflesh( BTREE *btree );

/* tachyon_implication.c */
IMP_RESULT implication ( void );

/* test function list */
void  test_line_null ( LINE *, char * );
void  test_line_in_null ( LINE *, char * );
void  test_line_flag_justify ( char  * );
void  test_state_normal ( Ulong, char * );
void  test_condition ( Uint, char * );
void  test_ass_list ( ASS_LIST *, char * );
void  test_just_info ( JUST_INFO *, char * );
void  test_just_info_null ( JUST_INFO *, char * );
void  test_btree_null ( BTREE *, char * );
void  test_lv_head_null ( char * );

LINE_STACK *atpg_justified_packjust_info ( JUST_INFO *, LINE_STACK * );

/* print function */
void  topgun_print_mes( int, int );
void  topgun_print_mes_id_type_state9( LINE * );
void  topgun_print_mes_just( LINE * );
void  topgun_print_mes_lv_head( void );
void  topgun_print_mes_can_just( LINE * );
void  topgun_print_mes_j_ass_list ( LINE *, ASS_LIST * );
void  topgun_print_mes_justified_search ( LINE *, Uint );
void  topgun_print_mes_gate_at_condition( LINE *, Ulong, Uint );
void  topgun_print_mes_gate_next_condition( LINE *, Ulong, Uint );
void  topgun_print_mes_gate_end_condition( LINE *, Ulong, Uint );
void  topgun_print_mes_just_e ( void );
void  topgun_print_mes_ent_just_flag ( void );
void  topgun_print_mes_btree_ass_only ( void );

/********************************************************************************
 * << Function >>
 *		all justified line check
 *
 * << Function ID >>
 *    	16-1
 *
 * << Return >>
 *	JUSTI_KEEP	: justified continue
 *	JUSTI_CONF	: justified conflict
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

GENE_RESULT atpg_justified
(
 Ulong imp_id
){

	JUST_INFO  *just_info;
	JUST_INFO  *tmp_just;

	GENE_RESULT	result;
	Ulong       start_lv; // 正当化を開始するlv_pi
	Ulong       i;        // カウンタ

	// char       *func_name = "atpg_justified"; //関数名

	topgun_3_utility_enter_start_time ( &Time_head.justified );

	/* btreeのメモリ確保 */
	atpg_make_btree();

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-5-1 未正当化信号線のリストアップ                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* 故障が到達した外部出力から入力側に正当化信号線の探索を開始する */

	/* start_lvと正当化が必要な信号線をLv_headに登録する */
	start_lv = atpg_get_just_info();

	topgun_print_mes ( 25, 0 );
	topgun_print_mes ( 25, 1 );


	for ( i = start_lv+1 ; i >  0 ; ){

		i--;

		just_info = Lv_head[ i ];


		Lv_head[ i ] = NULL;

		while ( just_info ) {

			/* 正当化させるlineを探索し、登録する */
			atpg_search_justified( just_info );


			/* 探索が終ったjust_infoをJUSTIY_NO, _FLのflagの記憶につかう */
			tmp_just  = just_info;
			just_info = just_info->next;

			atpg_btree_enter_just_flag ( tmp_just );

		}
	}

	topgun_print_mes_ent_just_flag ();

	/* Gene_head.btree->just_eに何も登録されない場合は終了 */
	if(Gene_head.btree->just_e == NULL){

		/*
		tmp_btree = Gene_head.btree;
		Gene_head.btree = Gene_head.btree->prev;
		Gene_head.btree->next = NULL;

		topgunFree ( tmp_btree, FMT_BTREE, 1, func_name );
		*/

		topgun_print_mes( 25, 5 );

		topgun_3_utility_enter_end_time ( &Time_head.justified );
		return( JUSTI_CONT );
	}
	topgun_print_mes_just_e ();

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-5-2 未正当化信号線を一つ選択する                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Gene_head.btree->just_eに登録されているlineから一つ選択し、
	   のこりは候補そのまま */
	just_info = atpg_select_line_justified( );

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-5-3 未正当化信号線の値を正当化する                           %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	result = atpg_line_justified( just_info, imp_id ); /* JUSTI_CONT or JUSTI_CONT */
	
	topgun_3_utility_enter_end_time ( &Time_head.justified );

	return( result );
}

/********************************************************************************
 * << Function >>
 *		enter justified lists
 *
 * << Function ID >>
 *    	16-15
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	 I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

void atpg_enter_justified
(
 JUST_INFO *just_info
 ){

	JUST_INFO *enter_just_info;

	char      *func_name = "atpg_enter_justified"; /* 関数名 */

	/* 故障の励起と故障の伝搬で発生 */
	/* tri-stateの1hotとか条件付ATPGとかでも発生しそう */

	test_just_info ( just_info, func_name );

	enter_just_info = atpg_make_just_info ( just_info->line, just_info->condition );

	/* back trace treeに登録 */
	atpg_ent_just_btree( enter_just_info );
}

/********************************************************************************
 * << Function >>
 *		justified by line
 *
 * << Function ID >>
 *    	16-7
 *
 * << Return >>
 *	JUSTI_KEEP	: justified continue
 *	JUSTI_CONF	: justified conflict
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

GENE_RESULT atpg_line_justified
(
 JUST_INFO *just_info,
 Ulong imp_id
 ){
	/* ★★★ 誤り ★★★ */
	/* 正常値が欲しいのか故障値も欲しいのか */
	/* 正当化の条件がみえない */
	/* とりあえず正常値だけ */

	/* ★★★ 修正 ★★★ 2005/09/05 */
	/* 正常値の正当化と故障値の正当化をそれぞれ考える */

	IMP_RESULT imp_result; /* 実施結果 */
	ASS_LIST *ass_list; /* Switch_line_justified の結果 */
	ASS_LIST *tmp_ass_list;
	ASS_LIST *top_ass_list; 

	char     *func_name = "atpg_line_justified"; /* 関数名 */


	topgun_print_mes( 25, 3 );
	topgun_print_mes_id_type_state9( just_info->line );

	// initail imp_result
	imp_result.i_state = IMP_KEEP;
	//imp_result.imp_id  = 0;

	/* 正当化する信号線とその値を求める */
	/* 関数のポインタversion */
	ass_list = Switch_line_justified[ just_info->line->type ]( just_info );

	// Dアルゴリズムのまま(直近の信号線に値を割り振る) 
	top_ass_list = ass_list;

	if ( ass_list->next == NULL ) {

		if ( ass_list->condition == COND_NORMAL ) {
			// case1 : single path traceで外部入力に値を設定する
			// 全て必要なものは難しく、どれでも良いものは簡単なものを
			//atpg_line_justified_single_path_trace( ass_list );
			
			// case2 : その近傍に値を割り振る
			
			// case3 : 一番含意数が大きい信号線に、値を設定する ( with IG )
			//atpg_line_justified_extend_mini_level_node( ass_list );

			// case4 : implying node
			atpg_line_justified_single_path_trace_with_implying_node( ass_list );
		}
			//! もともと故障値を狙っていたけど、後方追跡中に正常値の値に変更になった場合
		//if ( ( ass_list->condition == COND_FAILURE ) &&
		//	 ( ! ( ass_list->line->flag & FV_ASS ) ) ) {
		//		 ass_list->condition = COND_NORMAL;
		//}
	}


	
	test_ass_list ( ass_list, func_name );

	topgun_print_mes_j_ass_list ( just_info->line, ass_list);

	/* assignと正当化信号線に登録 */
    // 値割り当ての段階なので，0を設定する
	imp_result = gene_enter_state( ass_list, 0 );


    if ( imp_result.i_state == IMP_CONF ) {
		/* 衝突する設定値の場合 ASS_CONF->JUSTI_FAIL_ENTERへ変換  */
	
		ass_list = top_ass_list;
		while ( ass_list ) {
			tmp_ass_list = ass_list;
			ass_list = ass_list->next;
			topgunFree ( tmp_ass_list , FMT_ASS_LIST, 1, func_name );
		}

		return( JUSTI_FAIL_ENTER );
    }

	/* back trace tree の新規登録 */
	gene_ent_btree( ass_list );
	
	ass_list = top_ass_list;
	while ( ass_list ) {
		tmp_ass_list = ass_list;
		ass_list = ass_list->next;
		topgunFree ( tmp_ass_list , FMT_ASS_LIST, 1, func_name );
	}

	/* 今迄のbtreeの値割当履歴を表示 */
	topgun_print_mes_btree_ass_only ();

	/* 含意操作 */
    imp_result = implication();

	/* imp_traceの引渡し */
	gene_imp_trace_2_btree( imp_id );

    if ( IMP_CONF == imp_result.i_state ) {
		return( JUSTI_FAIL_IMP );
    }
	return( JUSTI_CONT );
}

ASS_LIST  *atpg_line_justified_in1( JUST_INFO *just_info ){

	char *func_name = "atpg_line_justified_in1"; /* 関数名 */
	topgun_error( FEC_PRG_JUSTIFIED, func_name );

	return( NULL ); /* 来ないけど */
}

ASS_LIST  *atpg_line_justified_in0( JUST_INFO *just_info ){

	char *func_name = "atpg_line_justified_in0"; /* 関数名 */

	topgun_error( FEC_PRG_JUSTIFIED, func_name );

	return( NULL ); /* 来ないけど */
}

/********************************************************************************
 * << Function >>
 *		entry candidate justified line list
 *
 * << Function ID >>
 *    	16-3
 *
 * << Return >>
 *     lv_pi    : 故障が伝搬した信号線のレベル
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/9
 *		2005/11/9 << M.Yoshimura FLEETS >> JUST_INFO導入により全面改訂
 *
 ********************************************************************************/

Ulong atpg_startlist_justified
(
 void
){
	/* 1stは故障が伝搬したPOだけ */
	/* 故障の入力はどうせたどれるはず */

	JUST_INFO  *just_info = NULL;

	char       *func_name = "atpg_startlist_justified"; /* 関数名 */

	/* $$$ dfront_cが伝搬したPOとはかぎらない $$$ */
	test_line_null ( Gene_head.propa_line, func_name );


	Gene_head.propa_line->flag |= JUSTIFY_NO;

	/* 正常値の情報を作成 */
	just_info = atpg_make_just_info ( Gene_head.propa_line, COND_NORMAL );

	/* 最初はLv_headへ直接登録する */
	atpg_enter_lv_head_just_info ( just_info );


	Gene_head.propa_line->flag |= JUSTIFY_FL;
	/* 故障値の情報を作成 */
	just_info = atpg_make_just_info ( Gene_head.propa_line, COND_FAILURE );

	/* 最初はLv_headへ直接登録する */
	atpg_enter_lv_head_just_info ( just_info );

	return ( Gene_head.propa_line->lv_pi );
}

/********************************************************************************
 * << Function >>
 *		現在の信号線からさらに探索する信号線と正当化する信号線を求める
 *
 * << Function ID >>
 *    	16-5
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description

 *	Switch_cand_justified				select function
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/9
 *
 ********************************************************************************/

void atpg_search_justified
(
 JUST_INFO *just_info
 ){
	LINE  *line;
	Uint  condition;
	JUST_RESULT just_result;
	Ulong i;

	char  *func_name = "atpg_search_justified"; /* 関数名 */

	test_just_info ( just_info, func_name );

	line      = just_info->line;
	condition = just_info->condition;

	/* JUSTIFYフラグを立てる個所とjustに登録する個所を求める */

	topgun_print_mes_justified_search ( line, condition );

	just_result = Switch_cand_justified[ line->type ]( just_info );

#ifndef TOPGUN_TEST
	if ( just_result == JUST_ERROR ) {
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
#endif /* TOPGUN_TEST */

	/* 結果の確認と次への処理 */
	if ( just_result & JUST_OWN ) {
		/* 正当化リストへ登録する */
		atpg_enter_justified( just_info );

		topgun_print_mes_gate_at_condition( line, OUT_PIN_NUM, condition );
	}
	else if ( just_result & JUST_ALL ) {

		for ( i = 0 ; i < line->n_in ; i++ ) {

			topgun_print_mes_gate_next_condition( line, i+1, condition );

			/* すべてのinにJUSTIFY flagをたてる場合 */
			atpg_enter_line_cand_justified( line->in[ i ], condition );

		}
	}
	else if ( just_result & JUST_1IN ) {

		/* in?にJUSTIFY flagをたてる場合 */
		i = (int)( ( just_result - JUST_1IN ) / JUST_1IN_N );

		topgun_print_mes_gate_next_condition( line, i+1, condition );

		atpg_enter_line_cand_justified( line->in[ i ], condition );

	}
	else if ( just_result & JUST_NONE ) {
		topgun_print_mes_gate_end_condition( line, OUT_PIN_NUM, condition );
	}
	else {
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
}


/********************************************************************************
 * << Function >>
 *		entry candidate justified line list
 *
 * << Function ID >>
 *    	16-16
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/10
 *
 ********************************************************************************/

void atpg_enter_line_cand_justified
(
 LINE *line,
 Uint condition
 ){

	JUST_INFO *just_info = NULL;
	char      *func_name = "atpg_enter_line_cand_justified"; /* 関数名 */

	test_line_null ( line, func_name );
	test_condition ( condition, func_name );

	if ( condition == COND_FAILURE ) {
		if ( ( ! ( line->flag & FP_IAP ) ) ||
			 ( line->flag & LINE_FLT ) ) {

			/* 故障伝搬の可能性がない場合 */
			condition = COND_NORMAL;
		}
	}

	/* チェック関数 */
	/* 重い... flag使った方が楽 */
	/* 結局flagで対応 */
	if ( condition == COND_NORMAL ) {
		if ( line->flag & JUSTIFY_NO ) {
			/* 登録済のためなにもしない */
			;
		}
		else {
			line->flag |= JUSTIFY_NO;
			/* just_info 作成 */
			just_info = atpg_make_just_info ( line, condition );

			/* Lv_headへ登録 */
			atpg_enter_lv_head_just_info ( just_info );
		}
	}
	else {
		if ( line->flag & JUSTIFY_FL ) {
			/* 登録済のためなにもしない */
			;
		}
		else {
			line->flag |= JUSTIFY_FL;
			/* just_info 作成 */
			just_info = atpg_make_just_info ( line, condition );

			/* Lv_headへ登録 */
			atpg_enter_lv_head_just_info ( just_info );
		}
	}
}


/********************************************************************************
 * << Function >>
 *		judge xor gate for justified
 *
 * << Function ID >>
 *    	16-14
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *      2005/09/29 multi gate 
 *
 ********************************************************************************/

#ifdef TOPGUN_XOR_ON
int atpg_check_cand_justified_xor
(
 JUST_INFO *just_info
 ){
	LINE *line  = just_info->line;

	int  result = 0; /* 処理結果変数の初期化 */

	STATE_3 out; /* state of justified line  */
	STATE_3 in0; /* state of line of input No.0 of justified line */
	STATE_3 in1; /* state of line of input No.1 of justified line */

	char *func_name = "atpg_check_cand_justified_xor"; /* 関数名 */

	test_just_info ( just_info, func_name );
	
	if ( just_info->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		out = atpg_get_state9_2_normal_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_normal_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_normal_state3( line->in[ 1 ]->state9 ) ;
	}
	else {
		/* COND_FAILURE */
		out = atpg_get_state9_2_failure_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_failure_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_failure_state3( line->in[ 1 ]->state9 ) ;
	}
	
	/* 正常値によって分岐 */
	if ( STATE3_X == out ) {
		;
	}
	else{
		/* in0 X かつ in1 Xなら自身を正当化 */
		if ( ( STATE3_X == in0 ) &&
			 ( STATE3_X == in0 ) ) {
			result |= JUST_OWN;
			return( result );
		}
		/* in0 X かつ in1 Xでないなら入力二つを正当化 */
		result |= JUST_ALL;

	}
	return ( result );
}

#endif /* TOPGUN_XOR_ON */

/********************************************************************************
 * << Function >>
 *		judge "XNOR" gate for justified
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *
 ********************************************************************************/

#ifdef TOPGUN_XOR_ON
int atpg_check_cand_justified_xnor
(
 JUST_INFO *just_info
 ){

	LINE *line  = just_info->line;
	int  result = 0; /* 処理結果変数の初期化 */

	STATE_3 out; /* state of justified line  */
	STATE_3 in0; /* state of line of input No.0 of justified line */
	STATE_3 in1; /* state of line of input No.1 of justified line */

	char *func_name = "atpg_check_cand_justified_xnor"; /* 関数名 */

	test_just_info ( just_info, func_name );
	
	if ( just_info->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		out = atpg_get_state9_2_normal_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_normal_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_normal_state3( line->in[ 1 ]->state9 ) ;
	}
	else {
		/* COND_FAILURE */
		out = atpg_get_state9_2_failure_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_failure_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_failure_state3( line->in[ 1 ]->state9 ) ;
	}
	
	/* 正常値によって分岐 */
	/* $$$ 別にしなくても良い? $$$ */
	if ( STATE3_X == out) {
		;
	}
	else{
		/* in0 X かつ in1 Xなら自身を正当化 */
		if ( ( STATE3_X == in0 ) &&
			 ( STATE3_X == in1 ) ) {
			result |= JUST_OWN;
			return( result );
		}
		/* in0 X かつ in1 Xでないなら入力二つを正当化 */
		result |= JUST_ALL;

#ifndef TOPGUN_CIRCUIT
		/* 含意操作もしくは論理式が不成立 */
		/* 正常値と故障値の両方がおかしいわけではない */
		/*
		if ( ( in0 == STATE3_X ) ||
			 ( in1 == STATE3_X ) ) {

			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		*/
#endif /* TOPGUN_CIRCUIT */
	}
	return( result );
}
#endif /* TOPGUN_XOR_ON */
/********************************************************************************
 * << Function >>
 *		judge gate with only one input for justified
 *
 * << Function ID >>
 *    	16-11
 *
 * << Return >>
 *	JUST_ALL    : 正当化フラグを入力へ伝搬
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/10
 *
 ********************************************************************************/


JUST_RESULT atpg_check_cand_justified_in1
(
 JUST_INFO *just_info
 ){

#ifndef TOPGUN_TEST
	STATE_3 normal3;
	STATE_3 failure3;

	char *func_name = "atpg_check_cand_justified_in1"; /* 関数名 */

	/* 含意操作のミスのみ */
	normal3  = atpg_get_state9_2_normal_state3( just_info->line->state9 );
	failure3 = atpg_get_state9_2_failure_state3( just_info->line->state9 );
	if ( ( normal3 == STATE3_X ) && ( failure3 == STATE3_X ) ) {
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
#endif /* TOPGUN_TEST */

	return( JUST_ALL );
}

/********************************************************************************
 * << Function >>
 *		judge gate with NO input for justified
 *
 * << Function ID >>
 *    	16-12
 *
 * << Return >>
 *      int (Void)
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/28
 *
 ********************************************************************************/


JUST_RESULT atpg_check_cand_justified_in0
(
 JUST_INFO *just_info
 ){
	/*	char *func_name = "atpg_check_cand_justified_in0"; *//* 関数名 */
	/* 	void       topgun_error(); */
	/* 	topgun_error( FEC_PRG_JUSTIFIED, func_name ); */

	return(JUST_NONE);
}


/********************************************************************************
 * << Function >>
 *		check finished to justified
 *
 * << Function ID >>
 *    	16-2
 *
 * << Return >>
 *  JUST_END    : finish to propagate fault
 *  JUST_CONT   : don't finish to propagate fault
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/18
 *
 ********************************************************************************/

int atpg_check_finish_justified
(
 void
){

	/* char       *func_name = "atpg_check_finish_justified"; *//* 関数名 */

	if ( Gene_head.flag & ATPG_JUSTI_S )  {  /* ATPG_JUSTI_Sが立ってて */
		/* atpg_get_just_infoを参照 */

		if ( Gene_head.btree == NULL ) {
			/* start前 */
			return ( JUSTI_CONT );
		}
		else if ( Gene_head.btree->just_e != NULL ) {
				return( JUSTI_CONT );			
		}
		/* Lv_head[]がNULLでなければ */
		/*
		start_lv = Gene_head.dfront_c->line->lv_pi;
	
		for ( i = start_lv+1 ; i > 0 ; ) {
			i--;
			if ( Lv_head[ i ] != NULL ) {        
				return( JUSTI_KEEP );
			}
		}
		*/
	}
	/* justifiedの必要候補がないなら終了 */
	/* 故障伝搬とも正当化も終了 = ATPG終了 */
	/* 2005/11/8 削除 */
	/*
	Gene_head.btree->imp_trace = Gene_head.imp_trace;
	Gene_head.imp_trace        = NULL;
	*/
	/* 2005/11/8 ここまで */
	
	/* 終了時のbtreeは空なので解放/消去 */

	/* $$$ なぜ２段ん戻るの ?? $$$ */
	/* just_flagの関係で戻しちゃだめ 
	tmp_btree = Gene_head.btree;
	Gene_head.btree = Gene_head.btree->prev;
	Gene_head.btree = Gene_head.btree->prev;

	topgunFree ( tmp_btree, FMT_BTREE, 1, func_name );
	*/

	topgun_print_mes( 25, 4 );
	
	return( JUSTI_PASS );
}



/********************************************************************************
 * << Function >>
 *		select line for justified
 *
 * << Function ID >>
 *    	16-6
 *
 * << Return >>
 *	line		: line poitner for justified
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

JUST_INFO  *atpg_select_line_justified
(
 void
){

	JUST_INFO  *select_just = NULL;
	//JUST_INFO  *just_info;

	char       *func_name = "atpg_select_line_justified"; /*  関数名 */

	test_btree_null ( Gene_head.btree, func_name );
	test_just_info ( Gene_head.btree->just_e, func_name );

	select_just = Switch_justified_method[ just_method ] ( Gene_head.btree->just_e ) ;

	return( select_just );
}

/********************************************************************************
 * << Function >>
 *		select line for justified
 *
 * << Function ID >>
 *    	16-6
 *
 * << Return >>
 *	line		: line poitner for justified
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

JUST_INFO  *atpg_select_line_justified_lv_pi_high
(
 JUST_INFO *just_info
){

	JUST_INFO  *select_just;

	char       *func_name = "atpg_select_line_justified_lv_pi_high"; /*  関数名 */

	test_just_info ( just_info, func_name );

	select_just = just_info;

	while ( just_info ) {
		/* lv_piの最も大きいjust_infoを選択する */
		if ( select_just->line->lv_pi < just_info->line->lv_pi ) {
			select_just = just_info;
		}
		just_info = just_info->next ;
	}

	return( select_just );
}

/********************************************************************************
 * << Function >>
 *		select line for justified
 *
 * << Function ID >>
 *    	16-6
 *
 * << Return >>
 *	line		: line poitner for justified
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

JUST_INFO  *atpg_select_line_justified_lv_pi_low
(
 JUST_INFO *just_info
){

	JUST_INFO  *select_just;

	char       *func_name = "atpg_select_line_justified_lv_pi_low"; /*  関数名 */

	test_just_info ( just_info, func_name );

	select_just = just_info;

	while ( just_info ) {
		/* lv_piの最も大きいjust_infoを選択する */
		if ( select_just->line->lv_pi > just_info->line->lv_pi ) {
			select_just = just_info;
		}
		just_info = just_info->next ;
	}

	return( select_just );
}

/********************************************************************************
 * << Function >>
 *		back track on justified
 *
 * << Function ID >>
 *    	16-4
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 * 		Masayoshi Yoshimura (Kyushu University)
 *
 * << Date >>
 *		2005/04/26 initailize
 *		2007/06/05 add imp_id
 *
 ********************************************************************************/


GENE_BACK gene_back_track_justified
(
 Ulong imp_id
){
	IMP_RESULT imp_result;
	STATE_3  r_state3;
	BTREE    *btree;
	BTREE    *tmp_btree; /* free用テンポラリ */
	ASS_LIST ass_list;
	ASS_LIST *top_ass_list;
	ASS_LIST *tmp_ass_list;
		
	char     *func_name = "gene_back_track_justified"; /* 関数名 */

	//inital imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id = 0;
	
	btree = Gene_head.btree;

	while ( btree != NULL ) {

		/* 最近正当化した値とそれに伴う含意操作の値を元に戻す */
		atpg_btree_state_reflesh( btree );


		/* 逆の値が設定できるかを調べる */
		r_state3 = atpg_check_reverse_state_btree( btree );

		if ( r_state3 != STATE3_C ) {

			/* 逆の値を設定する(btree) */
			atpg_enter_reverse_state ( r_state3, btree, &ass_list );

			test_ass_list ( &ass_list, func_name );
			
			/* assignと正当化信号線に登録 */
            // 一応含意に近いので，0としておく$$$$
            // imp_idはreverse_stateの番号-1が正解
			imp_result = gene_enter_state( &ass_list, 0 );

			top_ass_list = ass_list.next;
			while( top_ass_list ) {
				tmp_ass_list = top_ass_list;
				top_ass_list = top_ass_list->next;
				topgunFree ( tmp_ass_list , FMT_ASS_LIST, 1, func_name );
			}
			
			if ( IMP_CONF != imp_result.i_state ) {
				/* 含意操作を実施 */
				imp_result = implication();
				
				/* imp_traceの引渡し */
				gene_imp_trace_2_btree( imp_id );
				
				if ( IMP_CONF == imp_result.i_state ) {
					/* 設定したけど失敗した場合 */
					topgun_print_mes ( 18, 4 );
					
					/* まず今のbtreeで設定した値を元にもどす */
					/* 設定した逆の値を元にもどす */
					topgun_print_mes ( 18, 6 );
					atpg_btree_state_reflesh( btree );
				}
				else {
					/* 含意操作が成功したときはback track終了 */
					topgun_print_mes ( 18, 5 );
					return ( GENE_BACK_CONT );
				}
			}
			else{
				topgun_print_mes ( 18, 7 );
			}
		}

		/* 逆の値を設定できない */

		/* btreeのJUSTFY_NO, FLを戻し、just_e/dをfreeする */
		atpg_btree_just_info_reflesh ( btree->just_e );
		atpg_btree_just_info_reflesh ( btree->just_d );
		atpg_btree_just_info_reflesh ( btree->just_flag );

		btree->just_e = NULL;
		btree->just_d = NULL;
		
		/* 一段戻る */
		tmp_btree = btree;
		btree = btree->prev;
		if ( btree == NULL ) {
			/* 故障の伝搬箇所のJUSTIFY_NO, _FLを戻す */
			test_line_flag_justify( func_name );
			
		}
		else {
			btree->next = NULL;
		}

		Gene_head.btree = btree;

		topgunFree ( tmp_btree, FMT_BTREE, 1, func_name );
		
		topgun_print_mes ( 18, 6 );
	}
	/* justified で設定した値がすべて駄目 */
	/* propagateに戻る */
	return ( GENE_BACK_END );
}

/********************************************************************************
 * << Function >>
 *		initailized justified function
 *
 * << Function ID >>
 *    	6-8
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * 		void
 *
 * << extern >>
 * name			(I/O)	type		description
 *
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/6
 *
 ********************************************************************************/

void atpg_justified_init
(
 void
){
	Switch_cand_justified[ TOPGUN_PI   ] = &atpg_check_cand_justified_in0; /* pi  */
	Switch_cand_justified[ TOPGUN_PO   ] = &atpg_check_cand_justified_in1; /* po  */
	Switch_cand_justified[ TOPGUN_BR   ] = &atpg_check_cand_justified_in1; /* br  */
	Switch_cand_justified[ TOPGUN_INV  ] = &atpg_check_cand_justified_in1; /* inv */
	Switch_cand_justified[ TOPGUN_BUF  ] = &atpg_check_cand_justified_in1; /* buf */
	Switch_cand_justified[ TOPGUN_AND  ] = &atpg_check_cand_justified_and_nand_or_nor; /* and */
	Switch_cand_justified[ TOPGUN_NAND ] = &atpg_check_cand_justified_and_nand_or_nor; /* nand */
	Switch_cand_justified[ TOPGUN_OR   ] = &atpg_check_cand_justified_and_nand_or_nor; /* or */
	Switch_cand_justified[ TOPGUN_NOR  ] = &atpg_check_cand_justified_and_nand_or_nor; /* nor */
	Switch_cand_justified[ TOPGUN_BLKI ] = &atpg_check_cand_justified_in0; /* blk in */
	Switch_cand_justified[ TOPGUN_BLKO ] = &atpg_check_cand_justified_in1; /* blk out */	
	
#ifdef TOPGUN_XOR_ON
	Switch_cand_justified[ TOPGUN_XOR  ] = &atpg_check_cand_justified_xor; /* xor */
	Switch_cand_justified[ TOPGUN_XNOR ] = &atpg_check_cand_justified_xnor;/* xnor */
#endif /* TOPGUN_XOR_ON */


	Switch_line_justified[ TOPGUN_PI   ] = &atpg_line_justified_in0; /* pi  */
	Switch_line_justified[ TOPGUN_PO   ] = &atpg_line_justified_in1; /* po  */
	Switch_line_justified[ TOPGUN_BR   ] = &atpg_line_justified_in1; /* br  */
	Switch_line_justified[ TOPGUN_INV  ] = &atpg_line_justified_in1; /* inv */
	Switch_line_justified[ TOPGUN_BUF  ] = &atpg_line_justified_in1; /* buf */
	Switch_line_justified[ TOPGUN_AND  ] = &atpg_line_justified_and_nand_or_nor; /* and */
	Switch_line_justified[ TOPGUN_NAND ] = &atpg_line_justified_and_nand_or_nor; /* nand */
	Switch_line_justified[ TOPGUN_OR   ] = &atpg_line_justified_and_nand_or_nor; /* or */
	Switch_line_justified[ TOPGUN_NOR  ] = &atpg_line_justified_and_nand_or_nor; /* nor */
	Switch_line_justified[ TOPGUN_BLKI ] = &atpg_line_justified_in0; /* blk */
	Switch_line_justified[ TOPGUN_BLKO ] = &atpg_line_justified_in1; /* blk */	

#ifdef TOPGUN_XOR_ON	
	Switch_line_justified[ TOPGUN_XOR  ] = &atpg_line_justified_xor; /* xor */
	Switch_line_justified[ TOPGUN_XNOR ] = &atpg_line_justified_xnor;/* xnor */
#endif /* TOPGUN_XOR_ON */	

	Switch_justified_method[ JUST_METHOD_LV_PI_HIGH ] = &atpg_select_line_justified_lv_pi_high;
	Switch_justified_method[ JUST_METHOD_LV_PI_LOW  ] = &atpg_select_line_justified_lv_pi_low; 
	
}


/********************************************************************************
 * << Function >>
 *		back track on justified
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  ASS_R_CONF  : conflict reverse state for back trace
 *  STAT_??     : reverse state for back trace
 *
 * << Argument >>
 * name			(I/O)	type			description
 * btree         I      BTREE *         current back trace tree
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/8/26
 *
 ********************************************************************************/


STATE_3 atpg_check_reverse_state_btree
(
 BTREE *btree
){
	STATE_3 state3;
	STATE_3 r_state3;
	Ulong flag;
	Uint  reverse_condition = 0;
	
	char *func_name = "atpg_check_reverse_state_btree"; /* 関数名 */

	flag = btree->flag;

	state3 = STATE3_C; /* initialize */

	topgun_print_mes ( 18, 0 );
	
	/* 通常時がすでに両方試している */
	if ( ( ( flag & ASS_N_0 ) && ( flag & ASS_N_1 ) )|| 
		 ( ( flag & ASS_F_0 ) && ( flag & ASS_F_1 ) ) ) {

		topgun_print_mes ( 18, 2 );
		return ( STATE3_C ); /* 反転値を設定できない */
	}

	/* 逆のcondition(正 or 故)を求める */
	reverse_condition = atpg_reverse_condition ( btree->condition );

	/* 逆のconditionのstateを求める */
	r_state3 = atpg_get_state9_with_condition_2_state3
		( btree->line->state9, reverse_condition );
		
	if ( ( btree->line->flag & FP_IAP ) ||
		 ( r_state3 == STATE3_X ) ) {
	
		/* 正常値/故障値の両方が設定できる */
	
		/* ass_statから設定した値をもとめる */
		switch ( btree->ass_state3 ) {
		case STATE3_0:
			state3 = STATE3_1;
			break;
		case STATE3_1:
			state3 = STATE3_0;
			break;
		case STATE3_X:
		case STATE3_U:
		default:
			topgun_error( FEC_PRG_BACK_TRACK, func_name );		
		}
	}
	else {
		topgun_print_mes ( 18, 3 );
	}

	return ( state3 );
}

/********************************************************************************
 * << Function >>
 *		back track on justified
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  ASS_R_CONF  : conflict reverse state for back trace
 *  STAT_??     : reverse state for back trace
 *
 * << Argument >>
 * name			(I/O)	type			description
 * btree         I      BTREE *         current back trace tree
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/8/26
 *
 ********************************************************************************/


void atpg_enter_reverse_state
(
 STATE_3  r_state3,
 BTREE    *btree,
 ASS_LIST *ass_list
){
	
	char *func_name = "atpg_enter_reverse_state"; /* 関数名 */
	
	/* 逆の値を登録 */
	btree->ass_state3 = r_state3;

	/* btree->flagへ値の登録を記録する */

	test_condition ( btree->condition, func_name );

	if ( btree->condition == COND_NORMAL ) {
		ass_list->condition = COND_NORMAL;
		if ( r_state3 & STATE3_0 ) {
			btree->flag |= ASS_N_0;
		}
		if ( r_state3 & STATE3_1 ) {
			btree->flag |= ASS_N_1;
		}
	}
	else {
		ass_list->condition = COND_FAILURE;
		if ( r_state3 & STATE3_0 ) {
			btree->flag |= ASS_F_0;
		}
		if ( r_state3 & STATE3_1 ) {
			btree->flag |= ASS_F_1;
		}
	}

	ass_list->ass_state3  = r_state3;

	/* *** 変更が必要 *** */
	ass_list->line     = btree->line;
	ass_list->next     = NULL;

	test_ass_list ( ass_list, func_name );
	
}


/********************************************************************************
 * << Function >>
 *		and gate justisfied 
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	ass			: justified list
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/4
 *		2005/9/28 for multi input  and multi gate
 *
 ********************************************************************************/

ASS_LIST  *atpg_line_justified_and_nand_or_nor
(
 JUST_INFO *just_info
 ){

	LINE     *line;
	Uint     condition;
	Ulong    i;         // 入力信号線数カウンタ
	Ulong    ass_in;    // 値を割当る入力信号線数の番号

	STATE_3  line_state3;
	STATE_3  in_state3;

	ASS_LIST *ass_list = NULL;   // 値割り当ての信号線と値

	char    *func_name = "atpg_line_justified_and_nand_or_nor"; /* 関数名 */

	// とりあえず正常値だけ考える
	/* $$$ だめ!! 故障値も必要 2005/6/2 sub化
	   -> いやこれは含意操作では?? $$$ */

	test_just_info ( just_info, func_name );

	line      = just_info->line;
	condition = just_info->condition;

	line_state3 = atpg_get_state9_with_condition_2_state3
		( line->state9, condition );

	if ( line_state3 == Justify_line_state[ line->type ] ) {

		/* どこかに0(and/nand) or 1(or/nor) を設定する */
		/* 今stateがXでかつ条件を満す信号線を求める */
		/* 2005/09/29での条件はtm0が一番低い信号線 */
		/* $$$ 多入力ゲート段数などが考慮の候補 $$$ */

		ass_list = ( ASS_LIST * )topgunMalloc
			( FMT_ASS_LIST, sizeof( ASS_LIST ), 1, func_name );

		ass_list->next = NULL;


		/* 値を割当る信号線を求める */
		ass_in = line->n_in;

		for ( i = 0 ; i < line->n_in ; i++ ) {
			/* まずin[i]の値がXか調べる */

			in_state3 = atpg_get_state9_with_condition_2_state3
				(  line->in[ i ]->state9, condition );

			if ( STATE3_X == in_state3 ) {
				/* どの入力信号線を選択するのか */
				if ( atpg_justified_cand_update( line, i, ass_in ) ) {
					/* 更新する */
					ass_in = i;
				}
			}
		}

#ifndef TOPGUN_TEST
		if ( ass_in == line->n_in ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
#endif /* TOPGUN_TEST */

		/* lineの設定 */
		ass_list->line = line->in [ ass_in ];

		/* pre_statの設定 */
		/* **** 要確認 *** */
		/*
		ass->pre_stat = ass->line->stat;
		*/

		/* ass_statの設定 */
		ass_list->ass_state3 = Justified_line_state[line->type];

		/* condition の設定 */

		ass_list->condition = COND_NORMAL ;
		if ( condition == COND_FAILURE ) {
			if ( line->flag & FV_ASS ) {
				ass_list->condition = condition ;
			}
		}
	}
	else {
		/* 正当化する値がただしくないの場合 */
		topgunFree( ass_list, FMT_ASS_LIST, 1, func_name );
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	test_ass_list ( ass_list, func_name );

	return( ass_list );
}




/********************************************************************************
 * << Function >>
 *		どの入力信号線を選択するのかを决める関数
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  0           : stay
 *  1           : update
 *  if()文で判定するため
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line of start point for justified
 * cand_in       I      Ulong           the number of new candidate input line
 * ass_in        I      Ulong           the number of original candidate input line 
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/9/28
 *
 ********************************************************************************/


int atpg_justified_cand_update
(
 LINE  *line,
 Ulong cand_in,
 Ulong ass_in
){

	Ulong 	cand_value; /* 現在の候補の評価値 */
	Ulong 	ass_value;  /* 元々の候補の評価値 */
	
	if ( ass_in == line->n_in ) {
		/* 候補がまだ何も決っていない */
		/* -> 更新する */
		return ( 1 ) ;
	}

	/* 現在の候補の値を求める */
	cand_value = atpg_justified_get_cand_value ( line->in[ cand_in ] );
	
	/* 元々の候補の値を求める */
	/* $$$ 手間かかるなら引数にする手がある $$$ */
	ass_value  = atpg_justified_get_cand_value ( line->in[ ass_in  ] );
		
	return ( atpg_justified_judge ( cand_value , ass_value ) );
}

/********************************************************************************
 * << Function >>
 *		どの入力信号線を選択する基準となる値を求める関数
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  value       : 信号線を選択する基準値
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line for calclurate judgement value
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/9/28
 *
 ********************************************************************************/


Ulong atpg_justified_get_cand_value 
(
 LINE  *line
){
	Ulong value = 0;

	char  *func_name = "atpg_justified_get_cand_value"; /* 関数名 */

	/* 2005/09/29時点ではtm */
	switch ( line->out[0]->type ) {
	case TOPGUN_AND:
	case TOPGUN_NAND:
		value = line->tm_c0;
		break;
	case TOPGUN_OR:
	case TOPGUN_NOR:
		value = line->tm_c1;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
			
	return ( value );
}

/********************************************************************************
 * << Function >>
 *		どの入力信号線を選択する基準となる値から選択する関数
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  0           : stay
 *  1           : update
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line for calclurate judgement value
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/9/28
 *
 ********************************************************************************/


int atpg_justified_judge
(
 Ulong cand_value ,
 Ulong ass_value
 ){

	/* 2005/09/28ではmin */
	/* $$$ コマンド化が必要 $$$ */
	if ( cand_value < ass_value ) {
		return ( 1 );
	}
	else {
		return ( 0 );
	}
}

/********************************************************************************
 * << Function >>
 *		judge "AND/NAND/OR/NOR" gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/7
 *		2005/9/29 multi gate and multi input
 *
 ********************************************************************************/

JUST_RESULT atpg_check_cand_justified_and_nand_or_nor
(
 JUST_INFO *just_info
 ){
	STATE_3 in_state3;
	STATE_3 out_state3;

	LINE        *line  = just_info->line;
	JUST_RESULT just_result = JUST_ERROR; /* 処理結果変数の初期化 */

	Ulong   input_n;  /* the number of justified line */
	Ulong   i;        /* counter */

	char *func_name = "atpg_check_cand_justified_and_nand_or_nor"; /* 関数名 */

	test_just_info ( just_info, func_name );

	in_state3 = STATE3_C ; /* initialize */

	if ( just_info->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		out_state3 = atpg_get_state9_2_normal_state3 ( line->state9 );
	}
	else {
		/* COND_FAILURE */
		out_state3 = atpg_get_state9_2_failure_state3 ( line->state9 );
	}

	/* 出力値に応じて、どの信号線を正当化するか考える */

	if ( out_state3 == Justified_single_state[ line->type ] ) {

		input_n = FL_ULMAX; /* 初期化 */

		/* どの入力信号線が制御値なのか調べる */
		for ( i = 0 ;  i < line->n_in ; i++ ) {

			if ( just_info->condition == COND_NORMAL ) {
				in_state3 = atpg_get_state9_2_normal_state3
					( line->in[ i ]->state9 );
			}
			else {
				in_state3 = atpg_get_state9_2_failure_state3
					( line->in[ i ]->state9 );
			}


			if ( in_state3 == Justified_line_state[ line->type ] ) {
				/* 制御値の場合 */
				/* $$$ 簡易版 $$$ */
				input_n = i;
				break;
			}
			else {
				/* 制御値でない場合 */
				;
			}
		}

		/* 制御値の入力があるか */
		if( input_n != FL_ULMAX ) {
			/* 制御値の入力があった場合 */
			just_result = JUST_1IN;
			just_result += ( JUST_1IN_N * input_n );
		}
		else {
			/* 制御値の入力がなかった場合 = ALL X */
			just_result = JUST_OWN;
		}
	}
	else {
		if ( out_state3 == Justified_allin_state[ line->type ] ) {
			/* すべて非制御値の場合(AND/NOR = 1 , OR/NAND == 0) */
			just_result = JUST_ALL;
		}
		else {
			/* X or Uの場合 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}

	return ( just_result );
}

/********************************************************************************
 * << Function >>
 *		xor gate justisfied
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	ass			: justified list
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/8/2
 *		2005/10/21 for condition
 *
 ********************************************************************************/

/* 値割り当て1個所だけversion */

ASS_LIST  *atpg_line_justified_xor
(
 JUST_INFO *just_info
 ){
	LINE	 *in0;
	LINE     *in1;
	
	ASS_LIST *ass_list;
	
	STATE_3  line_state3;
	STATE_3  in0_state3;
	STATE_3  in1_state3;

	char     *func_name = "atpg_line_justified_xor"; /* 関数名 */

	test_just_info ( just_info, func_name );
	test_line_in_null ( just_info->line, func_name );
	
	ass_list = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ),
										1, func_name );
	/* inialized */
	ass_list->next  = NULL;
	ass_list->ass_state3 = STATE3_X;
	ass_list->condition = just_info->condition;

	in0      = just_info->line->in[ 0 ];
	in1      = just_info->line->in[ 1 ];
	
	/* とりあえず正常値優先でやってみた */
	if ( ass_list->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		line_state3 = atpg_get_state9_2_normal_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_normal_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_normal_state3 ( in1->state9 );
	}
	else {
		/* COND_FAILURE */
		line_state3 = atpg_get_state9_2_failure_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_failure_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_failure_state3 ( in1->state9 );
	}


	/* とりあえず正常値だけ考える */
	switch ( line_state3 ) {
	case STATE3_0:
		/* 11 or 00 にする */

		/* 片方がX, 片方が0 */
		/* case 1: in0 0 / in1 X -> in1 に 0を                */
		/* case 2: in0 X / in1 0 -> in1  0 が次のjustified    */

		/* 片方がX, 片方が1 -> Xに1をassign */ 
		/* case 3: in0 1 / in1 X -> in1  0 が次のjustified    */
		/* case 4: in0 X / in1 1 -> high 0 が次のjustified    */

		/* 両方がX、11か00の安い方 */
		/* case 5: in0 X / in1 X -> in1  0 が次のjustified    */


		/* NG(値が既に決っているため) */
		/* case 6: in0 0 / in1 0 -> just flag だけ            */
		/* case 7: in0 0 / in1 1 -> conflict */
		/* case 8: in0 1 / in1 0 -> in1  0 が次のjustified    */
		/* case 9: in0 1 / in1 1 -> 含意操作ミス(ERROR)       */

		/* caseの分け方、要検討 */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:
				/* case 5 */

				/* 00と11のどちらが容易か? */
				if ( ( in0->tm_c0 + in1->tm_c0 ) >
					 ( in0->tm_c1 + in1->tm_c1 ) ) {

					/* 11にする */
					ass_list->ass_state3 = STATE3_1;
					
					/* どっちの1が割り当て難いか
					   (実はどっちでもいいが速度の問題) */
					if ( in0->tm_c1 > in1->tm_c1 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				else{
					/* 00にする */
					ass_list->ass_state3 = STATE3_0;
					/* どっちの0が割り当て難いか
					   (実はどっちでもいいが速度の問題) */
					if ( in0->tm_c0 > in1->tm_c0 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				break;
			case STATE3_0:
				/* case 2 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 4 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if ( STATE3_X == in1_state3 ) {
			switch( in0_state3 ) {
			case STATE3_0:
				/* case 1 */
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 3 */
				ass_list->line              = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_X:
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_1:
		/* 01 or 10 にする */

		/* 片方がX, 片方が0 */ 
		/* case 1: in0 0 / in1 X -> in1 に 1を */
		/* case 2: in0 X / in1 0 -> in0 に 1を */

		/* 片方がX, 片方が1 -> Xに0をassign */ 
		/* case 3: in0 1 / in1 X -> in1 に 0を */
		/* case 4: in0 X / in1 1 -> in0 に 0を */

		/* 両方がX、01か10の安い方 */
		/* case 5: in0 X / in1 X -> tmをチェック */

		/* NG(値が既に決っているため) */
		/* case 6: in0 0 / in1 0 */
		/* case 7: in0 0 / in1 1 */
		/* case 8: in0 1 / in1 0 */
		/* case 9: in0 1 / in1 1 */

		/* caseの分け方、要検討 */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:

				/* 01と10のどちらが容易か? */
				if ( (in0->tm_c0 + in1->tm_c1 ) >
				     (in0->tm_c1 + in1->tm_c0 ) ) {
					/* 01にする */
					if( in0->tm_c0 > in1->tm_c1 ) {
						/* in0を0にするほうが難しい */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line              = in0;
					}
					else {
						/* in1を1にするほうが難しい */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line              = in1;
					}
				}
				else {
					/* 10にする */
					if( in0->tm_c1 > in1->tm_c0 ) {
						/* in0を1にするほうが難しい */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line              = in0;
					}
					else {
						/* in1を0にするほうが難しい */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line              = in1;
					}
				}
				break;
			case STATE3_0:
				ass_list->line              = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if( STATE3_X == in1_state3 ) {
			switch ( in0_state3 ) {
			case STATE3_0:
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_X:
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_X:
	default:
		/* きちゃだめ */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	test_ass_list ( ass_list, func_name );
	
	return( ass_list );
}

/********************************************************************************
 * << Function >>
 *		xnor gate justisfied
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	ass			: justified list
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/4
 *
 ********************************************************************************/

ASS_LIST  *atpg_line_justified_xnor
(
 JUST_INFO *just_info
 ){

	LINE	 *in0;
	LINE     *in1;

	ASS_LIST *ass_list;

	STATE_3  line_state3;
	STATE_3  in0_state3;
	STATE_3  in1_state3;

	char     *func_name = "atpg_line_justified_xnor"; /* 関数名 */

	test_just_info ( just_info, func_name );
	test_line_in_null ( just_info->line, func_name );

	ass_list = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ),
										1, func_name );

	/* inialized */
	ass_list->next  = NULL;
	ass_list->ass_state3 = STATE3_X;
	ass_list->condition = just_info->condition;
	
	in0      = just_info->line->in[ 0 ];
	in1      = just_info->line->in[ 1 ];

	/* とりあえず正常値優先でやってみた */
	if ( ass_list->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		line_state3 = atpg_get_state9_2_normal_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_normal_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_normal_state3 ( in1->state9 );
	}
	else {
		/* COND_FAILURE */
		line_state3 = atpg_get_state9_2_failure_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_failure_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_failure_state3 ( in1->state9 );
	}
	
	/* とりあえず正常値だけ考える */
	switch ( line_state3 ) {
	case STATE3_1:   /* <<-- xorとの違い (1/2) */
		/* 11 or 00 にする */

		/* 片方がX, 片方が0 */
		/* case 1: in0 0 / in1 X -> in1 に 0を                */
		/* case 2: in0 X / in1 0 -> in1  0 が次のjustified    */

		/* 片方がX, 片方が1 -> Xに1をassign */ 
		/* case 3: in0 1 / in1 X -> in1  0 が次のjustified    */
		/* case 4: in0 X / in1 1 -> high 0 が次のjustified    */

		/* 両方がX、11か00の安い方 */
		/* case 5: in0 X / in1 X -> in1  0 が次のjustified    */


		/* NG(値が既に決っているため) */
		/* case 6: in0 0 / in1 0 -> just flag だけ            */
		/* case 7: in0 0 / in1 1 -> conflict */
		/* case 8: in0 1 / in1 0 -> in1  0 が次のjustified    */
		/* case 9: in0 1 / in1 1 -> 含意操作ミス(ERROR)       */

		/* caseの分け方、要検討 */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:
				/* case 5 */

				/* 多分多いと思う */
				ass_list->line = in0;

				/* 00と11のどちらが容易か? */
				if ( ( in0->tm_c0 + in1->tm_c0 ) >
					 ( in0->tm_c1 + in1->tm_c1 ) ) {
					/* 11にする */
					ass_list->ass_state3 = STATE3_1;
					/* どっちの1が割り当て難いか
					   (実はどっちでもいいが速度の問題) */
					if ( in0->tm_c1 > in1->tm_c1 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				else{
					/* 00にする */
					ass_list->ass_state3 = STATE3_0;
					/* どっちの0が割り当て難いか
					   (実はどっちでもいいが速度の問題) */
					if ( in0->tm_c0 > in1->tm_c0 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				break;
			case STATE3_0:
				/* case 2 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 4 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if ( STATE3_X == in1_state3 ) {
			switch( in0_state3 ) {
			case STATE3_0:
				/* case 1 */
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 3 */
				ass_list->line              = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_X:
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_0: /* <<-- xorとの違い (1/2) */
		/* 01 or 10 にする */

		/* 片方がX, 片方が0 */ 
		/* case 1: in0 0 / in1 X -> in1 に 1を */
		/* case 2: in0 X / in1 0 -> in0 に 1を */

		/* 片方がX, 片方が1 -> Xに0をassign */ 
		/* case 3: in0 1 / in1 X -> in1 に 0を */
		/* case 4: in0 X / in1 1 -> in0 に 0を */

		/* 両方がX、01か10の安い方 */
		/* case 5: in0 X / in1 X -> tmをチェック */

		/* NG(値が既に決っているため) */
		/* case 6: in0 0 / in1 0 */
		/* case 7: in0 0 / in1 1 */
		/* case 8: in0 1 / in1 0 */
		/* case 9: in0 1 / in1 1 */

		/* caseの分け方、要検討 */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:

				/* 01と10のどちらが容易か? */
				if ( (in0->tm_c0 + in1->tm_c1 ) >
				     (in0->tm_c1 + in1->tm_c0 ) ) {
					/* 01にする */
					if( in0->tm_c0 > in1->tm_c1 ) {
						/* in0を0にするほうが難しい */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line = in0;
					}
					else {
						/* in1を1にするほうが難しい */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line = in1;
					}
				}
				else {
					/* 10にする */
					if( in0->tm_c1 > in1->tm_c0 ) {
						/* in0を1にするほうが難しい */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line = in0;
					}
					else {
						/* in1を0にするほうが難しい */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line = in1;
					}
				}
				break;
			case STATE3_0:
				ass_list->line       = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line       = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if( STATE3_X == in1_state3 ) {
			switch ( in0_state3 ) {
			case STATE3_0:
				ass_list->line       = in1;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line       = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_X:
			default:
				/* きちゃだめ */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_X:
	default:
		/* きちゃだめ */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	test_ass_list ( ass_list, func_name );
	
	return( ass_list );
}

/********************************************************************************
 * << Function >>
 *		正当化する必要がある信号線の情報を作成する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  JUST_INFO    : 正当化する信号線の情報
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line for calclurate judgement value
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/9
 *
 ********************************************************************************/


JUST_INFO *atpg_make_just_info
(
 LINE    *line,
 Uint    condition
 ){
	JUST_INFO   *just_info;

	char *func_name = "atpg_make_just_info"; /* 関数名 */

	test_line_null ( line, func_name );

	/* just_infoのメモリ確保 */
	just_info = ( JUST_INFO * )topgunMalloc( FMT_JUST_INFO, sizeof( JUST_INFO ),
											  1, func_name );

	just_info->line       = line;
	just_info->condition  = condition;
	just_info->next       = NULL;

	return ( just_info );
}

/********************************************************************************
 * << Function >>
 *		正当化する必要がある信号線の情報をbtreeへ登録する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *     void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justified infomation
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/9
 *
 ********************************************************************************/


void atpg_enter_btree_just_info
(
 JUST_INFO *just_info
 ){
	BTREE *btree = Gene_head.btree;
	
	char  *func_name = "atpg_enter_just_info"; /* 関数名 */

	test_just_info_null ( just_info, func_name );
	test_btree_null ( btree, func_name );

	/* btreeへの登録 */
	just_info->next = btree->just_e;
	btree->just_e   = just_info;

}

/********************************************************************************
 * << Function >>
 *		正当化する必要がある信号線の情報をLv_headへ登録する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *     void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justified infomation
 *
 * << extern >>
 * name			(I/O)	type			description
 *	Lv_head      I		JUST_INFO **	levelize header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/9
 *
 ********************************************************************************/


void atpg_enter_lv_head_just_info
(
 JUST_INFO *just_info
 ){

	char  *func_name = "atpg_enter_lv_head_just_info"; /* 関数名 */

	test_just_info_null ( just_info, func_name );

	/* btreeへの登録 */

	/* Lv_headへ登録 */
	just_info->next = Lv_head[ just_info->line->lv_pi ];
	Lv_head[ just_info->line->lv_pi ] = just_info;

}


/********************************************************************************
 * << Function >>
 *		正当化する必要がある信号線の情報を作成する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  start_lv    : 正当化する信号線の最大レベル
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	 I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/09
 *
 ********************************************************************************/


Ulong atpg_get_just_info
(
 void
 ){
	JUST_INFO *just_e    = NULL;
	JUST_INFO *just_info = NULL;
	Ulong     start_lv   = 0;

	char  *func_name = "atpg_get_just_info"; /* 関数名 */

	test_btree_null ( Gene_head.btree, func_name );

	/* すべてNULLか確認する */
	test_lv_head_null ( func_name );

	if ( Gene_head.btree->prev == NULL ) {
		/* 初めて正当化を開始する場合 */
		start_lv = atpg_startlist_justified ();
	}
	else {
		/* すでに正当化中 */
		just_e = Gene_head.btree->prev->just_e;

		while ( just_e ) {

			/* 単なるコピーを作成。
			   $$$ もっと良いやり方があるかも $$$ */
			just_info = atpg_make_just_info ( just_e->line, just_e->condition );

			/* Lv_headへ登録 */
			atpg_enter_lv_head_just_info ( just_info );

			/* start_lvを更新する */
			if ( start_lv < just_e->line->lv_pi ) {
				start_lv = just_e->line->lv_pi;
			}

			/* つぎの登録へ */
			just_e = just_e->next;
		}
	}

	return ( start_lv );
}

/********************************************************************************
 * << Function >>
 *		正当化する信号線の情報を消去する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/


void atpg_btree_just_info_reflesh
(
 JUST_INFO *just_info
 ){
	JUST_INFO *tmp_just_info; /* メモリ開放用 */

	char  *func_name = "atpg_btree_just_info_reflesh"; /* 関数名 */

	while ( just_info ) {

		if ( just_info->condition == COND_NORMAL ) {
			/* COND_NORMAL */
			just_info->line->flag &= ~JUSTIFY_NO;
		}
		else {
			/* COND_FAILURE */
			just_info->line->flag &= ~JUSTIFY_FL;
		}
		
		/* 次に戻すための準備 */
		tmp_just_info = just_info;
		just_info     = just_info->next;



		topgunFree( tmp_just_info, FMT_JUST_INFO, 1, func_name );
	}
}


/********************************************************************************
 * << Function >>
 *		0の値を割り当てる一番難しい信号線を求める
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_difficult_0
(
 LINE *line
 ){
	LINE  *in   = NULL; // 入力信号線のポインタ
	Ulong score = 0;    // 難しさの難易度
	Ulong i     = 0;    // 信号線入力数のカウンタ

	STATE_3 state3 = STATE3_X;
	
	char  *func_name = "atpg_line_justified_trace_difficult_0"; // 関数名

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score < line->in[ i ]->tm_c0 ) {

			//$$$ なんかチェッカ関数にした方が良さげ $$$
			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); // 9値から正常値を取り出す

			if ( state3 == STATE3_X ){
				score = line->in[ i ]->tm_c0;
				in    = line->in[ i ];
			}
		}
	}

	{
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		0の値を割り当てる一番易しい信号線を求める
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_easy_0
(
 LINE *line
 ){
	LINE  *in   = NULL;     // 入力信号線のポインタ
	Ulong score = FL_ULMAX; // 難しさの難易度
	Ulong i     = 0;        // 信号線入力数のカウンタ
	STATE_3 state3 = STATE3_X;

	char  *func_name = "atpg_line_justified_trace_easy_0"; // 関数名

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score > line->in[ i ]->tm_c0 ) {

			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); // 9値から正常値を取り出す

			if ( state3 == STATE3_X ){
				score = line->in[ i ]->tm_c0;
				in    = line->in[ i ];
			}
		}
	}
	{
		// チェック用
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		1の値を割り当てる一番難しい信号線を求める
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_difficult_1
(
 LINE *line
 ){
	LINE  *in   = NULL; // 入力信号線のポインタ
	Ulong score = 0;    // 難しさの難易度
	Ulong i     = 0;    // 信号線入力数のカウンタ

	STATE_3 state3 = STATE3_X;
	
	char  *func_name = "atpg_line_justified_trace_difficult_0"; // 関数名

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score < line->in[ i ]->tm_c1 ) {
			//$$$ なんかチェッカ関数にした方が良さげ $$$
			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); // 9値から正常値を取り出す

			if ( STATE3_X == state3 ) {
				score = line->in[ i ]->tm_c1;
				in    = line->in[ i ];
			}
		}
	}

	{
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		1の値を割り当てる一番易しい信号線を求める
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_easy_1
(
 LINE *line
 ){
	LINE  *in      = NULL;     // 入力信号線のポインタ
	Ulong score    = FL_ULMAX; // 難しさの難易度
	Ulong i        = 0;        // 信号線入力数のカウンタ
	STATE_3 state3 = STATE3_X;
		
	char  *func_name = "atpg_line_justified_trace_easy_1"; // 関数名

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score > line->in[ i ]->tm_c1 ) {
			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); /* 9値から正常値を取り出す */
			if ( state3 == STATE3_X ) {
				score = line->in[ i ]->tm_c1;
				in    = line->in[ i ];
			}
		}
	}
	{
		// チェック用
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		正当化する信号線を一番制御しやすい外部入力に変更する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/


void atpg_line_justified_single_path_trace
(
 ASS_LIST *org_ass_list
 ){
	LINE  *line = org_ass_list->line;
	STATE_3 state3 = org_ass_list->ass_state3;
	
	char  *func_name = "atpg_line_justified_single_path_trace_input"; // 関数名
	
	topgun_3_utility_enter_start_time ( &Time_head.justified_spt );
	
	// すべての入力値が必要なとき(ANDの出力1など)は一番難しいもの(無限大は除く)
	// 1つの入力値が必要なとき(ANDの出力0など)は一番優しいもの

	while ( 1 ) {

		// 入力が切れているときの対応を忘れずに
		if ( line->type == TOPGUN_PI ) {
			break;
		}

		if ( state3 == STATE3_0 ) {

			// 現在の信号線の割り当て値が0の場合

			// 関数のポインタ化でswitchをなくす
			switch ( line->type ) {
			
			case TOPGUN_OR:
				// 0で一番難しい入力
				line = atpg_line_justified_trace_difficult_0 ( line );
				break;
			case TOPGUN_NOR:
				// 1でどれでもよい良い
				line = atpg_line_justified_trace_easy_1 ( line );
				state3 = STATE3_1;
				break;
			case TOPGUN_AND:
				// 0でどれでもよい良い
				line = atpg_line_justified_trace_easy_0 ( line );
				break;
			case TOPGUN_NAND:
				// 1で一番難しい入力
				line = atpg_line_justified_trace_difficult_1 ( line );
				state3 = STATE3_1;
				break;
			case TOPGUN_INV:
				state3 = STATE3_1;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else {
			// 現在の信号線の割り当て値が1の場合
			switch ( line->type ) {
			
			case TOPGUN_OR:
				// 1でどれでもよい良い
				line = atpg_line_justified_trace_easy_1 ( line );
				break;
			case TOPGUN_NOR:
				// 0で一番難しい入力
				line = atpg_line_justified_trace_difficult_0 ( line );
				state3 = STATE3_0;
				break;
			case TOPGUN_AND:
				// 1で一番難しい入力
				line = atpg_line_justified_trace_difficult_1 ( line );
				break;
			case TOPGUN_NAND:
				// 0でどれでもよい良い
				line = atpg_line_justified_trace_easy_0 ( line );
				state3 = STATE3_0;
				break;
			case TOPGUN_INV:
				state3 = STATE3_0;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
	}
	org_ass_list->line       = line;
	org_ass_list->ass_state3 = state3;
	
	topgun_3_utility_enter_end_time ( &Time_head.justified_spt );
}

/********************************************************************************
 * << Function >>
 *		正当化する信号線を一番制御しやすい外部入力に変更する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2007/02/20
 *
 ********************************************************************************/


void atpg_line_justified_single_path_trace_with_implying_node
(
 ASS_LIST *org_ass_list
 ){
	LINE  *line = org_ass_list->line;
	STATE_3 state3 = org_ass_list->ass_state3;
	
	char  *func_name = "atpg_line_justified_single_path_trace_input"; // 関数名
	
	topgun_3_utility_enter_start_time ( &Time_head.justified_spt );
	
	// 1つの入力値が必要なとき(ANDの出力0など)は一番優しいもの
	// すべての入力値が必要なとき(ANDの出力1など)はその値

	while ( 1 ) {

		// 入力が切れているときの対応を忘れずに
		if ( line->type == TOPGUN_PI ) {
			break;
		}

		if ( state3 == STATE3_0 ) {

			// 現在の信号線の割り当て値が0の場合
			// 関数のポインタ化でswitchをなくす
			switch ( line->type ) {
			case TOPGUN_OR:
				// すべて0
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_0;
				return;
			case TOPGUN_NOR:
				// 1でどれでもよい良い
				line = atpg_line_justified_trace_easy_1 ( line );
				state3 = STATE3_1;
				break;
			case TOPGUN_AND:
				// 0でどれでもよい良い
				line = atpg_line_justified_trace_easy_0 ( line );
				break;
			case TOPGUN_NAND:
				// すべて1
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_0;
				return;
			case TOPGUN_INV:
				state3 = STATE3_1;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else {
			// 現在の信号線の割り当て値が1の場合
			switch ( line->type ) {
			
			case TOPGUN_OR:
				// 1でどれでもよい良い
				line = atpg_line_justified_trace_easy_1 ( line );
				break;
			case TOPGUN_NOR:
				// 入力信号線すべてを0にする
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_1;
				return;
			case TOPGUN_AND:
				// 入力信号線すべてを1にする
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_1;
				return;
			case TOPGUN_NAND:
				// 0でどれでもよい良い
				line = atpg_line_justified_trace_easy_0 ( line );
				state3 = STATE3_0;
				break;
			case TOPGUN_INV:
				state3 = STATE3_0;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
	}
	org_ass_list->line       = line;
	org_ass_list->ass_state3 = state3;
	
	topgun_3_utility_enter_end_time ( &Time_head.justified_spt );
}

/********************************************************************************
 * << Function >>
 *		正当化する信号線を一番制御しやすい外部入力に変更する
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/


void atpg_line_justified_extend_large_line 
(
 ASS_LIST *org_ass_list
 ){
}


/********************************************************************************
 * << Function >>
 *		正当化する信号線の探索を記憶する(for debug)
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/
#ifdef TOPGUN_NOUSE
void atpg_justified_packjust_info
(
 JUST_INFO  *just_info
 ){

	char  *func_name = "atpg_justify_pack_just_info"; /* 関数名 */


	new_stack = ( LINE_STACK * )topgunMalloc(FMT_LINE_STACK, sizeof( LINE_STACK ),
											  1, func_name );

	new_stack->line = just_info->line;
	new_stack->next = flag_stack;
	flag_stack      = new_stack;

	return ( flag_stack );

}




/********************************************************************************
 * << Function >>
 *		judge and gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-3
 *
 * << Return >>
 *	result		: justified line result
 *                JUST_OWN (0x04)      対象信号線自身
 *                JUST_ALL (0x08)      入力の全部
 *                JUST_1IN (0x10)      入力のひとつどこか 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *      2005/09/29 multi gate
 *
 ********************************************************************************/

int atpg_check_cand_justified_and
(
 LINE *line,
 Ulong flag
 ){

	int  result = 0; /* 処理結果変数の初期化 */
	void topgun_error();

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */

	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();

	
	char  *func_name = "atpg_check_cand_justified_and"; /* 関数名 */

	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}


	/* 出力値に応じて、どの信号線を正当化するか考える */
	switch ( out ) {
	case STATE3_1:
		/* 出力が1のときは両方の入力値が必要 */
		result |= JUST_ALL;
		break;

	case STATE3_0:
		/* 出力が0のときはどっちか片方の入力値が必要 */
		if ( (in0 == STATE3_0) &&
			 (in1 == STATE3_0) ) {
			/* in[0] = 0, in[1] = 0 */
			/* $$$ とりあえず $$$ */
			
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */
			
		}
		else if ( in0 == STATE3_0 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */
			
		}
		else if ( in1 == STATE3_0 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"1"のため */
			
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;
		}
		break;
	case STATE3_X:
		break;
	default:
		/* 正当化する/しないの判断以前の問題 */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}
	return( result );
}

/********************************************************************************
 * << Function >>
 *		judge nand gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *
 ********************************************************************************/

int atpg_check_cand_justified_nand
(
 LINE *line,
 int   flag
 ){

	int  result = 0; /* 処理結果変数の初期化 */
	void topgun_error();

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */

	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();

	char  *func_name = "atpg_check_cand_justified_nand"; /* 関数名 */


	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
	
	/* 出力値に応じて、どの信号線を正当化するか考える */
	switch ( out ) {
	case STATE3_0:
		/* 出力が1のときは両方の入力値が必要 */
		result |= JUST_ALL;
		return( result );

	case STATE3_1:
		/* 出力が0のときはどっちか片方の入力値が必要 */
		if ( (in0 == STATE3_0) &&
			 (in1 == STATE3_0) ) {
			/* in[0] = 0, in[1] = 0 */
			/* $$$ とりあえず $$$ */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */

			return( result );
		}
		else if ( in0 == STATE3_0 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */
			return( result );
		}
		else if ( in1 == STATE3_0 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"1"のため */
			return( result );
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;

			return( result );
		}
	case STATE3_X:
		return ( result );
		
	default:
		/* 正当化する/しないの判断以前の問題 */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}

	return ( 0 ); /* 来ないけど */
}

/********************************************************************************
 * << Function >>
 *		judge "OR" gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *
 ********************************************************************************/

int atpg_check_cand_justified_or
(
 LINE *line,
 int  flag
 ){

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */
	
	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();
	
	int  result = 0; /* 処理結果変数の初期化 */
	void topgun_error();
	char *func_name = "atpg_check_cand_justified_or"; /* 関数名 */


	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	
	/* 出力値に応じて、どの信号線を正当化するか考える */
	switch ( out ) {
	case STATE3_0:
		/* 出力が0のときは両方の入力値が必要 */
		result |= JUST_ALL;
		return( result );

	case STATE3_1:
		/* 出力が1のときはどっちか片方の入力値が必要 */
		if ( (in0 == STATE3_1) &&
			 (in1 == STATE3_1) ) {

			/* $$$ とりあえず $$$ */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */

			return( result );
		}
		else if ( in0 == STATE3_1 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */
			return( result );
		}
		else if ( in1 == STATE3_1 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"0"のため */
			return( result );
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;

			return( result );
		}
	case STATE3_X:
		return( result );
		
	default:
		/* 正当化する/しないの判断以前の問題 */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}
	return(0);
}

/********************************************************************************
 * << Function >>
 *		judge "NOR" gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/7
 *
 ********************************************************************************/

int atpg_check_cand_justified_nor
(
 LINE *line,
 int   flag
 ){

	int  result = 0; /* 処理結果変数の初期化 */

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */

	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();
	
	void topgun_error();
	char *func_name = "atpg_check_cand_justified_nor"; /* 関数名 */

	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
	
	/* 出力値に応じて、どの信号線を正当化するか考える */
	switch ( out ) {
	case STATE3_1:
		/* 出力が1のときは両方の入力値が必要 */
		result |= JUST_ALL;
		return( result );

	case STATE3_0:
		/* 出力が0のときはどっちか片方の入力値が必要 */
		if ( (in0 == STATE3_1) &&
			 (in1 == STATE3_1) ) {
			/* in[0] = 0, in[1] = 0 */
			/* $$$ とりあえず $$$ */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */
			return( result );
		}
		else if ( in0 == STATE3_1 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"のため */
			return( result );
		}
		else if ( in1 == STATE3_1 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"1"のため */
			return( result );
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;

			return( result );
		}
	case STATE3_X:
		return( result );
		
	default:
		/* 正当化する/しないの判断以前の問題 */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}
	return( 0 );
}
#endif /* TOPGUN_NOUSE */
/* End of File */
