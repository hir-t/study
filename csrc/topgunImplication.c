/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ް����˴ؤ���ؿ���
	
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

/* 2����XOR�δް����ؿ� */
void (* Xor_implication_out0[4][4])(IMP_RESULT_INFO *); /* out X-> 0 [in[0]][in[1]]?? */
void (* Xor_implication_out1[4][4])(IMP_RESULT_INFO *); /* out X-> 1 [in[0]][in[1]]?? */
void (* Xor_implication_in0 [4][4])(IMP_RESULT_INFO *); /* in  X-> 0 [ out ][ in  ]?? */
void (* Xor_implication_in1 [4][4])(IMP_RESULT_INFO *); /* in  X-> 1 [ out ][ in  ]?? */

/* 2����XNOR�δް����ؿ� */
void (* Xnor_implication_out0[4][4])(IMP_RESULT_INFO *); /* out X-> 0 [in[0]][in[1]]?? */
void (* Xnor_implication_out1[4][4])(IMP_RESULT_INFO *); /* out X-> 1 [in[0]][in[1]]?? */
void (* Xnor_implication_in0 [4][4])(IMP_RESULT_INFO *); /* in  X-> 0 [ out ][ in  ]?? */
void (* Xnor_implication_in1 [4][4])(IMP_RESULT_INFO *); /* in  X-> 1 [ out ][ in  ]?? */


extern   void implication_init ( void ); /* �ް����ؿ��ν���� */
extern   IMP_RESULT implication ( );  /* �ް����ؿ� */


int      atpg_check_state9_n_f( STATE_9 ); /* state�������ͤ��ξ��ͤΤ����줫��Ƚ�Ǥ��� */

void     atpg_enter_implication_list ( IMP_INFO * ); /* �ް����ꥹ�Ȥ���Ͽ���� */
void     atpg_enter_implication_list_input ( LINE * , STATE_3, Uint, Ulong, Ulong ); /* ���������ؤδް�����ް����ꥹ�Ȥ���Ͽ���� */
void     atpg_enter_implication_list_output (  LINE *, STATE_3, Uint, Ulong ); /* ���������شް����ꥹ�Ȥ���Ͽ���� */
Ulong    atpg_imp_make_input_pin_number( Ulong ); /* ���ϥԥ��ֹ��ǥ����ɤ��� */
STATE_3  atpg_invert_state3 ( STATE_3 ); /* ȿž����state��������� */
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
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9�ͤ��������ͤ���Ф� */
STATE_3 atpg_get_state9_2_failure_state3 ( STATE_9 ); /* 9�ͤ���ξ��ͤ���Ф� */
IMP_RESULT atpg_update_static_learn_state ( LEARN_LIST * );
IMP_RESULT atpg_update_indirect_state( LEARN_LIST *, Ulong );
void gene_learn_list_initalize( void ); /* ���ܴްդˤ��ް��� */

