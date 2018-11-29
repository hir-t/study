/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ξ㥷�ߥ�졼���ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include<stdio.h>
#include<stdlib.h> /* rand() */

#include "topgun.h"
#include "topgunState.h" /* STATE_9 */
#include "topgunLine.h"
#include "topgunFsim.h"
#include "topgunFlist.h"   /* FLIST & FLIST->info */
#include "topgunCompact.h"
#include "topgunStock.h"
#include "topgunMemory.h" /* FMT_XXX */
#include "topgunError.h" /* FEC_XXX */
#include "topgunTest.h" /* topgun_test_xxx */
#include "topgunTime.h"


FSIM_HEAD         Fsim_head;
char              **Tp_head;
EVENT_LIST        *Event_list_head;
extern LINE_INFO  Line_info;
extern LINE       *Line_head;
extern LINE       **Pi_head;
extern LINE       ***Lv_pi_head;

static Ulong *Test_pattern_check_bit;

static Ulong N_tp = 32;

/* original function list */
void fsim_ppsfp_val2( FSIM_INFO *, COMP_INFO * );
void fsim_ppsfp_val3( FSIM_INFO *, COMP_INFO * );

static void fsim_clear_line_flag ( void );

static void  fsim_logic_sim_val2_init( void );
static void  fsim_logic_sim_val3_init( void );

static void fsim_enter_pattern_from_cc_pat_val2( FSIM_INFO *, COMP_INFO *);
static void fsim_exe_logic_sim_val2( void );
static void fsim_exe_fault_sim_val2( FSIM_INFO * );


static void  fsim_fault_value_val2_init( void );
static void  fsim_fault_value_val3_init( void );

static Ulong fsim_count_fault_rest( void );
static void  fsim_output ( FSIM_INFO *, COMP_INFO * );
static void fsim_enter_pattern_from_stock_val2( FSIM_INFO *, COMP_INFO *, STOCK_INFO *, Ulong );
static void fsim_exe_fault_sim_val2_stock( FSIM_INFO *, STOCK_INFO *, Ulong );


static void (* fsim_fault_value_val2[ TOPGUN_NUM_PRIM ] )( LINE *, Ulong );
static void (* fsim_logic_sim_val2[ TOPGUN_NUM_PRIM ] ) ( LINE * );

static void (* fsim_fault_value_val3[ TOPGUN_NUM_PRIM ] )( LINE *, Ulong );
static void (* fsim_logic_sim_val3[ TOPGUN_NUM_PRIM ] ) ( LINE * );

static void fsim_logic_sim_val2_pi ( LINE * );
static void fsim_logic_sim_val2_in1 ( LINE * );
static void fsim_logic_sim_val2_inv ( LINE * );
static void fsim_logic_sim_val2_and ( LINE * );
static void fsim_logic_sim_val2_nand ( LINE * );
static void fsim_logic_sim_val2_or ( LINE * );
static void fsim_logic_sim_val2_nor ( LINE * );
static void fsim_logic_sim_val2_xor ( LINE * );
static void fsim_logic_sim_val2_xnor ( LINE * );
static void fsim_logic_sim_val2_error ( LINE * );

static void fsim_fault_value_val2_pi ( LINE *, Ulong );
static void fsim_fault_value_val2_po ( LINE *, Ulong );
static void fsim_fault_value_val2_in1 ( LINE *, Ulong );
static void fsim_fault_value_val2_inv ( LINE *, Ulong );
static void fsim_fault_value_val2_and ( LINE *, Ulong );
static void fsim_fault_value_val2_nand ( LINE *, Ulong );
static void fsim_fault_value_val2_or ( LINE *, Ulong );
static void fsim_fault_value_val2_nor ( LINE *, Ulong );
static void fsim_fault_value_val2_xor ( LINE *, Ulong );
static void fsim_fault_value_val2_xnor ( LINE *, Ulong );
static void fsim_fault_value_val2_error ( LINE *, Ulong );

static void fsim_logic_sim_val3_pi ( LINE * );
static void fsim_logic_sim_val3_in1 ( LINE * );
static void fsim_logic_sim_val3_inv ( LINE * );
static void fsim_logic_sim_val3_and ( LINE * );
static void fsim_logic_sim_val3_nand ( LINE * );
static void fsim_logic_sim_val3_or ( LINE * );
static void fsim_logic_sim_val3_nor ( LINE * );
static void fsim_logic_sim_val3_xor ( LINE * );
static void fsim_logic_sim_val3_xnor ( LINE * );
static void fsim_logic_sim_val3_error ( LINE * );

static void fsim_fault_value_val3_pi ( LINE *, Ulong );
static void fsim_fault_value_val3_po ( LINE *, Ulong );
static void fsim_fault_value_val3_in1 ( LINE *, Ulong );
static void fsim_fault_value_val3_inv ( LINE *, Ulong );
static void fsim_fault_value_val3_and ( LINE *, Ulong );
static void fsim_fault_value_val3_nand ( LINE *, Ulong );
static void fsim_fault_value_val3_or ( LINE *, Ulong );
static void fsim_fault_value_val3_nor ( LINE *, Ulong );
static void fsim_fault_value_val3_xor ( LINE *, Ulong );
static void fsim_fault_value_val3_xnor ( LINE *, Ulong );
static void fsim_fault_value_val3_error ( LINE *, Ulong );

static void fsim_generate_random_pattern_val2 ( Ulong, CC_PATTERN * );
extern void fsim_stock_fault_init ( STOCK_MODE );
extern void pattern_bridge_by_random( void );
extern void fsim_stock_pat_id( FSIM_INFO *, COMP_INFO *, STOCK_INFO *, Ulong );


Ulong (* fsim_stock_fault_select )( Ulong );
void  (* fsim_stock_fault_detect )( Ulong *, Ulong );
Ulong fsim_stock_fault_select_one_detect( Ulong );
Ulong fsim_stock_fault_select_double_detect( Ulong );

/* topgun_select.c */
Ulong select_fault_fsim_parallel ( LINE * );
Ulong select_fault_fsim_random ( LINE * );


/* topgun_state.c */
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 );

/* topgun_compact.c */
void  compact_get_pattern_with_id ( CC_PATTERN *, Ulong );
Ulong compact_get_buf_size( void );
COMP_CHECK_ALL_X compact_check_pattern_all_x ( CC_PATTERN * );
void compact_cc_pat_initialize( CC_PATTERN *);
void pattern_get_from_ramdom_generate( CC_PATTERN *, Ulong );


/* topgun_stock.c */
void stock_enter_pattern( CC_PATTERN * ); /* stock�إѥ�������Ϥ��� */
void stock_make_info( STOCK_INFO * );/* stock_info��������� */
void stock_get_pattern( CC_PATTERN *, Ulong ); /* stock����ѥ�������� */


/* topgun_uty.c */
Ulong utility_count_1_bit( Ulong );


/* print function */
void topgun_print_mes_logic_sim_val2_line( LINE * );
void topgun_print_mes_logic_sim_val3_line( LINE * );
void topgun_print_mes_fsim_state( Ulong, Ulong, Ulong );
void topgun_print_mes_fsim_end_state( Ulong, Ulong );
void topgun_print_mes_fsim_enter_line( LINE * );
void topgun_print_mes_fsim_cannot_enter_line( LINE *, Ulong );
void topgun_print_mes_fsim_exec_line( LINE * );
void topgun_print_mes_fsim_not_detect( LINE * , Ulong );
void topgun_print_mes_fsim_detect( LINE *, Ulong );
void topgun_print_mes_fsim_not_insert( LINE * );
void topgun_print_mes_fsim_input_pattern( Ulong, Ulong );
void topgun_print_mes_fsim_start( void );
void topgun_print_mes_fsim_end( void );
void topgun_print_mes_compact_pattern( Ulong *, Ulong *, char *);
void topgun_print_mes_n_compact( void );
void topgun_print_mes_fsim3_exec_line( LINE * );

/* test function */
void  test_line_in_null ( LINE *, char * );
void  test_line_value ( Ulong, char * );

/********************************************************************************
 * << Function >>
 *		Fault simulator only mode
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault         I      FLIST * certainly detect fault
 *                              by generated pattern for this fault 
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


void fsim
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
  ){

	/* Fsim�λ��ַ�¬���� */
	topgun_2_utility_enter_start_time( &Time_head.sim );


	topgun_print_mes_fsim_start();


	switch ( fsim_info->mode ) {
		
	case FSIM_OUTPUT:
		fsim_output( fsim_info, comp_info );
		break;
		
	case FSIM_PPSFP_VAL3 :
		/* Lsim & Fsim val3 �»� */
		fsim_ppsfp_val3( fsim_info, comp_info );
		break;
	default:
		/* Lsim & Fsim�»� */
		fsim_ppsfp_val2( fsim_info, comp_info );
		//fsim_ppsfp_val2_20060926( fsim_info, comp_info );
	}

	topgun_print_mes_fsim_end();

	/* Fsim�λ��ַ�¬��λ */
	topgun_2_utility_enter_end_time( &Time_head.sim );
}

/********************************************************************************
 * << Function >>
 *		Fault simulator only mode
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


void topgun_fsim_random( Ulong num_fault_sim, Ulong nokori ){

	Ulong i;
	Ulong times   = num_fault_sim;   /* �ξ㥷�ߥ�졼��������¹Բ�� */
	Ulong n_redun = nokori; /* Ϳ����줿��Ĺ�ξ�� */
	Ulong n_rest;   /* ���ߤλĤ�ξ�� */

	CC_PATTERN cc_pat; /* for stockž���� */
	
	FSIM_INFO fsim_info_random;

	
	/* Fsim�λ��ַ�¬���� */
	topgun_2_utility_enter_start_time( &Time_head.sim );
	topgun_2_utility_enter_start_time( &Time_head.random_sim );

	fsim_info_random.mode   = FSIM_RANDOM;
	fsim_info_random.cc_pat = NULL;
	fsim_info_random.n_detect = 0;
	fsim_info_random.n_cc_pat = Fsim_head.bit_size;
	fsim_info_random.num_pat = 0;
	fsim_info_random.num_waste_pat = 0;

	compact_cc_pat_initialize( &cc_pat );

	/* ������1��Υѥ������bit_size���� */
	for ( i = 0; i < times ; i+= fsim_info_random.n_cc_pat ) {

		n_rest = fsim_count_fault_rest();

		if ( i + fsim_info_random.n_cc_pat  > times ) {
			fsim_info_random.n_cc_pat  = (times - i);
		}
		
		if ( n_rest > n_redun ) {
			/* �Ĥ�ξ���������Ĺ�ξ�����¿�� */
			/* Fsim��³���� */
			topgun_print_mes_fsim_state( i+1, times, n_rest );

			fsim_clear_line_flag();

			/* ������ѥ��������� */
			fsim_generate_random_pattern_val2( fsim_info_random.n_cc_pat, &cc_pat );

			/* Lsim & Fsim�»� */
			/* 2���������ߥ�졼����� */
			fsim_exe_logic_sim_val2();

			/* 2�͸ξ㥷�ߥ�졼����� */
			fsim_exe_fault_sim_val2( &fsim_info_random );

		}
		else {
			/* �Ĥ�ξ���������Ĺ�ξ����Ʊ�������� */
			/* Fsim��λ���� */
			topgun_print_mes_fsim_end_state( times, n_redun );
			break;
		}

		{
			double rest;
			long a = 0;
			rest = ( ( Line_info.n_fault_atpg_count * 100 )/ (double)( Line_info.n_fault_atpg ));	
			printf("RANDOM     %6ld   Det %8ld Und %8ld ", i+fsim_info_random.n_cc_pat, a, a );
			printf("Sim  %4ld Rest %6.2f ( %8ld / %8ld )\n",
				   a, rest, Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );
		}
	}

	/* Fsim�λ��ַ�¬��λ */
	topgun_2_utility_enter_end_time( &Time_head.random_sim );
	topgun_2_utility_enter_end_time( &Time_head.sim );
}

/********************************************************************************
 * << Function >>
 *		Stock�ˤ���Ǥ�դΥѥ����ξ㥷�ߥ�졼����󤹤�
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

void fsim_stock_pat_id
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info,
 STOCK_INFO *stock_info,
 Ulong pat_id
 ){
	fsim_clear_line_flag();
		
	/* �Ĥ�ξ���������Ĺ�ξ�����¿�� */
	/* ����ѥ���������� */
	fsim_enter_pattern_from_stock_val2( fsim_info, comp_info, stock_info, pat_id );

	/* Lsim & Fsim�»� */
	/* 2���������ߥ�졼����� */
	fsim_exe_logic_sim_val2();

	/* 2�͸ξ㥷�ߥ�졼�����(pat�ֹ�) */
	fsim_exe_fault_sim_val2_stock( fsim_info, stock_info, pat_id );
}

/********************************************************************************
 * << Function >>
 *		Fault simulator initailize
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
 *		2005/12/22
 *
 ********************************************************************************/

void fsim_initialize
(
 void
 )
{
	Ulong i; /* ulong��bit��(char���ɤ�����) */
	Ulong j; /* ulong��bit��(char���ɤ�����) */

	char *func_name = "fsim_initialize";

	/* x bit���� */
	Fsim_head.bit_size = sizeof ( Ulong ) * BYTE ;

	/* �ƥ��ȥѥ���������bit����� */
	/* ��bit�ܤ������å�����������flag��������� */
	/* ���Τ�bit����ʬ��ʤ��ΤǼ����ɬ�פ��� */
	Test_pattern_check_bit = ( Ulong * )topgunMalloc
		( FMT_Ulong, sizeof ( Ulong), Fsim_head.bit_size, func_name );

	Test_pattern_check_bit[ 0 ] = BIT_1_ON;
	//printf("check bit %lu %lu\n", 0 , Test_pattern_check_bit[ 0 ]);
	for ( i = 1 ; i < Fsim_head.bit_size ; i++ ) {
		Test_pattern_check_bit[ i ] = Test_pattern_check_bit[ i-1 ] << 1;
		//printf("check bit %lu %lu\n", i , Test_pattern_check_bit[ i ]);
	}

	/* �ƥ��ȥѥ��������� */
	/*
	Tp_head = topgunMalloc ( FMT_Char_P, sizeof( char * ), N_tp, func_name );
	for ( i = 0 ; i < N_tp ; i++ ) {
		Tp_head[ i ] = topgunMalloc ( FMT_Char, Line_info.n_pi,
									   sizeof( char ), func_name );

		for( j = 0 ; j < Line_info.n_pi; j++ ) {
			Tp_head[ i ][ j ] = 0;
		}
	}
	*/

	/* Event_list_head�ν���� */

	Event_list_head = topgunMalloc( FMT_EVENT_LIST, sizeof( EVENT_LIST ),
									 Line_info.max_lv_pi, func_name );

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		Event_list_head[ i ].n_event = 0;
		Event_list_head[ i ].event   = NULL;

		Event_list_head[ i ].event  = topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
												Line_info.n_lv_pi[ i ], func_name );

		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ){
			Event_list_head[ i ].event[ j ] = NULL;
		}
	}


	/* �ؿ��Υݥ��󥿤ν���� */
	/* �������ߥ�졼�� */
	fsim_logic_sim_val2_init();
	fsim_logic_sim_val3_init();

	/* �ξ㥷�ߥ�졼�� */
	fsim_fault_value_val2_init();
	fsim_fault_value_val3_init();

	/* stock�ѥѥ��󵭽����ѿ��ν���� */
	/*
	Stock_pat_order  = (Ulong *)topgunMalloc( FMT_Ulong , sizeof( Ulong ),
										 Fsim_head.bit_size, func_name );

	for ( i = 0 ; i < Fsim_head.bit_size ; i++ ) {
		Stock_pat_order[ i ] = 0;
	}
	*/
}


