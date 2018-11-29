/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 含意操作に関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunState.h" // STATE_9
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunTime.h"
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h"  /* FEC_XX */
#include "topgunTest.h"   /* topgun_test_xxx */

extern GENE_HEAD	Gene_head;
extern LINE_INFO    Line_info;

IMP_RESULT (* Switch_implication[TOPGUN_NUM_PRIM])(IMP_INFO *);

void (* Switch_enter_imp_trace[ TOPGUN_NUM_COND ] )( LINE *);

/* 2入力XORの含意操作関数 */
void (* Xor_implication_out0[4][4])(IMP_RESULT_INFO *); /* out X-> 0 [in[0]][in[1]]?? */
void (* Xor_implication_out1[4][4])(IMP_RESULT_INFO *); /* out X-> 1 [in[0]][in[1]]?? */
void (* Xor_implication_in0 [4][4])(IMP_RESULT_INFO *); /* in  X-> 0 [ out ][ in  ]?? */
void (* Xor_implication_in1 [4][4])(IMP_RESULT_INFO *); /* in  X-> 1 [ out ][ in  ]?? */

/* 2入力XNORの含意操作関数 */
void (* Xnor_implication_out0[4][4])(IMP_RESULT_INFO *); /* out X-> 0 [in[0]][in[1]]?? */
void (* Xnor_implication_out1[4][4])(IMP_RESULT_INFO *); /* out X-> 1 [in[0]][in[1]]?? */
void (* Xnor_implication_in0 [4][4])(IMP_RESULT_INFO *); /* in  X-> 0 [ out ][ in  ]?? */
void (* Xnor_implication_in1 [4][4])(IMP_RESULT_INFO *); /* in  X-> 1 [ out ][ in  ]?? */


extern   void implication_init ( void ); /* 含意操作関数の初期化 */
extern   IMP_RESULT implication ( );  /* 含意操作関数 */


int      atpg_check_state9_n_f( STATE_9 ); /* stateが正常値か故障値のいずれかを判断する */

void     atpg_enter_implication_list ( IMP_INFO * ); /* 含意操作リストへ登録する */
void     atpg_enter_implication_list_input ( LINE * , STATE_3, Uint, Ulong, Ulong ); /* 入力方向への含意操作を含意操作リストへ登録する */
void     atpg_enter_implication_list_output (  LINE *, STATE_3, Uint, Ulong ); /* 出力方向へ含意操作リストを登録する */
Ulong    atpg_imp_make_input_pin_number( Ulong ); /* 入力ピン番号をデコードする */
STATE_3  atpg_invert_state3 ( STATE_3 ); /* 反転したstateを作成する */
Ulong    atpg_implication_pin_num_2_pin_id ( Ulong );
Ulong    atpg_implication_pin_id_2_pin_num ( Ulong );
void     gene_update_state( LINE *, STATE_3, Uint );
STATE_9  atpg_make_state ( STATE_3, Uint );
IMP_INFO *atpg_select_implication ( void );
IMP_INFO *atpg_make_imp_info ( LINE *, STATE_3, Uint , Ulong );

Uint  atpg_reverse_condition ( Uint );

static IMP_RESULT implication_update_input ( LINE *, Ulong, STATE_3, Uint, Ulong );
static IMP_RESULT implication_update_output( LINE *, STATE_3, Uint, Ulong );

static IMP_RESULT implication_pi ( IMP_INFO * );
static IMP_RESULT implication_one_input ( IMP_INFO * );
static IMP_RESULT implication_inv ( IMP_INFO * );
static IMP_RESULT implication_and ( IMP_INFO * );
static IMP_RESULT implication_nand ( IMP_INFO * );
static IMP_RESULT implication_or ( IMP_INFO * );
static IMP_RESULT implication_nor ( IMP_INFO * );
static IMP_RESULT implication_blk ( IMP_INFO * );

static void gene_enter_imp_trace_normal( LINE * );
static void gene_enter_imp_trace_failure( LINE * );

static void atpg_enter_implication_list_output_normal_one_out( LINE *, STATE_3, Ulong );
static void atpg_enter_implication_list_output_normal_multi_out( LINE *, STATE_3, Ulong );
static void atpg_enter_implication_list_output_failure_one_out( LINE *, STATE_3, Ulong );
static void atpg_enter_implication_list_output_failure_multi_out( LINE *, STATE_3, Ulong );

static void gene_enter_imp_info( LINE *, STATE_3, Uint, Ulong, Ulong );

static Ulong gene_calc_imp_id_no_input_i_normal( LINE *, Ulong );
static Ulong gene_calc_imp_id_no_input_i_failure( LINE *, Ulong );
static Ulong gene_calc_imp_id_all_input_normal( LINE * );
static Ulong gene_calc_imp_id_all_input_failure( LINE * );



#ifdef TOPGUN_XOR_ON
static IMP_RESULT implication_xor ( IMP_INFO * );
static IMP_RESULT implication_xnor ( IMP_INFO * );
#endif /* TOPGUN_XOR_ON */


void  implication_keep ( IMP_RESULT_INFO * );
void  implication_conflict ( IMP_RESULT_INFO * );
void  implication_out_state0 ( IMP_RESULT_INFO * );
void  implication_out_state1 ( IMP_RESULT_INFO * );
void  implication_out_stateU ( IMP_RESULT_INFO * );
void  implication_in0_state0 ( IMP_RESULT_INFO * );
void  implication_in0_state1 ( IMP_RESULT_INFO * );
void  implication_in0_stateU ( IMP_RESULT_INFO * );
void  implication_in1_state0 ( IMP_RESULT_INFO * );
void  implication_in1_state1 ( IMP_RESULT_INFO * );
void  implication_in1_stateU ( IMP_RESULT_INFO * );
void  implication_ino_state0 ( IMP_RESULT_INFO * );
void  implication_ino_state1 ( IMP_RESULT_INFO * );
void  implication_ino_stateU ( IMP_RESULT_INFO * );

/* state function */
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9値から正常値を取り出す */
STATE_3 atpg_get_state9_2_failure_state3 ( STATE_9 ); /* 9値から故障値を取り出す */
IMP_RESULT atpg_update_static_learn_state ( LEARN_LIST * );
IMP_RESULT atpg_update_indirect_state( LEARN_LIST *, Ulong );
void gene_learn_list_initalize( void ); /* 間接含意による含意用 */

/* topgun_state.hのマクロ */
//STATE_3 atpg_get_state9_with_condition_2_state3 ( STATE_9, Uint ); /* 9値からconditionによって3値を取り出す */


/* topgun_btree.c */
void     atpg_enter_imp_trace ( LINE *, Uint );

/* test function */
void  test_line_null ( LINE * , char * );
void  test_line_type ( LINE * , char * );
void  test_state_n_f ( STATE_9, char * );
void  test_state_normal ( STATE_9, char * );
void  test_state3 ( STATE_3, char * );
void  test_condition ( Uint, char * );


/* print function */
void  topgun_print_mes( int, int );
void  topgun_print_mes_id_type_stat( LINE * );
void  topgun_print_mes_result_X( Ulong );
void  topgun_print_mes_gate_state9( LINE * );
void  topgun_print_mes_gate_info ( LINE * );
void  topgun_print_mes_gate_stat ( LINE * );
void  topgun_print_mes_imp_info_new ( IMP_INFO * );
void  topgun_print_mes_gate_state9_new ( LINE *, Ulong, Ulong, Ulong );
void  topgun_print_mes_gate_info_learn( void );
void  topgun_print_mes_gate_state9_line( LINE * );

/********************************************************************************
 * << Function >>
 *		manger of implication
 *
 * << Function ID >>
 *    	?-?
 *
 * << Return >>
 *	IMP_KEEP	: keep state
 *	IMP_UPDA	: update state
 *	IMP_CONF	: conflict
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/6
 *
 ********************************************************************************/

/* visio 2006/04/24 */

IMP_RESULT implication
(
 void
){

	IMP_RESULT  imp_result;
	IMP_INFO   *imp_info = NULL;

	//char *func_name = "implication"; /* 関数名 */

	topgun_4_utility_enter_start_time ( &Time_head.implication );

	topgun_print_mes( 24, 0 );

	
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0; //最大何番目の値割当ての影響をうけたか

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-X-1 .impが空になるまで含意操作を実施                           %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    /* $$$ 含意操作をする順番も課題(1st : first in, first out) $$$ */

	while ( Gene_head.n_enter_imp_list ) {
		
		imp_info = &(Gene_head.imp_info_list [ --Gene_head.n_enter_imp_list ]) ;

		topgun_print_mes( 24, 2 );
		topgun_print_mes_gate_info ( imp_info->line );
		topgun_print_mes_gate_state9_new
			( imp_info->line, imp_info->pin_num, imp_info->condition, 0 );		

		imp_result = Switch_implication[ imp_info->line->type ]( imp_info );

		if ( imp_result.i_state == IMP_CONF ) {
			topgun_print_mes( 24, 3 );
			break;
		}
		/* print用 */
		if ( imp_result.i_state == IMP_KEEP ) {
			topgun_print_mes( 24, 4 );
		}
	}
	/* CONFで終了したか? */
	if ( imp_result.i_state == IMP_CONF ) {
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-X-5 CONFで終了した場合の後始末                             %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		Gene_head.n_enter_imp_list = 0;
	}

	topgun_4_utility_enter_end_time ( &Time_head.implication );
	return ( imp_result ); /* IMP_CONF と IMP_KEEP か IMP_UPDA */
}

/********************************************************************************
 * << Function >>
 *		initailized implication function
 *
 * << Function ID >>
 *    	6-1
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * 		Void
 *
 * << extern >>
 * name			(I/O)	type		description
 * Switch_imp_front
 * Switch_back_front
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/6
 *
 ********************************************************************************/