/* topgun_state.h�Υޥ��� */
//STATE_3 atpg_get_state9_with_condition_2_state3 ( STATE_9, Uint ); /* 9�ͤ���condition�ˤ�ä�3�ͤ���Ф� */


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

	//char *func_name = "implication"; /* �ؿ�̾ */

	topgun_4_utility_enter_start_time ( &Time_head.implication );

	topgun_print_mes( 24, 0 );

	
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0; //���粿���ܤ��ͳ����Ƥαƶ��򤦤�����

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-X-1 .imp�����ˤʤ�ޤǴް�����»�                           %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    /* $$$ �ް����򤹤���֤����(1st : first in, first out) $$$ */

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
		/* print�� */
		if ( imp_result.i_state == IMP_KEEP ) {
			topgun_print_mes( 24, 4 );
		}
	}
	/* CONF�ǽ�λ������? */
	if ( imp_result.i_state == IMP_CONF ) {
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-X-5 CONF�ǽ�λ�������θ����                             %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		Gene_head.n_enter_imp_list = 0;
	}

	topgun_4_utility_enter_end_time ( &Time_head.implication );
	return ( imp_result ); /* IMP_CONF �� IMP_KEEP �� IMP_UPDA */
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
	
	char        *func_name = "implication_init"; /* �ؿ�̾ */
	
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

	/* ���ܴް��� */
	gene_learn_list_initalize();

	
	/* Gene_head.imp_info_list�κ��� */
	Gene_head.n_imp_info_list = Line_info.n_line;
	
	Gene_head.imp_info_list = ( IMP_INFO * )topgunMalloc
		( FMT_IMP_INFO, sizeof( IMP_INFO ), Gene_head.n_imp_info_list, func_name );

	for ( i = 0 ; i < Gene_head.n_imp_info_list ; i++ ) {

		/* ����� */
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

	/* ����� */
	for ( i = 0 ; i < ( Line_info.n_line + 1 ) ; i++ ) {
		Gene_head.imp_list_n[ i ] = NULL;
	}
	for ( i = 0 ; i < ( Line_info.n_line + 1 ) ; i++ ) {
		Gene_head.imp_list_f[ i ] = NULL;
	}

	/* gene_imp_trace_xxx �ν���� */
	Switch_enter_imp_trace[ COND_NORMAL  ] = &gene_enter_imp_trace_normal;
	Switch_enter_imp_trace[ COND_FAILURE ] = &gene_enter_imp_trace_failure;

#ifdef TOPGUN_XOR_ON	
	Switch_implication[ TOPGUN_XOR  ] = &implication_xor;
	Switch_implication[ TOPGUN_XNOR ] = &implication_xnor;


	/* ���ʬ�� XOR 2������ */
	/* case 1: out  -> 0
	           case k: in0 �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
	           case c: in0 �� X�λ�, in1 �� 0�λ� -> in0 �� 0�˹���
			   case g: in0 �� X�λ�, in1 �� 1�λ� -> in0 �� 1�˹���
			   case o: in0 �� X�λ�, in1 �� U�λ� -> conf
			   case i: in0 �� 0�λ�, in1 �� X�λ� -> in1 �� 0�˹���
	           case a: in0 �� 0�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�
			   case e: in0 �� 0�λ�, in1 �� 1�λ� -> conf
			   case m: in0 �� 0�λ�, in1 �� U�λ� -> conf
			   case j: in0 �� 1�λ�, in1 �� X�λ� -> in0 �� 1�˹���
			   case b: in0 �� 1�λ�, in1 �� 0�λ� -> conf
			   case f: in0 �� 1�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�
			   case n: in0 �� 1�λ�, in1 �� U�λ� -> conf
			   case d: in0 �� U�λ�, in1 �� 0�λ� -> conf
			   case h: in0 �� U�λ�, in1 �� 1�λ� -> conf
			   case l: in0 �� U�λ�, in1 �� X�λ� -> conf
			   case p: in0 �� U�λ�, in1 �� U�λ� -> conf
	   case 2: out  -> 1
			   case k: in0 �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
			   case c: in0 �� X�λ�, in1 �� 0�λ� -> in0 �� 1�˹���
			   case g: in0 �� X�λ�, in1 �� 1�λ� -> in0 �� 0�˹���
			   case o: in0 �� X�λ�, in1 �� U�λ� -> conf
			   case i: in0 �� 0�λ�, in1 �� X�λ� -> in1 �� 1�˹���
			   case a: in0 �� 0�λ�, in1 �� 0�λ� -> conf
			   case e: in0 �� 0�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�
			   case m: in0 �� 0�λ�, in1 �� U�λ� -> conf
			   case j: in0 �� 1�λ�, in1 �� X�λ� -> in1 �� 0�˹���
			   case b: in0 �� 1�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�
			   case f: in0 �� 1�λ�, in1 �� 1�λ� -> conf
			   case n: in0 �� 1�λ�, in1 �� U�λ� -> conf
			   case d: in0 �� U�λ�, in1 �� 0�λ� -> conf
			   case h: in0 �� U�λ�, in1 �� 1�λ� -> conf
			   case l: in0 �� U�λ�, in1 �� X�λ� -> conf
			   case p: in0 �� U�λ�, in1 �� U�λ� -> conf
	   case 3: in -> 0
			   case k: out �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
			   case c: out �� X�λ�, in1 �� 0�λ� -> out �� 0�˹���
			   case g: out �� X�λ�, in1 �� 1�λ� -> out �� 1�˹���
			   case o: out �� X�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�			   
	           case i: out �� 0�λ�, in1 �� X�λ� -> in1 �� 0�˹���
	           case a: out �� 0�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�
	           case e: out �� 0�λ�, in1 �� 1�λ� -> conf
	           case m: out �� 0�λ�, in1 �� U�λ� -> conf			   			   
			   case j: out �� 1�λ�, in1 �� X�λ� -> in1 �� 1�˹���			   
			   case b: out �� 1�λ�, in1 �� 0�λ� -> conf
			   case f: out �� 1�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�			   
			   case n: out �� 1�λ�, in1 �� U�λ� -> conf
			   case l: out �� U�λ�, in1 �� X�λ� -> in1 �� U�˹���
			   case d: out �� U�λ�, in1 �� 0�λ� -> conf			   
			   case h: out �� U�λ�, in1 �� 1�λ� -> conf			   
			   case p: out �� U�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�
	   case 4: in0 -> 1

			   case k: out �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
			   case c: out �� X�λ�, in1 �� 0�λ� -> out �� 1�˹���
			   case g: out �� X�λ�, in1 �� 1�λ� -> out �� 0�˹���			   
			   case o: out �� X�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�
	           case i: out �� 0�λ�, in1 �� X�λ� -> in1 �� 1�˹���
			   case a: out �� 0�λ�, in1 �� 0�λ� -> conf
	           case e: out �� 0�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�
	           case m: out �� 0�λ�, in1 �� U�λ� -> conf
			   case j: out �� 1�λ�, in1 �� X�λ� -> in1 �� 0�˹���
			   case b: out �� 1�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�			   			   
			   case f: out �� 1�λ�, in1 �� 1�λ� -> conf
			   case n: out �� 1�λ�, in1 �� U�λ� -> conf			   
			   case l: out �� U�λ�, in1 �� X�λ� -> in1 �� U�˹���
			   case d: out �� U�λ�, in1 �� 0�λ� -> conf			   
			   case h: out �� U�λ�, in1 �� 1�λ� -> conf
			   case p: out �� U�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�

			   in_x��U�˹������줿�Ȥ�
			   -> out��ɬ��U�ˤʤ�
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

	/* ���ʬ�� XNOR 2������ */
	/* case 1: out  -> 0
			   case k: in0 �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
			   case c: in0 �� X�λ�, in1 �� 0�λ� -> in0 �� 1�˹���
			   case g: in0 �� X�λ�, in1 �� 1�λ� -> in0 �� 0�˹���
			   case o: in0 �� X�λ�, in1 �� U�λ� -> conf
			   case i: in0 �� 0�λ�, in1 �� X�λ� -> in1 �� 1�˹���
			   case a: in0 �� 0�λ�, in1 �� 0�λ� -> conf
			   case e: in0 �� 0�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�
			   case m: in0 �� 0�λ�, in1 �� U�λ� -> conf
			   case j: in0 �� 1�λ�, in1 �� X�λ� -> in1 �� 0�˹���
			   case b: in0 �� 1�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�
			   case f: in0 �� 1�λ�, in1 �� 1�λ� -> conf
			   case n: in0 �� 1�λ�, in1 �� U�λ� -> conf
			   case d: in0 �� U�λ�, in1 �� 0�λ� -> conf
			   case h: in0 �� U�λ�, in1 �� 1�λ� -> conf
			   case l: in0 �� U�λ�, in1 �� X�λ� -> conf
			   case p: in0 �� U�λ�, in1 �� U�λ� -> conf
	   case 2: out  -> 1
	           case k: in0 �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
	           case c: in0 �� X�λ�, in1 �� 0�λ� -> in0 �� 0�˹���
			   case g: in0 �� X�λ�, in1 �� 1�λ� -> in0 �� 1�˹���
			   case o: in0 �� X�λ�, in1 �� U�λ� -> conf
			   case i: in0 �� 0�λ�, in1 �� X�λ� -> in1 �� 0�˹���
	           case a: in0 �� 0�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�
			   case e: in0 �� 0�λ�, in1 �� 1�λ� -> conf
			   case m: in0 �� 0�λ�, in1 �� U�λ� -> conf
			   case j: in0 �� 1�λ�, in1 �� X�λ� -> in0 �� 1�˹���
			   case b: in0 �� 1�λ�, in1 �� 0�λ� -> conf
			   case f: in0 �� 1�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�
			   case n: in0 �� 1�λ�, in1 �� U�λ� -> conf
			   case d: in0 �� U�λ�, in1 �� 0�λ� -> conf
			   case h: in0 �� U�λ�, in1 �� 1�λ� -> conf
			   case l: in0 �� U�λ�, in1 �� X�λ� -> conf
			   case p: in0 �� U�λ�, in1 �� U�λ� -> conf
	   case 3: in -> 0
			   case k: out �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
			   case c: out �� X�λ�, in1 �� 0�λ� -> out �� 1�˹���
			   case g: out �� X�λ�, in1 �� 1�λ� -> out �� 0�˹���
			   case o: out �� X�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�			   
	           case i: out �� 0�λ�, in1 �� X�λ� -> in1 �� 1�˹���
	           case a: out �� 0�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�
	           case e: out �� 0�λ�, in1 �� 1�λ� -> conf
	           case m: out �� 0�λ�, in1 �� U�λ� -> conf			   			   
			   case j: out �� 1�λ�, in1 �� X�λ� -> in1 �� 0�˹���			   
			   case b: out �� 1�λ�, in1 �� 0�λ� -> conf
			   case f: out �� 1�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�			   
			   case n: out �� 1�λ�, in1 �� U�λ� -> conf
			   case l: out �� U�λ�, in1 �� X�λ� -> in1 �� U�˹���
			   case d: out �� U�λ�, in1 �� 0�λ� -> conf			   
			   case h: out �� U�λ�, in1 �� 1�λ� -> conf			   
			   case p: out �� U�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�
	   case 4: in0 -> 1
			   case k: out �� X�λ�, in1 �� X�λ� -> �ʤˤ⤷�ʤ�
			   case c: out �� X�λ�, in1 �� 0�λ� -> out �� 0�˹���
			   case g: out �� X�λ�, in1 �� 1�λ� -> out �� 1�˹���			   
			   case o: out �� X�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�
	           case i: out �� 0�λ�, in1 �� X�λ� -> in1 �� 0�˹���
			   case a: out �� 0�λ�, in1 �� 0�λ� -> conf
	           case e: out �� 0�λ�, in1 �� 1�λ� -> �ʤˤ⤷�ʤ�
	           case m: out �� 0�λ�, in1 �� U�λ� -> conf
			   case j: out �� 1�λ�, in1 �� X�λ� -> in1 �� 1�˹���
			   case b: out �� 1�λ�, in1 �� 0�λ� -> �ʤˤ⤷�ʤ�
			   case f: out �� 1�λ�, in1 �� 1�λ� -> conf
			   case n: out �� 1�λ�, in1 �� U�λ� -> conf			   
			   case l: out �� U�λ�, in1 �� X�λ� -> in1 �� U�˹���
			   case d: out �� U�λ�, in1 �� 0�λ� -> conf			   
			   case h: out �� U�λ�, in1 �� 1�λ� -> conf
			   case p: out �� U�λ�, in1 �� U�λ� -> �ʤˤ⤷�ʤ�

			   in_x��U�˹������줿�Ȥ�
			   -> out��ɬ��U�ˤʤ�
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
	IMP_INFO	*imp_info;  /* �ް����˴ؤ������ */

	char        *func_name = "atpg_select_implication"; /* �ؿ�̾ */

	/* ���ߤϺǸ������оݤȤ��� */
	imp_info   = Gene_head.imp_l;

	test_line_null ( imp_info->line, func_name );
	test_line_type ( imp_info->line, func_name );

	/* �ް����ꥹ�Ȥ˰�Ĥ�����Ͽ����Ƥ��ʤ���� */
	if ( Gene_head.imp_l == Gene_head.imp_f ) {
		/* imp_f��NULL�ˤ��� */
		Gene_head.imp_f = NULL;
		Gene_head.imp_l = NULL;
	}
	else{
		/* imp_l�������餹 */
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
	Ulong   i          = 0;  /* ������ */
	
	Uint    condition  = imp_info->condition; /* STAT_N or STAT_F */

	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* ���٤�Ʊ���ͤ�������å����� ON: Ʊ�� OFF: �ۤ��Τ����� */
	Ulong   one_check = ONE_FLAG_ON; /* ���٤�X��OK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)��¸�ߤ��뤫 */
	Ulong   one_pin   = FL_ULMAX;    /* one_check�ǤߤĤ������ϥԥ��ֹ� */

	Ulong   max_imp_id = 0; // imp_id
	Ulong   up_pin_id  = 0;

	LINE    *imp_line  = imp_info->line;

	char    *func_name = "implication_and"; /* �ؿ�̾ */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state  = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	imp_result.imp_id   = 0;
	
	
	/* ��� �Ȥꤢ����ver */
	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

	*/
	/* ���ʬ�� AND N������ */
	/* case 1: out  -> 0
	           case a: in�����٤�1�λ�->conf
	           case b: in�����X or U���Ĥꤹ�٤�1�λ�->X�����Ϥ�0�˹���
			   case c: ����ʳ��Ϥʤˤ⤷�ʤ�
	   case 2: out  -> 1
  	           case a: in�Τ����줫��0 or U �λ�->conf
			   case b: in�Τ����줫��X�λ�->X�����Ϥ�1�˹���
			   case c: ����ʳ��Ϥʤˤ⤷�ʤ�
	   case 3: in_x -> 0
  	           case a: out��1 or U�λ�->conf
			   case b: out��X�λ�->out��0�˹���
  	           case c: out��0�λ�->�ʤˤ⤷�ʤ�
	   case 4: in_x -> 1
   	           case a: out��1 or U�λ�->�ʤˤ⤷�ʤ�
			                            ( �ۤ���in��0�������� -> conf 
								          �̤δް�����ɬ�������å�����Ϥ�)
			   case b: out��0�λ�
			           case +: ¾�����Ϥ�0��1�ĤǤ�¸�ߤ�����  -> �ʤˤ⤷�ʤ�
			           case -: ¾�����Ϥ����٤�1�ξ��           -> conf
			           case *: ¾��1orU�ǰ�Ĥ���X��¸�ߤ����� -> ����X��0�˹���
			   case c: out��X�λ�
			           case +: �ۤ������Ϥ����٤�1�ξ�� -> out��1�˹���
					   case -: �ۤ������Ϥ����٤�1��U�ξ�� -> out��U�˹���
					   case *: 1��U�ʳ������Ϥ�¸�ߤ����� -> �ʤˤ⤷�ʤ�

	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_0 == imp_state3 ) {
			
			/* case 1 */

			/* ���Ϥξ��֤�Ĵ�� */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_0 == in_state3 ){
					/* ���Ϥ�0������ */
					/* case c ���� */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_1 == in_state3 ) {
					/* ���٤�̤���� */
				}
				else if ( STATE3_X == in_state3 ) {

					/* ���ʤ��Ȥ�case a�ǤϤʤ� */
					
					all_check = ALL_FLAG_OFF;
					if ( FL_ULMAX == one_pin ) {
						/* case b�β�ǽ������ */
						one_pin = i;
					}
					else {
						/* ʣ����X�����Ϥˤ��� */
						/* case c���� */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
 				else if ( STATE3_U == in_state3 ) {
					all_check = ALL_FLAG_OFF;
				}
			}
			/* Ĵ����λ!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default�� ) ���֤� */
				}
				else {
					/* case b */

					// max_imp_id�����
					// output 0 , one input X, the others 1 -> one input 0
					if ( condition == COND_NORMAL ) {
						max_imp_id = gene_calc_imp_id_no_input_i_normal
							( imp_line, one_pin );
					}
					else {
						max_imp_id = gene_calc_imp_id_no_input_i_failure
							( imp_line, one_pin );
					}
					
					// �ͤι��������ް����ꥹ�Ȥ���Ͽ����
					imp_result = implication_update_input
						( imp_line, one_pin, STATE3_0 , condition, max_imp_id );
				}
			}
			else {
				/* case a */

				/* IMP_CONF ���֤� */
				imp_result.i_state = IMP_CONF;

                // conf���װ��Ȥʤ�ľ����ͳ�����Ƥ����
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
					/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */

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
				
				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
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
				/* �ް����ν��֤��Ѳ�����Τǡ��ʤ��Ϥʤ� */
				/* �̤δް������ưפ�ȯ���Ǥ��뤿�ᡢ�����פ�ʤ��Ϥ� */
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

					/* ���Ϥξ��֤�Ĵ�� */
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
							/* case b�β�ǽ������ */
							one_pin = i;
						}
						else {
							/* ʣ����X�����Ϥˤ��� */
							/* case c���� */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						all_check = ALL_FLAG_OFF;
					}
				}
				/* Ĵ����λ!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + */
						/* IMP_KEEP ( default�� ) ���֤� */
					}
					else {
						/* case * */
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */

						// one_pin�ʳ��Τ��٤Ƥ�imp_id�κ����ͤ�max_imp_id�Ȥ���
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

					/* IMP_CONF ���֤� */
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
					
					/* ���Ϥξ��֤�Ĵ�� */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;
					
					if ( STATE3_1 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknown������ */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* Ĵ����λ!! */
				if ( ALL_FLAG_OFF != all_check ) {
					if ( UNK_FLAG_ON == unk_check ) {
						/* case - */
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal ( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure ( imp_line );
						}
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_output
							( imp_line, STATE3_U , condition, max_imp_id );
					}
					else {
						/* case + */
						// ���٤Ƥ�input���Ф��ơ�imp_id�κ����ͤ�max_imp_id�Ȥ���
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
					/* IMP_KEEP ( default�� ) ���֤� */
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

	Ulong   i          = 0;  /* ������ */
	
	Uint    condition  = imp_info->condition; /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* ���٤�Ʊ���ͤ�������å����� ON: Ʊ�� OFF: �ۤ��Τ����� */
	Ulong   one_check = ONE_FLAG_ON; /* ���٤�X��OK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)��¸�ߤ��뤫 */
	Ulong   one_pin   = FL_ULMAX;    /* one_check�ǤߤĤ������ϥԥ��ֹ� */

	Ulong   max_imp_id = 0;
	Ulong   up_pin_id  = 0;

	LINE    *imp_line  = imp_info->line;

	char    *func_name = "implication_or"; /* �ؿ�̾ */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );


	// initail imp_result
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id  = 0;
	

	/* ��� �Ȥꤢ����ver */

	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

	*/
	/* ���ʬ�� OR N������ */
	/* case 1: out  -> 1
	           case a: in�����٤�0�λ�->conf
	           case b: in�����X���Ĥꤹ�٤�0 or U�λ�->X�����Ϥ�1�˹���
			   case c: ����ʳ��Ϥʤˤ⤷�ʤ�
	   case 2: out  -> 0
  	           case a: in�Τ����줫��1 or U �λ�->conf
			   case b: in�Τ����줫��X�λ�->X�����Ϥ�0�˹���
			   case c: ����ʳ��Ϥʤˤ⤷�ʤ�
	   case 3: in_x -> 1
  	           case a: out��0 or U�λ�->conf
			   case b: out��X�λ�->out��1�˹���
  	           case c: out��1�λ�->�ʤˤ⤷�ʤ�
	   case 4: in_x -> 0
   	           case a: out��0 or U�λ�->�ʤˤ⤷�ʤ�
			                            ( �ۤ���in��1�������� -> conf 
								          �̤δް�����ɬ�������å�����Ϥ�)
			   case b: out��1�λ�
			           case +: ¾�����Ϥ�1��1�ĤǤ�¸�ߤ�����  -> �ʤˤ⤷�ʤ�
			           case -: ¾�����Ϥ����٤�0�ξ��           -> conf
			           case *: ¾��0orU�ǰ�Ĥ���X��¸�ߤ����� -> ����X��1�˹���
			   case c: out��X�λ�
			           case +: �ۤ������Ϥ����٤�0�ξ�� -> out��0�˹���
					   case -: �ۤ������Ϥ����٤�0��U�ξ�� -> out��U�˹���
					   case *: 0��U�ʳ������Ϥ�¸�ߤ����� -> �ʤˤ⤷�ʤ�
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_1 == imp_state3 ) {
			
			/* case 1 */

			/* ���Ϥξ��֤�Ĵ�� */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9,  condition );

				if ( STATE3_1 == in_state3 ){
					/* ���Ϥ�1������ */
					/* case c ���� */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_0 == in_state3 ) {
					
					/* ���٤�̤���� */
					
				}
				else if ( STATE3_X == in_state3 ) {
					
					/* ���ʤ��Ȥ�case a�ǤϤʤ� */
					all_check = ALL_FLAG_OFF;
					
					if ( FL_ULMAX == one_pin ) {
						
						/* case b�β�ǽ������ */
						one_pin = i;
						
					}
					else {
						
						/* ʣ����X�����Ϥˤ��� */
						/* case c���� */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
				else {
					/* ���֤���֤��ʤ� */
					/*  STATE3_U == in_stat */
					all_check = ALL_FLAG_OFF;
				}
			}
			/* Ĵ����λ!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default�� ) ���֤� */
				}
				else {
					/* case b */

					/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
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

				/* ����ȯ�� -> IMP_CONF ���֤� */
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

					/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
					imp_result = implication_update_input 
						( imp_line, i, STATE3_0 , condition, max_imp_id );
					
				}
				else if ( ( STATE3_1 == in_state3 ) ||
						  ( STATE3_U == in_state3 ) ) {
					/* case a */

					/* ����ȯ�� -> IMP_CONF ���֤� */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
					break;
				}
			}
			/* case c */
			
			/* �����ʤ� -> IMP_KEEP ( default�� ) ���֤� */
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
				
				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
				imp_result = implication_update_output
					( imp_line, STATE3_1, condition, max_imp_id );

			}
			else if ( STATE3_1 == out_state3 ) {
				/* case c */

				/* �����ʤ� -> IMP_KEEP ( default�� ) ���֤� */
			}
			else {
				/* case a */

				/* ����ȯ�� -> IMP_CONF ���֤� */
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
				/* �̤δް������ưפ�ȯ���Ǥ��뤿�ᡢ�����פ�ʤ��Ϥ� */
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

					/* ���Ϥξ��֤�Ĵ�� */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_1 == in_state3 ) {

						/* case +���� */
						all_check = ALL_FLAG_OFF;
						one_check = ONE_FLAG_OFF;
						break;
					}
					else if ( STATE3_X == in_state3 ) {
						all_check = ALL_FLAG_OFF;
						if ( FL_ULMAX == one_pin ) {
							/* case *�β�ǽ������ */
							one_pin = i;
						}
						else {
							/* ʣ����X�����Ϥˤ��� */
							/* case +���� */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						all_check = ALL_FLAG_OFF;
					}
				}
				/* Ĵ����λ!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + */
						/* IMP_KEEP ( default�� ) ���֤� */
					}
					else {
						/* case * */
						/* �ͤι��� */

						// one_pin�ʳ��Τ��٤Ƥ�imp_id�κ����ͤ�max_imp_id�Ȥ���
						// output 0 , one input X, the others 1 -> one input 0
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_no_input_i_normal
								( imp_line, one_pin );
						}
						else {
							max_imp_id = gene_calc_imp_id_no_input_i_failure
								( imp_line, one_pin );
						}
						
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						implication_update_input 
							( imp_line, one_pin, STATE3_1, condition, max_imp_id );

					}
				}
				else {
					/* case - */

					/* ����ȯ�� -> IMP_CONF ���֤� */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
				}
			}
			else {
				/* case c */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {
					/* ���Ϥξ��֤�Ĵ�� */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_0 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {
							/* case *���� */
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknown������ */
							/* case - or * */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* Ĵ����λ!! */
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
						
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_output
							( imp_line, STATE3_U , condition, max_imp_id );
					}
					else {
						/* case + */
						// ���٤Ƥ�input���Ф��ơ�imp_id�κ����ͤ�max_imp_id�Ȥ���
						// output X , all input 0 -> output 0
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_output
							( imp_line, STATE3_0 , condition, max_imp_id );
					}
				}
				else {
					/* case - */
					/* IMP_KEEP ( default�� ) ���֤� */
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

	Ulong   i          = 0;  /* ������ */
	
	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* ���٤�Ʊ���ͤ�������å����� ON: Ʊ�� OFF: �ۤ��Τ����� */
	Ulong   one_check = ONE_FLAG_ON; /* ���٤�X��OK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)��¸�ߤ��뤫 */
	Ulong   one_pin   = FL_ULMAX;    /* one_check�ǤߤĤ������ϥԥ��ֹ� */

	Ulong   max_imp_id = 0;
	Ulong   up_pin_id  = 0;

	
	LINE    *imp_line  = imp_info->line;

	static char *func_name = "implication_nand"; /* �ؿ�̾ */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id  = 0;

	/* ��� �Ȥꤢ����ver */

	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

	*/
	/* ���ʬ�� NAND N������ */
	/* case 1: out  -> 1
	           case a: in�����٤�1�λ�->conf
	           case b: in�����X or U���Ĥꤹ�٤�1�λ�->X�����Ϥ�0�˹���
			   case c: ����ʳ��Ϥʤˤ⤷�ʤ�
	   case 2: out  -> 0
  	           case a: in�Τ����줫��0 or U �λ�->conf
			   case b: in�Τ����줫��X�λ�->X�����Ϥ�1�˹���
			   case c: ����ʳ��Ϥʤˤ⤷�ʤ�
	   case 3: in_x -> 0
  	           case a: out��0 or U�λ�->conf
			   case b: out��X�λ�->out��1�˹���
  	           case c: out��1�λ�->�ʤˤ⤷�ʤ�
	   case 4: in_x -> 1
   	           case a: out��0 or U�λ�->�ʤˤ⤷�ʤ�
			                            ( �ۤ���in��0 or U�������� -> conf 
								          �̤δް�����ɬ�������å�����Ϥ�)
			   case b: out��1�λ�
			           case +: ¾�����Ϥ�0��1�ĤǤ�¸�ߤ�����  -> �ʤˤ⤷�ʤ�
			           case -: ¾�����Ϥ����٤�1�ξ��           -> conf
			           case *: ¾��1orU�ǰ�Ĥ���X��¸�ߤ����� -> ����X��0�˹���
			           case /: X��ʣ��¸�ߤ�����               -> �ʤˤ⤷�ʤ�
			   case c: out��X�λ�
			           case +: �ۤ������Ϥ����٤�1�ξ��    -> out��0�˹���
					   case -: �ۤ������Ϥ����٤�1��U�ξ�� -> out��U�˹���
					   case *: 1��U�ʳ������Ϥ�¸�ߤ����� -> �ʤˤ⤷�ʤ�
					   
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_1 == imp_state3 ) {
			
			/* case 1 */

			/* ���Ϥξ��֤�Ĵ�� */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_0 == in_state3 ){
					/* ���Ϥ�0������ */
					/* case c ���� */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_1 == in_state3 ) {
					/* ���٤�̤���� */
				}
				else if ( STATE3_X == in_state3 ) {
					/* ���ʤ��Ȥ�case a�ǤϤʤ� */
					all_check = ALL_FLAG_OFF;
					if ( FL_ULMAX == one_pin ) {
						/* case b�β�ǽ������ */
						one_pin = i;
					}
					else {
						/* ʣ����X�����Ϥˤ��� */
						/* case c���� */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
				else if ( STATE3_U == in_state3 ) {
					all_check = ALL_FLAG_OFF;
				}
			}
			/* Ĵ����λ!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default�� ) ���֤� */
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
					/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
					imp_result = implication_update_input 
						( imp_line, one_pin, STATE3_0 , condition, max_imp_id );
				}
			}
			else {
				/* case a */

				/* IMP_CONF ���֤� */
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
					/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
					imp_result = implication_update_input 
						( imp_line, i, STATE3_1 , condition, max_imp_id );
					
				}
				else if ( ( STATE3_0 == in_state3 ) ||
						  ( STATE3_U == in_state3 ) ) {
					/* case a */

					/* IMP_CONF ���֤� */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
					break;
				}
				else {
					/* case c */
					
					/* IMP_KEEP ( default�� ) ���֤� */
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
				
				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
				imp_result = implication_update_output
					( imp_line, STATE3_1 , condition, max_imp_id );
				
			}
			else if ( STATE3_1 == out_state3 ) {
				/* case c */
				
				/* IMP_KEEP ( default�� ) ���֤� */
			}
			else {
				/* case a */

				/* IMP_CONF ���֤� */
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
				/* �̤δް������ưפ�ȯ���Ǥ��뤿�ᡢ�����פ�ʤ��Ϥ� */
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

					/* ���Ϥξ��֤�Ĵ�� */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_0 == in_state3 ) {
						/* case + ���� */
						all_check = ALL_FLAG_OFF;
						one_check = ONE_FLAG_OFF;
						break;
					}
					else if ( STATE3_X == in_state3 ) {
						all_check = ALL_FLAG_OFF;
						if ( FL_ULMAX == one_pin ) {
							/* case - �β�ǽ������ */
							one_pin = i;
						}
						else {
							/* ʣ����X�����Ϥˤ��� */
							/* case  / ���� */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						all_check = ALL_FLAG_OFF;
					}
				}
				/* Ĵ����λ!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + or / */
						/* IMP_KEEP ( default�� ) ���֤� */
					}
					else {
						/* case * */

						// one_pin�ʳ��Τ��٤Ƥ�imp_id�κ����ͤ�max_imp_id�Ȥ���
						// output 0 , one input X, the others 1 -> one input 0
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_no_input_i_normal
								( imp_line, one_pin );
						}
						else {
							max_imp_id = gene_calc_imp_id_no_input_i_failure
								( imp_line, one_pin );
						}
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_input
							( imp_line, one_pin, STATE3_0 , condition, max_imp_id );
						
					}	
				}
				else {
					/* case - */

					/* IMP_CONF ���֤� */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
				}
			}
			else {
				/* case c */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {

					test_line_null ( imp_line->in[ i ], func_name );
					
					/* ���Ϥξ��֤�Ĵ�� */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					/* ����Ȥ���0�Ϥʤ�
					   �����㳰Ū�ˤ��뤬�����Ӵް�����Ԥ� */
					if ( STATE3_1 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {

							/* case * ���� */
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknown������ */

							/* case - or * */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* Ĵ����λ!! */
				if ( ALL_FLAG_OFF != all_check ) {
					if ( UNK_FLAG_ON == unk_check ) {
						/* case - */

						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
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
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_output
							( imp_line, STATE3_0 , condition, max_imp_id  );

					}
				}
				else {
					/* case * */
					/* IMP_KEEP ( default�� ) ���֤� */
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

	Ulong   i          = 0;  /* ������ */
	
	Uint    condition  = imp_info->condition;  /* STAT_N or STAT_F */
	
	STATE_3 imp_state3 = imp_info->new_state3;
	STATE_3 in_state3;
	STATE_3 out_state3;

	Ulong   all_check = ALL_FLAG_ON; /* ���٤�Ʊ���ͤ�������å����� ON: Ʊ�� OFF: �ۤ��Τ����� */
	Ulong   one_check = ONE_FLAG_ON; /* ���٤�X��OK */
	Ulong   unk_check = UNK_FLAG_OFF;/* U(unknown)��¸�ߤ��뤫 */
	Ulong   one_pin   = FL_ULMAX;    /* one_check�ǤߤĤ������ϥԥ��ֹ� */

	Ulong   max_imp_id = 0;
	Ulong   up_pin_id  = 0;
	
	LINE    *imp_line  = imp_info->line;

	//char    *func_name = "implication_nor"; /* �ؿ�̾ */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state  = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id = 0;

	
	/* ��� �Ȥꤢ����ver */

	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

	*/
	/* ���ʬ�� NOR N������ */
	/* case 1: out  -> 0
	           case a: in�����٤�0�λ�                 -> conf
	           case b: in�����X���Ĥꤹ�٤�0 or U�λ� -> X�����Ϥ�1�˹���
			   case c: ����ʳ�                        -> �ʤˤ⤷�ʤ�
	   case 2: out  -> 1
  	           case a: in�Τ����줫��1 or U �λ�       -> conf
			   case b: in�Τ����줫��X�λ�             -> X�����Ϥ�0�˹���
			   case c: ����ʳ�                        -> �ʤˤ⤷�ʤ�
	   case 3: in_x -> 1
  	           case a: out��1 or U�λ�                 -> conf
			   case b: out��X�λ�                      -> out��0�˹���
  	           case c: out��0�λ�                      -> �ʤˤ⤷�ʤ�
	   case 4: in_x -> 0
   	           case a: out��1 or U�λ�->�ʤˤ⤷�ʤ�
			                            ( �ۤ���in��1�������� -> conf(�����ǽ�����Ω����櫓�ǤϤʤ�) 
								          �̤δް�����ɬ�������å�����Ϥ�)
			   case b: out��0�λ�
			           case +: ¾�����Ϥ�1��1�ĤǤ�¸�ߤ�����  -> �ʤˤ⤷�ʤ�
			           case -: ¾�����Ϥ����٤�0�ξ��           -> conf
			           case *: ¾��0orU�ǰ�Ĥ���X��¸�ߤ����� -> ����X��1�˹���
			           case /: X��ʣ��¸�ߤ�����               -> �ʤˤ⤷�ʤ�					   
			   case c: out��X�λ�
			           case +: �ۤ������Ϥ����٤�0�ξ��    -> out��1�˹���
					   case -: �ۤ������Ϥ����٤�0��U�ξ�� -> out��U�˹���
					   case *: 0��U�ʳ������Ϥ�¸�ߤ����� -> �ʤˤ⤷�ʤ�
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */
		if ( STATE3_0 == imp_state3 ) {
			
			/* case 1 */

			/* ���Ϥξ��֤�Ĵ�� */
			for ( i = 0 ; i < imp_line->n_in ; i++ ) {
				in_state3 = atpg_get_state9_with_condition_2_state3
					( imp_line->in[ i ]->state9, condition );

				if ( STATE3_1 == in_state3 ){
					/* ���Ϥ�1������ */
					/* case c ���� */
					all_check = ALL_FLAG_OFF;
					one_check = ONE_FLAG_OFF;
					break;
				}
				else if ( STATE3_0 == in_state3 ) {
					/* ���٤�̤���� */
				}
				else if ( STATE3_X == in_state3 ) {
					/* ���ʤ��Ȥ�case a�ǤϤʤ� */
					all_check = ALL_FLAG_OFF;
					if ( FL_ULMAX == one_pin ) {
						/* case b�β�ǽ������ */
						one_pin = i;
					}
					else {
						/* ʣ����X�����Ϥˤ��� */
						/* case c���� */
						one_check = ONE_FLAG_OFF;
						break;
					}
				}
				else if ( STATE3_U == in_state3 ) {
					all_check = ALL_FLAG_OFF;
				}
			}
			/* Ĵ����λ!! */
			if ( ALL_FLAG_OFF == all_check ) {
				if ( ONE_FLAG_OFF == one_check ) {
					/* case c */
					
					/* IMP_KEEP ( default�� ) ���֤� */
				}
				else {
					/* case b */
					
					// max_imp_id�����
					// output 0 , one input X, the others 1 -> one input 0
					if ( condition == COND_NORMAL ) {
						max_imp_id = gene_calc_imp_id_no_input_i_normal
							( imp_line, one_pin );
					}
					else {
						max_imp_id = gene_calc_imp_id_no_input_i_failure
							( imp_line, one_pin );
					}
					
					// �ͤι��������ް����ꥹ�Ȥ���Ͽ����
					imp_result = implication_update_input 
						( imp_line, one_pin, STATE3_1 , condition, max_imp_id );
					
				}
			}
			else {
				/* case a */

				/* IMP_CONF ���֤� */
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

					// ���׻�����ɬ�פϤʤ�$$$
					// output 1 -> all inputs 0 */
					if ( condition == COND_NORMAL ) {
						max_imp_id = imp_line->imp_id_n;
					}
					else {
						max_imp_id = imp_line->imp_id_f;
					}
					
					/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */					
					imp_result = implication_update_input 
						( imp_line, i, STATE3_0 , condition, max_imp_id );
					
				}
				else if ( ( STATE3_1 == in_state3 ) ||
						  ( STATE3_U == in_state3 ) ) {
					/* case a */

					/* IMP_CONF ���֤� */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
					break;
				}
				else {
					/* case c */

					/* IMP_KEEP ( default�� ) ���֤� */
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
				
				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
				imp_result = implication_update_output
					( imp_line, STATE3_0 , condition, max_imp_id );
				
			}
			else if ( STATE3_0 == out_state3 ) {
				/* case c */

				/* IMP_KEEP ( default�� ) ���֤� */
			}
			else {
				/* case a */

				/* IMP_CONF ���֤� */
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
				/* �̤δް������ưפ�ȯ���Ǥ��뤿�ᡢ�����פ�ʤ��Ϥ� */
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

					/* ���Ϥξ��֤�Ĵ�� */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_1 == in_state3 ) {
						/* case + ���� */
						all_check = ALL_FLAG_OFF;
						one_check = ONE_FLAG_OFF;
						break;
					}
					else if ( STATE3_X == in_state3 ) {
						/* case -�β�ǽ���ʤ� */
						all_check = ALL_FLAG_OFF;
						if ( FL_ULMAX == one_pin ) {
							/* case *�β�ǽ������ */
							one_pin = i;
						}
						else {
							/* ʣ����X�����Ϥˤ��� */
							/* case / ���� */
							one_check = ONE_FLAG_OFF;
							break;
						}
					}
					else if ( STATE3_U == in_state3 ) {
						/* case -�β�ǽ���ʤ� */
						all_check = ALL_FLAG_OFF;
					}
				}
				/* Ĵ����λ!! */
				if ( ALL_FLAG_OFF == all_check ) {
					if ( ONE_FLAG_OFF == one_check ) {
						/* case + */
						/* IMP_KEEP ( default�� ) ���֤� */
					}
					else {
						/* case * */

						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_input 
							( imp_line, one_pin, STATE3_1 , condition, max_imp_id );

					}	
				}
				else {
					/* case - */

					/* IMP_CONF ���֤� */
					imp_result.i_state = IMP_CONF;
					//$$$ imp_result.imp_id = !!;
				}
			}
			else {
				/* case c */
				for ( i = 0 ; i < imp_line->n_in ; i++ ) {
					/* ���Ϥξ��֤�Ĵ�� */
					in_state3 = atpg_get_state9_with_condition_2_state3
						( imp_line->in[ i ]->state9, condition ) ;

					if ( STATE3_0 != in_state3 ) {
						if ( STATE3_U != in_state3 ) {
							/* case * ���� */
							all_check = ALL_FLAG_OFF;
							break;
						}
						else {
							/* unknown������ */
							/* case +�β�ǽ���ʤ� */
							unk_check = UNK_FLAG_ON;
						}
					}
				}
				/* Ĵ����λ!! */
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
						
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_output
							( imp_line, STATE3_U , condition, max_imp_id );
					}
					else {
						/* case + */

						// ���٤Ƥ�input���Ф��ơ�imp_id�κ����ͤ�max_imp_id�Ȥ���
						// output X , all input 1 -> output 1
						if ( condition == COND_NORMAL ) {
							max_imp_id = gene_calc_imp_id_all_input_normal
								( imp_line );
						}
						else {
							max_imp_id = gene_calc_imp_id_all_input_failure
								( imp_line );
						}
						
						/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
						imp_result = implication_update_output
							( imp_line, STATE3_1 , condition, max_imp_id );
					}
				}
				else {
					/* case - */
					
					/* IMP_KEEP ( default�� ) ���֤� */
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
	*//* �ؿ�̾ */

	/* ��������� ?? */
	/* �������ͤ��Ѳ��Ϥ��äƤ�ް����Ǥ�����¦������Ѳ��Ϥ��ꤨ�ʤ� */
	/* ����¦�ؤΥȥ졼���ǹԤ�졢PI���Τ�ΤǤϹԤ�ʤ� */
	/* *** �����κݤν���¦�ްդϲ���Ǥ��� *** */

	/* ʣ�����Ϥξ��ȳ������Ϥ��Τ�Τ��ͤ������Ƥ�����
	   �ް����򤹤�ɬ���������� */


	/* line��state�ˤ���ް�����ް����ꥹ�Ȥ���Ͽ���� */
	/* @@@@@
	atpg_enter_implication_list_output
		( imp_info->line, imp_info->new_state3 );
	*/

	/* �ް�����Ͽ���� */
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
	
	//char *func_name = "implication_one_input"; /* �ؿ�̾ */

	in_state3  = STATE3_C;
	out_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );
	
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id  = 0;

	/* ��� �Ȥꤢ����ver */

	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

	*/
	/* ���ʬ�� �̲ᥲ���� ������ */
	/* case 1: out  -> 0
	           case a: in��0      -> �ʤˤ⤷�ʤ� 
	           case b: in��X      -> 0�˹���
			   case c: in��1 or U -> conf
	   case 2: out  -> 1
  	           case a: in��1      -> �ʤˤ⤷�ʤ� 
			   case b: in��X      -> 1�˹���
			   case c: in��0 or U -> conf
	   case 3: in   -> 0
  	           case a: out��1 or U�λ� -> conf
			   case b: out��X          -> out��0�˹���
  	           case c: out��0          -> �ʤˤ⤷�ʤ�
	   case 4: in_x -> 1
   	           case a: out��0 or U�λ� -> conf 
			   case b: out��X�λ�      -> out��1�˹���
			   case c: out��1�λ�      -> �ʤˤ⤷�ʤ�
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

		/* buf�Τ���in[0]����Ǥ� (bus�ξ��������) */
		in_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition );

		if ( STATE3_X == in_state3 ) {
			/* case b */
			
			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			imp_result = implication_update_input
				( imp_line, 0, imp_state3 , condition, max_imp_id );
			
		}
		else if ( imp_state3  == in_state3  ) {
			/* case a */
			/* ���⤷�ʤ� */
			/* IMP_KEEP ( default�� ) ���֤� */
			imp_result.i_state = IMP_KEEP;
		}
		else {
			/* case c */
			/* ���� */
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

			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			imp_result = implication_update_output
				( imp_line, imp_state3 , condition, max_imp_id );
			
		}
		else if ( imp_state3  == out_state3  ) {
			/* case a */
			/* ���⤷�ʤ� */
			/* IMP_KEEP ( default�� ) ���֤� */
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

	//char *func_name = "implication_inv"; /* �ؿ�̾ */
	
	in_state3     = STATE3_C;
	out_state3    = STATE3_C;
	update_state3 = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	// initail imp_result
	imp_result.i_state = IMP_KEEP; /* IMP_KEEP or IMP_CONF */
	//imp_result.imp_id = 0;

	/* ��� �Ȥꤢ����ver */

	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

	*/
	/* ���ʬ�� INV ������ */
	/* case 1: out  -> 0
	           case a: in��1      -> �ʤˤ⤷�ʤ� 
	           case b: in��X      -> 1�˹���
			   case c: in��0 or U -> conf
	   case 2: out  -> 1
  	           case a: in��0      -> �ʤˤ⤷�ʤ� 
			   case b: in��X      -> 0�˹���
			   case c: in��1 or U -> conf
	   case 3: in   -> 0
  	           case a: out��0 or U�λ� -> conf
			   case b: out��X          -> out��1�˹���
  	           case c: out��1          -> �ʤˤ⤷�ʤ�
	   case 4: in_x -> 1
   	           case a: out��1 or U�λ� -> conf 
			   case b: out��X�λ�      -> out��0�˹���
			   case c: out��1�λ�      -> �ʤˤ⤷�ʤ�
	*/


	/* ���������ͤ�ȿž�ͤ���� */
	update_state3 = atpg_invert_state3 ( imp_state3 );
	
	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */

		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->imp_id_n;
		}
		else {
			max_imp_id = imp_line->imp_id_f;
		}
		

		/* inv�Τ���in[0]����Ǥ� (bus�ξ��������) */
		in_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition );

		
		if ( STATE3_X == in_state3 ) {
			/* case b */
			
			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			imp_result = implication_update_input
				( imp_line, 0, update_state3, condition, max_imp_id );
		}
		else if ( update_state3 == in_state3 ) {
			/* case a */
			/* ���⤷�ʤ� */
			imp_result.i_state = IMP_KEEP;
		}
		else {
			/* case c */
			/* ���� */
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

			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			imp_result = implication_update_output
				( imp_line, update_state3, condition, max_imp_id );
		}
		else if ( update_state3 == out_state3 ) {
			/* case a */
			/* ���⤷�ʤ� */
			/* IMP_KEEP ( default�� ) ���֤� */
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

	/* $$$ ���Ȥǹͤ��� $$$ */

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
	
	//char *func_name = "implication_blk"; /* �ؿ�̾ */
		
	in_state3     = STATE3_C;
	out_state3    = STATE3_C;

	topgun_test_line_null ( imp_line, func_name );

	/* ��� �Ȥꤢ����ver */

	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

	*/
	/* ���ʬ�� PO ������ */
	/* case 1: out  -> 0
	           case a: in��0      -> �ʤˤ⤷�ʤ� 
	           case b: in��X      -> 0�˹���
			   case c: in��1 or U -> conf
	   case 2: out  -> 1
  	           case a: in��1      -> �ʤˤ⤷�ʤ� 
			   case b: in��X      -> 1�˹���
			   case c: in��0 or U -> conf
	   case 3: in   -> 0
  	           case a: out��1 or U�λ� -> conf
			   case b: out��X          -> out��0�˹���
  	           case c: out��0          -> �ʤˤ⤷�ʤ�
	   case 4: in_x -> 1
   	           case a: out��0 or U�λ� -> conf 
			   case b: out��X�λ�      -> out��1�˹���
			   case c: out��1�λ�      -> �ʤˤ⤷�ʤ�
	*/

	if ( OUT_PIN_NUM == imp_info->pin_num ) {
		/* case 1 or case 2 */

		if ( condition == COND_NORMAL ) {
			max_imp_id = imp_line->imp_id_n;
		}
		else {
			max_imp_id = imp_line->imp_id_f;
		}
        
		/* buf�Τ���in[0]����Ǥ� (bus�ξ��������) */
		in_state3 = atpg_get_state9_with_condition_2_state3
			( imp_line->in[ 0 ]->state9, condition );

		if ( STATE3_X == in_state3 ) {
			/* case b */

			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			result = implication_update_input ( imp_line, 0, STATE3_U, condition, max_imp_id );
		}
		else if ( in_state3 == imp_state3 ) {
			/* case a */
			/* ���⤷�ʤ� */
		}
		else {
			/* case c */
			/* ���� */
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
			
			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			result = implication_update_output ( imp_line, STATE3_U, condition, max_imp_id );

		}
		else if ( in_state3 == out_state3 ) {
			/* case a */
			/* ���⤷�ʤ� */
			/* IMP_KEEP ( default�� ) ���֤� */
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

	//char    *func_name = "implication_xor"; /* �ؿ�̾ */

	in0_state3 = STATE3_C;
	in1_state3 = STATE3_C;
	ino_state3 = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );

	/* ��� �Ȥꤢ����ver */

	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

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

		/* �ް�����̤�Ƚ�� */
		if ( imp_result_info.result == IMP_UPDA ) {

			up_pin_id  = atpg_implication_pin_num_2_pin_id ( imp_result_info.pin_num );
			
			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			result = implication_update_input
				( imp_line, up_pin_id, imp_result_info.state3, condition );
			
		}
		else {
			/* ���⤷�ʤ�(IMP_KEEP) or ����(IMP_CONF) */
			return ( imp_result_info.result );
		}
	}
	else {
		/* case3 or case4 */

		/* ��������pin id����� */
		in = atpg_implication_pin_num_2_pin_id ( imp_info->pin_num );

		/* �������ʤ��ä�pin id����� */
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
			/* �͹��� (IMP_UPDA) */
			
			if ( OUT_PIN_NUM == imp_result_info.pin_num ) {
				/* ���������Ϥξ�� */
				
				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
				result = implication_update_output
					( imp_line, imp_result_info.state3, condition );
				
			}
			else {
				/* ���������Ϥξ�� */

				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
				result = implication_update_input
					( imp_line, other, imp_result_info.state3, condition );
			}

		}
		else {
			/* ���⤷�ʤ�(IMP_KEEP) or ����(IMP_CONF) */
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

	//char    *func_name = "implication_xnor"; /* �ؿ�̾ */

	in0_state3 = STATE3_C;
	in1_state3 = STATE3_C;
	ino_state3 = STATE3_C;
	out_state3 = STATE3_C;

	test_line_null ( imp_line, func_name );
	
	/* 
	   1. new_state������å�����
	      -> ���Τ� or �ΤΤ� or ����ξ��(���뤫����->�Ȥꤢ����NG)
	   2. pin��ǥ����ɤ���
	   3. �ް����η�̤�����
	   4. 3.���Ѳ������ͤ�������ϡ�stack���Ѥߡ���λ�η�̤��֤�
	   5. 3.�Ǿ��ͤ����Ф����Ͼ��ͤη�̤��֤�

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

		/* �ް�����̤�Ƚ�� */
		if ( imp_result_info.result == IMP_UPDA ) {

			up_pin_id  = atpg_implication_pin_id_2_pin_num ( imp_result_info.pin_num );
			
			/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
			result = implication_update_input
				( imp_line, up_pin_id, imp_result_info.state3, condition );

		}
		else {
			/* ���⤷�ʤ�(IMP_KEEP) or ����(IMP_CONF) */
			return ( imp_result_info.result );
		}
	}
	else {
		/* case3 or case4 */

		/* ��������pin id����� */
		in = atpg_implication_pin_num_2_pin_id ( imp_info->pin_num );

		/* �������ʤ��ä�pin id����� */
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
			/* �͹��� (IMP_UPDA) */
			
			if ( OUT_PIN_NUM == imp_result_info.pin_num ) {
				/* ���������Ϥξ�� */
				
				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
				result = implication_update_output
					( imp_line, imp_result_info.state3, condition );
			}
			else {
				/* ���������Ϥξ�� */

				/* �ͤι��������ް����ꥹ�Ȥ���Ͽ���� */
				result = implication_update_input
					( imp_line, other, imp_result_info.state3, condition );
			}

		}
		else {
			/* ���⤷�ʤ�(IMP_KEEP) or ����(IMP_CONF) */
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

	Ulong i = 0; /* ���������Ͽ������� */
	Ulong org_id = line->line_id; /* ��Ū��ID */
	LINE  *out = NULL; /* ���ϥ����Ȥ�line�ݥ��� */

	char *func_name = "atpg_get_imp_pin_id"; /* �ؿ�̾ */

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
	
	/* �ºݤϤ��ʤ� */
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
	imp_result_info->pin_num = FL_ULMAX; /* �� OUT_PIN_NUM�Ǥ���Ф褤 */
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
	imp_result_info->pin_num = FL_ULMAX; /* �� OUT_PIN_NUM�Ǥ���Ф褤 */
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
	imp_result_info->pin_num = FL_ULMAX; /* �� OUT_PIN_NUM�Ǥ���Ф褤 */
	imp_result_info->i_state = IMP_UPDA;
}


/********************************************************************************
 * << Function >>
 *		IMP_UPDA�����»ܴؿ� to input
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

	//char *func_name = "implication_update_input"; /* �ؿ�̾ */

	topgun_test_line_null ( in, func_name );

	imp_result.i_state = IMP_UPDA;
	//imp_result.imp_id = 0;

	if ( in->flag & FV_ASS ) {
		/* �ξ��ͤ������ǽ */
		/* �ͤι��� */

		gene_update_state ( in, state3, condition );
		
		/* line��state�ˤ���ް�����ް����ꥹ�Ȥ���Ͽ���� */
		atpg_enter_implication_list_input
			( in, state3, condition, OUT_PIN_NUM, max_imp_id );
		
		topgun_print_mes_gate_state9_new( line, pin_id+1, condition, 1 );
		topgun_print_mes( 24, 5 );

		/* �ް�����Ͽ���� */
		Switch_enter_imp_trace[ condition ] ( in );

		if ( condition == COND_NORMAL ) {
			indirect_flag = 1;
		}

		if ( ! ( in->flag & FP_IAP ) ) {
			/* ���·�ϩ�Ǥʤ� */
			/* �դ��ͤ���˹��� */

			reverse_condition = atpg_reverse_condition ( condition );
			in_state3 = atpg_get_state9_with_condition_2_state3
				( in->state9, reverse_condition );

			if ( STATE3_X == in_state3 ) {
				
				topgun_print_mes ( 24, 6 );
			
				gene_update_state ( in, state3, reverse_condition );
		
				/* line��state�ˤ���ް�����ް����ꥹ�Ȥ���Ͽ���� */
				atpg_enter_implication_list_output
					( in, state3, reverse_condition, max_imp_id ); /* �����������ɲ� 2005/11/29 */
				atpg_enter_implication_list_input
					( in, state3, reverse_condition, OUT_PIN_NUM, max_imp_id );
		
				topgun_print_mes_gate_state9_new( line, pin_id+1, reverse_condition, 1 );
				topgun_print_mes( 24, 5 );
			
				/* �ް�����Ͽ���� */
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
		/* �����ͤȤ��ƹ��� */
		if ( condition == COND_FAILURE ) {
			topgun_print_mes( 24, 7 );
		}

		in_state3 = atpg_get_state9_with_condition_2_state3
			( in->state9, COND_NORMAL );

		if ( STATE3_X == in_state3 ) {
			
			/* �ͤι��� */
			gene_update_state ( in, state3, COND_NORMAL );

			/* line��state�ˤ���ް�����ް����ꥹ�Ȥ���Ͽ���� */
			atpg_enter_implication_list_input
				( in, state3, COND_NORMAL, OUT_PIN_NUM, max_imp_id );

			topgun_print_mes_gate_state9_new( line, pin_id+1, COND_NORMAL, 1 );
			topgun_print_mes( 24, 5 );

			/* �ް�����Ͽ���� */
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

	/* �����ͤι�����ȯ�����������ܴްդ�»ܤ��� */
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
 *		IMP_UPDA�����»ܴؿ� to output
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
	
	//char *func_name = "implication_update_output"; /* �ؿ�̾ */

	topgun_test_line_null ( line, func_name );

	// initail imp_result
	imp_result.i_state = IMP_UPDA;
	//imp_result.imp_id  = 0;

	if ( line->flag & FV_ASS ) {
		/* �ͤι��� */	
		gene_update_state ( line, state3, condition );

		/* line��state�ˤ���ް�����ް����ꥹ�Ȥ���Ͽ���� */
		atpg_enter_implication_list_output
			( line, state3, condition, max_imp_id );

		topgun_print_mes_gate_state9_new( line, OUT_PIN_NUM, condition, 1 );
		topgun_print_mes( 24, 5 );

		/* �ް�����Ͽ���� */
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

				/* line��state�ˤ���ް�����ް����ꥹ�Ȥ���Ͽ���� */
				atpg_enter_implication_list_output
					( line, state3, reverse_condition, max_imp_id );

				topgun_print_mes_gate_state9_new( line, OUT_PIN_NUM, reverse_condition, 1 );
				topgun_print_mes( 24, 5 );

				/* �ް�����Ͽ���� */
				Switch_enter_imp_trace[ reverse_condition ] ( line );

				if ( reverse_condition == COND_NORMAL ) { //!< ���֤�����
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
		/* �����ͤȤ��ƹ��� */
		if ( condition == COND_NORMAL ) {
			/* $$$ ¿ʬ�ʤ��Ϥ� ������� $$$ */
			topgun_print_mes ( 24, 7 );
		}

		line_state3 = atpg_get_state9_with_condition_2_state3
			( line->state9, COND_NORMAL );

		if ( STATE3_X == line_state3 ) {

			/* �ͤι��� */	
			gene_update_state ( line, state3, COND_NORMAL );
			
			/* line��state�ˤ���ް�����ް����ꥹ�Ȥ���Ͽ���� */
			atpg_enter_implication_list_output
				( line, state3, COND_NORMAL, max_imp_id );
			
			topgun_print_mes_gate_state9_new( line, OUT_PIN_NUM, COND_NORMAL, 1 );
			topgun_print_mes( 24, 5 );
			
			/* �ް�����Ͽ���� */
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

	/* �����ͤι�����ȯ�����������ܴްդ�»ܤ��� */
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
	
	char *func_name = "atpg_make_imp_info"; /* �ؿ�̾ */

	topgun_test_line_null ( line, func_name );
	topgun_test_state3 ( new_state3, func_name );

	/* imp_info�Υ������ */
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
	
	static char *func_name = "gene_enter_imp_info"; /* �ؿ�̾ */

	topgun_test_line_null ( line, func_name );
	topgun_test_state3 ( new_state3, func_name );

	if ( Gene_head.n_enter_imp_list == Gene_head.n_imp_info_list ) {
		/* ��¤򤦤ä����� */
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

    char *func_name = "atpg_enter_implication_list"; /* �ؿ�̾ */

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
	
	//char *func_name = "atpg_enter_implication_list_input"; /* �ؿ�̾ */

	topgun_test_line_null ( line, func_name );
	topgun_test_state3 ( new_state3, func_name );

	if ( ( line->flag & LINE_FLT ) &&
		 ( condition == COND_FAILURE ) ) 	{
		/* �ξ㿮�����θξ��ͤ�����¦�δް����Ϥ��ʤ� */
		;
	}
	else {

		gene_enter_imp_info	( line, new_state3, condition, pin_num, max_imp_id);
				

		/* ���Ϥ�Fanout Stem�ξ�� */
		if ( line->n_out > 1 ) {
			for ( i = 0 ; i < line->n_out ; i++ ) {
				if  ( line->out[ i ]->flag & LINE_FLT ) {
					/* �ξ㿮�����ؤ������ް�����NG */
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

	char *func_name = "atpg_enter_implication_list_output"; /* �ؿ�̾ */

	topgun_test_state3 ( new_state3, func_name );
	//topgun_test_condition ( condition, func_name );

	/* �ξ��ͤθξ㿮�����ؤ������ް�����NG */

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
		/* �ξ���(1)���� */
		
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
				
		/* imp_line��FanOutStem�ξ��
		   -> ɬ����³��� branch */
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
			/* �ξ㿮�����ؤ������ް�����NG */
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

	pin_id = atpg_implication_pin_id_2_pin_num     /* �������뿮������pin�ֹ� */
		( atpg_get_imp_pin_id ( imp_line ) );
			
	gene_enter_imp_info
		( imp_line->out[0],                     /* �ް����뿮����(���Ͽ�����) */
		  new_state3,                          /* �������줿�� */
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
		/* �ξ㿮����(2)���� */
		/* �ξ��ͤǤ��ĸξ㿮���������ް�����NG */
		topgun_print_mes ( 24, 8 );
	}
	else {
		/* imp_line��FanOutStem�Ǥʤ���� */
				
		pin_id = atpg_implication_pin_id_2_pin_num     /* �������뿮������pin�ֹ� */
			( atpg_get_imp_pin_id ( imp_line ) );

				
		gene_enter_imp_info
			( imp_line->out[0],                     /* �ް����뿮����(���Ͽ�����) */
			  new_state3,                           /* �������줿�� */
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
	Ulong i; //������
	
	//char *func_name = "gene_calc_imp_id_no_input_i_normal"; 
	
	max_imp_id = line->imp_id_n;

	for ( i = 0 ; i < line->n_in ; i++ ) {
		//�ɤ���0������ifʸȴ�����ۤ���®������
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
	Ulong i; //������
	
	//char *func_name = "gene_calc_imp_id_no_input_i_normal"; 

	max_imp_id = line->imp_id_n;

	for ( i = 0 ; i < line->n_in ; i++ ) {
		//�ɤ���0������ifʸȴ�����ۤ���®������
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
	Ulong i; //������
	
	//char *func_name = "gene_calc_imp_id_all_input_normal"; 
	
	for ( i = 1 ; i < line->n_in ; i++ ) {
		//�ɤ���0������ifʸȴ�����ۤ���®������
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
	Ulong i; //������
	
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

	char     *func_name = "atpg_enter_imp_trace"; /* �ؿ�̾ */

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