/********************************************************************************
 * << Function >>
 *		Single Pattern Single Fault Propagation 
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
 *		2005/12/22
 *
 ********************************************************************************/

void fsim_ppsfp_val2_20060926
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
 )
{

	Ulong i,j,k,m,n; /* bit���Υ����� */
	Ulong test_pattern_position = 0 ;
	Ulong n_test_pattern_set    = 0;
	Ulong valid_bit             = 0; /* �ѥ�����¸�ߤ���bit��1��Ω�äƤ�(���̤�����1) */
	Ulong total_detect_bit      = 0; /* �ξ�򸡽Ф���bit */
	Ulong fprop_flag            = 0;
	Ulong pattern_detect_mask   = 0;
	Ulong fsim_start_level      = 0;
	Ulong detect_sa0_flag       = 0;
	Ulong detect_sa1_flag       = 0;

	Ulong n_tp = fsim_info->n_cc_pat;

	Ulong *pattern_a = fsim_info->cc_pat->pattern_a;
	Ulong *pattern_b = fsim_info->cc_pat->pattern_b;

	Ulong shift_count = 0;
	Ulong width_count = 0;

	LINE  *line; /* ���ߥ�졼����󤹤�饤��ݥ��� */
	LINE  *pi_line; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */
	LINE  *cur_line; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */
	LINE  *event_line; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */

	EVENT_LIST *schedule_elist;
	EVENT_LIST *cur_e_list;

	//char *func_name = "fsim_spsfp_val2";

	topgun_print_mes_compact_pattern
		( pattern_a, pattern_b, "FSIM ");
	topgun_print_mes_n_compact();

	/* FFR */
	/* $$$ ���Ȥ� $$$ */

	/* ������� */

	/* ������ */
	fsim_clear_line_flag();

	/* �ѥ�������֤ν���� */
	test_pattern_position = 0 ;

	/* N_tp ?? */
	while ( test_pattern_position == 0 ) {

		topgun_3_utility_enter_start_time( &Time_head.logic_sim );

		/* ����ѥ���������� */

		n_test_pattern_set = 0;
		valid_bit          = 0;



		/* ����� */
		for ( i = 0 ; i < Line_info.n_pi; i++ ) {
			Pi_head [ i ]->n_val_a = 0;
		}

				
		//printf("Pi_head setting pattern\n");

		while ( ( test_pattern_position != n_tp ) &&
				( n_test_pattern_set < Fsim_head.bit_size ) ) {

			if ( n_tp != 1 ){
				/* �ѥ���ѥ���⡼�� */
				pattern_a = comp_info->cc_pat_wodge[ n_test_pattern_set ]->pattern_a;
				pattern_b = comp_info->cc_pat_wodge[ n_test_pattern_set ]->pattern_b;
			}
			
			//printf("a: %u\n",pattern_a[0]);
			//printf("b: %u\n",pattern_b[0]);
			
			shift_count = 0;
			width_count = 0;

			for ( i = 0 ; i < Line_info.n_pi; i++, shift_count++ ) {

				pi_line = Pi_head [ i ];

				if ( shift_count == Fsim_head.bit_size ) {
					shift_count = 0;
					width_count++;
					
					/*
					if ( width_count == ( ( Line_info.n_pi / Fsim_head.bit_size ) + 1 ) ) {
						printf("width error %ld\n", width_count);
						exit(0);
					}
					*/
				}

				if ( pattern_a[ width_count ] & ( BIT_1_ON << shift_count ) ) {
					//printf("1");
					/* 1�ξ�� */
					pi_line->n_val_a = ( ( pi_line->n_val_a & valid_bit ) |
										( PARA_V1_BIT & ( BIT_1_ON << n_test_pattern_set ) ) );
				}
				else if ( pattern_b[ width_count ] & ( BIT_1_ON << shift_count ) ) {
					//printf("0");
					/* 0�ξ�� */
					pi_line->n_val_a = ( ( pi_line->n_val_a & valid_bit ) |
										( PARA_V0_BIT & ( BIT_1_ON << n_test_pattern_set ) ) );
				}
				else {
					//printf("X");
					/* X�ξ�� */

					if ( ( rand() & RAND_BIT ) == 0 ) {
						pi_line->n_val_a = ( ( pi_line->n_val_a & valid_bit ) |
											( PARA_V0_BIT & ( BIT_1_ON << n_test_pattern_set ) ) );
					}
					else {
						pi_line->n_val_a = ( ( pi_line->n_val_a & valid_bit ) |
											( PARA_V1_BIT & ( BIT_1_ON << n_test_pattern_set ) ) );
					}
				}
				/*
				if ( Tp_head[ 0 ][ i ] == STATE_0_CODE ) {
					pi_line->n_val_a = ( ( pi_line->n_val_a & valid_bit ) |
										( PARA_V0_BIT & ( BIT0_ON << n_test_pattern_set ) ) );
				}
				else if ( Tp_head[ 0 ][ i ] == STATE_1_CODE ) {
					pi_line->n_val_a = ( ( pi_line->n_val_a & valid_bit ) |
										( PARA_V1_BIT & ( BIT0_ON << n_test_pattern_set ) ) );
				}
				else {
					topgun_error( FEC_NO_ERROR_CODE, func_name );
				}
				*/

			}
			valid_bit |= Test_pattern_check_bit [ n_test_pattern_set ];
			n_test_pattern_set++;
			test_pattern_position++;
			//printf("\n");
		}

		//printf("parallel %ld\n", test_pattern_position );

		/* pi_head��n_val_a������å� */
		/*
		printf("Pi_head setting pattern\n");
		for ( i = 0 ; i < Line_info.n_pi ; i++ ) {
			printf("%lu\n", Pi_head[i]->n_val_a);
		}
		*/

		/* 2���������ߥ�졼����� */

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {

			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {
				line = Lv_pi_head [ i ] [ j ] ; /* $$$ 2���å�������� $$$ */

				/* ��٥����ͤ���� */
				fsim_logic_sim_val2 [ line->type ] ( line );

				topgun_print_mes_logic_sim_val2_line( line );
			}
		}

		/* �ξ����¥ե饰�ν���� */

		fprop_flag = NO_PROP_FAULT;

		/* ��FFR���Ф��Ƹξ㥷�ߥ�졼�����Ϥ��Ȥ� */
		/* �����������Ф��Ƹξ㥷�ߥ�졼����� */

		topgun_3_utility_enter_end_time( &Time_head.logic_sim );
		topgun_3_utility_enter_start_time( &Time_head.fault_sim );

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {


			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				line = Lv_pi_head[ i ][ j ];

				/* ��bit�ˤ�����ξ����¥ޥ�������(pattern_detect_mask) */
				/* ���Ȥ�FFR�� */

				pattern_detect_mask = ALL_BIT_OFF;
				detect_sa0_flag = ALL_BIT_OFF;
				detect_sa1_flag = ALL_BIT_OFF;

				/* ���Ф����ξ��flag */
				if ( line->flist[ FSIM_SA0 ] != NULL ) {
					if ( ! ( line->flist[ FSIM_SA0 ]->info & TOPGUN_DTS ) ) {

						if ( ( ( line->flist[ FSIM_SA0 ]->info & TOPGUN_DCD ) &&
							   ( line->flist[ FSIM_SA0 ]->info & TOPGUN_DTA ) ) ||
							 ( ! ( line->flist[ FSIM_SA0 ]->info & TOPGUN_DCD ) ) ) {

							/* sa-0��ʬ�ब̤����ξ�� */

							/* 0���ब���ФǤ��뤫? */
							detect_sa0_flag = line->n_val_a & valid_bit;

							if ( detect_sa0_flag ) {
								pattern_detect_mask =
									( pattern_detect_mask | detect_sa0_flag )
									& valid_bit;

								/*
								  printf("F_SIM Insert %8ld SA0",line->line_id);
								  topgun_print_mes_n();
								*/
							}
						}
					}
					else {
						topgun_print_mes_fsim_cannot_enter_line( line, FSIM_SA0 );
					}
				}
				if ( line->flist[ FSIM_SA1 ] != NULL ) {

					if ( ! ( line->flist[ FSIM_SA1 ]->info & TOPGUN_DTS ) ) {

						if ( ( ( line->flist[ FSIM_SA1 ]->info & TOPGUN_DCD ) &&
							   ( line->flist[ FSIM_SA1 ]->info & TOPGUN_DTA ) ) ||
							 ( ! ( line->flist[ FSIM_SA1 ]->info & TOPGUN_DCD ) ) ) {
							/* sa-1��ʬ�ब̤����ξ�� */

							/* 1���ब���ФǤ��뤫? */
							detect_sa1_flag = ( ~( line->n_val_a ) & valid_bit ); 

							if ( detect_sa1_flag ) {


								pattern_detect_mask =
									( pattern_detect_mask | detect_sa1_flag )
									& valid_bit;
							/*
							printf("F_SIM Insert %8ld SA1",line->line_id);
							topgun_print_mes_n();
							*/
							}
						}
					}
					else {
						topgun_print_mes_fsim_cannot_enter_line( line, FSIM_SA1 );
					}
				}

				if ( pattern_detect_mask != ALL_BIT_OFF ) {
					/* �ξ㤬�����ǽ�Ǥ���ʤ�� */
					/*
					printf("F_SIM START %ld\n",line->line_id);
					*/
					
					fprop_flag++; /* �ξ�ID�ߤ����ʤ�� �ξ㤴�Ȥν�����򤵤��� */

					/* �ξ�����ե饰 */
					line->fault_set_flag = pattern_detect_mask;

					/* ���٥�ȥꥹ�Ȥ���Ͽ */

					/* flag���ѹ� */
					line->mp_flag = FSIM_EVENT_ON;
					/* ��Ƭ����Ͽ���� */
					schedule_elist = & ( Event_list_head [ line->lv_pi ] );
					schedule_elist->event [ schedule_elist->n_event ] = line;
					schedule_elist->n_event++;

					topgun_print_mes_fsim_enter_line( line );

					fsim_start_level = line->lv_pi;

					/* ���ߥ�졼������»� */
					Fsim_head.detect_mask = ALL_BIT_OFF;

					for ( k = fsim_start_level ; k < Line_info.max_lv_pi ; k++ ) {

						cur_e_list = &( Event_list_head[ k ] );

						for ( m = 0 ; m < cur_e_list->n_event ; m++ ) {

							cur_line = cur_e_list->event[ m ] ;

							/* ���ο������Υ��٥�Ƚ�λ */
							cur_line->mp_flag = FSIM_EVENT_OFF;

							fsim_fault_value_val2[ cur_line->type ]( cur_line, fprop_flag );

							/* �ξ㿮��������ȿž������ */
							cur_line->f_val_a ^=  cur_line->fault_set_flag;

							topgun_print_mes_fsim_exec_line( cur_line );

							/* �ξ�����? */
							if ( cur_line->n_val_a != cur_line->f_val_a ) {
								/* �ξ�������!! */

								/* �ξ�������ե饰������ */
								cur_line->fprop_flag = fprop_flag;

								/* ���Ͽ������򥤥٥�ȥꥹ�Ȥ���Ͽ */

								for ( n = 0; n < cur_line->n_out ; n++ ) {

									event_line = cur_line->out[ n ];

									if ( event_line->mp_flag == FSIM_EVENT_OFF ) {
										/* ̤��Ͽ�ξ�� */

										event_line->mp_flag = FSIM_EVENT_ON;
										schedule_elist = & ( Event_list_head [ event_line->lv_pi ] );
										schedule_elist->event[ schedule_elist->n_event ] = event_line;
										schedule_elist->n_event++;

										topgun_print_mes_fsim_enter_line( event_line );
									}
								}
							}

						}
						/* ���ߤΥ�٥�������٥�Ƚ�λ */
						cur_e_list->n_event = 0;
					}
					/*
					printf("F_SIM END\n");
					*/
					if ( Fsim_head.detect_mask ) {
						/* �ξ㤬���ФǤ��� */
						/* ����ƥ�����ѥ��ȥ졼���� */
						/* $$$ ���Ȥ� $$$ */


						/* ���Ф���Ͽ */
						if ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit ) {

							total_detect_bit |= ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit );

							if ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DCD ) ){
							/* �Ĥ�ATPG�оݸξ����︺����(check_end��) */
								if  ( ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DTX ) ) &&
									  ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DTR ) ) &&
									  ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DTA ) ) ) {
									Line_info.n_fault_atpg_count--;
									/*
									printf("C-F2  : %8ld sa0\n", line->line_id);
									*/
								}
							}
							/* sa0�ξ�� */
							line->flist[FSIM_SA0]->info |= TOPGUN_DCD;
							line->flist[FSIM_SA0]->info |= TOPGUN_DTS;


							topgun_print_mes_fsim_detect( line, FSIM_SA0 );
							/*
							{
								printf("F : line %8ld", line->line_id);
								printf(" sa0\n");
							}
							*/
						}
						
						if ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit ) {

							total_detect_bit |= ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit );

							if ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DCD ) ){
								if ( ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DTX ) ) &&
									 ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DTR ) ) &&
									 ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DTA ) ) ) {
									/* �Ĥ�ATPG�оݸξ����︺����(check_end��) */
									Line_info.n_fault_atpg_count--;
									//printf("C-F2  : %8ld sa1\n", line->line_id);
								}
							}
							
							/* sa1�ξ�� */
							line->flist[FSIM_SA1]->info |= TOPGUN_DCD;
							line->flist[FSIM_SA1]->info |= TOPGUN_DTS;

							topgun_print_mes_fsim_detect( line, FSIM_SA1 );
							/*
							{
								printf("F : line %8ld", line->line_id);
								printf(" sa1\n");
							}
							*/
						}
					}
					else {
						/* �ξ㤬���ФǤ��ʤ��ä� */
						if ( detect_sa0_flag & Fsim_head.detect_mask ) {
							topgun_print_mes_fsim_not_detect(line, FSIM_SA0);
						}
						if ( detect_sa1_flag & Fsim_head.detect_mask ) {
							topgun_print_mes_fsim_not_detect(line, FSIM_SA1);
						}
					}

					/* �ξ�����ե饰 */
					line->fault_set_flag = ALL_BIT_OFF;
				}
				else {
					topgun_print_mes_fsim_not_insert(line);
				}
			}
		}
		topgun_3_utility_enter_end_time( &Time_head.fault_sim );
	}

	/*
	{
		printf("32bit %3lu ", utility_count_1_bit ( total_detect_bit ) );
		Ulong tmp = total_detect_bit;
		for (i = 0; i < Fsim_head.bit_size; i++ ) {
			if ( tmp % 2  == 1 ) {
				printf("1");
			}
			else {
				printf("0");
			}
			tmp /= 2;
		}
		printf("\n");
	}
	*/

	/* fsim2��pat�Ͽ����� */
	fsim_info->num_pat  += utility_count_1_bit ( total_detect_bit ); 
}