void implication_init
(
 void
){

	Ulong i;
	
	char        *func_name = "implication_init"; /* 関数名 */
	
	Switch_implication[ TOPGUN_PI   ] = &implication_pi;
	Switch_implication[ TOPGUN_PO   ] = &implication_one_input;
	Switch_implication[ TOPGUN_BR   ] = &implication_one_input;
	Switch_implication[ TOPGUN_INV  ] = &implication_inv;
	Switch_implication[ TOPGUN_BUF  ] = &implication_one_input;
	Switch_implication[ TOPGUN_AND  ] = &implication_and;
	Switch_implication[ TOPGUN_NAND ] = &implication_nand;
	Switch_implication[ TOPGUN_OR   ] = &implication_or;
	Switch_implication[ TOPGUN_NOR  ] = &implication_nor;
	Switch_implication[ TOPGUN_BLKI ] = &implication_blk;
	Switch_implication[ TOPGUN_BLKO ] = &implication_blk;

	/* 間接含意用 */
	gene_learn_list_initalize();

	
	/* Gene_head.imp_info_listの作成 */
	Gene_head.n_imp_info_list = Line_info.n_line;
	
	Gene_head.imp_info_list = ( IMP_INFO * )topgunMalloc
		( FMT_IMP_INFO, sizeof( IMP_INFO ), Gene_head.n_imp_info_list, func_name );

	for ( i = 0 ; i < Gene_head.n_imp_info_list ; i++ ) {

		/* 初期化 */
		Gene_head.imp_info_list[ i ].line       = NULL;
		Gene_head.imp_info_list[ i ].new_state3 = STATE3_C;
		Gene_head.imp_info_list[ i ].condition  = COND_UNKNOWN;
		Gene_head.imp_info_list[ i ].pin_num    = FL_ULMAX;

	}
	Gene_head.n_enter_imp_list = 0;


	Gene_head.imp_list_n  = ( LINE ** )topgunMalloc
		( FMT_LINE_P, sizeof( LINE * ), ( Line_info.n_line + 1 ), func_name );
	Gene_head.imp_list_f  = ( LINE ** )topgunMalloc
		( FMT_LINE_P, sizeof( LINE * ), ( Line_info.n_line + 1 ), func_name );

	/* 初期化 */
	for ( i = 0 ; i < ( Line_info.n_line + 1 ) ; i++ ) {
		Gene_head.imp_list_n[ i ] = NULL;
	}
	for ( i = 0 ; i < ( Line_info.n_line + 1 ) ; i++ ) {
		Gene_head.imp_list_f[ i ] = NULL;
	}

	/* gene_imp_trace_xxx の初期化 */
	Switch_enter_imp_trace[ COND_NORMAL  ] = &gene_enter_imp_trace_normal;
	Switch_enter_imp_trace[ COND_FAILURE ] = &gene_enter_imp_trace_failure;

#ifdef TOPGUN_XOR_ON	
	Switch_implication[ TOPGUN_XOR  ] = &implication_xor;
	Switch_implication[ TOPGUN_XNOR ] = &implication_xnor;


	/* 場合分け XOR 2入力版 */
	/* case 1: out  -> 0
	           case k: in0 が Xの時, in1 が Xの時 -> なにもしない
	           case c: in0 が Xの時, in1 が 0の時 -> in0 を 0に更新
			   case g: in0 が Xの時, in1 が 1の時 -> in0 を 1に更新
			   case o: in0 が Xの時, in1 が Uの時 -> conf
			   case i: in0 が 0の時, in1 が Xの時 -> in1 を 0に更新
	           case a: in0 が 0の時, in1 が 0の時 -> なにもしない
			   case e: in0 が 0の時, in1 が 1の時 -> conf
			   case m: in0 が 0の時, in1 が Uの時 -> conf
			   case j: in0 が 1の時, in1 が Xの時 -> in0 を 1に更新
			   case b: in0 が 1の時, in1 が 0の時 -> conf
			   case f: in0 が 1の時, in1 が 1の時 -> なにもしない
			   case n: in0 が 1の時, in1 が Uの時 -> conf
			   case d: in0 が Uの時, in1 が 0の時 -> conf
			   case h: in0 が Uの時, in1 が 1の時 -> conf
			   case l: in0 が Uの時, in1 が Xの時 -> conf
			   case p: in0 が Uの時, in1 が Uの時 -> conf
	   case 2: out  -> 1
			   case k: in0 が Xの時, in1 が Xの時 -> なにもしない
			   case c: in0 が Xの時, in1 が 0の時 -> in0 を 1に更新
			   case g: in0 が Xの時, in1 が 1の時 -> in0 を 0に更新
			   case o: in0 が Xの時, in1 が Uの時 -> conf
			   case i: in0 が 0の時, in1 が Xの時 -> in1 を 1に更新
			   case a: in0 が 0の時, in1 が 0の時 -> conf
			   case e: in0 が 0の時, in1 が 1の時 -> なにもしない
			   case m: in0 が 0の時, in1 が Uの時 -> conf
			   case j: in0 が 1の時, in1 が Xの時 -> in1 を 0に更新
			   case b: in0 が 1の時, in1 が 0の時 -> なにもしない
			   case f: in0 が 1の時, in1 が 1の時 -> conf
			   case n: in0 が 1の時, in1 が Uの時 -> conf
			   case d: in0 が Uの時, in1 が 0の時 -> conf
			   case h: in0 が Uの時, in1 が 1の時 -> conf
			   case l: in0 が Uの時, in1 が Xの時 -> conf
			   case p: in0 が Uの時, in1 が Uの時 -> conf
	   case 3: in -> 0
			   case k: out が Xの時, in1 が Xの時 -> なにもしない
			   case c: out が Xの時, in1 が 0の時 -> out を 0に更新
			   case g: out が Xの時, in1 が 1の時 -> out を 1に更新
			   case o: out が Xの時, in1 が Uの時 -> なにもしない			   
	           case i: out が 0の時, in1 が Xの時 -> in1 を 0に更新
	           case a: out が 0の時, in1 が 0の時 -> なにもしない
	           case e: out が 0の時, in1 が 1の時 -> conf
	           case m: out が 0の時, in1 が Uの時 -> conf			   			   
			   case j: out が 1の時, in1 が Xの時 -> in1 を 1に更新			   
			   case b: out が 1の時, in1 が 0の時 -> conf
			   case f: out が 1の時, in1 が 1の時 -> なにもしない			   
			   case n: out が 1の時, in1 が Uの時 -> conf
			   case l: out が Uの時, in1 が Xの時 -> in1 を Uに更新
			   case d: out が Uの時, in1 が 0の時 -> conf			   
			   case h: out が Uの時, in1 が 1の時 -> conf			   
			   case p: out が Uの時, in1 が Uの時 -> なにもしない
	   case 4: in0 -> 1

			   case k: out が Xの時, in1 が Xの時 -> なにもしない
			   case c: out が Xの時, in1 が 0の時 -> out を 1に更新
			   case g: out が Xの時, in1 が 1の時 -> out を 0に更新			   
			   case o: out が Xの時, in1 が Uの時 -> なにもしない
	           case i: out が 0の時, in1 が Xの時 -> in1 を 1に更新
			   case a: out が 0の時, in1 が 0の時 -> conf
	           case e: out が 0の時, in1 が 1の時 -> なにもしない
	           case m: out が 0の時, in1 が Uの時 -> conf
			   case j: out が 1の時, in1 が Xの時 -> in1 を 0に更新
			   case b: out が 1の時, in1 が 0の時 -> なにもしない			   			   
			   case f: out が 1の時, in1 が 1の時 -> conf
			   case n: out が 1の時, in1 が Uの時 -> conf			   
			   case l: out が Uの時, in1 が Xの時 -> in1 を Uに更新
			   case d: out が Uの時, in1 が 0の時 -> conf			   
			   case h: out が Uの時, in1 が 1の時 -> conf
			   case p: out が Uの時, in1 が Uの時 -> なにもしない

			   in_xがUに更新されたとき
			   -> outは必ずUになる
	*/
	/*                    in0      in1                              */
	Xor_implication_out0[ STATE3_X ][STATE3_X] = &implication_keep;
	Xor_implication_out0[ STATE3_X ][STATE3_0] = &implication_in0_state0;
	Xor_implication_out0[ STATE3_X ][STATE3_1] = &implication_in0_state1;
	Xor_implication_out0[ STATE3_X ][STATE3_U] = &implication_conflict;
	Xor_implication_out0[ STATE3_0 ][STATE3_X] = &implication_in1_state0;
	Xor_implication_out0[ STATE3_0 ][STATE3_0] = &implication_keep;
	Xor_implication_out0[ STATE3_0 ][STATE3_1] = &implication_conflict;
	Xor_implication_out0[ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xor_implication_out0[ STATE3_1 ][STATE3_X] = &implication_in1_state1;
	Xor_implication_out0[ STATE3_1 ][STATE3_0] = &implication_conflict;
	Xor_implication_out0[ STATE3_1 ][STATE3_1] = &implication_keep;
	Xor_implication_out0[ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xor_implication_out0[ STATE3_U ][STATE3_X] = &implication_conflict;
	Xor_implication_out0[ STATE3_U ][STATE3_0] = &implication_conflict;
	Xor_implication_out0[ STATE3_U ][STATE3_1] = &implication_conflict;
	Xor_implication_out0[ STATE3_U ][STATE3_U] = &implication_conflict;

	Xor_implication_out1[ STATE3_X ][STATE3_X] = &implication_keep;
	Xor_implication_out1[ STATE3_X ][STATE3_0] = &implication_in0_state1;
	Xor_implication_out1[ STATE3_X ][STATE3_1] = &implication_in0_state0;
	Xor_implication_out1[ STATE3_X ][STATE3_U] = &implication_conflict;
	Xor_implication_out1[ STATE3_0 ][STATE3_X] = &implication_in1_state1;
	Xor_implication_out1[ STATE3_0 ][STATE3_0] = &implication_conflict;
	Xor_implication_out1[ STATE3_0 ][STATE3_1] = &implication_keep;
	Xor_implication_out1[ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xor_implication_out1[ STATE3_1 ][STATE3_X] = &implication_in1_state0;
	Xor_implication_out1[ STATE3_1 ][STATE3_0] = &implication_keep;
	Xor_implication_out1[ STATE3_1 ][STATE3_1] = &implication_conflict;
	Xor_implication_out1[ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xor_implication_out1[ STATE3_U ][STATE3_X] = &implication_conflict;
	Xor_implication_out1[ STATE3_U ][STATE3_0] = &implication_conflict;
	Xor_implication_out1[ STATE3_U ][STATE3_1] = &implication_conflict;
	Xor_implication_out1[ STATE3_U ][STATE3_U] = &implication_conflict;

	/*                    out      in_other                           */
	Xor_implication_in0 [ STATE3_X ][STATE3_X] = &implication_keep;
	Xor_implication_in0 [ STATE3_X ][STATE3_0] = &implication_out_state0;
	Xor_implication_in0 [ STATE3_X ][STATE3_1] = &implication_out_state1;
	Xor_implication_in0 [ STATE3_X ][STATE3_U] = &implication_keep;
	Xor_implication_in0 [ STATE3_0 ][STATE3_X] = &implication_ino_state0;
	Xor_implication_in0 [ STATE3_0 ][STATE3_0] = &implication_keep;
	Xor_implication_in0 [ STATE3_0 ][STATE3_1] = &implication_conflict;
	Xor_implication_in0 [ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xor_implication_in0 [ STATE3_1 ][STATE3_X] = &implication_ino_state1;
	Xor_implication_in0 [ STATE3_1 ][STATE3_0] = &implication_conflict;
	Xor_implication_in0 [ STATE3_1 ][STATE3_1] = &implication_keep;
	Xor_implication_in0 [ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xor_implication_in0 [ STATE3_U ][STATE3_X] = &implication_ino_stateU;
	Xor_implication_in0 [ STATE3_U ][STATE3_0] = &implication_conflict;
	Xor_implication_in0 [ STATE3_U ][STATE3_1] = &implication_conflict;
	Xor_implication_in0 [ STATE3_U ][STATE3_U] = &implication_keep;

	Xor_implication_in1 [ STATE3_X ][STATE3_X] = &implication_keep;
	Xor_implication_in1 [ STATE3_X ][STATE3_0] = &implication_out_state1;
	Xor_implication_in1 [ STATE3_X ][STATE3_1] = &implication_out_state0;
	Xor_implication_in1 [ STATE3_X ][STATE3_U] = &implication_keep;
	Xor_implication_in1 [ STATE3_0 ][STATE3_X] = &implication_ino_state1;
	Xor_implication_in1 [ STATE3_0 ][STATE3_0] = &implication_conflict;
	Xor_implication_in1 [ STATE3_0 ][STATE3_1] = &implication_keep;
	Xor_implication_in1 [ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xor_implication_in1 [ STATE3_1 ][STATE3_X] = &implication_ino_state0;
	Xor_implication_in1 [ STATE3_1 ][STATE3_0] = &implication_keep;
	Xor_implication_in1 [ STATE3_1 ][STATE3_1] = &implication_conflict;
	Xor_implication_in1 [ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xor_implication_in1 [ STATE3_U ][STATE3_X] = &implication_ino_stateU;
	Xor_implication_in1 [ STATE3_U ][STATE3_0] = &implication_conflict;
	Xor_implication_in1 [ STATE3_U ][STATE3_1] = &implication_conflict;
	Xor_implication_in1 [ STATE3_U ][STATE3_U] = &implication_keep;

	/* 場合分け XNOR 2入力版 */
	/* case 1: out  -> 0
			   case k: in0 が Xの時, in1 が Xの時 -> なにもしない
			   case c: in0 が Xの時, in1 が 0の時 -> in0 を 1に更新
			   case g: in0 が Xの時, in1 が 1の時 -> in0 を 0に更新
			   case o: in0 が Xの時, in1 が Uの時 -> conf
			   case i: in0 が 0の時, in1 が Xの時 -> in1 を 1に更新
			   case a: in0 が 0の時, in1 が 0の時 -> conf
			   case e: in0 が 0の時, in1 が 1の時 -> なにもしない
			   case m: in0 が 0の時, in1 が Uの時 -> conf
			   case j: in0 が 1の時, in1 が Xの時 -> in1 を 0に更新
			   case b: in0 が 1の時, in1 が 0の時 -> なにもしない
			   case f: in0 が 1の時, in1 が 1の時 -> conf
			   case n: in0 が 1の時, in1 が Uの時 -> conf
			   case d: in0 が Uの時, in1 が 0の時 -> conf
			   case h: in0 が Uの時, in1 が 1の時 -> conf
			   case l: in0 が Uの時, in1 が Xの時 -> conf
			   case p: in0 が Uの時, in1 が Uの時 -> conf
	   case 2: out  -> 1
	           case k: in0 が Xの時, in1 が Xの時 -> なにもしない
	           case c: in0 が Xの時, in1 が 0の時 -> in0 を 0に更新
			   case g: in0 が Xの時, in1 が 1の時 -> in0 を 1に更新
			   case o: in0 が Xの時, in1 が Uの時 -> conf
			   case i: in0 が 0の時, in1 が Xの時 -> in1 を 0に更新
	           case a: in0 が 0の時, in1 が 0の時 -> なにもしない
			   case e: in0 が 0の時, in1 が 1の時 -> conf
			   case m: in0 が 0の時, in1 が Uの時 -> conf
			   case j: in0 が 1の時, in1 が Xの時 -> in0 を 1に更新
			   case b: in0 が 1の時, in1 が 0の時 -> conf
			   case f: in0 が 1の時, in1 が 1の時 -> なにもしない
			   case n: in0 が 1の時, in1 が Uの時 -> conf
			   case d: in0 が Uの時, in1 が 0の時 -> conf
			   case h: in0 が Uの時, in1 が 1の時 -> conf
			   case l: in0 が Uの時, in1 が Xの時 -> conf
			   case p: in0 が Uの時, in1 が Uの時 -> conf
	   case 3: in -> 0
			   case k: out が Xの時, in1 が Xの時 -> なにもしない
			   case c: out が Xの時, in1 が 0の時 -> out を 1に更新
			   case g: out が Xの時, in1 が 1の時 -> out を 0に更新
			   case o: out が Xの時, in1 が Uの時 -> なにもしない			   
	           case i: out が 0の時, in1 が Xの時 -> in1 を 1に更新
	           case a: out が 0の時, in1 が 0の時 -> なにもしない
	           case e: out が 0の時, in1 が 1の時 -> conf
	           case m: out が 0の時, in1 が Uの時 -> conf			   			   
			   case j: out が 1の時, in1 が Xの時 -> in1 を 0に更新			   
			   case b: out が 1の時, in1 が 0の時 -> conf
			   case f: out が 1の時, in1 が 1の時 -> なにもしない			   
			   case n: out が 1の時, in1 が Uの時 -> conf
			   case l: out が Uの時, in1 が Xの時 -> in1 を Uに更新
			   case d: out が Uの時, in1 が 0の時 -> conf			   
			   case h: out が Uの時, in1 が 1の時 -> conf			   
			   case p: out が Uの時, in1 が Uの時 -> なにもしない
	   case 4: in0 -> 1
			   case k: out が Xの時, in1 が Xの時 -> なにもしない
			   case c: out が Xの時, in1 が 0の時 -> out を 0に更新
			   case g: out が Xの時, in1 が 1の時 -> out を 1に更新			   
			   case o: out が Xの時, in1 が Uの時 -> なにもしない
	           case i: out が 0の時, in1 が Xの時 -> in1 を 0に更新
			   case a: out が 0の時, in1 が 0の時 -> conf
	           case e: out が 0の時, in1 が 1の時 -> なにもしない
	           case m: out が 0の時, in1 が Uの時 -> conf
			   case j: out が 1の時, in1 が Xの時 -> in1 を 1に更新
			   case b: out が 1の時, in1 が 0の時 -> なにもしない
			   case f: out が 1の時, in1 が 1の時 -> conf
			   case n: out が 1の時, in1 が Uの時 -> conf			   
			   case l: out が Uの時, in1 が Xの時 -> in1 を Uに更新
			   case d: out が Uの時, in1 が 0の時 -> conf			   
			   case h: out が Uの時, in1 が 1の時 -> conf
			   case p: out が Uの時, in1 が Uの時 -> なにもしない

			   in_xがUに更新されたとき
			   -> outは必ずUになる
	*/
	/*                    in0      in1                              */
	Xnor_implication_out0[ STATE3_X ][STATE3_X] = &implication_keep;
	Xnor_implication_out0[ STATE3_X ][STATE3_0] = &implication_in0_state1;
	Xnor_implication_out0[ STATE3_X ][STATE3_1] = &implication_in0_state0;
	Xnor_implication_out0[ STATE3_X ][STATE3_U] = &implication_conflict;
	Xnor_implication_out0[ STATE3_0 ][STATE3_X] = &implication_in1_state1;
	Xnor_implication_out0[ STATE3_0 ][STATE3_0] = &implication_conflict;
	Xnor_implication_out0[ STATE3_0 ][STATE3_1] = &implication_keep;
	Xnor_implication_out0[ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xnor_implication_out0[ STATE3_1 ][STATE3_X] = &implication_in1_state0;
	Xnor_implication_out0[ STATE3_1 ][STATE3_0] = &implication_keep;
	Xnor_implication_out0[ STATE3_1 ][STATE3_1] = &implication_conflict;
	Xnor_implication_out0[ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xnor_implication_out0[ STATE3_U ][STATE3_X] = &implication_conflict;
	Xnor_implication_out0[ STATE3_U ][STATE3_0] = &implication_conflict;
	Xnor_implication_out0[ STATE3_U ][STATE3_1] = &implication_conflict;
	Xnor_implication_out0[ STATE3_U ][STATE3_U] = &implication_conflict;
	
	Xnor_implication_out1[ STATE3_X ][STATE3_X] = &implication_keep;
	Xnor_implication_out1[ STATE3_X ][STATE3_0] = &implication_in0_state0;
	Xnor_implication_out1[ STATE3_X ][STATE3_1] = &implication_in0_state1;
	Xnor_implication_out1[ STATE3_X ][STATE3_U] = &implication_conflict;
	Xnor_implication_out1[ STATE3_0 ][STATE3_X] = &implication_in1_state0;
	Xnor_implication_out1[ STATE3_0 ][STATE3_0] = &implication_keep;
	Xnor_implication_out1[ STATE3_0 ][STATE3_1] = &implication_conflict;
	Xnor_implication_out1[ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xnor_implication_out1[ STATE3_1 ][STATE3_X] = &implication_in1_state1;
	Xnor_implication_out1[ STATE3_1 ][STATE3_0] = &implication_conflict;
	Xnor_implication_out1[ STATE3_1 ][STATE3_1] = &implication_keep;
	Xnor_implication_out1[ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xnor_implication_out1[ STATE3_U ][STATE3_X] = &implication_conflict;
	Xnor_implication_out1[ STATE3_U ][STATE3_0] = &implication_conflict;
	Xnor_implication_out1[ STATE3_U ][STATE3_1] = &implication_conflict;
	Xnor_implication_out1[ STATE3_U ][STATE3_U] = &implication_conflict;

	/*                    out      in_other                           */
	Xnor_implication_in0 [ STATE3_X ][STATE3_X] = &implication_keep;
	Xnor_implication_in0 [ STATE3_X ][STATE3_0] = &implication_out_state1;
	Xnor_implication_in0 [ STATE3_X ][STATE3_1] = &implication_out_state0;
	Xnor_implication_in0 [ STATE3_X ][STATE3_U] = &implication_keep;
	Xnor_implication_in0 [ STATE3_0 ][STATE3_X] = &implication_ino_state1;
	Xnor_implication_in0 [ STATE3_0 ][STATE3_0] = &implication_keep;
	Xnor_implication_in0 [ STATE3_0 ][STATE3_1] = &implication_conflict;
	Xnor_implication_in0 [ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xnor_implication_in0 [ STATE3_1 ][STATE3_X] = &implication_ino_state0;
	Xnor_implication_in0 [ STATE3_1 ][STATE3_0] = &implication_conflict;
	Xnor_implication_in0 [ STATE3_1 ][STATE3_1] = &implication_keep;
	Xnor_implication_in0 [ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xnor_implication_in0 [ STATE3_U ][STATE3_X] = &implication_ino_stateU;
	Xnor_implication_in0 [ STATE3_U ][STATE3_0] = &implication_conflict;
	Xnor_implication_in0 [ STATE3_U ][STATE3_1] = &implication_conflict;
	Xnor_implication_in0 [ STATE3_U ][STATE3_U] = &implication_keep;

	Xnor_implication_in1 [ STATE3_X ][STATE3_X] = &implication_keep;
	Xnor_implication_in1 [ STATE3_X ][STATE3_0] = &implication_out_state0;
	Xnor_implication_in1 [ STATE3_X ][STATE3_1] = &implication_out_state1;
	Xnor_implication_in1 [ STATE3_X ][STATE3_U] = &implication_keep;
	Xnor_implication_in1 [ STATE3_0 ][STATE3_X] = &implication_ino_state0;
	Xnor_implication_in1 [ STATE3_0 ][STATE3_0] = &implication_conflict;
	Xnor_implication_in1 [ STATE3_0 ][STATE3_1] = &implication_keep;
	Xnor_implication_in1 [ STATE3_0 ][STATE3_U] = &implication_conflict;
	Xnor_implication_in1 [ STATE3_1 ][STATE3_X] = &implication_ino_state1;
	Xnor_implication_in1 [ STATE3_1 ][STATE3_0] = &implication_keep;
	Xnor_implication_in1 [ STATE3_1 ][STATE3_1] = &implication_conflict;
	Xnor_implication_in1 [ STATE3_1 ][STATE3_U] = &implication_conflict;
	Xnor_implication_in1 [ STATE3_U ][STATE3_X] = &implication_ino_stateU;
	Xnor_implication_in1 [ STATE3_U ][STATE3_0] = &implication_conflict;
	Xnor_implication_in1 [ STATE3_U ][STATE3_1] = &implication_conflict;
	Xnor_implication_in1 [ STATE3_U ][STATE3_U] = &implication_keep;
#endif /* TOPGUN_XOR_ON */
	
}

/********************************************************************************
 * << Function >>
 *		select line, pin and state from implication lists & clear memroy
 *
 * << Function ID >>
 *    	6-6
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     Void
 *
 * << extern >>
 *	ATPG_HEAD	Gene_head	infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/12
 *
 ********************************************************************************/

#ifdef OLD_IMP
IMP_INFO *atpg_select_implication
(
 void
){
	IMP_INFO	*imp_info;  /* 含意操作に関する情報 */

	char        *func_name = "atpg_select_implication"; /* 関数名 */

	/* 現在は最後尾を対象とする */
	imp_info   = Gene_head.imp_l;

	test_line_null ( imp_info->line, func_name );
	test_line_type ( imp_info->line, func_name );

	/* 含意操作リストに一個しか登録されていない場合 */
	if ( Gene_head.imp_l == Gene_head.imp_f ) {
		/* imp_fもNULLにする */
		Gene_head.imp_f = NULL;
		Gene_head.imp_l = NULL;
	}
	else{
		/* imp_lだけずらす */
		Gene_head.imp_l = Gene_head.imp_l->next;
	}
	
	topgun_print_mes( 24, 2 );
	topgun_print_mes_gate_info ( imp_info->line );
	topgun_print_mes_gate_state9_new
		( imp_info->line, imp_info->pin_num, imp_info->condition, 0 );

	return( imp_info );
}
#endif /* OLD_IMP */

/********************************************************************************
 * << Function >>
 *		And of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/13
 *
 ********************************************************************************/

IMP_RESULT implication_and
(
 IMP_INFO *imp_info
 ){


	IMP_RESULT imp_result;
	Ulong   i          = 0;  /* カウンタ */
	
	Uint    condition  = imp_info->condition; /* STAT_N or STAT_F */

	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* すべて同じ値かをチェックする ON: 同じ OFF: 異るものがある */
	Ulong   one_check = ONE_FLAG_ON; /* 一度のXはOK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)が存在するか */
	Ulong   one_pin   = FL_ULMAX;    /* one_checkでみつけた入力ピン番号 */

	Ulong   max_imp_id = 0; // imp_id
	Ulong   up_pin_id  = 0;

	LINE    *imp_line  = imp_info->line;

	char    *func_name = "implication_and"; /* 関数名 */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state  = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	imp_result.imp_id   = 0;
	
	
	/* 手順 とりあえずver */
	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/
	/* 場合分け AND N入力版 */
	/* case 1: out  -> 0
	           case a: inがすべて1の時->conf
	           case b: inが一つX or U、残りすべて1の時->Xの入力を0に更新
			   case c: それ以外はなにもしない
	   case 2: out  -> 1
  	           case a: inのいずれかが0 or U の時->conf
			   case b: inのいずれかがXの時->Xの入力を1に更新
			   case c: それ以外はなにもしない
	   case 3: in_x -> 0
  	           case a: outが1 or Uの時->conf
			   case b: outがXの時->outを0に更新
  	           case c: outが0の時->なにもしない
	   case 4: in_x -> 1
   	           case a: outが1 or Uの時->なにもしない
			                            ( ほかのinに0がある場合 -> conf 
								          別の含意操作で必ずチェックするはず)
			   case b: outが0の時
			           case +: 他の入力に0が1つでも存在する場合  -> なにもしない
			           case -: 他の入力がすべて1の場合           -> conf
			           case *: 他は1orUで一つだけXが存在する場合 -> そのXを0に更新
			   case c: outがXの時
			           case +: ほかの入力がすべて1の場合 -> outを1に更新
					   case -: ほかの入力がすべて1かUの場合 -> outをUに更新
					   case *: 1とU以外の入力が存在する場合 -> なにもしない

	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_0 == imp_state3 ) {
			
			/* case 1 */

			/* 入力の状態を調査 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_0 == in_state3 ){
					/* 入力に0がある */
					/* case c 確定 */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_1 == in_state3 ) {
					/* すべて未確定 */
				}
				else if ( STATE3_X == in_state3 ) {

					/* 少なくともcase aではない */
					
					all_check = ALL_FLAG_OFF;
					if ( FL_ULMAX == one_pin ) {
						/* case bの可能性あり */
						one_pin = i;
					}
					else {
						/* 複数のXが入力にある */
						/* case c確定 */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
 				else if ( STATE3_U == in_state3 ) {
					all_check = ALL_FLAG_OFF;
				}
			}
			/* 調査完了!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default値 ) を返す */
				}
				else {
					/* case b */

					// max_imp_idを求める
					// output 0 , one input X, the others 1 -> one input 0
					if ( condition == COND_NORMAL ) {
						max_imp_id = gene_calc_imp_id_no_input_i_normal
							( imp_line, one_pin );
					}
					else {
						max_imp_id = gene_calc_imp_id_no_input_i_failure
							( imp_line, one_pin );
					}
					
					// 値の更新し、含意操作リストへ登録する
					imp_result = implication_update_input
						( imp_line, one_pin, STATE3_0 , condition, max_imp_id );
				}
			}
			else {
				/* case a */

				/* IMP_CONF を返す */
				imp_result.i_state = IMP_CONF;

                // confの要因となる直近の値割り当てを求める
                if ( condition == COND_NORMAL ) {
                    imp_result.imp_id = gene_calc_imp_id_all_input_normal
                        ( imp_line );
                }
                else {
                    imp_result.imp_id = gene_calc_imp_id_all_input_failure
                        ( imp_line );
                }
			}
		}
		else {
			/* case 2 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_X == in_state3 ) {
					/* case b */
					/* 値の更新し、含意操作リストへ登録する */

					// output 1 -> all inputs 1 */
					if ( condition == COND_NORMAL ) {
						max_imp_id = imp_line->imp_id_n;
					}
					else {
						max_imp_id = imp_line->imp_id_f;
					}
					imp_result = implication_update_input
						( imp_line, i, STATE3_1 , condition, max_imp_id );
				}
				else if ( ( STATE3_0 == in_state3 ) ||
						  ( STATE3_U == in_state3 ) ) {
					/* case a */
					imp_result.i_state = IMP_CONF;

                    if ( condition == COND_NORMAL ) {
                        imp_result.imp_id = imp_line->in[ i ]->imp_id_n;
                    }
                    else {
                        imp_result.imp_id = imp_line->in[ i ]->imp_id_f;
                    }
					break;
				}
			}
		}
	}
	else {
		/* case3 or case4 */
		if ( STATE3_0 == imp_state3 ) {
			/* case 3 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );

			if ( STATE3_X == out_state3 ) {
				/* case b */
				
				up_pin_id  = atpg_implication_pin_num_2_pin_id ( imp_info->pin_num );

				// one input 0 -> output 0 */
				if ( condition == COND_NORMAL ) {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_n;
				}
				else {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_f;
				}
				
				/* 値の更新し、含意操作リストへ登録する */
				imp_result = implication_update_output
					( imp_line, STATE3_0 , condition, max_imp_id );
				
			}
			else if ( STATE3_0 == out_state3 ) {
				/* case c */
			}
			else {
				/* case a */
				imp_result.i_state = IMP_CONF;
                if ( condition == COND_NORMAL ) {
                    imp_result.imp_id = imp_line->imp_id_n;
                }
                else {
                    imp_result.imp_id = imp_line->imp_id_f;
                }
			}
		}
		else {
			/* case 4 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );

			if ( ( STATE3_1 == out_state3 ) ||
				 ( STATE3_U == out_state3 ) ) {
				/* case a */
				
#ifndef TOPGUN_TEST
				/* 含意操作の順番で変化するので、なくはない */
				/* 別の含意操作で容易に発見できるため、本来要らないはず */
			    for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				    in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition ) ;
					
				    if ( ( STATE3_0 == in_state3 ) ||
					 ( STATE3_U == in_state3 ) ) { 
					result = IMP_CONF;
					break;
				    }
			    }
#endif // TOPGUN_TEST
			}
			else if ( STATE3_0 == out_state3 ) {
				/* case b */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {

					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_0 == in_state3 ) {
						all_check = ALL_FLAG_OFF;
						one_check = ONE_FLAG_OFF;
						break;
					}
					else if ( STATE3_X == in_state3 ) {
						all_check = ALL_FLAG_OFF;
						if ( FL_ULMAX == one_pin ) {
							/* case bの可能性あり */
							one_pin = i;
						}
						else {
							/* 複数のXが入力にある */
							/* case c確定 */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						all_check = ALL_FLAG_OFF;
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + */
						/* IMP_KEEP ( default値 ) を返す */
					}
					else {
						/* case * */
						/* 値の更新し、含意操作リストへ登録する */

						// one_pin以外のすべてのimp_idの最大値をmax_imp_idとする
						// output 0 , one input X, the others 1 -> one input 0
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_no_input_i_normal
								( imp_line, one_pin );
						}
						else {
							max_imp_id = gene_calc_imp_id_no_input_i_failure
								( imp_line, one_pin );
						}
						imp_result = implication_update_input 
							( imp_line, one_pin, STATE3_0 , condition, max_imp_id );

					}	
				}
				else {
					/* case - */

					/* IMP_CONF を返す */
					imp_result.i_state = IMP_CONF;
                    if ( condition == COND_NORMAL ) {
                        imp_result.imp_id = gene_calc_imp_id_no_input_i_normal ( imp_line, one_pin );
                    }
                    else {
                        imp_result.imp_id = gene_calc_imp_id_no_input_i_failure ( imp_line, one_pin );
                    }
				}
			}
			else {
				/* case c */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {

					test_line_null ( imp_line->in[ i ], func_name );
					
					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;
					
					if ( STATE3_1 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknownがある */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF != all_check ) {
					if ( UNK_FLAG_ON == unk_check ) {
						/* case - */
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal ( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure ( imp_line );
						}
						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_output
							( imp_line, STATE3_U , condition, max_imp_id );
					}
					else {
						/* case + */
						// すべてのinputに対して、imp_idの最大値をmax_imp_idとする
						// output X , all input 1 -> output 1
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal ( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure ( imp_line );
						}
						imp_result = implication_update_output						
							( imp_line, STATE3_1, condition, max_imp_id );
					}
				}
				else {
					/* case * */
					/* IMP_KEEP ( default値 ) を返す */
				}
			}
		}
	}
	return ( imp_result ) ;
}

/********************************************************************************
 * << Function >>
 *		Or of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/14
 *
 ********************************************************************************/

IMP_RESULT implication_or
(
 IMP_INFO *imp_info
 ){
	
	IMP_RESULT imp_result;

	Ulong   i          = 0;  /* カウンタ */
	
	Uint    condition  = imp_info->condition; /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* すべて同じ値かをチェックする ON: 同じ OFF: 異るものがある */
	Ulong   one_check = ONE_FLAG_ON; /* 一度のXはOK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)が存在するか */
	Ulong   one_pin   = FL_ULMAX;    /* one_checkでみつけた入力ピン番号 */

	Ulong   max_imp_id = 0;
	Ulong   up_pin_id  = 0;

	LINE    *imp_line  = imp_info->line;

	char    *func_name = "implication_or"; /* 関数名 */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );


	// initail imp_result
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id  = 0;
	

	/* 手順 とりあえずver */

	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/
	/* 場合分け OR N入力版 */
	/* case 1: out  -> 1
	           case a: inがすべて0の時->conf
	           case b: inが一つX、残りすべて0 or Uの時->Xの入力を1に更新
			   case c: それ以外はなにもしない
	   case 2: out  -> 0
  	           case a: inのいずれかが1 or U の時->conf
			   case b: inのいずれかがXの時->Xの入力を0に更新
			   case c: それ以外はなにもしない
	   case 3: in_x -> 1
  	           case a: outが0 or Uの時->conf
			   case b: outがXの時->outを1に更新
  	           case c: outが1の時->なにもしない
	   case 4: in_x -> 0
   	           case a: outが0 or Uの時->なにもしない
			                            ( ほかのinに1がある場合 -> conf 
								          別の含意操作で必ずチェックするはず)
			   case b: outが1の時
			           case +: 他の入力に1が1つでも存在する場合  -> なにもしない
			           case -: 他の入力がすべて0の場合           -> conf
			           case *: 他は0orUで一つだけXが存在する場合 -> そのXを1に更新
			   case c: outがXの時
			           case +: ほかの入力がすべて0の場合 -> outを0に更新
					   case -: ほかの入力がすべて0かUの場合 -> outをUに更新
					   case *: 0とU以外の入力が存在する場合 -> なにもしない
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_1 == imp_state3 ) {
			
			/* case 1 */

			/* 入力の状態を調査 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9,  condition );

				if ( STATE3_1 == in_state3 ){
					/* 入力に1がある */
					/* case c 確定 */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_0 == in_state3 ) {
					
					/* すべて未確定 */
					
				}
				else if ( STATE3_X == in_state3 ) {
					
					/* 少なくともcase aではない */
					all_check = ALL_FLAG_OFF;
					
					if ( FL_ULMAX == one_pin ) {
						
						/* case bの可能性あり */
						one_pin = i;
						
					}
					else {
						
						/* 複数のXが入力にある */
						/* case c確定 */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
				else {
					/* たぶん一番こない */
					/*  STATE3_U == in_stat */
					all_check = ALL_FLAG_OFF;
				}
			}
			/* 調査完了!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default値 ) を返す */
				}
				else {
					/* case b */

					/* 値の更新し、含意操作リストへ登録する */
					// output 0 , one input X, the others 1 -> one input 0
					if ( condition == COND_NORMAL ) {
						max_imp_id = gene_calc_imp_id_no_input_i_normal
							( imp_line, one_pin );
					}
					else {
						max_imp_id = gene_calc_imp_id_no_input_i_failure
							( imp_line, one_pin );
					}
					
					imp_result = implication_update_input 
						( imp_line, one_pin, STATE3_1 , condition, max_imp_id );
				}
			}
			else {
				/* case a */

				/* 衝突発生 -> IMP_CONF を返す */
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = 0;
			}
		}
		else {
			/* case 2 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_X == in_state3 ) {
					/* case b */

					/* 値の更新し、含意操作リストへ登録する */
					imp_result = implication_update_input 
						( imp_line, i, STATE3_0 , condition, max_imp_id );
					
				}
				else if ( ( STATE3_1 == in_state3 ) ||
						  ( STATE3_U == in_state3 ) ) {
					/* case a */

					/* 衝突発生 -> IMP_CONF を返す */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
					break;
				}
			}
			/* case c */
			
			/* 更新なし -> IMP_KEEP ( default値 ) を返す */
		}
			
	}
	else {
		/* case3 or case4 */
		if ( STATE3_1 == imp_state3 ) {
			/* case 3 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );

			if ( STATE3_X == out_state3 ) {
				/* case b */
				up_pin_id  = atpg_implication_pin_num_2_pin_id ( imp_info->pin_num );

				// one input 0 -> output 0 */
				if ( condition == COND_NORMAL ) {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_n;
				}
				else {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_f;
				}
				
				/* 値の更新し、含意操作リストへ登録する */
				imp_result = implication_update_output
					( imp_line, STATE3_1, condition, max_imp_id );

			}
			else if ( STATE3_1 == out_state3 ) {
				/* case c */

				/* 更新なし -> IMP_KEEP ( default値 ) を返す */
			}
			else {
				/* case a */

				/* 衝突発生 -> IMP_CONF を返す */
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = !!;
			}
		}
		else {
			/* case 4 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );


			if ( ( STATE3_0 == out_state3 ) ||
				 ( STATE3_U == out_state3 ) ) {
				/* case a */
#ifndef TOPGUN_TEST
				/* 別の含意操作で容易に発見できるため、本来要らないはず */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;
					
					if ( ( STATE3_1 == in_state3 ) ||
						 ( STATE3_U == in_state3 ) ) {
						imp_result.i_state = IMP_CONF;
						//$$$ imp_result.imp_id = !!;
						break;
					}
				}