/********************************************************************************
 * << Function >>
 *		Single Pattern Single Fault Propagation 
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
 *		2005/12/22
 *
 ********************************************************************************/

void fsim_ppsfp_val2
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
 )
{

	//char *func_name = "fsim_spsfp_val2";

	topgun_3_utility_enter_start_time( &Time_head.sim_val2 );
	
	/* ������� */

	fsim_clear_line_flag();

	/* ����ѥ���������� */
	fsim_enter_pattern_from_cc_pat_val2( fsim_info, comp_info );

	/* 2���������ߥ�졼����� */
	fsim_exe_logic_sim_val2();

	/* 2�͸ξ㥷�ߥ�졼����� */
	fsim_exe_fault_sim_val2( fsim_info );
	
	topgun_3_utility_enter_end_time( &Time_head.sim_val2 );

}

/********************************************************************************
 * << Function >>
 *		Single Pattern Single Fault Propagation  for Value 3 ( 0, 1, X )
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
 *		2005/12/22
 *
 ********************************************************************************/

void fsim_ppsfp_val3
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
 )
{

	Ulong i,j,k,m,n; /* bit���Υ����� */
	Ulong test_pattern_position = 0 ;
	Ulong n_test_pattern_set    = 0;
	Ulong valid_bit             = 0; /* �ѥ�����¸�ߤ���bit��1��Ω�äƤ�(���̤�����1) */
	Ulong total_detect_bit      = 0; /* �ξ�򸡽Ф���bit */
	Ulong pattern_detect_mask   = 0;
	Ulong fsim_start_level      = 0;
	Ulong detect_sa0_flag       = 0;
	Ulong detect_sa1_flag       = 0;

	Ulong n_tp = fsim_info->n_cc_pat;

	Ulong fprop_flag            = 0;

	Ulong *pattern_a = fsim_info->cc_pat->pattern_a;
	Ulong *pattern_b = fsim_info->cc_pat->pattern_b;

	Ulong shift_count = 0;
	Ulong width_count = 0;

	LINE  *line; /* ���ߥ�졼����󤹤�饤��ݥ��� */
	LINE  *pi; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */
	LINE  *cur_line; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */
	LINE  *event_line; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */

	EVENT_LIST *schedule_elist;
	EVENT_LIST *cur_e_list;

	//char *func_name = "fsim_spsfp_val3";

	topgun_3_utility_enter_start_time( &Time_head.sim_val3 );
	
	topgun_print_mes_compact_pattern
		( pattern_a, pattern_b, "FSIM ");
	topgun_print_mes_n_compact();

	/* FFR */
	/* $$$ ���Ȥ� $$$ */

	/* ������� */

	/* ������ */
	fsim_clear_line_flag();

	/* �ѥ�������֤ν���� */
	test_pattern_position = 0 ;

	topgun_3_utility_enter_start_time( &Time_head.logic_sim );

	/* ����ѥ���������� */

	n_test_pattern_set = 0;
	valid_bit          = 0;

	while ( ( test_pattern_position != n_tp ) &&
			( n_test_pattern_set < Fsim_head.bit_size ) ) {

		if ( n_tp != 1 ){
			/* �ѥ���ѥ���⡼�� */
			pattern_a = comp_info->cc_pat_wodge[ n_test_pattern_set ]->pattern_a;
			pattern_b = comp_info->cc_pat_wodge[ n_test_pattern_set ]->pattern_b;
		}
		shift_count = 0;
		width_count = 0;
			
		for ( i = 0 ; i < Line_info.n_pi; i++, shift_count++ ) {

			pi = Pi_head [ i ];

			if ( shift_count == Fsim_head.bit_size ) {
				shift_count = 0;
				width_count++;
			}					

			if ( pattern_a[ width_count ] & ( BIT0_ON << shift_count ) ) {
				/* 1�ξ�� */
				pi->n_val_a = ( ( pi->n_val_a & valid_bit ) |
							   ( PARA_V1_BIT & ( BIT0_ON << n_test_pattern_set ) ) );
				pi->n_val_b = ( ( pi->n_val_b & valid_bit ) |
							   ( PARA_V0_BIT & ( BIT0_ON << n_test_pattern_set ) ) );
			}
			else if ( pattern_b[ width_count ] & ( BIT0_ON << shift_count ) ) {
				/* 0�ξ�� */
				pi->n_val_a = ( ( pi->n_val_a & valid_bit ) |
							   ( PARA_V0_BIT & ( BIT0_ON << n_test_pattern_set ) ) );
				pi->n_val_b = ( ( pi->n_val_b & valid_bit ) |
							   ( PARA_V1_BIT & ( BIT0_ON << n_test_pattern_set ) ) );
			}
			else {
				/* X�ξ�� */
				pi->n_val_a = ( ( pi->n_val_a & valid_bit ) |
							   ( PARA_V0_BIT & ( BIT0_ON << n_test_pattern_set ) ) );
				pi->n_val_b = ( ( pi->n_val_b & valid_bit ) |
							   ( PARA_V0_BIT & ( BIT0_ON << n_test_pattern_set ) ) );

			}
		}
		valid_bit |= Test_pattern_check_bit [ n_test_pattern_set ];
		n_test_pattern_set++;
		test_pattern_position++;
	}

	//printf("parallel %ld\n", test_pattern_position );

	/* pi_head��n_val_a������å� */
		/*
		  printf("Pi_head setting pattern\n");
		for ( i = 0 ; i < Line_info.n_pi ; i++ ) {
			printf("%ld\n", Pi_head[i]->n_val_a);
		}
		*/
	
	/* 3���������ߥ�졼����� */
	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {

		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {
			line = Lv_pi_head [ i ] [ j ] ; /* $$$ 2���å�������� $$$ */

			/* ��٥����ͤ���� */
			fsim_logic_sim_val3 [ line->type ] ( line );

			topgun_print_mes_logic_sim_val3_line( line );
		}
	}

	/* ��FFR���Ф��Ƹξ㥷�ߥ�졼�����Ϥ��Ȥ� */
	/* �����������Ф��Ƹξ㥷�ߥ�졼����� */

	topgun_3_utility_enter_end_time( &Time_head.logic_sim );
	topgun_3_utility_enter_start_time( &Time_head.fault_sim );

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {


		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {
			
			line = Lv_pi_head[ i ][ j ];

			/* ��bit�ˤ�����ξ����¥ޥ�������(pattern_detect_mask) */
			/* ���Ȥ�FFR�� */

			pattern_detect_mask = ALL_BIT_OFF;
			detect_sa0_flag = ALL_BIT_OFF;
			detect_sa1_flag = ALL_BIT_OFF;

			/* ���Ф����ξ��flag */
			if ( line->flist[ FSIM_SA0 ] != NULL ) {
				if ( ! ( line->flist[ FSIM_SA0 ]->info & TOPGUN_DCD ) ) {
					if ( ! ( line->flist[ FSIM_SA0 ]->info & TOPGUN_DTS ) ) {
						if ( ! ( line->flist[ FSIM_SA0 ]->info & TOPGUN_DTX ) ) {
						/* sa-0��ʬ�ब̤����ξ�� */

						/* 0���ब���ФǤ��뤫? */
						/* �����ͤ�1���ɤ��� */
							detect_sa0_flag = line->n_val_a & valid_bit;
					
							if ( detect_sa0_flag ) {
								pattern_detect_mask =
									( pattern_detect_mask | detect_sa0_flag )
									& valid_bit;

							/*
							  printf("F_SIM Insert %8ld SA0",line->line_id);
							  topgun_print_mes_n();
							*/
							}
						}
					}
				}
				else {
					topgun_print_mes_fsim_cannot_enter_line( line, FSIM_SA0 );
				}
			}
			if ( line->flist[ FSIM_SA1 ] != NULL ) {
				
				if ( ! ( line->flist[ FSIM_SA1 ]->info & TOPGUN_DCD ) ) {
					if ( ! ( line->flist[ FSIM_SA1 ]->info & TOPGUN_DTS ) ) {
						if ( ! ( line->flist[ FSIM_SA1 ]->info & TOPGUN_DTX ) ) {
								/* sa-1��ʬ�ब̤����ξ�� */

								/* 1���ब���ФǤ��뤫? */
								/* �����ͤ�0���ɤ��� */
							detect_sa1_flag = line->n_val_b  & valid_bit; 
							
							if ( detect_sa1_flag ) {

								pattern_detect_mask =
									( pattern_detect_mask | detect_sa1_flag )
									& valid_bit;
									/*
									  printf("F_SIM Insert %8ld SA1",line->line_id);
									  topgun_print_mes_n();
									*/
							}
						}
					}
				}
				else {
					topgun_print_mes_fsim_cannot_enter_line( line, FSIM_SA1 );
				}
			}

			if ( pattern_detect_mask != ALL_BIT_OFF ) {
					/* �ξ㤬�����ǽ�Ǥ���ʤ�� */

				//printf("F_SIM 3 START %ld\n",line->line_id);
				
				fprop_flag++; /* �ξ�ID�ߤ����ʤ�� �ξ㤴�Ȥν�����򤵤��� */

				/* �ξ�����ե饰 */
				line->fault_set_flag = pattern_detect_mask;

					/* ���٥�ȥꥹ�Ȥ���Ͽ */

					/* flag���ѹ� */
				line->mp_flag = FSIM_EVENT_ON;
				/* ��Ƭ����Ͽ���� */
				schedule_elist = & ( Event_list_head [ line->lv_pi ] );
				schedule_elist->event [ schedule_elist->n_event ] = line;
				schedule_elist->n_event++;
				
				topgun_print_mes_fsim_enter_line( line );

				fsim_start_level = line->lv_pi;

				/* ���ߥ�졼������»� */
				Fsim_head.detect_mask = ALL_BIT_OFF;

				for ( k = fsim_start_level ; k < Line_info.max_lv_pi ; k++ ) {

					cur_e_list = &( Event_list_head[ k ] );

					for ( m = 0 ; m < cur_e_list->n_event ; m++ ) {

						cur_line = cur_e_list->event[ m ] ;

						/* ���ο������Υ��٥�Ƚ�λ */
						cur_line->mp_flag = FSIM_EVENT_OFF;

						fsim_fault_value_val3[ cur_line->type ]( cur_line, fprop_flag );

						/* �ξ㿮��������ȿž������ */
						cur_line->f_val_a ^=  cur_line->fault_set_flag;
						cur_line->f_val_b ^=  cur_line->fault_set_flag;
						
						topgun_print_mes_fsim3_exec_line( cur_line );
						
						/* �ξ�����? */

						/* 3�ͤξ�������!!! 2006/10/12
						   $$$ �Ƽ��̤β�ǽ�����ʤ���С�����ifʸ�Ϥ��äƤ�褤 $$$
						if ( ( cur_line->n_val_a ^ cur_line->f_val_a ) &
							 ( cur_line->n_val_b ^ cur_line->f_val_b ) ) {
						*/
						
							//if ( cur_line->n_val_a != cur_line->f_val_a ) {
							/* �ξ�������!! */

							/* �ξ�������ե饰������ */
							cur_line->fprop_flag = fprop_flag;

							/* ���Ͽ������򥤥٥�ȥꥹ�Ȥ���Ͽ */

							for ( n = 0; n < cur_line->n_out ; n++ ) {

								event_line = cur_line->out[ n ];

								if ( event_line->mp_flag == FSIM_EVENT_OFF ) {
									/* ̤��Ͽ�ξ�� */

									event_line->mp_flag = FSIM_EVENT_ON;
									schedule_elist = & ( Event_list_head [ event_line->lv_pi ] );
									schedule_elist->event[ schedule_elist->n_event ] = event_line;
									schedule_elist->n_event++;
									
									topgun_print_mes_fsim_enter_line( event_line );
								}
							}
						/* 3�ͤξ�������!!! 2006/10/12							
						}
						*/
					}
					/* ���ߤΥ�٥�������٥�Ƚ�λ */
					cur_e_list->n_event = 0;
				}
				//printf("F_SIM 3 END\n");
	
				if ( Fsim_head.detect_mask ) {
					/* �ξ㤬���ФǤ��� */
					/* ����ƥ�����ѥ��ȥ졼���� */
					/* $$$ ���Ȥ� $$$ */
					

					/* ���Ф���Ͽ */
					if ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit ) {

						total_detect_bit |= ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit );
						
						if  ( ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DCD ) ) &&
							  ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DTR ) ) ) {
							/* �Ĥ�ATPG�оݸξ����︺����(check_end��) */
							Line_info.n_fault_atpg_count--;
							//printf("C-F3  : %8ld sa0\n", line->line_id);
						}
						/* sa0�ξ�� */
						/* val3�Ǹ��� */
						line->flist[FSIM_SA0]->info |= TOPGUN_DTX;

						topgun_print_mes_fsim_detect( line, FSIM_SA0 );

						/*
						{
							printf("F 3  : line %8ld", line->line_id);
							printf(" sa0\n");
						}
						*/
					}
						
					if ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit ) {

						total_detect_bit |= ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit );

						if ( ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DCD ) ) &&
							 ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DTR ) ) ){
							/* �Ĥ�ATPG�оݸξ����︺����(check_end��) */
							Line_info.n_fault_atpg_count--;
							//printf("C-F3  : %8ld sa1\n", line->line_id);
						}
							
						/* sa1�ξ�� */
						/* val3�Ǹ��� */
						line->flist[FSIM_SA1]->info |= TOPGUN_DTX;

						topgun_print_mes_fsim_detect( line, FSIM_SA1 );
						/*
						{
							printf("F 3 : line %8ld", line->line_id);
							printf(" sa1\n");
						}
						*/
					}
				}
				else {
					/* �ξ㤬���ФǤ��ʤ��ä� */
					if ( detect_sa0_flag & Fsim_head.detect_mask ) {
						topgun_print_mes_fsim_not_detect(line, FSIM_SA0);
					}
					if ( detect_sa1_flag & Fsim_head.detect_mask ) {
						topgun_print_mes_fsim_not_detect(line, FSIM_SA1);
					}
				}
				
				/* �ξ�����ե饰 */
				line->fault_set_flag = ALL_BIT_OFF;
			}
			else {
				topgun_print_mes_fsim_not_insert(line);
			}
		}
	}
	//fsim_info->num_pat  += utility_count_1_bit ( total_detect_bit );
	
	topgun_3_utility_enter_end_time( &Time_head.sim_val3 );
	topgun_3_utility_enter_end_time( &Time_head.fault_sim );
}

	
void fsim_logic_sim_val2_init()
{

	fsim_logic_sim_val2[TOPGUN_PI]   = &fsim_logic_sim_val2_pi;
	fsim_logic_sim_val2[TOPGUN_PO]   = &fsim_logic_sim_val2_in1;
	fsim_logic_sim_val2[TOPGUN_BR]   = &fsim_logic_sim_val2_in1;
	fsim_logic_sim_val2[TOPGUN_BUF]  = &fsim_logic_sim_val2_in1;
	fsim_logic_sim_val2[TOPGUN_INV]  = &fsim_logic_sim_val2_inv;
	fsim_logic_sim_val2[TOPGUN_AND]  = &fsim_logic_sim_val2_and;
	fsim_logic_sim_val2[TOPGUN_NAND] = &fsim_logic_sim_val2_nand;
	fsim_logic_sim_val2[TOPGUN_OR]   = &fsim_logic_sim_val2_or;
	fsim_logic_sim_val2[TOPGUN_NOR]  = &fsim_logic_sim_val2_nor;
	fsim_logic_sim_val2[TOPGUN_XOR]  = &fsim_logic_sim_val2_xor;
	fsim_logic_sim_val2[TOPGUN_XNOR] = &fsim_logic_sim_val2_xnor;
	fsim_logic_sim_val2[TOPGUN_BLKI] = &fsim_logic_sim_val2_error;
	fsim_logic_sim_val2[TOPGUN_BLKO] = &fsim_logic_sim_val2_in1;
	fsim_logic_sim_val2[TOPGUN_UNK]  = &fsim_logic_sim_val2_error;
	
}