#endif /* TOPGUN_TEST */
			}
			else if ( STATE3_1 == out_state3 ) {
				/* case b */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {

					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_1 == in_state3 ) {

						/* case +確定 */
						all_check = ALL_FLAG_OFF;
						one_check = ONE_FLAG_OFF;
						break;
					}
					else if ( STATE3_X == in_state3 ) {
						all_check = ALL_FLAG_OFF;
						if ( FL_ULMAX == one_pin ) {
							/* case *の可能性あり */
							one_pin = i;
						}
						else {
							/* 複数のXが入力にある */
							/* case +確定 */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						all_check = ALL_FLAG_OFF;
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + */
						/* IMP_KEEP ( default値 ) を返す */
					}
					else {
						/* case * */
						/* 値の更新 */

						// one_pin以外のすべてのimp_idの最大値をmax_imp_idとする
						// output 0 , one input X, the others 1 -> one input 0
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_no_input_i_normal
								( imp_line, one_pin );
						}
						else {
							max_imp_id = gene_calc_imp_id_no_input_i_failure
								( imp_line, one_pin );
						}
						
						/* 値の更新し、含意操作リストへ登録する */
						implication_update_input 
							( imp_line, one_pin, STATE3_1, condition, max_imp_id );

					}
				}
				else {
					/* case - */

					/* 衝突発生 -> IMP_CONF を返す */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
				}
			}
			else {
				/* case c */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {
					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_0 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {
							/* case *確定 */
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknownがある */
							/* case - or * */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF != all_check ) {
					if ( UNK_FLAG_ON == unk_check ) {
						/* case - */
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						
						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_output
							( imp_line, STATE3_U , condition, max_imp_id );
					}
					else {
						/* case + */
						// すべてのinputに対して、imp_idの最大値をmax_imp_idとする
						// output X , all input 0 -> output 0
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						
						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_output
							( imp_line, STATE3_0 , condition, max_imp_id );
					}
				}
				else {
					/* case - */
					/* IMP_KEEP ( default値 ) を返す */
				}
			}
		}
	}
	return ( imp_result ) ;
}

/********************************************************************************
 * << Function >>
 *		Nand of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/14
 *
 ********************************************************************************/

IMP_RESULT implication_nand
(
 IMP_INFO *imp_info
 ){
	
	IMP_RESULT imp_result;

	Ulong   i          = 0;  /* カウンタ */
	
	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* すべて同じ値かをチェックする ON: 同じ OFF: 異るものがある */
	Ulong   one_check = ONE_FLAG_ON; /* 一度のXはOK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)が存在するか */
	Ulong   one_pin   = FL_ULMAX;    /* one_checkでみつけた入力ピン番号 */

	Ulong   max_imp_id = 0;
	Ulong   up_pin_id  = 0;

	
	LINE    *imp_line  = imp_info->line;

	static char *func_name = "implication_nand"; /* 関数名 */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id  = 0;

	/* 手順 とりあえずver */

	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/
	/* 場合分け NAND N入力版 */
	/* case 1: out  -> 1
	           case a: inがすべて1の時->conf
	           case b: inが一つX or U、残りすべて1の時->Xの入力を0に更新
			   case c: それ以外はなにもしない
	   case 2: out  -> 0
  	           case a: inのいずれかが0 or U の時->conf
			   case b: inのいずれかがXの時->Xの入力を1に更新
			   case c: それ以外はなにもしない
	   case 3: in_x -> 0
  	           case a: outが0 or Uの時->conf
			   case b: outがXの時->outを1に更新
  	           case c: outが1の時->なにもしない
	   case 4: in_x -> 1
   	           case a: outが0 or Uの時->なにもしない
			                            ( ほかのinに0 or Uがある場合 -> conf 
								          別の含意操作で必ずチェックするはず)
			   case b: outが1の時
			           case +: 他の入力に0が1つでも存在する場合  -> なにもしない
			           case -: 他の入力がすべて1の場合           -> conf
			           case *: 他は1orUで一つだけXが存在する場合 -> そのXを0に更新
			           case /: Xが複数存在する場合               -> なにもしない
			   case c: outがXの時
			           case +: ほかの入力がすべて1の場合    -> outを0に更新
					   case -: ほかの入力がすべて1かUの場合 -> outをUに更新
					   case *: 1とU以外の入力が存在する場合 -> なにもしない
					   
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_1 == imp_state3 ) {
			
			/* case 1 */

			/* 入力の状態を調査 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_0 == in_state3 ){
					/* 入力に0がある */
					/* case c 確定 */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_1 == in_state3 ) {
					/* すべて未確定 */
				}
				else if ( STATE3_X == in_state3 ) {
					/* 少なくともcase aではない */
					all_check = ALL_FLAG_OFF;
					if ( FL_ULMAX == one_pin ) {
						/* case bの可能性あり */
						one_pin = i;
					}
					else {
						/* 複数のXが入力にある */
						/* case c確定 */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
				else if ( STATE3_U == in_state3 ) {
					all_check = ALL_FLAG_OFF;
				}
			}
			/* 調査完了!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default値 ) を返す */
				}
				else {
					/* case b */

					// output 1 , one input X, the others 1 -> one input 0
					if ( condition == COND_NORMAL ) {
						max_imp_id = gene_calc_imp_id_no_input_i_normal
							( imp_line, one_pin );
					}
					else {
						max_imp_id = gene_calc_imp_id_no_input_i_failure
							( imp_line, one_pin );
					}
					/* 値の更新し、含意操作リストへ登録する */
					imp_result = implication_update_input 
						( imp_line, one_pin, STATE3_0 , condition, max_imp_id );
				}
			}
			else {
				/* case a */

				/* IMP_CONF を返す */
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = !!;
			}
		}
		else {
			/* case 2 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_X == in_state3 ) {
					/* case b */

					// output 0 -> all inputs 1 */
					if ( condition == COND_NORMAL ) {
						max_imp_id = imp_line->imp_id_n;
					}
					else {
						max_imp_id = imp_line->imp_id_f;
					}
					/* 値の更新し、含意操作リストへ登録する */
					imp_result = implication_update_input 
						( imp_line, i, STATE3_1 , condition, max_imp_id );
					
				}
				else if ( ( STATE3_0 == in_state3 ) ||
						  ( STATE3_U == in_state3 ) ) {
					/* case a */

					/* IMP_CONF を返す */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
					break;
				}
				else {
					/* case c */
					
					/* IMP_KEEP ( default値 ) を返す */
				}
			}
		}
	}
	else {
		/* case3 or case4 */
		if ( STATE3_0 == imp_state3 ) {
			/* case 3 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );

			if ( STATE3_X == out_state3 ) {
				/* case b */
				up_pin_id  = atpg_implication_pin_num_2_pin_id ( imp_info->pin_num );
				if ( condition == COND_NORMAL ) {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_n;
				}
				else {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_f;
				}
				
				/* 値の更新し、含意操作リストへ登録する */
				imp_result = implication_update_output
					( imp_line, STATE3_1 , condition, max_imp_id );
				
			}
			else if ( STATE3_1 == out_state3 ) {
				/* case c */
				
				/* IMP_KEEP ( default値 ) を返す */
			}
			else {
				/* case a */

				/* IMP_CONF を返す */
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = !!;
			}
		}
		else {
			/* case 4 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );

			if ( ( STATE3_0 == out_state3 ) ||
				 ( STATE3_U == out_state3 ) ) {
				/* case a */
#ifndef TOPGUN_TEST
				/* 別の含意操作で容易に発見できるため、本来要らないはず */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;
					
					if ( ( STATE3_0 == in_state3 ) ||
						 ( STATE3_U == in_state3 ) ) {
						imp_result.i_state = IMP_CONF;
						//$$$ imp_result.imp_id = !!;
						break;
					}
				}