void fsim_fault_value_val2_init()
{

	fsim_fault_value_val2[TOPGUN_PI]   = &fsim_fault_value_val2_pi;
	fsim_fault_value_val2[TOPGUN_PO]   = &fsim_fault_value_val2_po;
	fsim_fault_value_val2[TOPGUN_BUF]  = &fsim_fault_value_val2_in1;
	fsim_fault_value_val2[TOPGUN_INV]  = &fsim_fault_value_val2_inv;
	fsim_fault_value_val2[TOPGUN_BR]   = &fsim_fault_value_val2_in1;
	fsim_fault_value_val2[TOPGUN_AND]  = &fsim_fault_value_val2_and;
	fsim_fault_value_val2[TOPGUN_NAND] = &fsim_fault_value_val2_nand;
	fsim_fault_value_val2[TOPGUN_OR]   = &fsim_fault_value_val2_or;
	fsim_fault_value_val2[TOPGUN_NOR]  = &fsim_fault_value_val2_nor;
	fsim_fault_value_val2[TOPGUN_XOR]  = &fsim_fault_value_val2_xor;
	fsim_fault_value_val2[TOPGUN_XNOR] = &fsim_fault_value_val2_xnor;
	fsim_fault_value_val2[TOPGUN_BLKI] = &fsim_fault_value_val2_error;
	fsim_fault_value_val2[TOPGUN_BLKO] = &fsim_fault_value_val2_in1;
	fsim_fault_value_val2[TOPGUN_UNK]  = &fsim_fault_value_val2_error;
		
}

void fsim_logic_sim_val3_init()
{

	fsim_logic_sim_val3[TOPGUN_PI]   = &fsim_logic_sim_val3_pi;
	fsim_logic_sim_val3[TOPGUN_PO]   = &fsim_logic_sim_val3_in1;
	fsim_logic_sim_val3[TOPGUN_BR]   = &fsim_logic_sim_val3_in1;
	fsim_logic_sim_val3[TOPGUN_BUF]  = &fsim_logic_sim_val3_in1;
	fsim_logic_sim_val3[TOPGUN_INV]  = &fsim_logic_sim_val3_inv;
	fsim_logic_sim_val3[TOPGUN_AND]  = &fsim_logic_sim_val3_and;
	fsim_logic_sim_val3[TOPGUN_NAND] = &fsim_logic_sim_val3_nand;
	fsim_logic_sim_val3[TOPGUN_OR]   = &fsim_logic_sim_val3_or;
	fsim_logic_sim_val3[TOPGUN_NOR]  = &fsim_logic_sim_val3_nor;
	fsim_logic_sim_val3[TOPGUN_XOR]  = &fsim_logic_sim_val3_xor;
	fsim_logic_sim_val3[TOPGUN_XNOR] = &fsim_logic_sim_val3_xnor;
	fsim_logic_sim_val3[TOPGUN_BLKI] = &fsim_logic_sim_val3_error;
	fsim_logic_sim_val3[TOPGUN_BLKO] = &fsim_logic_sim_val3_in1;
	fsim_logic_sim_val3[TOPGUN_UNK]  = &fsim_logic_sim_val3_error;
	
}

void fsim_fault_value_val3_init()
{

	fsim_fault_value_val3[TOPGUN_PI]   = &fsim_fault_value_val3_pi;
	fsim_fault_value_val3[TOPGUN_PO]   = &fsim_fault_value_val3_po;
	fsim_fault_value_val3[TOPGUN_BUF]  = &fsim_fault_value_val3_in1;
	fsim_fault_value_val3[TOPGUN_INV]  = &fsim_fault_value_val3_inv;
	fsim_fault_value_val3[TOPGUN_BR]   = &fsim_fault_value_val3_in1;
	fsim_fault_value_val3[TOPGUN_AND]  = &fsim_fault_value_val3_and;
	fsim_fault_value_val3[TOPGUN_NAND] = &fsim_fault_value_val3_nand;
	fsim_fault_value_val3[TOPGUN_OR]   = &fsim_fault_value_val3_or;
	fsim_fault_value_val3[TOPGUN_NOR]  = &fsim_fault_value_val3_nor;
	fsim_fault_value_val3[TOPGUN_XOR]  = &fsim_fault_value_val3_xor;
	fsim_fault_value_val3[TOPGUN_XNOR] = &fsim_fault_value_val3_xnor;
	fsim_fault_value_val3[TOPGUN_BLKI] = &fsim_fault_value_val3_error;
	fsim_fault_value_val3[TOPGUN_BLKO] = &fsim_fault_value_val3_in1;
	fsim_fault_value_val3[TOPGUN_UNK]  = &fsim_fault_value_val3_error;
		
}


/********************************************************************************
 * << Function >>
 *		PI��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_pi
(
 LINE *line
 ){
	/* ���⤷�ʤ� */
	
}

/********************************************************************************
 * << Function >>
 *		1������ž��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_in1
(
 LINE *line
 ){
	char *func_name = "fsim_logic_sim_val2_in1";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[0]->n_val_a, func_name );

	/* ���������ͤ���� */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	
}


/********************************************************************************
 * << Function >>
 *		1����ȿž��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_inv
(
 LINE *line
 ){

	char *func_name = "fsim_logic_sim_val2_inv";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[0]->n_val_a, func_name );
	
	/* ���������ͤ����(ȿž������) */
	line->n_val_a = ~( line->in[ 0 ]->n_val_a );
	
}

/********************************************************************************
 * << Function >>
 *		AND��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_and
(
 LINE *line
 ){

	Ulong i; /* ���Ͽ������� */
	char *func_name = "fsim_logic_sim_val2_and";

	test_line_in_null ( line, func_name );
	
	/* ���������ͤ���� */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	test_line_value ( line->in[0]->n_val_a, func_name );

	for ( i = 1 ; i < line->n_in ; i++ ) {
		line->n_val_a &= line->in[ i ]->n_val_a;
		test_line_value ( line->in[i]->n_val_a, func_name );
	}

	test_line_value ( line->n_val_a, func_name );
}

/********************************************************************************
 * << Function >>
 *		NAND��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_nand
(
 LINE *line
 ){

	Ulong i; /* ���Ͽ������� */
	char *func_name = "fsim_logic_sim_val2_nand";

	test_line_in_null ( line, func_name );

	/* ���������ͤ���� */
	/* �ޤ�AND��ʬ */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	test_line_value ( line->in[0]->n_val_a, func_name );
		
	for ( i = 1 ; i < line->n_in ; i++ ) {
		line->n_val_a &= line->in[ i ]->n_val_a;
		test_line_value ( line->in[i]->n_val_a, func_name );
	}
	/* inv��ʬ */
	line->n_val_a = ~( line->n_val_a );
	topgun_test_line_value ( line->n_val_a, func_name );
}


/********************************************************************************
 * << Function >>
 *		OR��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_or
(
 LINE *line
 ){

	Ulong i; /* ���Ͽ������� */
	char *func_name = "fsim_logic_sim_val2_or";

	test_line_in_null ( line, func_name );

	/* ���������ͤ���� */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	test_line_value ( line->in[0]->n_val_a, func_name );

	for ( i = 1 ; i < line->n_in ; i++ ) {
		line->n_val_a |= line->in[ i ]->n_val_a;
		test_line_value ( line->in[i]->n_val_a, func_name );
	}
	test_line_value ( line->n_val_a, func_name );
}


/********************************************************************************
 * << Function >>
 *		NOR��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_nor
(
 LINE *line
 ){

	Ulong i; /* ���Ͽ������� */
	char *func_name = "fsim_logic_sim_val2_nor";

	test_line_in_null ( line, func_name );

	/* ���������ͤ���� */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	test_line_value ( line->in[0]->n_val_a, func_name );

	for ( i = 1 ; i < line->n_in ; i++ ) {
		line->n_val_a |= line->in[ i ]->n_val_a;
		test_line_value ( line->in[i]->n_val_a, func_name );
	}
	/* inv��ʬ */
	line->n_val_a = ~( line->n_val_a );
	test_line_value ( line->n_val_a, func_name );
}

/********************************************************************************
 * << Function >>
 *		XOR��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_xor
(
 LINE *line
 ){

	char *func_name = "fsim_logic_sim_val2_xor";

	test_line_in_null ( line, func_name );

	/* ���������ͤ���� */
	line->n_val_a = ( line->in[ 0 ]->n_val_a ) ^ ( line->in[ 1 ]->n_val_a ) ;

	test_line_value ( line->in[0]->n_val_a, func_name );
	test_line_value ( line->in[1]->n_val_a, func_name );
	test_line_value ( line->n_val_a, func_name );
	
}

/********************************************************************************
 * << Function >>
 *		XNOR��������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_xnor
(
 LINE *line
 ){

	char *func_name = "fsim_logic_sim_val2_xnor";

	test_line_in_null ( line, func_name );

	/* ���������ͤ���� */
	line->n_val_a = ( line->in[ 0 ]->n_val_a ) ^ ( line->in[ 1 ]->n_val_a ) ;

	/* inv��ʬ */
	line->n_val_a = ~( line->n_val_a );

	test_line_value ( line->in[0]->n_val_a, func_name );
	test_line_value ( line->in[1]->n_val_a, func_name );
	test_line_value ( line->n_val_a, func_name );
}

/********************************************************************************
 * << Function >>
 *		���б���������2�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val2_error
(
 LINE *line
 ){

	char *func_name = "fsim_logic_sim_val2_xnor";
	
	/* 2�ͤ�����Sim��blki, unknown�����äƤϤʤ�ʤ� */
	topgun_error( FEC_FSIM_VAL2_BLKI, func_name );
}

/********************************************************************************
 * << Function >>
 *		PI��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_pi
(
 LINE *line,
 Ulong flag
 ){
	line->f_val_a = line->n_val_a;
}

/********************************************************************************
 * << Function >>
 *		1���ϥ�������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_po
(
 LINE *line,
 Ulong flag
 ){

	char *func_name = "fsim_fault_value_val2_po";

	test_line_in_null ( line, func_name );

	/* ���Ϥ˸ξ㤬���¤��Ƥ����� */
	/* in0��ɬ��in[0]�����¤��Ƥ��� */
	if ( line->in[ 0 ]->fprop_flag == flag ) {
		line->f_val_a = line->in[ 0 ]->f_val_a;
	}
	else {
		line->f_val_a = line->in[ 0 ]->n_val_a;
	}
	/*
	printf("before mask ");
	topgun_print_mes_val2( Fsim_head.detect_mask);
	printf("\n");
	printf("normal      ");
	topgun_print_mes_val2( line->n_val_a );
	printf("\n");
	printf("failure     ");
	topgun_print_mes_val2( line->f_val_a );
	printf("\n");
	printf("set flag    ");
	topgun_print_mes_val2( line->fault_set_flag );
	printf("\n");
	*/


	
	/* �ξ㸡�Хޥ����� (�ξ���¬�Ǥ���ս�=PO�Τ�¸��)*/
	Fsim_head.detect_mask |= ( line->n_val_a ^ ( line->f_val_a ^ line->fault_set_flag ));

	/*
	printf("after mask  ");
	topgun_print_mes_val2( Fsim_head.detect_mask);
	printf("\n");
	*/
	
	
}

/********************************************************************************
 * << Function >>
 *		1���ϥ�������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_in1
(
 LINE *line,
 Ulong flag
 ){

	/* buffer, branch */
	char *func_name = "fsim_fault_value_val2_in1";

	test_line_in_null ( line, func_name );

	/* ���Ϥ˸ξ㤬���¤��Ƥ����� */
	/* in0��ɬ��in[0]�����¤��Ƥ��� */
	if ( line->in[ 0 ]->fprop_flag == flag ) {
		line->f_val_a = line->in[ 0 ]->f_val_a;
	}
	else {
		line->f_val_a = line->in[ 0 ]->n_val_a;
	}

}

/********************************************************************************
 * << Function >>
 *		inv��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_inv
(
 LINE *line,
 Ulong flag
 ){

	char *func_name = "fsim_fsim_sim_val2_inv";

	test_line_in_null ( line, func_name );

	
	if ( line->in[ 0 ]->fprop_flag == flag ) {
		line->f_val_a = ~( line->in[ 0 ]->f_val_a ) ;
	}
	else {
		line->f_val_a = ~( line->in[ 0 ]->n_val_a );
	}
}

/********************************************************************************
 * << Function >>
 *		and��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_and
(
 LINE *line,
 Ulong flag
 ){

	LINE *in;
	Ulong i;

	char *func_name = "fsim_fault_value_val2_and";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];


	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		line->f_val_a = in->f_val_a;
	}
	else {
		line->f_val_a = in->n_val_a;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a &= in->f_val_a;
		}
		else {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a &= in->n_val_a;
		}
	}
}	


/********************************************************************************
 * << Function >>
 *		nand��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_nand
(
 LINE *line,
 Ulong flag
 ){

	LINE  *in;
	Ulong i;
	
	char *func_name = "fsim_fsim_sim_val2_nand";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];


	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		line->f_val_a = in->f_val_a;
	}
	else {
		line->f_val_a = in->n_val_a;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a &= in->f_val_a;
		}
		else {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a &= in->n_val_a;
		}
	}

	/* �Ǹ��ȿž������ */
	line->f_val_a = ~( line->f_val_a );
	
}

/********************************************************************************
 * << Function >>
 *		or��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_or
(
 LINE *line,
 Ulong flag
 ){
	LINE *in;
	Ulong i;

	char *func_name = "fsim_fault_value_val2_or";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];


	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		line->f_val_a = in->f_val_a;
	}
	else {
		line->f_val_a = in->n_val_a;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a |= in->f_val_a;
		}
		else {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a |= in->n_val_a;
		}
	}
}

/********************************************************************************
 * << Function >>
 *		nor��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_nor
(
 LINE *line,
 Ulong flag
 ){
	LINE *in;
	Ulong i;

	char *func_name = "fsim_fault_value_val2_nor";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];


	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		line->f_val_a = in->f_val_a;
	}
	else {
		line->f_val_a = in->n_val_a;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a |= in->f_val_a;
		}
		else {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a |= in->n_val_a;
		}
	}
	
	/* �Ǹ��ȿž������ */
	line->f_val_a = ~( line->f_val_a );
	
}

/********************************************************************************
 * << Function >>
 *		or��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_xor
(
 LINE *line,
 Ulong flag
 ){

	LINE *in0, *in1;
	Ulong in0_f_val_a, in1_f_val_a;

	char *func_name = "fsim_fault_value_val2_xor";

	test_line_in_null ( line, func_name );

	in0 = line->in[ 0 ];
	in1 = line->in[ 1 ];

	/* in0 */
	if ( in0->fprop_flag == flag ) {
		in0_f_val_a = in0->f_val_a;
	}
	else {
		in0_f_val_a = in0->n_val_a;
	}

	/* in1 */
	if ( in1->fprop_flag == flag ) {
		in1_f_val_a = in1->f_val_a;
	}
	else {
		in1_f_val_a = in1->n_val_a;
	}

	/* XOR��Ȥ� */
	line->f_val_a = ( in0_f_val_a ^ in1_f_val_a );
}

/********************************************************************************
 * << Function >>
 *		or��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_xnor
(
 LINE *line,
 Ulong flag
 ){
	
	LINE *in0, *in1;
	Ulong in0_f_val_a, in1_f_val_a;

	char *func_name = "fsim_fault_value_val2_xor";

	test_line_in_null ( line, func_name );

	in0 = line->in[ 0 ];
	in1 = line->in[ 1 ];

	/* in0 */
	if ( in0->fprop_flag == flag ) {
		in0_f_val_a = in0->f_val_a;
	}
	else {
		in0_f_val_a = in0->n_val_a;
	}

	/* in1 */
	if ( in1->fprop_flag == flag ) {
		in1_f_val_a = in1->f_val_a;
	}
	else {
		in1_f_val_a = in1->n_val_a;
	}

	/* XOR��ȤäƺǸ��ȿž������ */
	line->f_val_a = ~( in0_f_val_a ^ in1_f_val_a );
}

/********************************************************************************
 * << Function >>
 *		or��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val2_error
(
 LINE *line,
 Ulong flag
 ){

	char *func_name = "fsim_fault_value_val2_error";

	/* 2�ͤ�����Sim��blki, unknown�����äƤϤʤ�ʤ� */
	topgun_error( FEC_FSIM_VAL2_BLKI, func_name );
}

/********************************************************************************
 * << Function >>
 *		PI��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val3_pi
(
 LINE *line
 ){
	/* ���⤷�ʤ� */
	
}

/********************************************************************************
 * << Function >>
 *		1������ž��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/09/13
 *
 ********************************************************************************/

void fsim_logic_sim_val3_in1
(
 LINE *line
 ){
	char *func_name = "fsim_logic_sim_val3_in1";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[0]->n_val_a, func_name );
	test_line_value ( line->in[0]->n_val_b, func_name );

	/* ���������ͤ���� */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	line->n_val_b = line->in[ 0 ]->n_val_b;
	
}


/********************************************************************************
 * << Function >>
 *		1����ȿž��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/09/13
 *
 ********************************************************************************/

void fsim_logic_sim_val3_inv
(
 LINE *line
 ){

	char *func_name = "fsim_logic_sim_val3_inv";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[0]->n_val_a, func_name );
	test_line_value ( line->in[0]->n_val_b, func_name );
	
	/* ���������ͤ����(ȿž������) */
	line->n_val_a = line->in[ 0 ]->n_val_b;
	line->n_val_b = line->in[ 0 ]->n_val_a;
	
}

/********************************************************************************
 * << Function >>
 *		AND��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val3_and
(
 LINE *line
 ){

	Ulong i; /* ���Ͽ������� */
	char *func_name = "fsim_logic_sim_val3_and";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[ 0 ]->n_val_a, func_name );
	test_line_value ( line->in[ 0 ]->n_val_b, func_name );
	
	/* ���������ͤ���� */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	line->n_val_b = line->in[ 0 ]->n_val_b;


	for ( i = 1 ; i < line->n_in ; i++ ) {
		test_line_value ( line->in[ i ]->n_val_a, func_name );
		test_line_value ( line->in[ i ]->n_val_b, func_name );
		
		line->n_val_a &= line->in[ i ]->n_val_a;
		line->n_val_b |= line->in[ i ]->n_val_b;

	}
	test_line_value ( line->n_val_a, func_name );
	test_line_value ( line->n_val_b, func_name );
}

/********************************************************************************
 * << Function >>
 *		NAND��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/09/13
 *
 ********************************************************************************/

void fsim_logic_sim_val3_nand
(
 LINE *line
 ){

	Ulong n_val_a;
	Ulong n_val_b;
	Ulong i; /* ���Ͽ������� */
	char *func_name = "fsim_logic_sim_val3_nand";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[ 0 ]->n_val_a, func_name );
	test_line_value ( line->in[ 0 ]->n_val_b, func_name );
	
	/* ���������ͤ���� */
	n_val_a = line->in[ 0 ]->n_val_a;
	n_val_b = line->in[ 0 ]->n_val_b;


	for ( i = 1 ; i < line->n_in ; i++ ) {
		test_line_value ( line->in[ i ]->n_val_a, func_name );
		test_line_value ( line->in[ i ]->n_val_b, func_name );
		
		n_val_a &= line->in[ i ]->n_val_a;
		n_val_b |= line->in[ i ]->n_val_b;

	}
	
	/* nand�ʤΤ�ȿž������ */
	line->n_val_a = n_val_b;
	line->n_val_b = n_val_a;
	
	test_line_value ( line->n_val_a, func_name );
	test_line_value ( line->n_val_b, func_name );
}


/********************************************************************************
 * << Function >>
 *		OR��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val3_or
(
 LINE *line
 ){

	Ulong i; /* ���Ͽ������� */
	char *func_name = "fsim_logic_sim_val3_or";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[ 0 ]->n_val_a, func_name );
	test_line_value ( line->in[ 0 ]->n_val_b, func_name );
	
	/* ���������ͤ���� */
	line->n_val_a = line->in[ 0 ]->n_val_a;
	line->n_val_b = line->in[ 0 ]->n_val_b;


	for ( i = 1 ; i < line->n_in ; i++ ) {
		test_line_value ( line->in[ i ]->n_val_a, func_name );
		test_line_value ( line->in[ i ]->n_val_b, func_name );
		
		line->n_val_a |= line->in[ i ]->n_val_a;
		line->n_val_b &= line->in[ i ]->n_val_b;

	}
	test_line_value ( line->n_val_a, func_name );
	test_line_value ( line->n_val_b, func_name );

}


/********************************************************************************
 * << Function >>
 *		NOR��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val3_nor
(
 LINE *line
 ){

	Ulong n_val_a; /* ����Sim�η�� */
	Ulong n_val_b; /* ����Sim�η�� */
	Ulong i; /* ���Ͽ������� */
	static char *func_name = "fsim_logic_sim_val3_nor";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[ 0 ]->n_val_a, func_name );
	test_line_value ( line->in[ 0 ]->n_val_b, func_name );
	
	/* ���������ͤ���� */
	n_val_a = line->in[ 0 ]->n_val_a;
	n_val_b = line->in[ 0 ]->n_val_b;


	for ( i = 1 ; i < line->n_in ; i++ ) {
		test_line_value ( line->in[ i ]->n_val_a, func_name );
		test_line_value ( line->in[ i ]->n_val_b, func_name );
		
		n_val_a |= line->in[ i ]->n_val_a;
		n_val_b &= line->in[ i ]->n_val_b;

	}

	/* nor�ʤΤǷ�̤�ȿž������ */
	line->n_val_a = n_val_b;
	line->n_val_b = n_val_a;
	
	test_line_value ( line->n_val_a, func_name );
	test_line_value ( line->n_val_b, func_name );
}

/********************************************************************************
 * << Function >>
 *		XOR��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val3_xor
(
 LINE *line
 ){

	char *func_name = "fsim_logic_sim_val3_xor";

	test_line_in_null ( line, func_name );
	test_line_value ( line->in[0]->n_val_a, func_name );
	test_line_value ( line->in[1]->n_val_a, func_name );

	/* ���������ͤ���� */
	line->n_val_a =  ( ( ( line->in[ 0 ]->n_val_a ) & ( line->in[ 1 ]->n_val_b ) ) |
					   ( ( line->in[ 0 ]->n_val_b ) & ( line->in[ 1 ]->n_val_a ) ) );
	line->n_val_b =  ( ( ( line->in[ 0 ]->n_val_a ) & ( line->in[ 0 ]->n_val_b ) )|
					   ( ( line->in[ 1 ]->n_val_a ) & ( line->in[ 1 ]->n_val_b ) ) );
					   
	test_line_value ( line->n_val_a, func_name );
	
}

/********************************************************************************
 * << Function >>
 *		XNOR��������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val3_xnor
(
 LINE *line
 ){

	Ulong n_val_a;
	Ulong n_val_b;
	char *func_name = "fsim_logic_sim_val3_xnor";

	test_line_in_null ( line, func_name );

	test_line_value ( line->in[0]->n_val_a, func_name );
	test_line_value ( line->in[1]->n_val_a, func_name );

	/* ���������ͤ���� */
	n_val_a =  ( ( ( line->in[ 0 ]->n_val_a ) & ( line->in[ 1 ]->n_val_b ) ) |
				 ( ( line->in[ 0 ]->n_val_b ) & ( line->in[ 1 ]->n_val_a ) ) );
	n_val_b =  ( ( ( line->in[ 0 ]->n_val_a ) & ( line->in[ 0 ]->n_val_b ) ) |
				 ( ( line->in[ 1 ]->n_val_a ) & ( line->in[ 1 ]->n_val_b ) ) );
					   
	test_line_value ( line->n_val_a, func_name );

	/* inv��ʬ */
	line->n_val_a =	line->n_val_b;
	line->n_val_b = line->n_val_a;

}

/********************************************************************************
 * << Function >>
 *		���б���������3�ͤ��������ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_logic_sim_val3_error
(
 LINE *line
 ){

	char *func_name = "fsim_logic_sim_val3_xnor";
	
	/* 2�ͤ�����Sim��blki, unknown�����äƤϤʤ�ʤ� */
	topgun_error( FEC_FSIM_VAL3_BLKI, func_name );
}

/********************************************************************************
 * << Function >>
 *		PI��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_pi
(
 LINE *line,
 Ulong flag
 ){
	line->f_val_a = line->n_val_a;
	line->f_val_b = line->n_val_b;
}

/********************************************************************************
 * << Function >>
 *		1���ϥ�������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_po
(
 LINE *line,
 Ulong flag
 ){

	Ulong f_val_a;
	Ulong f_val_b;

	Ulong fault_set_flag = line->fault_set_flag;

	char *func_name = "fsim_fault_value_val3_po";

	test_line_in_null ( line, func_name );

	if ( line->in[ 0 ]->fprop_flag == flag ) {
		/* ���Ϥ˸ξ㤬���¤��Ƥ����� */
		line->f_val_a = line->in[ 0 ]->f_val_a;
		line->f_val_b = line->in[ 0 ]->f_val_b;
	}
	else {
		line->f_val_a = line->in[ 0 ]->n_val_a;
		line->f_val_b = line->in[ 0 ]->n_val_b;

	}

	/*
	printf("before mask ");
	topgun_print_mes_val3( Fsim_head.detect_mask);
	printf("\n");
	printf("normal      ");
	topgun_print_mes_val3( line->n_val_a );
	printf("\n");
	printf("failure     ");
	topgun_print_mes_val3( line->f_val_a );
	printf("\n");
	printf("set flag    ");
	topgun_print_mes_val3( line->fault_set_flag );
	printf("\n");
	*/


	f_val_a = ( ( line->f_val_a & ~fault_set_flag ) | ( line->f_val_b &  fault_set_flag ) );
	f_val_b = ( ( line->f_val_a &  fault_set_flag ) | ( line->f_val_b & ~fault_set_flag ) );
	
	
	/* �ξ㸡�Хޥ����� (�ξ���¬�Ǥ���ս�=PO�Τ�¸��)*/
	Fsim_head.detect_mask |= ( ( line->n_val_a & f_val_b ) | ( line->n_val_b & f_val_a ) );
	//Fsim_head.pd_mask     |= ( ( line->n_val_a & ~f_val_a & ~f_val_b ) | ( line->n_val_b & ~f_val_a & ~f_val_b ) );
	
	/*
	printf("after mask  ");
	topgun_print_mes_val3( Fsim_head.detect_mask);
	printf("\n");
	*/
	
	
}