#endif /* TOPGUN_TEST */
			}
			else if ( STATE3_1 == out_state3 ) {
				/* case b */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {

					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_0 == in_state3 ) {
						/* case + 確定 */
						all_check = ALL_FLAG_OFF;
						one_check = ONE_FLAG_OFF;
						break;
					}
					else if ( STATE3_X == in_state3 ) {
						all_check = ALL_FLAG_OFF;
						if ( FL_ULMAX == one_pin ) {
							/* case - の可能性あり */
							one_pin = i;
						}
						else {
							/* 複数のXが入力にある */
							/* case  / 確定 */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						all_check = ALL_FLAG_OFF;
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + or / */
						/* IMP_KEEP ( default値 ) を返す */
					}
					else {
						/* case * */

						// one_pin以外のすべてのimp_idの最大値をmax_imp_idとする
						// output 0 , one input X, the others 1 -> one input 0
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_no_input_i_normal
								( imp_line, one_pin );
						}
						else {
							max_imp_id = gene_calc_imp_id_no_input_i_failure
								( imp_line, one_pin );
						}
						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_input
							( imp_line, one_pin, STATE3_0 , condition, max_imp_id );
						
					}	
				}
				else {
					/* case - */

					/* IMP_CONF を返す */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
				}
			}
			else {
				/* case c */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {

					test_line_null ( imp_line->in[ i ], func_name );
					
					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					/* 前提として0はない
					   ただ例外的にあるが、別途含意操作を行う */
					if ( STATE3_1 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {

							/* case * 確定 */
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknownがある */

							/* case - or * */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF != all_check ) {
					if ( UNK_FLAG_ON == unk_check ) {
						/* case - */

						/* 値の更新し、含意操作リストへ登録する */
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						imp_result = implication_update_output
							( imp_line, STATE3_U , condition, max_imp_id );
					}
					else {
						/* case + */

						// calculate max_imp_id
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_output
							( imp_line, STATE3_0 , condition, max_imp_id  );

					}
				}
				else {
					/* case * */
					/* IMP_KEEP ( default値 ) を返す */
				}
			}
		}
	}
	return ( imp_result ) ;
}

/********************************************************************************
 * << Function >>
 *		Nor of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/14
 *
 ********************************************************************************/

IMP_RESULT implication_nor
(
 IMP_INFO *imp_info
 ){
	IMP_RESULT imp_result;

	Ulong   i          = 0;  /* カウンタ */
	
	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* すべて同じ値かをチェックする ON: 同じ OFF: 異るものがある */
	Ulong   one_check = ONE_FLAG_ON; /* 一度のXはOK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)が存在するか */
	Ulong   one_pin   = FL_ULMAX;    /* one_checkでみつけた入力ピン番号 */

	Ulong   max_imp_id = 0;
	Ulong   up_pin_id  = 0;
	
	LINE    *imp_line  = imp_info->line;

	//char    *func_name = "implication_nor"; /* 関数名 */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state  = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id = 0;

	
	/* 手順 とりあえずver */

	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/
	/* 場合分け NOR N入力版 */
	/* case 1: out  -> 0
	           case a: inがすべて0の時                 -> conf
	           case b: inが一つX、残りすべて0 or Uの時 -> Xの入力を1に更新
			   case c: それ以外                        -> なにもしない
	   case 2: out  -> 1
  	           case a: inのいずれかが1 or U の時       -> conf
			   case b: inのいずれかがXの時             -> Xの入力を0に更新
			   case c: それ以外                        -> なにもしない
	   case 3: in_x -> 1
  	           case a: outが1 or Uの時                 -> conf
			   case b: outがXの時                      -> outを0に更新
  	           case c: outが0の時                      -> なにもしない
	   case 4: in_x -> 0
   	           case a: outが1 or Uの時->なにもしない
			                            ( ほかのinに1がある場合 -> conf(ここで初めて成立するわけではない) 
								          別の含意操作で必ずチェックするはず)
			   case b: outが0の時
			           case +: 他の入力に1が1つでも存在する場合  -> なにもしない
			           case -: 他の入力がすべて0の場合           -> conf
			           case *: 他は0orUで一つだけXが存在する場合 -> そのXを1に更新
			           case /: Xが複数存在する場合               -> なにもしない					   
			   case c: outがXの時
			           case +: ほかの入力がすべて0の場合    -> outを1に更新
					   case -: ほかの入力がすべて0かUの場合 -> outをUに更新
					   case *: 0とU以外の入力が存在する場合 -> なにもしない
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_0 == imp_state3 ) {
			
			/* case 1 */

			/* 入力の状態を調査 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_1 == in_state3 ){
					/* 入力に1がある */
					/* case c 確定 */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_0 == in_state3 ) {
					/* すべて未確定 */
				}
				else if ( STATE3_X == in_state3 ) {
					/* 少なくともcase aではない */
					all_check = ALL_FLAG_OFF;
					if ( FL_ULMAX == one_pin ) {
						/* case bの可能性あり */
						one_pin = i;
					}
					else {
						/* 複数のXが入力にある */
						/* case c確定 */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
				else if ( STATE3_U == in_state3 ) {
					all_check = ALL_FLAG_OFF;
				}
			}
			/* 調査完了!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default値 ) を返す */
				}
				else {
					/* case b */
					
					// max_imp_idを求める
					// output 0 , one input X, the others 1 -> one input 0
					if ( condition == COND_NORMAL ) {
						max_imp_id = gene_calc_imp_id_no_input_i_normal
							( imp_line, one_pin );
					}
					else {
						max_imp_id = gene_calc_imp_id_no_input_i_failure
							( imp_line, one_pin );
					}
					
					// 値の更新し、含意操作リストへ登録する
					imp_result = implication_update_input 
						( imp_line, one_pin, STATE3_1 , condition, max_imp_id );
					
				}
			}
			else {
				/* case a */

				/* IMP_CONF を返す */
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = !!;
			}
		}
		else {
			/* case 2 */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_X == in_state3 ) {
					/* case b */

					// 毎回計算する必要はない$$$
					// output 1 -> all inputs 0 */
					if ( condition == COND_NORMAL ) {
						max_imp_id = imp_line->imp_id_n;
					}
					else {
						max_imp_id = imp_line->imp_id_f;
					}
					
					/* 値の更新し、含意操作リストへ登録する */					
					imp_result = implication_update_input 
						( imp_line, i, STATE3_0 , condition, max_imp_id );
					
				}
				else if ( ( STATE3_1 == in_state3 ) ||
						  ( STATE3_U == in_state3 ) ) {
					/* case a */

					/* IMP_CONF を返す */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
					break;
				}
				else {
					/* case c */

					/* IMP_KEEP ( default値 ) を返す */
				}
			}
		}
			
	}
	else {
		/* case3 or case4 */
		if ( STATE3_1 == imp_state3 ) {
			
			/* case 3 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );

			if ( STATE3_X == out_state3 ) {
				/* case b */
				
				up_pin_id  = atpg_implication_pin_num_2_pin_id
					( imp_info->pin_num );

				// one input 0 -> output 0 */
				if ( condition == COND_NORMAL ) {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_n;
				}
				else {
					max_imp_id =  imp_line->in[ up_pin_id ]->imp_id_f;
				}
				
				/* 値の更新し、含意操作リストへ登録する */
				imp_result = implication_update_output
					( imp_line, STATE3_0 , condition, max_imp_id );
				
			}
			else if ( STATE3_0 == out_state3 ) {
				/* case c */

				/* IMP_KEEP ( default値 ) を返す */
			}
			else {
				/* case a */

				/* IMP_CONF を返す */
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = !!;
			}
		}
		else {
			/* case 4 */
			out_state3 = atpg_get_state9_with_condition_2_state3
				( imp_line->state9, condition );


			if ( ( STATE3_1 == out_state3 ) ||
				 ( STATE3_U == out_state3 ) ) {
				/* case a */
#ifndef TOPGUN_TEST
				/* 別の含意操作で容易に発見できるため、本来要らないはず */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;
					
					if ( ( STATE3_1 == in_state3 ) ||
						 ( STATE3_U == in_state3 ) ) {
						imp_result.i_state = IMP_CONF;
						//$$$ imp_result.imp_id = !!;
						break;
					}
				}
#endif /* TOPGUN_TEST */
			}
			else if ( STATE3_0 == out_state3 ) {
				/* case b */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {

					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_1 == in_state3 ) {
						/* case + 確定 */
						all_check = ALL_FLAG_OFF;
						one_check = ONE_FLAG_OFF;
						break;
					}
					else if ( STATE3_X == in_state3 ) {
						/* case -の可能性なし */
						all_check = ALL_FLAG_OFF;
						if ( FL_ULMAX == one_pin ) {
							/* case *の可能性あり */
							one_pin = i;
						}
						else {
							/* 複数のXが入力にある */
							/* case / 確定 */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						/* case -の可能性なし */
						all_check = ALL_FLAG_OFF;
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + */
						/* IMP_KEEP ( default値 ) を返す */
					}
					else {
						/* case * */

						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_input 
							( imp_line, one_pin, STATE3_1 , condition, max_imp_id );

					}	
				}
				else {
					/* case - */

					/* IMP_CONF を返す */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
				}
			}
			else {
				/* case c */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {
					/* 入力の状態を調査 */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_0 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {
							/* case * 確定 */
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknownがある */
							/* case +の可能性なし */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* 調査完了!! */
				if ( ALL_FLAG_OFF != all_check ) {
					if ( UNK_FLAG_ON == unk_check ) {
						/* case - */

						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						
						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_output
							( imp_line, STATE3_U , condition, max_imp_id );
					}
					else {
						/* case + */

						// すべてのinputに対して、imp_idの最大値をmax_imp_idとする
						// output X , all input 1 -> output 1
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						
						/* 値の更新し、含意操作リストへ登録する */
						imp_result = implication_update_output
							( imp_line, STATE3_1 , condition, max_imp_id );
					}
				}
				else {
					/* case - */
					
					/* IMP_KEEP ( default値 ) を返す */
				}
			}
		}
	}
	return ( imp_result ) ;
}

/********************************************************************************
 * << Function >>
 *		PI of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/17
 *
 ********************************************************************************/

/*
                 
   +----+  out
   | PI  >------ 
   +----+
*/


IMP_RESULT implication_pi
(
 IMP_INFO *imp_info
 ){
	/*
	  char *func_name = "implication_pi";
	*//* 関数名 */

	/* きちゃだめ ?? */
	/* 挿入で値の変化はあっても含意操作では入力側からの変化はありえない */
	/* 入力側へのトレースで行われ、PIそのものでは行わない */
	/* *** 挿入の際の出力側含意は課題である *** */

	/* 複数出力の場合と外部入力そのものに値を割り当てた時に
	   含意操作をする必要性がある */


	/* lineをstateにする含意操作を含意操作リストへ登録する */
	/* @@@@@
	atpg_enter_implication_list_output
		( imp_info->line, imp_info->new_state3 );
	*/

	/* 含意操作を記録する */
	/* atpg_enter_imp_trace ( imp_info->line, condition ); */

	IMP_RESULT imp_result;
	imp_result.i_state = IMP_KEEP;
	//imp_result.imp_id = 0;
	
	return ( imp_result ) ;
}


/********************************************************************************
 * << Function >>
 *		One input gate ( BR, Buf, PO ) of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/17
 *
 ********************************************************************************/

/*
                 
    in[0]  +------+  out
   --------+ gate |---- 
           +------+

	only one input and equal state between in[0] and out
*/

IMP_RESULT implication_one_input
(
 IMP_INFO *imp_info
 ){
	IMP_RESULT imp_result;
	
	LINE    *imp_line  = imp_info->line;

	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   max_imp_id = 0;
	
	//char *func_name = "implication_one_input"; /* 関数名 */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );
	
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id  = 0;

	/* 手順 とりあえずver */

	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/
	/* 場合分け 通過ゲート 入力版 */
	/* case 1: out  -> 0
	           case a: inが0      -> なにもしない 
	           case b: inがX      -> 0に更新
			   case c: inが1 or U -> conf
	   case 2: out  -> 1
  	           case a: inが1      -> なにもしない 
			   case b: inがX      -> 1に更新
			   case c: inが0 or U -> conf
	   case 3: in   -> 0
  	           case a: outが1 or Uの時 -> conf
			   case b: outがX          -> outを0に更新
  	           case c: outが0          -> なにもしない
	   case 4: in_x -> 1
   	           case a: outが0 or Uの時 -> conf 
			   case b: outがXの時      -> outを1に更新
			   case c: outが1の時      -> なにもしない
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */

		// output -> input
		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->imp_id_n;
		}
		else {
			max_imp_id = imp_line->imp_id_f;
		}

		/* bufのためin[0]決め打ち (busの場合は要注意) */
		in_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition );

		if ( STATE3_X == in_state3 ) {
			/* case b */
			
			/* 値の更新し、含意操作リストへ登録する */
			imp_result = implication_update_input
				( imp_line, 0, imp_state3 , condition, max_imp_id );
			
		}
		else if ( imp_state3  == in_state3  ) {
			/* case a */
			/* 何もしない */
			/* IMP_KEEP ( default値 ) を返す */
			imp_result.i_state = IMP_KEEP;
		}
		else {
			/* case c */
			/* 衝突 */
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
		}
	}		
	else {
		/* case3 or case4 */
		// input
		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->in[ 0 ]->imp_id_n;
		}
		else {
			max_imp_id = imp_line->in[ 0 ]->imp_id_f;
		}
		
		out_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->state9, condition );

		if ( STATE3_X == out_state3  ) {
			/* case b */

			/* 値の更新し、含意操作リストへ登録する */
			imp_result = implication_update_output
				( imp_line, imp_state3 , condition, max_imp_id );
			
		}
		else if ( imp_state3  == out_state3  ) {
			/* case a */
			/* 何もしない */
			/* IMP_KEEP ( default値 ) を返す */
			imp_result.i_state = IMP_KEEP;
		}
		else {
			/* case c */
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
		}
	}
	return ( imp_result ) ;
}

/********************************************************************************
 * << Function >>
 *		INV of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/17
 *
 ********************************************************************************/

/*
                 
    in[0]  +----+  out
   --------+ INV >---- 
           +----+
*/

IMP_RESULT implication_inv
(
 IMP_INFO *imp_info
 ){
	IMP_RESULT imp_result;

	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */

	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;
	STATE_3 update_state3;

	Ulong   max_imp_id = 0;

	LINE  *imp_line = imp_info->line;

	//char *func_name = "implication_inv"; /* 関数名 */
	
	in_state3     = STATE3_C;
	out_state3    = STATE3_C;
	update_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id = 0;

	/* 手順 とりあえずver */

	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/
	/* 場合分け INV 入力版 */
	/* case 1: out  -> 0
	           case a: inが1      -> なにもしない 
	           case b: inがX      -> 1に更新
			   case c: inが0 or U -> conf
	   case 2: out  -> 1
  	           case a: inが0      -> なにもしない 
			   case b: inがX      -> 0に更新
			   case c: inが1 or U -> conf
	   case 3: in   -> 0
  	           case a: outが0 or Uの時 -> conf
			   case b: outがX          -> outを1に更新
  	           case c: outが1          -> なにもしない
	   case 4: in_x -> 1
   	           case a: outが1 or Uの時 -> conf 
			   case b: outがXの時      -> outを0に更新
			   case c: outが1の時      -> なにもしない
	*/


	/* 更新する値の反転値を求める */
	update_state3 = atpg_invert_state3 ( imp_state3 );
	
	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */

		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->imp_id_n;
		}
		else {
			max_imp_id = imp_line->imp_id_f;
		}
		

		/* invのためin[0]決め打ち (busの場合は要注意) */
		in_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition );

		
		if ( STATE3_X == in_state3 ) {
			/* case b */
			
			/* 値の更新し、含意操作リストへ登録する */
			imp_result = implication_update_input
				( imp_line, 0, update_state3, condition, max_imp_id );
		}
		else if ( update_state3 == in_state3 ) {
			/* case a */
			/* 何もしない */
			imp_result.i_state = IMP_KEEP;
		}
		else {
			/* case c */
			/* 衝突 */
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
		}
	}		
	else {
		/* case3 or case4 */
		out_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->state9, condition );
		
		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->in[ 0 ]->imp_id_n;
		}
		else {
			max_imp_id = imp_line->in[ 0 ]->imp_id_f;
		}

		if ( STATE3_X == out_state3 ) {
			/* case b */

			/* 値の更新し、含意操作リストへ登録する */
			imp_result = implication_update_output
				( imp_line, update_state3, condition, max_imp_id );
		}
		else if ( update_state3 == out_state3 ) {
			/* case a */
			/* 何もしない */
			/* IMP_KEEP ( default値 ) を返す */
			imp_result.i_state = IMP_KEEP;
		}
		else {
			/* case c */
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
		}
	}
	return ( imp_result ) ;
}



/********************************************************************************
 * << Function >>
 *		Black Box gate of  implication 
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/17
 *
 ********************************************************************************/

/*
                 
    in[0]  +-----------+  out
   --------+ black box |------X
           +-----------+

		        or 

           +-----------+  out  +----+
        X--+ black box |-------+ ?? +--
           +-----------+       +----+
		   
	black box ( one input and one output )
*/

IMP_RESULT implication_blk
(
 IMP_INFO *imp_info
 ){

	/* $$$ あとで考える $$$ */

	IMP_RESULT imp_result;
	//Ulong   max_imp_id = 0;
    
	imp_result.i_state = IMP_KEEP;
	//imp_result.imp_id  = 0;
	
#ifdef TOPGUN_BLK	

	IMP_RESULT result  = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	LINE  *imp_line = imp_info->line;

	Uint  condition = imp_info->condition;  /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;
	
	//char *func_name = "implication_blk"; /* 関数名 */
		
	in_state3     = STATE3_C;
	out_state3    = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	/* 手順 とりあえずver */

	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/
	/* 場合分け PO 入力版 */
	/* case 1: out  -> 0
	           case a: inが0      -> なにもしない 
	           case b: inがX      -> 0に更新
			   case c: inが1 or U -> conf
	   case 2: out  -> 1
  	           case a: inが1      -> なにもしない 
			   case b: inがX      -> 1に更新
			   case c: inが0 or U -> conf
	   case 3: in   -> 0
  	           case a: outが1 or Uの時 -> conf
			   case b: outがX          -> outを0に更新
  	           case c: outが0          -> なにもしない
	   case 4: in_x -> 1
   	           case a: outが0 or Uの時 -> conf 
			   case b: outがXの時      -> outを1に更新
			   case c: outが1の時      -> なにもしない
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */

		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->imp_id_n;
		}
		else {
			max_imp_id = imp_line->imp_id_f;
		}
        
		/* bufのためin[0]決め打ち (busの場合は要注意) */
		in_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition );

		if ( STATE3_X == in_state3 ) {
			/* case b */

			/* 値の更新し、含意操作リストへ登録する */
			result = implication_update_input ( imp_line, 0, STATE3_U, condition, max_imp_id );
		}
		else if ( in_state3 == imp_state3 ) {
			/* case a */
			/* 何もしない */
		}
		else {
			/* case c */
			/* 衝突 */
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
			result = IMP_CONF;
		}
	}		
	else {
		/* case3 or case4 */
		out_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->state9, condition );

		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->in[ 0 ]->imp_id_n;
		}
		else {
			max_imp_id = imp_line->in[ 0 ]->imp_id_f;
		}
        
		if ( STATE3_X == out_state3 ) {
			/* case b */
			
			/* 値の更新し、含意操作リストへ登録する */
			result = implication_update_output ( imp_line, STATE3_U, condition, max_imp_id );

		}
		else if ( in_state3 == out_state3 ) {
			/* case a */
			/* 何もしない */
			/* IMP_KEEP ( default値 ) を返す */
		}
		else {
			/* case c */
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
			result = IMP_CONF;
		}
	}
	return ( result ) ;

#endif /* TOPGUN_BLK */

	return ( imp_result ) ;
}



/********************************************************************************
 * << Function >>
 *		Xor of  implication ( in = 2 )
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/
#ifdef TOPGUN_XOR_ON
IMP_RESULT implication_xor
(
 IMP_INFO *imp_info
 ){
	IMP_RESULT result  = IMP_KEEP; /* IMP_KEEP or IMP_CONF */

	Ulong   in         = 0;
	Ulong   other      = 0;
	Ulong   up_pin_id  = 0;  
	
	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */

	IMP_RESULT_INFO imp_result_info;
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in0_state3;
	STATE_3 in1_state3;
	STATE_3 ino_state3;
	STATE_3 out_state3;

	LINE    *imp_line  = imp_info->line;

	//char    *func_name = "implication_xor"; /* 関数名 */

	in0_state3 = STATE3_C;
	in1_state3 = STATE3_C;
	ino_state3 = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );

	/* 手順 とりあえずver */

	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/



	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
			
		in0_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition ) ;
		in1_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 1 ]->state9, condition ) ;

		if ( STATE3_0 == imp_state3 ) {
			/* case 1 */
			Xor_implication_out0[ in0_state3 ][ in1_state3 ] ( &imp_result_info );
		}
		else {
			/* case 2 */
			Xor_implication_out1[ in0_state3 ][ in1_state3 ] ( &imp_result_info );
		}

		/* 含意操作結果を判断 */
		if ( imp_result_info.result == IMP_UPDA ) {

			up_pin_id  = atpg_implication_pin_num_2_pin_id ( imp_result_info.pin_num );
			
			/* 値の更新し、含意操作リストへ登録する */
			result = implication_update_input
				( imp_line, up_pin_id, imp_result_info.state3, condition );
			
		}
		else {
			/* 何もしない(IMP_KEEP) or 衝突(IMP_CONF) */
			return ( imp_result_info.result );
		}
	}
	else {
		/* case3 or case4 */

		/* 更新したpin idを求める */
		in = atpg_implication_pin_num_2_pin_id ( imp_info->pin_num );

		/* 更新しなかったpin idを求める */
		/* when in = 1, other is 0. when in = 0, other is 1 */
		other = in^IMP_XOR;

		out_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->state9, condition );
		ino_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ other ]->state9, condition ) ;
		
		if ( STATE3_0 == imp_state3 ) {
			/* case 3 */
			Xor_implication_in0[ out_state3 ][ ino_state3 ] ( &imp_result_info );
		}
		else {
			/* case 4 */
			Xor_implication_in1[ out_state3 ][ ino_state3 ] ( &imp_result_info );
		}

		if ( IMP_UPDA == imp_result_info.result ) {
			/* 値更新 (IMP_UPDA) */
			
			if ( OUT_PIN_NUM == imp_result_info.pin_num ) {
				/* 更新が出力の場合 */
				
				/* 値の更新し、含意操作リストへ登録する */
				result = implication_update_output
					( imp_line, imp_result_info.state3, condition );
				
			}
			else {
				/* 更新が入力の場合 */

				/* 値の更新し、含意操作リストへ登録する */
				result = implication_update_input
					( imp_line, other, imp_result_info.state3, condition );
			}

		}
		else {
			/* 何もしない(IMP_KEEP) or 衝突(IMP_CONF) */
			return ( imp_result_info.result );
		}

	}
	return ( result ) ;
}
#endif /* TOPGUN_XOR_ON */

/********************************************************************************
 * << Function >>
 *		Xnor of  implication ( in = 2 )
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	stat		: state of implication result
 *
 * << Argument >>
 * args			(I/O)	type		description
 * imp_info		I		IMP_INFO *	infomation of implication
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

#ifdef TOPGUN_XOR_ON
IMP_RESULT implication_xnor
(
 IMP_INFO *imp_info
 ){
	IMP_RESULT result  = IMP_KEEP; /* IMP_KEEP or IMP_CONF */

	Ulong   in         = 0;
	Ulong   other      = 0;
	Ulong   up_pin_id  = 0;  
	
	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */

	IMP_RESULT_INFO imp_result_info;
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in0_state3;
	STATE_3 in1_state3;
	STATE_3 ino_state3;
	STATE_3 out_state3;

	LINE    *imp_line  = imp_info->line;

	//char    *func_name = "implication_xnor"; /* 関数名 */

	in0_state3 = STATE3_C;
	in1_state3 = STATE3_C;
	ino_state3 = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );
	
	/* 
	   1. new_stateをチェックする
	      -> 正のみ or 故のみ or 正故両方(あるか不明->とりあえずNG)
	   2. pinをデコードする
	   3. 含意操作の結果を得る
	   4. 3.で変化した値がある場合は、stackに積み、完了の結果を返す
	   5. 3.で衝突したばあいは衝突の結果を返す

	*/



	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */

		in0_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition ) ;
		in1_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 1 ]->state9, condition ) ;

		if ( STATE3_0 == imp_state3 ) {
			/* case 1 */
			Xnor_implication_out0[ in0_state3 ][ in1_state3 ] ( &imp_result_info );
		}
		else {
			/* case 2 */
			Xnor_implication_out1[ in0_state3 ][ in1_state3 ] ( &imp_result_info );
		}

		/* 含意操作結果を判断 */
		if ( imp_result_info.result == IMP_UPDA ) {

			up_pin_id  = atpg_implication_pin_id_2_pin_num ( imp_result_info.pin_num );
			
			/* 値の更新し、含意操作リストへ登録する */
			result = implication_update_input
				( imp_line, up_pin_id, imp_result_info.state3, condition );

		}
		else {
			/* 何もしない(IMP_KEEP) or 衝突(IMP_CONF) */
			return ( imp_result_info.result );
		}
	}
	else {
		/* case3 or case4 */

		/* 更新したpin idを求める */
		in = atpg_implication_pin_num_2_pin_id ( imp_info->pin_num );

		/* 更新しなかったpin idを求める */
		/* when in = 1, other is 0. when in = 0, other is 1 */
		other = in^IMP_XOR;

		out_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->state9, condition );
		ino_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ other ]->state9, condition ) ;
		
		if ( STATE3_0 == imp_state3 ) {
			/* case 3 */
			Xnor_implication_in0[ out_state3 ][ ino_state3 ] ( &imp_result_info );
		}
		else {
			/* case 4 */
			Xnor_implication_in1[ out_state3 ][ ino_state3 ] ( &imp_result_info );
		}

		if ( IMP_UPDA == imp_result_info.result ) {
			/* 値更新 (IMP_UPDA) */
			
			if ( OUT_PIN_NUM == imp_result_info.pin_num ) {
				/* 更新が出力の場合 */
				
				/* 値の更新し、含意操作リストへ登録する */
				result = implication_update_output
					( imp_line, imp_result_info.state3, condition );
			}
			else {
				/* 更新が入力の場合 */

				/* 値の更新し、含意操作リストへ登録する */
				result = implication_update_input
					( imp_line, other, imp_result_info.state3, condition );
			}

		}
		else {
			/* 何もしない(IMP_KEEP) or 衝突(IMP_CONF) */
			return ( imp_result_info.result );
		}
	}
	return ( result ) ;
}
#endif /* TOPGUN_XOR_ON */