/********************************************************************************
 * << Function >>
 *		1���ϥ�������3�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_in1
(
 LINE *line,
 Ulong flag
 ){

	/* buffer, branch */
	char *func_name = "fsim_fault_value_val3_in1";

	test_line_in_null ( line, func_name );

	/* ���Ϥ˸ξ㤬���¤��Ƥ����� */
	/* in0��ɬ��in[0]�����¤��Ƥ��� */
	if ( line->in[ 0 ]->fprop_flag == flag ) {
		line->f_val_a = line->in[ 0 ]->f_val_a;
		line->f_val_b = line->in[ 0 ]->f_val_b;
	}
	else {
		line->f_val_a = line->in[ 0 ]->n_val_a;
		line->f_val_b = line->in[ 0 ]->n_val_b;
	}

}

/********************************************************************************
 * << Function >>
 *		inv��������3�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_inv
(
 LINE *line,
 Ulong flag
 ){

	char *func_name = "fsim_fsim_sim_val3_inv";

	test_line_in_null ( line, func_name );

	
	if ( line->in[ 0 ]->fprop_flag == flag ) {
		line->f_val_a = line->in[ 0 ]->f_val_b ;
		line->f_val_b = line->in[ 0 ]->f_val_a ;
	}
	else {
		line->f_val_a = line->in[ 0 ]->n_val_b;
		line->f_val_b = line->in[ 0 ]->n_val_a;
	}
}

/********************************************************************************
 * << Function >>
 *		and��������3�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_and
(
 LINE *line,
 Ulong flag
 ){

	LINE *in;
	Ulong i;

	char *func_name = "fsim_fault_value_val3_and";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];

	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		line->f_val_a = in->f_val_a;
		line->f_val_b = in->f_val_b;
	}
	else {
		line->f_val_a = in->n_val_a;
		line->f_val_b = in->n_val_b;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a &= in->f_val_a;
			line->f_val_b |= in->f_val_b;
		}
		else {
			line->f_val_a &= in->n_val_a;
			line->f_val_b |= in->n_val_b;
		}
	}
}	


/********************************************************************************
 * << Function >>
 *		nand��������3�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_nand
(
 LINE *line,
 Ulong flag
 ){

	LINE  *in;
	Ulong i;
	Ulong f_val_a;
	Ulong f_val_b;
	
	char *func_name = "fsim_fsim_sim_val3_nand";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];


	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		f_val_a = in->f_val_a;
		f_val_b = in->f_val_b;
	}
	else {
		f_val_a = in->n_val_a;
		f_val_b = in->n_val_b;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			f_val_a &= in->f_val_a;
			f_val_b |= in->f_val_b;
		}
		else {
			f_val_a &= in->n_val_a;
			f_val_b |= in->n_val_b;
		}
	}

	/* �Ǹ��ȿž������ */
	line->f_val_a = f_val_b;
	line->f_val_b = f_val_a;
	
}

/********************************************************************************
 * << Function >>
 *		or��������3�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_or
(
 LINE *line,
 Ulong flag
 ){
	LINE *in;
	Ulong i;

	char *func_name = "fsim_fault_value_val3_or";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];


	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		line->f_val_a = in->f_val_a;
		line->f_val_b = in->f_val_b;
	}
	else {
		line->f_val_a = in->n_val_a;
		line->f_val_b = in->n_val_b;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a |= in->f_val_a;
			line->f_val_b &= in->f_val_b;
		}
		else {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			line->f_val_a |= in->n_val_a;
			line->f_val_b &= in->n_val_b;
		}
	}
}

/********************************************************************************
 * << Function >>
 *		nor��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_nor
(
 LINE *line,
 Ulong flag
 ){
	LINE *in;
	Ulong i;
	Ulong f_val_a;
	Ulong f_val_b;

	char *func_name = "fsim_fault_value_val3_nor";

	test_line_in_null ( line, func_name );

	in = line->in[ 0 ];


	/* ����ͤ����� */
	/* $$$ ^���Ĥ��ޤ�����ifʸ�ä����� */
	if ( in->fprop_flag == flag ){
		/* ����[0]�˸ξ㤬���¤��Ƥ����� */
		f_val_a = in->f_val_a;
		f_val_b = in->f_val_b;
	}
	else {
		f_val_a = in->n_val_a;
		f_val_b = in->n_val_b;
	}

	for ( i = 1 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];

		if ( in->fprop_flag == flag ) {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			f_val_a |= in->f_val_a;
			f_val_b &= in->f_val_b;
		}
		else {
			/* ����[i]�˸ξ㤬���¤��Ƥ����� */
			f_val_a |= in->n_val_a;
			f_val_b &= in->n_val_b;
		}
	}
	
	/* �Ǹ��ȿž������ */
	line->f_val_a = f_val_b;
	line->f_val_b = f_val_a;
	
}

/********************************************************************************
 * << Function >>
 *		xor��������3�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_xor
(
 LINE *line,
 Ulong flag
 ){

	LINE *in0, *in1;
	Ulong in0_f_val_a, in0_f_val_b;
	Ulong in1_f_val_a, in1_f_val_b;

	char *func_name = "fsim_fault_value_val3_xor";

	test_line_in_null ( line, func_name );

	in0 = line->in[ 0 ];
	in1 = line->in[ 1 ];

	/* in0 */
	if ( in0->fprop_flag == flag ) {
		in0_f_val_a = in0->f_val_a;
		in0_f_val_b = in0->f_val_b;
	}
	else {
		in0_f_val_a = in0->n_val_a;
		in0_f_val_b = in0->n_val_b;
	}

	/* in1 */
	if ( in1->fprop_flag == flag ) {
		in1_f_val_a = in1->f_val_a;
		in1_f_val_b = in1->f_val_b;
	}
	else {
		in1_f_val_a = in1->n_val_a;
		in1_f_val_b = in1->n_val_b;
	}

	/* XOR��Ȥ� */
	line->f_val_a =  ( ( ( in0_f_val_a ) & ( in1_f_val_b ) ) |
					   ( ( in0_f_val_b ) & ( in1_f_val_a ) ) );
	line->f_val_b =  ( ( ( in0_f_val_a ) & ( in0_f_val_b ) ) |
					   ( ( in1_f_val_a ) & ( in1_f_val_b ) ) );
}

/********************************************************************************
 * << Function >>
 *		xnor��������2�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_xnor
(
 LINE *line,
 Ulong flag
 ){
	
	LINE *in0, *in1;
	Ulong in0_f_val_a, in0_f_val_b;
	Ulong in1_f_val_a, in1_f_val_b;

	char *func_name = "fsim_fault_value_val3_xor";

	test_line_in_null ( line, func_name );

	in0 = line->in[ 0 ];
	in1 = line->in[ 1 ];

	/* in0 */
	if ( in0->fprop_flag == flag ) {
		in0_f_val_a = in0->f_val_a;
		in0_f_val_b = in0->f_val_b;
	}
	else {
		in0_f_val_a = in0->n_val_a;
		in0_f_val_b = in0->n_val_b;
	}

	/* in1 */
	if ( in1->fprop_flag == flag ) {
		in1_f_val_a = in1->f_val_a;
		in1_f_val_b = in1->f_val_b;
	}
	else {
		in1_f_val_a = in1->n_val_a;
		in1_f_val_b = in1->n_val_b;
	}

	/* XOR��ȤäƺǸ��ȿž������ */
	line->f_val_b =  ( ( ( in0_f_val_a ) & ( in1_f_val_b ) ) |
					   ( ( in0_f_val_b ) & ( in1_f_val_a ) ) ) ;
	line->f_val_a =  ( ( ( in0_f_val_a ) & ( in0_f_val_b ) ) |
					   ( ( in1_f_val_a ) & ( in1_f_val_b ) ) );
}

/********************************************************************************
 * << Function >>
 *		���б���������3�ͤθξ㥷�ߥ�졼����� 
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/05
 *
 ********************************************************************************/

void fsim_fault_value_val3_error
(
 LINE *line,
 Ulong flag
 ){

	char *func_name = "fsim_fault_value_val3_error";

	/* 3�ͤ�����Sim��blki, unknown�����äƤϤʤ�ʤ� */
	topgun_error( FEC_FSIM_VAL3_BLKI, func_name );
}


/********************************************************************************
 * << Function >>
 *		Fsim�Ѥ�flag�򸵤ˤ�ɤ�
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/27
 *
 ********************************************************************************/

void fsim_clear_line_flag
(
 void
 ){
	Ulong i; /* line�� */
	LINE  *line; 

	for ( i = 0 ; i < Line_info.n_line ; i++ ) {
		line = & ( Line_head[ i ] );
		/* OFF�ˤ��� */
		line->fprop_flag = NO_PROP_FAULT;
		
		/* ��bit OFF�ˤ��� */
		line->fault_set_flag = ALL_BIT_OFF;

		/* ���٥�Ⱦ��֤ν���� */
		line->mp_flag = FSIM_EVENT_OFF;
	}
}
	

/********************************************************************************
 * << Function >>
 *		������ѥ��������������
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/27
 *
 ********************************************************************************/

void fsim_generate_random_pattern_val2
(
 Ulong n_cc_pat, /* �ѥ���� */
 CC_PATTERN *cc_pat //stock�� 
){
	Ulong i; /* �ƥ��ȥѥ������ */
	Ulong j; /* �������Ͽ� */

	Ulong bit_1_set   = 0;

	LINE  *pi_line = NULL; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */

	/* ����� */
	for ( i = 0 ; i < Line_info.n_pi; i++ ) {
		/* 2�ͤʤΤ�n_val_a�����Ǥ褤 */
		Pi_head [ i ]->n_val_a = 0;
	}

	for ( i = 0 ; i < n_cc_pat ; i++ ) {

		bit_1_set = ( BIT_1_ON << i );

		//printf("gene_pat ");	
		/* �������ϥԥ󤴤Ȥ��ͤ����ꤷ�Ƥ��� */
		for ( j = 0 ; j < Line_info.n_pi; j++ ) {

			pi_line = Pi_head [ j ];
			
			if ( ( rand() & RAND_BIT ) == 0 ) {
				//pi_line->n_val_a |= ( PARA_V0_BIT & bit_1_set );
				//printf("0");
			}
			else {
				pi_line->n_val_a |= ( PARA_V1_BIT & bit_1_set );
				//printf("1");
			}
		}
		//printf("\n");
		pattern_get_from_ramdom_generate ( cc_pat, bit_1_set );
		stock_enter_pattern( cc_pat );
	}
}

/********************************************************************************
 * << Function >>
 *		������ѥ��������������
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/27
 *
 ********************************************************************************/

void pattern_bridge_by_random
(
 void
 )
{
	Ulong i; /* �ƥ��ȥѥ������ */
	Ulong j; /* �������Ͽ� */
	LINE *pi; /* �������Ϥ�LINE�ݥ��� */

	STATE_3 state3 = STATE3_C; /* �Ƴ������Ϥ������� */

	char *func_name = "pattern_bridge_by_random";

	for ( i = 0 ; i < N_tp ; i++ ) {
		for ( j = 0 ; j < Line_info.n_pi ; j++ ) {

			pi = Pi_head[ j ];

			/* PI�������ͤ���� */
			state3 = atpg_get_state9_2_normal_state3 ( pi->state9 );

			/* $$$ �ؿ��Υݥ��󥿲���ɬ�פ��� $$$ */
			switch ( state3 ) {
			case STATE3_X:
			case STATE3_U:
				/* �����X��0/1������ */
				if ( ( rand() & RAND_BIT ) == 0 ) {
					Tp_head[ i ][ j ] = STATE_0_CODE;
				}
				else {
					Tp_head[ i ][ j ] = STATE_1_CODE;
				}
				break;
			case STATE3_0:
				Tp_head[ i ][ j ] = STATE_0_CODE;
				break;
			case STATE3_1:
				Tp_head[ i ][ j ] = STATE_1_CODE;
				break;
			default:
				topgun_error( FEC_PRG_LINE_STAT, func_name );
			}
		}
	}
	topgun_print_mes_fsim_input_pattern( N_tp, 0 );
}

/********************************************************************************
 * << Function >>
 *		�Ĥ�ξ���򥫥���Ȥ���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

Ulong fsim_count_fault_rest
(
 void
)
{
	Ulong i;
	Ulong cnt;
	LINE  *line;

	/* �Ĥ�ξ��������� */
	/* $$$ ���������ˡ�׸�Ƥ $$$ */
	for ( cnt=0, i = 0 ; i < Line_info.n_line; i++ ) {
		line = &(Line_head[i]);
		if ( line->flist[FSIM_SA0] != NULL ) {
			if( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DCD ) ) {
				cnt++;
			}
		}
		if ( line->flist[FSIM_SA1] != NULL ) {
			if( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DCD ) ) {
				cnt++;
			}
		}
	}
	return ( cnt );
}

/********************************************************************************
 * << Function >>
 *		�ѥ�����������λ��Ĥä��ξ���Ф��Ƹξ㥷�ߥ�졼������»ܤ���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

void fsim_output
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
) {
	Ulong i = 0; /* ���̥Хåե��Υ����� */
	Ulong result;
	Ulong buf_size;


	buf_size = compact_get_buf_size();

	for ( i = 0 ; i < buf_size ; i++ ) {
		
		/* 0���ܤ���1�ѥ�������äƤ��� */
		compact_get_pattern_with_id ( fsim_info->cc_pat, i );

		/* �Ȥꤢ���������å� */
		result =  compact_check_pattern_all_x ( fsim_info->cc_pat );
		if ( result == COMP_PAT_ALL_X ) {
			/* ����X�ʤ齪λ */
			//break;
		}
		else {
			/* �ξ㥷�ߥ�졼������»� */
			//fsim_ppsfp_val2_20060926( fsim_info, comp_info );
			fsim_ppsfp_val2( fsim_info, comp_info );
		}
	}
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե����������ѥ����ξ㥷�ߥ�졼�������ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