/********************************************************************************
 * << Function >>
 *		getting pin num from pin id ( input(0) => in[0] => pin_id 1  output (1))
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * ass_line		I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

Ulong atpg_implication_pin_id_2_pin_num
(
 Ulong pin_id
 ){
	return( ( pin_id + 1 ) ) ;
}

/********************************************************************************
 * << Function >>
 *		getting pin id from pin num ( id 1 => in[0] , id 2 => in[1] )
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * ass_line		I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

Ulong atpg_implication_pin_num_2_pin_id
(
 Ulong pin_num
 ){
	return( ( pin_num - 1 ) ) ;
}


/********************************************************************************
 * << Function >>
 *		getting pin id on front implication for implication
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * ass_line		I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

Ulong extern inline atpg_get_imp_pin_id
(
 LINE *line
 ){

	Ulong i = 0; /* ゲート入力数カウンタ */
	Ulong org_id = line->line_id; /* 目的のID */
	LINE  *out = NULL; /* 出力ゲートのlineポインタ */

	char *func_name = "atpg_get_imp_pin_id"; /* 関数名 */

#ifndef TOPGUN_TEST	
	if ( line == NULL ) {
		topgun_error( FEC_PRG_LINE_ID, func_name );
	}
	if ( line->n_out != 1 ) {
		topgun_error( FEC_PRG_LINE_ID, func_name );
	}
	if ( line->out[0] == NULL ) {
		topgun_error( FEC_PRG_LINE_ID, func_name );
	}
#endif /* TOPGUN_TEST */

	out = line->out[0];
	
	for ( i = 0 ; i< out->n_in; i++ ) {
		if ( org_id == out->in[ i ]->line_id ) {
			return ( i );
		}
	}
	topgun_error( FEC_PRG_LINE_ID, func_name );
	
	/* 実際はこない */
	return(FL_ULMAX);
}


 
/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_KEEP ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_keep
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->i_state = IMP_KEEP;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_CONF ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_conflict
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->i_state = IMP_CONF;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA out state 0 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_out_state0
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_0;
	imp_result_info->pin_num = OUT_PIN_NUM;
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA out state 1 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_out_state1
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_1;
	imp_result_info->pin_num = OUT_PIN_NUM;
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA out state U ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_out_stateU
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_U;
	imp_result_info->pin_num = OUT_PIN_NUM;
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA in0 state 0 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_in0_state0
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_0;
	imp_result_info->pin_num = IN0_PIN_NUM;
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA in0 state 1 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_in0_state1
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_1;
	imp_result_info->pin_num = IN0_PIN_NUM;
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA in1 state 0 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_in1_state0
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_0;
	imp_result_info->pin_num = IN1_PIN_NUM;
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA in1 state 1 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_in1_state1
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_1;
	imp_result_info->pin_num = IN1_PIN_NUM;
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA in other state 0 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_ino_state0
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_0;
	imp_result_info->pin_num = FL_ULMAX; /* 非 OUT_PIN_NUMであればよい */
	imp_result_info->i_state = IMP_UPDA;
}

/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA in other state 0 ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_ino_state1
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_1;
	imp_result_info->pin_num = FL_ULMAX; /* 非 OUT_PIN_NUMであればよい */
	imp_result_info->i_state = IMP_UPDA;
}


/********************************************************************************
 * << Function >>
 *		make implication result ( IMP_UPDA in other state U ) 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void implication_ino_stateU
(
 IMP_RESULT_INFO *imp_result_info
){
	imp_result_info->state3  = STATE3_U;
	imp_result_info->pin_num = FL_ULMAX; /* 非 OUT_PIN_NUMであればよい */
	imp_result_info->i_state = IMP_UPDA;
}


/********************************************************************************
 * << Function >>
 *		IMP_UPDA処理実施関数 to input
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/20
 *
 ********************************************************************************/

IMP_RESULT implication_update_input
(
 LINE    *line,
 Ulong   pin_id,
 STATE_3 state3,
 Uint    condition,
 Ulong   max_imp_id
){

	LINE     *in = line->in[ pin_id ];
	IMP_RESULT imp_result;
	char indirect_flag = 0;
	STATE_3 in_state3 = STATE3_C;
	
	CONDITION reverse_condition = 0;

	//char *func_name = "implication_update_input"; /* 関数名 */

	topgun_test_line_null ( in, func_name );

	imp_result.i_state = IMP_UPDA;
	//imp_result.imp_id = 0;

	if ( in->flag & FV_ASS ) {
		/* 故障値を設定可能 */
		/* 値の更新 */

		gene_update_state ( in, state3, condition );
		
		/* lineをstateにする含意操作を含意操作リストへ登録する */
		atpg_enter_implication_list_input
			( in, state3, condition, OUT_PIN_NUM, max_imp_id );
		
		topgun_print_mes_gate_state9_new( line, pin_id+1, condition, 1 );
		topgun_print_mes( 24, 5 );

		/* 含意操作を記録する */
		Switch_enter_imp_trace[ condition ] ( in );

		if ( condition == COND_NORMAL ) {
			indirect_flag = 1;
		}

		if ( ! ( in->flag & FP_IAP ) ) {
			/* 伝搬経路でない */
			/* 逆の値も一緒に更新 */

			reverse_condition = atpg_reverse_condition ( condition );
			in_state3 = atpg_get_state9_with_condition_2_state3
				( in->state9, reverse_condition );

			if ( STATE3_X == in_state3 ) {
				
				topgun_print_mes ( 24, 6 );
			
				gene_update_state ( in, state3, reverse_condition );
		
				/* lineをstateにする含意操作を含意操作リストへ登録する */
				atpg_enter_implication_list_output
					( in, state3, reverse_condition, max_imp_id ); /* 出力方向も追加 2005/11/29 */
				atpg_enter_implication_list_input
					( in, state3, reverse_condition, OUT_PIN_NUM, max_imp_id );
		
				topgun_print_mes_gate_state9_new( line, pin_id+1, reverse_condition, 1 );
				topgun_print_mes( 24, 5 );
			
				/* 含意操作を記録する */
				Switch_enter_imp_trace[ reverse_condition ] ( in );

				if ( reverse_condition == COND_NORMAL ) {
					indirect_flag = 1;
				}
			}
			else if ( state3 == in_state3 ) {
				/* keep */
				/* normal_condition is update -> imp_result is update */
			}
			else {
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = !!;
				return ( imp_result );
			}
		}
	}
	else {
		/* 正常値として更新 */
		if ( condition == COND_FAILURE ) {
			topgun_print_mes( 24, 7 );
		}

		in_state3 = atpg_get_state9_with_condition_2_state3
			( in->state9, COND_NORMAL );

		if ( STATE3_X == in_state3 ) {
			
			/* 値の更新 */
			gene_update_state ( in, state3, COND_NORMAL );

			/* lineをstateにする含意操作を含意操作リストへ登録する */
			atpg_enter_implication_list_input
				( in, state3, COND_NORMAL, OUT_PIN_NUM, max_imp_id );

			topgun_print_mes_gate_state9_new( line, pin_id+1, COND_NORMAL, 1 );
			topgun_print_mes( 24, 5 );

			/* 含意操作を記録する */
			Switch_enter_imp_trace[ COND_NORMAL ] ( in );

			indirect_flag = 1;
		}
		else if ( state3 == in_state3 ) {
			/* keep */
			/* normal_condition is update -> imp_result is update */
		}
		else {
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
			return ( imp_result );
		}
	}

	/* 正常値の更新が発生した→間接含意を実施する */
	if ( indirect_flag == 1 ) {
		if ( state3 == STATE3_0 ) {
			if ( in->imp_0 != NULL ) {
				topgun_print_mes( 24, 9 );
				topgun_print_mes_gate_info_learn();
				topgun_print_mes_gate_state9_line( in );
			}
			imp_result = atpg_update_indirect_state ( in->imp_0, max_imp_id );
		}
		else {
			if ( in->imp_1 != NULL ) {
				topgun_print_mes( 24, 9 );
				topgun_print_mes_gate_info_learn();
				topgun_print_mes_gate_state9_line( in );
			}
			imp_result = atpg_update_indirect_state ( in->imp_1, max_imp_id );
		}
	}
	return ( imp_result );
	//return ( IMP_UPDA );
}

/********************************************************************************
 * << Function >>
 *		IMP_UPDA処理実施関数 to output
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * name			(I/O)	type			    description
 * imp_result_info	 I/O	IMP_RESULT_INFO *	implication result
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/20
 *
 ********************************************************************************/

IMP_RESULT implication_update_output
(
 LINE    *line,
 STATE_3 state3,
 Uint    condition,
 Ulong   max_imp_id
){
	CONDITION reverse_condition = 0;
	IMP_RESULT imp_result;
	char indirect_flag = 0;
	STATE_3 line_state3 = STATE3_C;
	
	//char *func_name = "implication_update_output"; /* 関数名 */

	topgun_test_line_null ( line, func_name );

	// initail imp_result
	imp_result.i_state = IMP_UPDA;
	//imp_result.imp_id  = 0;

	if ( line->flag & FV_ASS ) {
		/* 値の更新 */	
		gene_update_state ( line, state3, condition );

		/* lineをstateにする含意操作を含意操作リストへ登録する */
		atpg_enter_implication_list_output
			( line, state3, condition, max_imp_id );

		topgun_print_mes_gate_state9_new( line, OUT_PIN_NUM, condition, 1 );
		topgun_print_mes( 24, 5 );

		/* 含意操作を記録する */
		Switch_enter_imp_trace[ condition ] ( line );

		if ( condition == COND_NORMAL ) {
			indirect_flag = 1;
		}

		if ( ! ( line->flag & FP_IAP ) ) {

			reverse_condition = atpg_reverse_condition ( condition );

			line_state3 = atpg_get_state9_with_condition_2_state3
				( line->state9, reverse_condition );

			if ( STATE3_X == line_state3 ) {
			
				topgun_print_mes ( 24, 6 );

				gene_update_state ( line, state3, reverse_condition );

				/* lineをstateにする含意操作を含意操作リストへ登録する */
				atpg_enter_implication_list_output
					( line, state3, reverse_condition, max_imp_id );

				topgun_print_mes_gate_state9_new( line, OUT_PIN_NUM, reverse_condition, 1 );
				topgun_print_mes( 24, 5 );

				/* 含意操作を記録する */
				Switch_enter_imp_trace[ reverse_condition ] ( line );

				if ( reverse_condition == COND_NORMAL ) { //!< たぶん不要
					indirect_flag = 1;
				}
			}
			else if ( state3 == line_state3 ) {
				/* keep */
				/* normal_condition is update -> imp_result is update */
			}
			else {
				imp_result.i_state = IMP_CONF;
				//$$$ imp_result.imp_id = !!;
				return ( imp_result );
			}
		}
	}
	else {
		/* 正常値として更新 */
		if ( condition == COND_NORMAL ) {
			/* $$$ 多分ないはず 削除候補 $$$ */
			topgun_print_mes ( 24, 7 );
		}

		line_state3 = atpg_get_state9_with_condition_2_state3
			( line->state9, COND_NORMAL );

		if ( STATE3_X == line_state3 ) {

			/* 値の更新 */	
			gene_update_state ( line, state3, COND_NORMAL );
			
			/* lineをstateにする含意操作を含意操作リストへ登録する */
			atpg_enter_implication_list_output
				( line, state3, COND_NORMAL, max_imp_id );
			
			topgun_print_mes_gate_state9_new( line, OUT_PIN_NUM, COND_NORMAL, 1 );
			topgun_print_mes( 24, 5 );
			
			/* 含意操作を記録する */
			Switch_enter_imp_trace[ COND_NORMAL ] ( line );
			indirect_flag = 1;
		}
		else if ( state3 == line_state3 ) {
			/* keep */
			/* normal_condition is update -> imp_result is update */
		}
		else {
			imp_result.i_state = IMP_CONF;
			//$$$ imp_result.imp_id = !!;
			return ( imp_result );
		}
	}

	/* 正常値の更新が発生した→間接含意を実施する */
	if ( indirect_flag == 1 ) {
		if ( state3 == STATE3_0 ) {
			if ( line->imp_0 != NULL ) {
				topgun_print_mes( 24, 9 );
				topgun_print_mes_gate_info_learn();
				topgun_print_mes_gate_state9_line( line );
			}
			imp_result = atpg_update_indirect_state ( line->imp_0, max_imp_id );
		}
		else {
			if ( line->imp_1 != NULL ) {
				topgun_print_mes( 24, 9 );
				topgun_print_mes_gate_info_learn();
				topgun_print_mes_gate_state9_line( line );
			}
			imp_result = atpg_update_indirect_state ( line->imp_1, max_imp_id );
		}
	}
	
	return ( imp_result );

}




/********************************************************************************
 * << Function >>
 *		make imp_info 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * line		    I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

IMP_INFO extern inline *atpg_make_imp_info
(
 LINE    *line,
 STATE_3 new_state3,
 Uint    condition,
 Ulong   pin_num
 ){
	IMP_INFO   *imp_info;
	
	char *func_name = "atpg_make_imp_info"; /* 関数名 */

	topgun_test_line_null ( line, func_name );
	topgun_test_state3 ( new_state3, func_name );

	/* imp_infoのメモリ確保 */
	imp_info = ( IMP_INFO * )topgunMalloc( FMT_IMP_INFO, sizeof( IMP_INFO ),
												1, func_name );
		
	imp_info->line       = line;
	imp_info->new_state3 = new_state3;
	imp_info->condition  = condition;
	imp_info->pin_num    = pin_num;
	imp_info->next       = NULL;

	return ( imp_info );
}

/********************************************************************************
 * << Function >>
 *		enter imp_info 
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * line		    I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/08/11
 *
 ********************************************************************************/


void extern inline gene_enter_imp_info
(
 LINE    *line,
 STATE_3 new_state3,
 Uint    condition,
 Ulong   pin_num,
 Ulong   max_imp_id
 ){
	IMP_INFO   *imp_info;
	
	static char *func_name = "gene_enter_imp_info"; /* 関数名 */

	topgun_test_line_null ( line, func_name );
	topgun_test_state3 ( new_state3, func_name );

	if ( Gene_head.n_enter_imp_list == Gene_head.n_imp_info_list ) {
		/* 上限をうったため */
		topgun_error( FEC_PRG_IMPLICATION, func_name );
	}

	imp_info =  &(Gene_head.imp_info_list [ Gene_head.n_enter_imp_list++ ] );
		
	imp_info->line       = line;
	imp_info->new_state3 = new_state3;
	imp_info->condition  = condition;
	imp_info->pin_num    = pin_num;
	imp_info->max_imp_id = max_imp_id;

}


/********************************************************************************
 * << Function >>
 *		execute that line, pin and state enter implication lists
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * ass_line		I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

#ifdef OLD_IMP
void atpg_enter_implication_list
(
 IMP_INFO   *imp_info
 ){

    char *func_name = "atpg_enter_implication_list"; /* 関数名 */

    if ( Gene_head.imp_f != NULL ) {
	Gene_head.imp_f->next = imp_info;
    }
    Gene_head.imp_f = imp_info;
	
    if ( Gene_head.imp_l == NULL ) {
	Gene_head.imp_l = imp_info;
    }
}
#endif /* OLD_IMP */

/********************************************************************************
 * << Function >>
 *		execute that line, pin and state enter implication lists
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * ass_line		I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

void atpg_enter_implication_list_input
(
 LINE    *line,
 STATE_3 new_state3,
 Uint    condition,
 Ulong   pin_num,
 Ulong   max_imp_id
 ){
	Ulong     i = 0;
	
	//char *func_name = "atpg_enter_implication_list_input"; /* 関数名 */

	topgun_test_line_null ( line, func_name );
	topgun_test_state3 ( new_state3, func_name );

	if ( ( line->flag & LINE_FLT ) &&
		 ( condition == COND_FAILURE ) ) 	{
		/* 故障信号線の故障値の入力側の含意操作はしない */
		;
	}
	else {

		gene_enter_imp_info	( line, new_state3, condition, pin_num, max_imp_id);
				

		/* 入力がFanout Stemの場合 */
		if ( line->n_out > 1 ) {
			for ( i = 0 ; i < line->n_out ; i++ ) {
				if  ( line->out[ i ]->flag & LINE_FLT ) {
					/* 故障信号線への前方含意操作はNG */
					;
				}
				else {
					gene_enter_imp_info
						( line->out[ i ], new_state3, condition, IN0_PIN_NUM, max_imp_id );
				}
			}
		}
	}
}

/********************************************************************************
 * << Function >>
 *		execute that line, pin and state enter implication lists
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * ass_line		I		LINE *		impication line pointer
 * imp_stat		I		int         0: nothing, 1: front, 2:back 3: both
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

void atpg_enter_implication_list_output
(
 LINE      *imp_line,
 STATE_3   new_state3,
 CONDITION condition,
 Ulong max_imp_id
 ){

	char *func_name = "atpg_enter_implication_list_output"; /* 関数名 */

	topgun_test_state3 ( new_state3, func_name );
	//topgun_test_condition ( condition, func_name );

	/* 故障値の故障信号線への前方含意操作はNG */

	switch ( condition ) {
	case COND_NORMAL:
		if ( imp_line->n_out == 1 ) {
			atpg_enter_implication_list_output_normal_one_out
				( imp_line, new_state3, max_imp_id );
		}
		else {
			atpg_enter_implication_list_output_normal_multi_out
				(imp_line, new_state3, max_imp_id );
		}
		break;
	case COND_FAILURE:
		/* 故障値(1)確定 */
		
		if ( imp_line->n_out == 1 ) {
			atpg_enter_implication_list_output_failure_one_out
				( imp_line, new_state3, max_imp_id );

		}
		else {
			atpg_enter_implication_list_output_failure_multi_out
				(imp_line, new_state3, max_imp_id );
		}
		break;
	default:
		topgun_error ( FEC_PRG_LINE_COND, func_name );
	}
}


void atpg_enter_implication_list_output_normal_multi_out
(
 LINE      *imp_line,
 STATE_3   new_state3,
 Ulong     max_imp_id 
 ){
	Ulong i;
	
	for ( i = 0; i < imp_line->n_out ; i++ ) {
		
		topgun_test_line_null ( imp_line->out[ i ], func_name );
				
		/* imp_lineがFanOutStemの場合
		   -> 必ず接続先は branch */
		gene_enter_imp_info
			( imp_line->out[ i ], new_state3, COND_NORMAL, IN0_PIN_NUM, max_imp_id );
	}
}

 
void atpg_enter_implication_list_output_failure_multi_out
(
 LINE      *imp_line,
 STATE_3   new_state3,
 Ulong     max_imp_id 
 ){
	Ulong i;
	
	for ( i = 0; i < imp_line->n_out ; i++ ) {
				
		topgun_test_line_null ( imp_line->out[ i ], func_name );
				
		if  ( imp_line->out[ i ]->flag & LINE_FLT ) {
			/* 故障信号線への前方含意操作はNG */
			;
		}
		else {
			gene_enter_imp_info
				( imp_line->out[ i ], new_state3, COND_FAILURE, IN0_PIN_NUM, max_imp_id );
		}
	}
}

void atpg_enter_implication_list_output_normal_one_out
(
 LINE      *imp_line,
 STATE_3   new_state3,
 Ulong     max_imp_id 
 ){
	Ulong pin_id = 0;

	topgun_test_line_null ( imp_line->out[ 0 ], func_name );

	pin_id = atpg_implication_pin_id_2_pin_num     /* 更新する信号線のpin番号 */
		( atpg_get_imp_pin_id ( imp_line ) );
			
	gene_enter_imp_info
		( imp_line->out[0],                     /* 含意操作する信号線(出力信号線) */
		  new_state3,                          /* 更新された値 */
		  COND_NORMAL,
		  pin_id,
		  max_imp_id );
}

void atpg_enter_implication_list_output_failure_one_out
(
 LINE      *imp_line,
 STATE_3   new_state3,
 Ulong     max_imp_id 
 ){
	Ulong pin_id = 0;

	topgun_test_line_null ( imp_line->out[ 0 ], func_name );

	if ( imp_line->out[0]->flag & LINE_FLT ) {
		/* 故障信号線(2)確定 */
		/* 故障値でかつ故障信号線が搦む含意操作はNG */
		topgun_print_mes ( 24, 8 );
	}
	else {
		/* imp_lineがFanOutStemでない場合 */
				
		pin_id = atpg_implication_pin_id_2_pin_num     /* 更新する信号線のpin番号 */
			( atpg_get_imp_pin_id ( imp_line ) );

				
		gene_enter_imp_info
			( imp_line->out[0],                     /* 含意操作する信号線(出力信号線) */
			  new_state3,                           /* 更新された値 */
			  COND_FAILURE,                            /* STAT_N or STAT_F */
			  pin_id,
			  max_imp_id );
	}
}


/********************************************************************************
 * << Function >>
 *		calculate max imp_id for normal value
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * line			I		LINE *		impication line pointer
 * x_input_id	I		Ulong       input number with x value 
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura ( Kyushu University )
 *
 * << Date >>
 *		2007/06/15
 *
 ********************************************************************************/

Ulong gene_calc_imp_id_no_input_i_normal
(
 LINE *line,
 Ulong x_input_id
 ){
	
	Ulong max_imp_id = 0;
	Ulong i; //カウンタ
	
	//char *func_name = "gene_calc_imp_id_no_input_i_normal"; 
	
	max_imp_id = line->imp_id_n;

	for ( i = 0 ; i < line->n_in ; i++ ) {
		//どうせ0だからif文抜いたほうが速いかも
		if ( i != x_input_id ) {
			if ( max_imp_id < line->in[ i ]->imp_id_n ) {
				max_imp_id = line->in[ i ]->imp_id_n;
			}
		}
	}
	return ( max_imp_id );
}

/********************************************************************************
 * << Function >>
 *		calculate max imp_id for failure value
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * line			I		LINE *		impication line pointer
 * x_input_id	I		Ulong       input number with x value 
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura ( Kyushu University )
 *
 * << Date >>
 *		2007/06/15
 *
 ********************************************************************************/

Ulong gene_calc_imp_id_no_input_i_failure
(
 LINE *line,
 Ulong x_input_id
 ){
	
	Ulong max_imp_id = 0;
	Ulong i; //カウンタ
	
	//char *func_name = "gene_calc_imp_id_no_input_i_normal"; 

	max_imp_id = line->imp_id_n;

	for ( i = 0 ; i < line->n_in ; i++ ) {
		//どうせ0だからif文抜いたほうが速いかも
		if ( i != x_input_id ) {
			if ( max_imp_id < line->in[ i ]->imp_id_n ) {
				max_imp_id = line->in[ i ]->imp_id_n;
			}
		}
	}
	return ( max_imp_id );
}

/********************************************************************************
 * << Function >>
 *		calculate max imp_id of normal value for all input lines
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * line			I		LINE *		impication line pointer
 * x_input_id	I		Ulong       input number with x value 
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura ( Kyushu University )
 *
 * << Date >>
 *		2007/06/15
 *
 ********************************************************************************/

Ulong gene_calc_imp_id_all_input_normal
(
 LINE *line
 ){
	
	Ulong max_imp_id = line->in[ 0 ]->imp_id_n;
	Ulong i; //カウンタ
	
	//char *func_name = "gene_calc_imp_id_all_input_normal"; 
	
	for ( i = 1 ; i < line->n_in ; i++ ) {
		//どうせ0だからif文抜いたほうが速いかも
		if ( max_imp_id < line->in[ i ]->imp_id_n ) {
			max_imp_id = line->in[ i ]->imp_id_n;
		}
	}
	return ( max_imp_id );
}

/********************************************************************************
 * << Function >>
 *		calculate max imp_id of failure value for all input lines
 *
 * << Function ID >>
 *    	6-?
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type		description
 * line			I		LINE *		impication line pointer
 *
 * << extern >>
 * 		NONE
 *
 * << author >>
 * 		Masayoshi Yoshimura ( Kyushu University )
 *
 * << Date >>
 *		2007/06/15
 *
 ********************************************************************************/

Ulong gene_calc_imp_id_all_input_failure
(
 LINE *line
 ){
	
	Ulong max_imp_id = line->in[ 0 ]->imp_id_f;
	Ulong i; //カウンタ
	
	// char *func_name = "gene_calc_imp_id_all_input_normal"; 
	
	for ( i = 1 ; i < line->n_in ; i++ ) {
		if ( max_imp_id < line->in[ i ]->imp_id_f ) {
			max_imp_id = line->in[ i ]->imp_id_f;
		}
	}
	return ( max_imp_id );
}

/********************************************************************************
 * << Function >>
 *		enter line of implication in back trace tree
 *
 * << Function ID >>
 *	   	19-2
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type			description
 * line			I		LINE *			line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 * Gene_head	I/O		ATPG_HEAD	aptg head infomation
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/3/17
 *
 ********************************************************************************/

void gene_enter_imp_trace_normal
(
 LINE *line
 ){
#ifndef OLD_IMP2
	Gene_head.cnt_n++;

#ifndef OLD_IMP3
	Gene_head.last_n++;
#else	
	if ( Gene_head.last_n == FL_ULMAX ) {
		Gene_head.last_n = 0;
	}
	else {
		Gene_head.last_n++;
	}
#endif /* OLD_IMP3 */	
	Gene_head.imp_list_n[ Gene_head.last_n ] = line;
#endif /* OLD_IMP2 */

}

void gene_enter_imp_trace_failure
(
 LINE *line
 ){
#ifndef OLD_IMP2
	Gene_head.cnt_f++;
#ifndef OLD_IMP3
	Gene_head.last_f++;
#else	
	if ( Gene_head.last_f == FL_ULMAX ) {
		Gene_head.last_f = 0;
	}
	else {
		Gene_head.last_f++;
	}
#endif /* OLD_IMP3 */	
	Gene_head.imp_list_f[ Gene_head.last_f ] = line;
#endif /* OLD_IMP2 */
}

void atpg_enter_imp_trace
(
 LINE *line,
 CONDITION condition
 ){
#ifdef OLD_IMP2
	IMP_TRACE *imp_trace;

	char     *func_name = "atpg_enter_imp_trace"; /* 関数名 */

	/*
	topgun_print_mes( 22, 2 );
	topgun_print_mes_id_state9_0( line, line->state9, condition );
	*/

	imp_trace = (IMP_TRACE *)topgunMalloc( FMT_IMP_TRACE, sizeof( IMP_TRACE ),
										   1, func_name);

	imp_trace->condition = condition;
	imp_trace->line      = line;

	imp_trace->next = Gene_head.imp_trace;
	Gene_head.imp_trace = imp_trace;
#endif /* OLD_IMP2 */
}

/* End of File */