void fsim_enter_pattern_from_cc_pat_val2
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info
) {

	Ulong i = 0; /* ������ */
	Ulong j = 0;
	
	Ulong shift_count = 0; /* �ѥ���γ������bit���֤򼨤� */
	Ulong width_count = 0; /* �ѥ���γ��򼨤� */

	Ulong bit_1_set   = 0;
	Ulong bit_1_shift = 0;
	
	Ulong *pattern_a  = NULL;
	Ulong *pattern_b  = NULL;

	Ulong  n_cc_pat = fsim_info->n_cc_pat; /* �ѥ���ѥ���� */

	LINE  *pi_line = NULL; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */

	CC_PATTERN cc_pat;         /* stock�ؤ�ž���� */

	/* ����� */
	for ( i = 0 ; i < Line_info.n_pi; i++ ) {
		/* 2�ͤʤΤ�n_val_a�����Ǥ褤 */
		Pi_head [ i ]->n_val_a = 0;
	}

	/* ����ѥ�����ʾ�ξ��ϡ�����ѥ�����ˤ��� */
	if ( n_cc_pat > Fsim_head.bit_size ) {
		n_cc_pat = Fsim_head.bit_size;
	}

	//printf("Pi_head setting pattern\n");
	for ( i = 0 ; i < n_cc_pat ; i++ ) {

		if ( n_cc_pat == 1 ){
			pattern_a = fsim_info->cc_pat->pattern_a;
			pattern_b = fsim_info->cc_pat->pattern_b;

		}
		else {
			/* �ѥ���ѥ���⡼�� */
			pattern_a = comp_info->cc_pat_wodge[ i ]->pattern_a;
			pattern_b = comp_info->cc_pat_wodge[ i ]->pattern_b;
		}
		//printf("a: %u\n",pattern_a[0]);
		//printf("b: %u\n",pattern_b[0]);

		//printf("PAT %2lu ",i);

		/* for stock */
		cc_pat.pattern_a = pattern_a;
		cc_pat.pattern_b = pattern_b;
		
		topgun_print_mes_compact_pattern ( pattern_a, pattern_b, "FSIM ");
		topgun_print_mes_n_compact();

		bit_1_set = ( BIT_1_ON << i );
		
		shift_count = 0;
		width_count = 0;
		
		/* �������ϥԥ󤴤Ȥ��ͤ����ꤷ�Ƥ��� */
		for ( j = 0 ; j < Line_info.n_pi; j++, shift_count++ ) {

			pi_line = Pi_head [ j ];
			
			if ( shift_count == Fsim_head.bit_size ) {
				shift_count = 0;
				width_count++;
			/*
			  if ( width_count == ( ( Line_info.n_pi / Fsim_head.bit_size ) + 1 ) ) {
			  printf("width error %ld\n", width_count);
			  exit(0);
			  }
			*/
			}

			bit_1_shift = ( BIT_1_ON << shift_count );

			if ( pattern_a[ width_count ] & bit_1_shift ) {
				/* 1�ξ�� */
				pi_line->n_val_a |= ( PARA_V1_BIT & bit_1_set );

				//printf("1");
				
			}
			else if ( pattern_b[ width_count ] & bit_1_shift ) {
				/* 0�ξ�� */
				pi_line->n_val_a |= ( PARA_V0_BIT & bit_1_set );

				//printf("0");
			}
			else {
				/* X�ξ�� */

				//printf("X");

				if ( ( rand() & RAND_BIT ) == 0 ) {
					pi_line->n_val_a |= ( PARA_V0_BIT & bit_1_set );

					/* for stocking patttern */
					pattern_b[ width_count ] |= bit_1_shift;

					//printf("0");

				}
				else {
					pi_line->n_val_a |= ( PARA_V1_BIT & bit_1_set );
					
					/* for stocking patttern */
					pattern_a[ width_count ] |= bit_1_shift;

					//printf("1");
				}
			}
		}
		//printf("\n");

		stock_enter_pattern( &cc_pat );
		
	}

	/* pi_head��n_val_a������å� */
	//printf("parallel %ld\n", test_pattern_position );
	/*
	printf("Pi_head setting pattern\n");
	for ( i = 0 ; i < Line_info.n_pi ; i++ ) {
		printf("%lu\n", Pi_head[i]->n_val_a);
	}
	*/
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե����������ѥ����ξ㥷�ߥ�졼�������ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

void fsim_enter_pattern_from_stock_val2
(
 FSIM_INFO *fsim_info,
 COMP_INFO *comp_info,
 STOCK_INFO *stock_info,
 Ulong start_position
) {

	Ulong i = 0; /* ������ */
	Ulong j = 0;
	
	Ulong shift_count = 0; /* �ѥ���γ������bit���֤򼨤� */
	Ulong width_count = 0; /* �ѥ���γ��򼨤� */

	Ulong bit_1_set   = 0;
	Ulong bit_1_shift = 0;

	Ulong *pattern_a  = NULL;
	Ulong *pattern_b  = NULL;

	Ulong  n_cc_pat = fsim_info->n_cc_pat; /* �ѥ���ѥ���� */

	LINE  *pi_line = NULL; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */

	CC_PATTERN cc_pat;         /* stock�ؤ�ž���� */
	Ulong current_position = start_position;


	/* ����� */
	for ( i = 0 ; i < Line_info.n_pi; i++ ) {
		/* 2�ͤʤΤ�n_val_a�����Ǥ褤 */
		Pi_head [ i ]->n_val_a = 0;
	}

	/* ����ѥ�����ʾ�ξ��ϡ�����ѥ�����ˤ��� */
	if ( n_cc_pat > Fsim_head.bit_size ) {
		n_cc_pat = Fsim_head.bit_size;
	}

	//printf("PAT ST ");
	
	//printf("Pi_head setting pattern\n");
	/* for �ʤΤϥѥ���ѥ����� */
	for ( i = 0 ; i < n_cc_pat ; i++, current_position++ ){

		/* �ѥ����stock�������� */
		stock_get_pattern( &cc_pat, current_position );
		
		bit_1_set = ( BIT_1_ON << i );
		
		shift_count = 0;
		width_count = 0;

		pattern_a = cc_pat.pattern_a;
		pattern_b = cc_pat.pattern_b;
		
		/* �������ϥԥ󤴤Ȥ��ͤ����ꤷ�Ƥ��� */
		for ( j = 0 ; j < Line_info.n_pi; j++, shift_count++ ) {

			pi_line = Pi_head [ j ];
			
			if ( shift_count == Fsim_head.bit_size ) {
				shift_count = 0;
				width_count++;
			}

			bit_1_shift = ( BIT_1_ON << shift_count );

			if ( pattern_a[ width_count ] & bit_1_shift ) {
				/* 1�ξ�� */
				pi_line->n_val_a |= ( PARA_V1_BIT & bit_1_set );

				//printf("1");
			}
			else {
				/* 2�ͤʤΤ� */
				/*
				if ( pattern_b[ width_count ] & bit_1_shift ) {
				*/
				/* 0�ξ�� */
				pi_line->n_val_a |= ( PARA_V0_BIT & bit_1_set );

				//printf("0");
			}
		}

		//printf("\n");
	}
}


/********************************************************************************
 * << Function >>
 *		2���������ߥ�졼�����򤹤�
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

void fsim_exe_logic_sim_val2
(
 void
 )
{
	Ulong i; /* ������ */
	Ulong j; /* ������ */

	LINE  *line = NULL;
	
	topgun_3_utility_enter_start_time( &Time_head.logic_sim );
	
	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {

		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {
			line = Lv_pi_head [ i ] [ j ] ; /* $$$ 2���å�������� $$$ */

			/* ��٥����ͤ���� */
			fsim_logic_sim_val2 [ line->type ] ( line );
			
			topgun_print_mes_logic_sim_val2_line( line );
		}
	}
	topgun_3_utility_enter_end_time( &Time_head.logic_sim );
}


/********************************************************************************
 * << Function >>
 *		2�͸ξ㥷�ߥ�졼�����򤹤�
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/


void fsim_exe_fault_sim_val2
(
 FSIM_INFO *fsim_info
 )
{
	Ulong i = 0; /* ������ */
	Ulong j = 0;
	Ulong k = 0;
	Ulong m = 0;
	Ulong n = 0;

	Ulong detect_pat = 0;
	Ulong undetect_pat = 0;

	Ulong select_result = 0;

	Ulong fprop_flag            = 0;
	
	Ulong total_detect_bit = 0; /* �ξ�򸡽Ф���bit */
	
	Ulong pattern_detect_mask = ALL_BIT_OFF; /* �ѥ���ѥ����Ƚ����bit�� */
	Ulong detect_sa0_flag = ALL_BIT_OFF;
	Ulong detect_sa1_flag = ALL_BIT_OFF;

	Ulong fsim_start_level = 0;
	
	LINE  *line = NULL;
	LINE  *cur_line = NULL;   /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */
	LINE  *event_line = NULL; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */

	Ulong valid_bit = 0;

	EVENT_LIST *schedule_elist; /* ���٥����Ͽ�ꥹ���ѥݥ��� */
	EVENT_LIST *cur_e_list;     /* ���٥���ѥݥ��� */

	const FSIM_MODE mode = fsim_info->mode;

	
	topgun_3_utility_enter_start_time( &Time_head.fault_sim );


	/* valid_bit�κ��� */
	if ( fsim_info->n_cc_pat >= Fsim_head.bit_size ) {
		valid_bit = FSIM_ALL_ON;
	}
	else {
		for ( i = 0 ; i < fsim_info->n_cc_pat ; i++ ) {
			valid_bit |= Test_pattern_check_bit [ i ];
		}
	}

	/* ��FFR���Ф��Ƹξ㥷�ߥ�졼�����Ϥ��Ȥ� */
	/* �����������Ф��Ƹξ㥷�ߥ�졼����� */
	//printf("REAL_PAT        ");
	
	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {

		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			line = Lv_pi_head[ i ][ j ];
			
			/* ��bit�ˤ�����ξ����¥ޥ�������(pattern_detect_mask) */
			/* ���Ȥ�FFR�� */

			pattern_detect_mask = ALL_BIT_OFF;
			detect_sa0_flag = ALL_BIT_OFF;
			detect_sa1_flag = ALL_BIT_OFF;


			switch ( mode ) {
			case FSIM_RANDOM:
				select_result = select_fault_fsim_random( line );
				break;
			default:
				select_result = select_fault_fsim_parallel( line );
			}

			if ( select_result & SELECT_ON_SA0 ) {
				/* sa-0�θξ�����򤹤� */

				/* 0���ब���ФǤ��뤫? */
				detect_sa0_flag = line->n_val_a & valid_bit;

				if ( detect_sa0_flag ) {
					pattern_detect_mask =
						( pattern_detect_mask | detect_sa0_flag ) & valid_bit;

					/*
					  printf("F_SIM Insert %8ld SA0",line->line_id);
					  topgun_print_mes_n();
					*/
				}
			}
			if ( select_result & SELECT_ON_SA1 ) {
				/* sa-1�θξ�����򤹤� */

				/* 1���ब���ФǤ��뤫? */
				detect_sa1_flag = ( ~( line->n_val_a ) & valid_bit ); 

				if ( detect_sa1_flag ) {
					pattern_detect_mask =
						( pattern_detect_mask | detect_sa1_flag ) & valid_bit;
					/*
					  printf("F_SIM Insert %8ld SA1",line->line_id);
					  topgun_print_mes_n();
					*/
				}
			}

			
			if ( pattern_detect_mask != ALL_BIT_OFF ) {
				/* �ξ㤬�����ǽ�Ǥ���ʤ�� */
				/*
				  printf("F_SIM START %ld\n",line->line_id);
				*/
					
				fprop_flag++; /* �ѥ��󤴤Ȥθξ�ID�ߤ����ʤ�� �ξ㤴�Ȥν�����򤵤��� */

				/* �ξ�����ե饰 */
				line->fault_set_flag = pattern_detect_mask;

				/* ���٥�ȥꥹ�Ȥ���Ͽ */

				/* flag���ѹ� */
				line->mp_flag = FSIM_EVENT_ON;
				/* ��Ƭ����Ͽ���� */
				schedule_elist = & ( Event_list_head [ line->lv_pi ] );
				schedule_elist->event [ schedule_elist->n_event ] = line;
				schedule_elist->n_event++;

				topgun_print_mes_fsim_enter_line( line );

				fsim_start_level = line->lv_pi;

				/* ���ߥ�졼������»� */
				Fsim_head.detect_mask = ALL_BIT_OFF;

				for ( k = fsim_start_level ; k < Line_info.max_lv_pi ; k++ ) {

					cur_e_list = &( Event_list_head[ k ] );

					for ( m = 0 ; m < cur_e_list->n_event ; m++ ) {

						cur_line = cur_e_list->event[ m ] ;

						/* ���ο������Υ��٥�Ƚ�λ */
						cur_line->mp_flag = FSIM_EVENT_OFF;

						fsim_fault_value_val2[ cur_line->type ]( cur_line, fprop_flag );

						/* �ξ㿮��������ȿž������ */
						cur_line->f_val_a ^=  cur_line->fault_set_flag;

						topgun_print_mes_fsim_exec_line( cur_line );

						/* �ξ�����? */
						if ( cur_line->n_val_a != cur_line->f_val_a ) {
							/* �ξ�������!! */

							/* �ξ�������ե饰������ */
							cur_line->fprop_flag = fprop_flag;

							/* ���Ͽ������򥤥٥�ȥꥹ�Ȥ���Ͽ */
							for ( n = 0; n < cur_line->n_out ; n++ ) {

								event_line = cur_line->out[ n ];

								if ( event_line->mp_flag == FSIM_EVENT_OFF ) {
									/* ̤��Ͽ�ξ�� */

									event_line->mp_flag = FSIM_EVENT_ON;
									schedule_elist = & ( Event_list_head [ event_line->lv_pi ] );
									schedule_elist->event[ schedule_elist->n_event ] = event_line;
									schedule_elist->n_event++;

									topgun_print_mes_fsim_enter_line( event_line );
								}
							}
						}

					}
					/* ���ߤΥ�٥�������٥�Ƚ�λ */
					cur_e_list->n_event = 0;
				}
				/*
				  printf("F_SIM END\n");
				*/


				if ( Fsim_head.detect_mask ) {
					/* �ξ㤬���ФǤ��� */
					/* ����ƥ�����ѥ��ȥ졼���� */
					/* $$$ ���Ȥ� $$$ */

					/* ���Ф���Ͽ */
					if ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit ) {

						total_detect_bit |= ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit );

						if ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DCD ) ){
							/* �Ĥ�ATPG�оݸξ����︺����(check_end��) */
							if  ( ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DTX ) ) &&
								  ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DTR ) ) &&
								  ( ! ( line->flist[FSIM_SA0]->info & TOPGUN_DTA ) ) ) {
								Line_info.n_fault_atpg_count--;
								/*
								  printf("C-F2  : %8ld sa0\n", line->line_id);
								*/
							}
						}
						/* sa0�ξ�� */

						if ( fsim_info->mode == FSIM_RANDOM ) {
							line->flist[FSIM_SA0]->info |= TOPGUN_DTR;
						}
						else {
							line->flist[FSIM_SA0]->info |= TOPGUN_DTS;
							line->flist[FSIM_SA0]->info |= TOPGUN_DCD;
						}
						//printf(" %lu_0", line->line_id );


						topgun_print_mes_fsim_detect( line, FSIM_SA0 );
						/*
						  {
						  printf("F : line %8ld", line->line_id);
						  printf(" sa0\n");
						  }
						*/
					}
						
					if ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit ) {

						total_detect_bit |= ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit );

						if ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DCD ) ){
							if ( ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DTX ) ) &&
								 ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DTR ) ) &&
								 ( ! ( line->flist[FSIM_SA1]->info & TOPGUN_DTA ) ) ) {
								/* �Ĥ�ATPG�оݸξ����︺����(check_end��) */
								Line_info.n_fault_atpg_count--;
								//printf("C-F2  : %8ld sa1\n", line->line_id);
							}
						}
							
						/* sa1�ξ�� */
						if ( fsim_info->mode == FSIM_RANDOM ) {
							line->flist[FSIM_SA1]->info |= TOPGUN_DTR;
						}
						else {
							line->flist[FSIM_SA1]->info |= TOPGUN_DTS;
							line->flist[FSIM_SA1]->info |= TOPGUN_DCD;
						}
						//printf(" %lu_1", line->line_id );
						
						topgun_print_mes_fsim_detect( line, FSIM_SA1 );
						/*
						  {
						  printf("F : line %8ld", line->line_id);
						  printf(" sa1\n");
						  }
						*/
					}
				}
				
				else {
					/* �ξ㤬���ФǤ��ʤ��ä� */
					if ( detect_sa0_flag & Fsim_head.detect_mask ) {
						topgun_print_mes_fsim_not_detect(line, FSIM_SA0);
					}
					if ( detect_sa1_flag & Fsim_head.detect_mask ) {
						topgun_print_mes_fsim_not_detect(line, FSIM_SA1);
					}
				}

				/* �ξ�����ե饰 */
				line->fault_set_flag = ALL_BIT_OFF;
			}
			else {
				topgun_print_mes_fsim_not_insert( line );
			}
		}
	}
	//printf("\n");

	/* fsim2��pat�Ͽ����� */
	detect_pat = utility_count_1_bit ( total_detect_bit );
	undetect_pat = utility_count_1_bit ( valid_bit );
	fsim_info->num_pat  += detect_pat;
	fsim_info->num_waste_pat  += ( undetect_pat - detect_pat );

	topgun_3_utility_enter_end_time( &Time_head.fault_sim );
}

/********************************************************************************
 * << Function >>
 *		2�͸ξ㥷�ߥ�졼�����򤹤�
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/


void fsim_exe_fault_sim_val2_stock
(
 FSIM_INFO *fsim_info,
 STOCK_INFO *stock_info,
 Ulong no_pat
 )
{
	Ulong i = 0; /* ������ */
	Ulong k = 0;
	Ulong m = 0;
	Ulong n = 0;

	Ulong detect_pat = 0;
	Ulong undetect_pat = 0;

	Ulong select_result = 0;

	Ulong fprop_flag = 0;

	Ulong total_detect_bit = 0; /* �ξ�򸡽Ф���bit */
	
	Ulong pattern_detect_mask = ALL_BIT_OFF; /* �ѥ���ѥ����Ƚ����bit�� */
	Ulong detect_sa0_flag = ALL_BIT_OFF;
	Ulong detect_sa1_flag = ALL_BIT_OFF;

	Ulong fsim_start_level = 0;
	
	LINE  *line = NULL;
	LINE  *cur_line = NULL;   /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */
	LINE  *event_line = NULL; /* ���ߥ�졼����󤹤볰�������ѥ饤��ݥ��� */

	Ulong valid_bit = 0;

	EVENT_LIST *schedule_elist; /* ���٥����Ͽ�ꥹ���ѥݥ��� */
	EVENT_LIST *cur_e_list;     /* ���٥���ѥݥ��� */

	STOCK_FAULT *stock_fault = NULL;

	
	topgun_3_utility_enter_start_time( &Time_head.fault_sim );


	/* valid_bit�κ��� */
	if ( fsim_info->n_cc_pat >= Fsim_head.bit_size ) {
		valid_bit = FSIM_ALL_ON;
	}
	else {
		for ( i = 0 ; i < fsim_info->n_cc_pat ; i++ ) {
			valid_bit |= Test_pattern_check_bit [ i ];
		}
	}

	/* ��FFR���Ф��Ƹξ㥷�ߥ�졼�����Ϥ��Ȥ� */
	/* �����������Ф��Ƹξ㥷�ߥ�졼����� */

	//printf("STOCK_PAT %5lu ", no_pat );
	

	stock_fault = stock_info->stock_fault;
	
	while( stock_fault ) {
			
		/* ��bit�ˤ�����ξ����¥ޥ�������(pattern_detect_mask) */
		/* ���Ȥ�FFR�� */

		if ( ( stock_fault->no_pat_0 == STOCK_F_END ) &&
			 ( stock_fault->no_pat_1 == STOCK_F_END ) ) {
			stock_fault = stock_fault->next;
			continue;
		}
		
		pattern_detect_mask = ALL_BIT_OFF;
		detect_sa0_flag = ALL_BIT_OFF;
		detect_sa1_flag = ALL_BIT_OFF;

		line          = stock_fault->line;
		select_result = stock_fault->flag; 

		if ( select_result & SELECT_ON_SA0 ) {
			/* sa-0�θξ�����򤹤� */

			if ( fsim_stock_fault_select ( stock_fault->no_pat_0 ) ) {
				
				/* 0���ब���ФǤ��뤫? */
				detect_sa0_flag = line->n_val_a & valid_bit;

				if ( detect_sa0_flag ) {
					pattern_detect_mask =
						( pattern_detect_mask | detect_sa0_flag ) & valid_bit;
					
					/*
					  printf("F_SIM Insert %8ld SA0",line->line_id);
					  topgun_print_mes_n();
					*/
				}
			}
		}
		
		if ( select_result & SELECT_ON_SA1 ) {
			/* sa-1�θξ�����򤹤� */
			if ( fsim_stock_fault_select ( stock_fault->no_pat_1 ) ) {
				
				/* 1���ब���ФǤ��뤫? */
				detect_sa1_flag = ( ~( line->n_val_a ) & valid_bit ); 

				if ( detect_sa1_flag ) {
					pattern_detect_mask =
						( pattern_detect_mask | detect_sa1_flag ) & valid_bit;
					/*
					  printf("F_SIM Insert %8ld SA1",line->line_id);
					  topgun_print_mes_n();
					*/
				}
			}
		}
			
		if ( pattern_detect_mask != ALL_BIT_OFF ) {
			/* �ξ㤬�����ǽ�Ǥ���ʤ�� */
			/*
			  printf("F_SIM START %ld\n",line->line_id);
			*/
					
			fprop_flag++; /* �ѥ��󤴤Ȥθξ�ID�ߤ����ʤ�� �ξ㤴�Ȥν�����򤵤��� */

			/* �ξ�����ե饰 */
			line->fault_set_flag = pattern_detect_mask;

			/* ���٥�ȥꥹ�Ȥ���Ͽ */

			/* flag���ѹ� */
			line->mp_flag = FSIM_EVENT_ON;
			/* ��Ƭ����Ͽ���� */
			schedule_elist = & ( Event_list_head [ line->lv_pi ] );
			schedule_elist->event [ schedule_elist->n_event ] = line;
			schedule_elist->n_event++;

			topgun_print_mes_fsim_enter_line( line );

			fsim_start_level = line->lv_pi;

			/* ���ߥ�졼������»� */
			Fsim_head.detect_mask = ALL_BIT_OFF;

			for ( k = fsim_start_level ; k < Line_info.max_lv_pi ; k++ ) {

				cur_e_list = &( Event_list_head[ k ] );
				
				for ( m = 0 ; m < cur_e_list->n_event ; m++ ) {

					cur_line = cur_e_list->event[ m ] ;

					/* ���ο������Υ��٥�Ƚ�λ */
					cur_line->mp_flag = FSIM_EVENT_OFF;

					fsim_fault_value_val2[ cur_line->type ]( cur_line, fprop_flag );

					/* �ξ㿮��������ȿž������ */
					cur_line->f_val_a ^=  cur_line->fault_set_flag;

					topgun_print_mes_fsim_exec_line( cur_line );

					/* �ξ�����? */
					if ( cur_line->n_val_a != cur_line->f_val_a ) {
						/* �ξ�������!! */

						/* �ξ�������ե饰������ */
						cur_line->fprop_flag = fprop_flag;

						/* ���Ͽ������򥤥٥�ȥꥹ�Ȥ���Ͽ */
						for ( n = 0; n < cur_line->n_out ; n++ ) {

							event_line = cur_line->out[ n ];

							if ( event_line->mp_flag == FSIM_EVENT_OFF ) {
								/* ̤��Ͽ�ξ�� */

								event_line->mp_flag = FSIM_EVENT_ON;
								schedule_elist = & ( Event_list_head [ event_line->lv_pi ] );
								schedule_elist->event[ schedule_elist->n_event ] = event_line;
								schedule_elist->n_event++;

								topgun_print_mes_fsim_enter_line( event_line );
							}
						}
					}

				}
				/* ���ߤΥ�٥�������٥�Ƚ�λ */
				cur_e_list->n_event = 0;
			}
			/*
			  printf("F_SIM END\n");
			*/


			if ( Fsim_head.detect_mask ) {
				/* �ξ㤬���ФǤ��� */
				/* ����ƥ�����ѥ��ȥ졼���� */
				/* $$$ ���Ȥ� $$$ */

				/* ���Ф���Ͽ */
				if ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit ) {

					total_detect_bit |= ( detect_sa0_flag & Fsim_head.detect_mask & valid_bit );

					/* sa0�ξ�� */

					if ( fsim_info->mode == FSIM_STOCK ) {

						fsim_stock_fault_detect ( &(stock_fault->no_pat_0), no_pat );
						/*
						if ( stock_fault->no_pat_0 == 0 ) {
							// 1���ܤθ���
							stock_fault->no_pat_0 = no_pat;
						}
						else {
							// 2���ܤθ���
							stock_fault->no_pat_0 = FL_ULMAX;
						}
						*/
						//printf(" %lu_0", line->line_id );						
					}
					topgun_print_mes_fsim_detect( line, FSIM_SA0 );
				}
						
				if ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit ) {

					total_detect_bit |= ( detect_sa1_flag & Fsim_head.detect_mask & valid_bit );

					
					/* sa1�ξ�� */
					if ( fsim_info->mode == FSIM_STOCK ) {

						fsim_stock_fault_detect ( &(stock_fault->no_pat_1), no_pat );
						/*
						if ( stock_fault->no_pat_1 == 0 ) {
							// 1���ܤθ��� 
							stock_fault->no_pat_1 = no_pat;
						}
						else {
							// 2���ܤθ���
							stock_fault->no_pat_1 = FL_ULMAX;
						}
						*/
						//printf(" %lu_1", line->line_id );
					}
					topgun_print_mes_fsim_detect( line, FSIM_SA1 );
				}
			}
			else {
				/* �ξ㤬���ФǤ��ʤ��ä� */
				if ( detect_sa0_flag & Fsim_head.detect_mask ) {
					topgun_print_mes_fsim_not_detect(line, FSIM_SA0);
				}
				if ( detect_sa1_flag & Fsim_head.detect_mask ) {
					topgun_print_mes_fsim_not_detect(line, FSIM_SA1);
				}
			}

			/* �ξ�����ե饰 */
			line->fault_set_flag = ALL_BIT_OFF;
		}
		else {
			topgun_print_mes_fsim_not_insert( line );
		}

		stock_fault = stock_fault->next;
	}

	/* fsim2��pat�Ͽ����� */
	detect_pat = utility_count_1_bit ( total_detect_bit );
	undetect_pat = utility_count_1_bit ( valid_bit );

	//printf("\n");

	/*
	fsim_info->num_pat  += detect_pat;
	fsim_info->num_waste_pat  += ( undetect_pat - detect_pat );
	*/

	topgun_3_utility_enter_end_time( &Time_head.fault_sim );
}


/********************************************************************************
 * << Function >>
 *		�ξ�����򤹤���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

Ulong fsim_stock_fault_select_one_detect
(
 Ulong no_pat
 ){
	if ( no_pat == STOCK_F_N_DET ) {
		return ( 1 ) ;
	}
	else {
		return ( 0 );
	}
}

/********************************************************************************
 * << Function >>
 *		�ξ�����򤹤���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

Ulong fsim_stock_fault_select_double_detect
(
 Ulong no_pat
 ){
	if ( no_pat < STOCK_F_D_DET ) {
		return ( 1 ) ;
	}
	else {
		return ( 0 );
	}
}

/********************************************************************************
 * << Function >>
 *		�ξ�����򤹤���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

void fsim_stock_fault_detect_one_detect
(
 Ulong *no_pat_x,
 Ulong no_pat
 ){
	*no_pat_x = STOCK_F_END;
}

/********************************************************************************
 * << Function >>
 *		�ξ�����򤹤���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

void fsim_stock_fault_detect_double_detect
(
 Ulong *no_pat_x,
 Ulong no_pat
 ){
	if ( *no_pat_x == STOCK_F_N_DET ) {
		*no_pat_x = no_pat;
	}
	else {
		*no_pat_x = STOCK_F_D_DET;
	}
}

/********************************************************************************
 * << Function >>
 *		�ξ�����򤹤���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line pointer
 * flag          I      Ulong   flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/11
 *
 ********************************************************************************/

void fsim_stock_fault_detect_reverse
(
 Ulong *no_pat_x,
 Ulong no_pat
 ){
	*no_pat_x = no_pat;
}

void fsim_stock_fault_init
(
 STOCK_MODE mode
 ){

	switch ( mode  ) {
	case STOCK_DDET:
		fsim_stock_fault_select = &fsim_stock_fault_select_double_detect;
		fsim_stock_fault_detect = &fsim_stock_fault_detect_double_detect;
		break;
	case STOCK_ONE_DET:
		fsim_stock_fault_select = &fsim_stock_fault_select_one_detect;
		fsim_stock_fault_detect = &fsim_stock_fault_detect_one_detect;
		break;
	case STOCK_REVERSE: //select��OneDet��Ʊ����ʬ����ȼ�
		fsim_stock_fault_select = &fsim_stock_fault_select_one_detect;
		fsim_stock_fault_detect = &fsim_stock_fault_detect_reverse;
		break;
	default:
		break;
	}
}

/* End of File */
