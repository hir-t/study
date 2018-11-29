/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ���̥Хåե��˴ؤ���ؿ���
	
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

static CC_PATTERN tmp_pat;  /* ���̥Хåե������촹��������Ѥ��� */

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
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9�ͤ��������ͤ���Ф� */

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
 *		���̥Хåե��ޥ͡�����
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *  ATPG_FINISH : ATPG����λ
 *  ???         : ATPG���Ǥ��ڤ� $$$ ��ǽ�ɲû����ɲ� $$$
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
	/* �ѥ������Хåե������Ϥ��� */
	/* -> ���ä�(���̤Ǥ���) */
	/* -> ���ä�(���̤Ǥ��������Хåե������ä�) */
	/* -> ����ʤ�(���̤Ǥ��������Хåե���ʤ�) */
	/* ->�������Υѥ�����������֤� */

	Ulong    num_x_state = 0;
	Ulong    i = 0; /* ���̥Хåե��������Υ����� */
	Ulong    comp_buf_id = 0; /* ���������ѥ�������줿���̥Хåե���ID */

	topgun_2_utility_enter_start_time ( &Time_head.compact );
	
	topgun_print_mes_n_compact();
	
	for ( i = 0 ; i < Comp_head.buf_size ; i++ ) {

		/* �Хåե�i�ΰ��̲�ǽ���򤷤�٤� */
		comp_info->result = compact_check_possibility ( i, comp_info->cc_pat ) ;

		topgun_print_mes_compact_pattern( comp_info->cc_pat->pattern_a
										  , comp_info->cc_pat->pattern_b, Comp_head.buf_size, Comp_head.buf_size );
		topgun_print_mes_n_compact();

		topgun_print_mes_compact_pattern
			( Comp_head.buf_a[ i ], Comp_head.buf_b[ i ], i, Comp_head.buf_size );
		topgun_print_mes_compact_posibbility( comp_info->result );
		
		if ( comp_info->result == COMPACT_OK ) {

			comp_buf_id = i;
			
			/* ���̤��� */
			compact_enter_pattern_buffer ( comp_buf_id, comp_info->cc_pat ) ;

			/* i��X�ο���׻����� */
			compact_min_x_state_recalc ( comp_buf_id );
			
			/* min_x_buf��Ʒ׻����� */
			compact_search_min_x_state();
			
			topgun_print_mes_compact_pattern
				( Comp_head.buf_a[ comp_buf_id ], Comp_head.buf_b[ comp_buf_id ], (Comp_head.buf_size + 1)  , Comp_head.buf_size );
			topgun_print_mes_n_compact();
			
			break;
		}
	}

	if ( comp_info->result == COMPACT_NG ) {
		/* ���٤ƤΤ˥Хåե��ǰ��̤Ǥ��ʤ� */
		
		/* ����X�ξ��ʤ��ѥ���������Ф��� */

		/* ���������ѥ������X�ο����Ȥ�� */
		num_x_state = compact_count_x_state ( comp_info->cc_pat->pattern_a,
											  comp_info->cc_pat->pattern_b );

		topgun_print_mes_min_check ( num_x_state, Comp_head.min_n_x_state );

		
		if ( Comp_head.min_n_x_state < num_x_state ) {
			/* ���̥Хåե������äƤ�����X������ */

			/* �ѥ�������äƤ��� */
			compact_get_pattern_with_id ( &tmp_pat, Comp_head.min_x_buf_id );

			topgun_print_mes_compact_pattern
				( tmp_pat.pattern_a, tmp_pat.pattern_b, i, Comp_head.buf_size);
			topgun_print_mes_n_compact();
		
			/* �������Хåե��˾�񤭤��� */
			compact_overwrite_pattern
				( Comp_head.min_x_buf_id, comp_info->cc_pat ) ;

			topgun_print_mes_compact_pattern
				( Comp_head.buf_a[ Comp_head.min_x_buf_id ],
				  Comp_head.buf_b[ Comp_head.min_x_buf_id ], ( Comp_head.buf_size+1), Comp_head.buf_size);
			topgun_print_mes_n_compact();

			/* min_x_buf��Ʒ׻����� */
			compact_search_min_x_state();
		}
		else {
			/* �ѥ����������X������ */
		}
	}
	else if ( comp_info->result == COMPACT_OK ) {
		/* Fsim3��ư���뤫 */
		if ( FSIM_VAL3 == 1  ) {
			/* Fsim3��ư������Υѥ��������� */
			/* i���ܤ����äƤ��� */
			/* �ѥ�������äƤ��� */
			compact_get_pattern_with_id ( comp_info->cc_pat, comp_buf_id );
		}
	}

	topgun_2_utility_enter_end_time ( &Time_head.compact );
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե��ޥ͡�����
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *  ATPG_FINISH : ATPG����λ
 *  ???         : ATPG���Ǥ��ڤ� $$$ ��ǽ�ɲû����ɲ� $$$
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

	/* �ѥ������Хåե������Ϥ��� */
	/* -> ���ä�(���̤Ǥ���) */
	/* -> ���ä�(���̤Ǥ��������Хåե������ä�) */
	/* -> ����ʤ�(���̤Ǥ��������Хåե���ʤ�) */
	/* ->�������Υѥ�����������֤� */


	Ulong    mini_x_state = FL_ULMAX;  
	Ulong    mini_x_state_buf_id = FL_ULMAX;

	Ulong    current_x_state = 0;

	Ulong    current_result = COMPACT_NG;
	
	Ulong    num_x_state = 0;
	Ulong    i = 0; /* ���̥Хåե��������Υ����� */
	Ulong    comp_buf_id = 0; /* ���������ѥ�������줿���̥Хåե���ID */

	topgun_2_utility_enter_start_time ( &Time_head.compact );
	
	topgun_print_mes_n_compact();

	comp_info->result = COMPACT_NG;
	
	//printf("COMP EXE ");

	for ( i = 0 ; i < Comp_head.buf_size ; i++ ) {

		/* �Хåե�i�ΰ��̲�ǽ���򤷤�٤� */
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

			/* �ޤ�tmp_pat�˥��ԡ����� */
			compact_get_pattern_with_id ( &tmp_pat, comp_buf_id );
			
			/* �����̤��� */
			compact_trial ( &tmp_pat, comp_info->cc_pat ) ;

			/* X�ο���׻����� */
			num_x_state = compact_count_x_state ( tmp_pat.pattern_a,
												  tmp_pat.pattern_b  );

			/* X����ʬ����� */
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
		/* ���٤ƤΤ˥Хåե��ǰ��̤Ǥ��ʤ� */
		//printf("COMP_NG\n");
		/* ����X�ξ��ʤ��ѥ���������Ф��� */

		/* ���������ѥ������X�ο����Ȥ�� */
		num_x_state = compact_count_x_state ( comp_info->cc_pat->pattern_a,
											  comp_info->cc_pat->pattern_b );

		topgun_print_mes_min_check ( num_x_state, Comp_head.min_n_x_state );

		
		if ( Comp_head.min_n_x_state < num_x_state ) {
			/* ���̥Хåե������äƤ�����X������ */

			/* �ѥ�������äƤ��� */
			compact_get_pattern_with_id ( &tmp_pat, Comp_head.min_x_buf_id );

			topgun_print_mes_compact_pattern
				( tmp_pat.pattern_a, tmp_pat.pattern_b, i, Comp_head.buf_size);
			topgun_print_mes_n_compact();
		
			/* �������Хåե��˾�񤭤��� */
			compact_overwrite_pattern
				( Comp_head.min_x_buf_id, comp_info->cc_pat ) ;

			topgun_print_mes_compact_pattern
				( Comp_head.buf_a[ Comp_head.min_x_buf_id ],
				  Comp_head.buf_b[ Comp_head.min_x_buf_id ], ( Comp_head.buf_size+1), Comp_head.buf_size);
			topgun_print_mes_n_compact();

			/* min_x_buf��Ʒ׻����� */
			compact_search_min_x_state();
		}
		else {
			/* �ѥ����������X������ */
		}
	}
	else if ( comp_info->result == COMPACT_OK ) {

		comp_buf_id = mini_x_state_buf_id;

		//printf("COMP_IN  : %5lu\n",comp_buf_id);
			
		/* ���̤��� */
		compact_enter_pattern_buffer ( comp_buf_id, comp_info->cc_pat ) ;

		/* i��X�ο���׻����� */
		compact_min_x_state_recalc ( comp_buf_id );
			
		/* min_x_buf��Ʒ׻����� */
		compact_search_min_x_state();
			
		topgun_print_mes_compact_pattern
			( Comp_head.buf_a[ comp_buf_id ], Comp_head.buf_b[ comp_buf_id ], (Comp_head.buf_size + 1)  , Comp_head.buf_size );
		topgun_print_mes_n_compact();
		
		/* Fsim3��ư���뤫 */
		if ( FSIM_VAL3 == 1  ) {
			/* Fsim3��ư������Υѥ��������� */
			/* i���ܤ����äƤ��� */
			/* �ѥ�������äƤ��� */
			compact_get_pattern_with_id ( comp_info->cc_pat, comp_buf_id );
		}
	}

	topgun_2_utility_enter_end_time ( &Time_head.compact );
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե�����ѥ��������Ф���
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
	Ulong i = 0; /* ���̥Хåե������� */

	/* reterun_pattern���ͤ򥳥ԡ����� */
	char *func_name = "compact_get_patetrn";

	test_null_variable ( return_pattern, FMT_CC_PATTERN_P, func_name );

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		return_pattern->pattern_a[ i ] = Comp_head.buf_a[ buf_id ][ i ];
		return_pattern->pattern_b[ i ] = Comp_head.buf_b[ buf_id ][ i ];

	}
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե��ν����
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
	Ulong i = 0; /* ���̥Хåե������� */
	Ulong j = 0; /* ���̥Хåե��� */

	/* ���Ƥ˥Хåե���������PI���� (�����¿�� ���ޤ���Ф��ơ�PI��ݤ��뤿��) */
	/* �ѥ����󤴤Ȥν���(���̲�/�Բ�Ƚ�ǡ����̡������)���ưפʤ��� */

	char *func_name = "compact_buffer_initialize";

	/* 1�ѥ����� 2bit(buf_a��buf_b)�Ȥ��� */
	Comp_head.width = ( Line_info.n_pi / Fsim_head.bit_size ) + 1 ;


	//printf("width %ld\n", Comp_head.width);
	//printf("Comp_head.buf_size %ld\n", Comp_head.buf_size );

	
	/* �Ȥꤢ��������Хåե��������Ǻ�� */
	Comp_head.buf_a = ( Ulong **)topgunMalloc
		( FMT_Ulong_P, sizeof(  Ulong *), Comp_head.buf_size, func_name );
	Comp_head.buf_b = ( Ulong **)topgunMalloc
		( FMT_Ulong_P, sizeof(  Ulong *), Comp_head.buf_size, func_name );


	Comp_head.num_x_state = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof( Ulong ), Comp_head.buf_size, func_name );
	
	/* �Ȥꤢ���������� "X" �˽���� */

	for ( i = 0 ; i < Comp_head.buf_size ; i++ ) {

		Comp_head.buf_a[ i ] = ( Ulong *)topgunMalloc
			( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );
		Comp_head.buf_b[ i ] = ( Ulong *)topgunMalloc
			( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );

		for ( j = 0 ; j < Comp_head.width ; j++ ) {

			/* X(a = 0, b = 0) �ǽ�������� */
			Comp_head.buf_a[ i ][ j ] = 0;
			Comp_head.buf_b[ i ][ j ] = 0;
		}

		Comp_head.num_x_state[ i ] = 0;
	}

	/* ���̥Хåե�������ư�ļ����ˤĤ����ѥ�����ν���� */
	tmp_pat.pattern_a = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );
	tmp_pat.pattern_b = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );

	for ( j = 0 ; j < Comp_head.width ; j++ ) {

			/* X(a = 0, b = 0) �ǽ�������� */
			tmp_pat.pattern_a[ j ] = 0;
			tmp_pat.pattern_b[ j ] = 0;
	}
	Comp_head.min_x_buf_id = 0;

}


/********************************************************************************
 * << Function >>
 *		���̥Хåե��ؤΥѥ����󰵽�
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

	Ulong i; /* ���̥Хåե�����(Comp_head.width)�Υ����� */

	 char *func_name = "compact_buffer_enter_pattern";
	
	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );

	/* ������֤��Ǥ˰��̲�ǽ�Ǥ��뤳�Ȥϳ�ǧ�Ѥߡ� */

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		���������ꤵ����bit�黻
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
 *		���Υѥ����󰵽�
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

	Ulong i; /* ���̥Хåե�����(Comp_head.width)�Υ����� */

	 char *func_name = "compact_buffer_enter_pattern";
	
	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	tmp_pat_a = tmp_pat->pattern_a;
	tmp_pat_b = tmp_pat->pattern_b;

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );

	/* ������֤��Ǥ˰��̲�ǽ�Ǥ��뤳�Ȥϳ�ǧ�Ѥߡ� */

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		���������ꤵ����bit�黻
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
 *		���̥Хåե��ؤΥѥ�����ξ��
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

	Ulong i; /* ���̥Хåե�����(Comp_head.width)�Υ����� */

	char *func_name = "compact_buffer_enter_pattern";

	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );	

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		���������ꤵ����bit�黻
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

		/* ���򤷤Ƥ�����Τ򥳥ԡ����Ƥ����� */
		pattern_a[ i ] = tmp_pat.pattern_a [ i ];
		pattern_b[ i ] = tmp_pat.pattern_b [ i ];
	}

	compact_min_x_state_recalc ( buffer_id );
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե����Ǥ�եѥ������¸�ߤ���X�ο������
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       �ѥ��������X�ο��������
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

	/* ��¤����ꤹ��Τ��Ĥμ� */

	Ulong i; /* ���̥Хåե�����(Comp_head.width)�Υ����� */
	Ulong num_x_state = 0; /* X�ο� */

	Ulong tmp_pattern = 0;

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 ) */
		tmp_pattern = ( Comp_head.buf_a[ buffer_id ][ i ] |
						Comp_head.buf_b[ buffer_id ][ i ] );

		/* $$$ bit���ޤ��0�Ͼ夲��Ȥ��ư�ö���֡�$$$ */
		/* $$$ ���֤��������ǥإå��˳Ф������Ƥ����Ƥ��Ȥǰ����Ф褵��) */
		num_x_state += utility_count_0_bit ( tmp_pattern );
	}

	topgun_print_mes_compact_count_x_in_buf ( buffer_id, num_x_state);
	
	return ( num_x_state );

}

/********************************************************************************
 * << Function >>
 *		���̥Хåե����Ǥ�եѥ������¸�ߤ���X�ο������
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       �ѥ��������X�ο��������
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

	/* ��¤����ꤹ��Τ��Ĥμ� */

	Ulong i = 0; /* ���̥Хåե�����(Comp_head.width)�Υ����� */
	Ulong num_x_state = 0; /* X�ο� */

	Ulong tmp_pattern = 0;

	char *func_name = "compact_count_x_state";

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );
	
	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 ) */
		tmp_pattern = ( pattern_a[ i ] | pattern_b[ i ] );

		/* $$$ bit���ޤ��0�Ͼ夲��Ȥ��ư�ö���֡�$$$ */
		/* $$$ ���֤��������ǥإå��˳Ф������Ƥ����Ƥ��Ȥǰ����Ф褵��) */
		num_x_state += utility_count_0_bit ( tmp_pattern );
	}

	topgun_print_mes_compact_count_x_in_pat ( num_x_state) ;

	return ( num_x_state );

}
	 
/********************************************************************************
 * << Function >>
 *		���̥Хåե����Ǥ�եѥ������¸�ߤ���X�ο������
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       �ѥ��������X�ο��������
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
		/* X�����ʤ��Ȥ���ξ���ι��� */
		Comp_head.min_n_x_state  = num_x_state;
		Comp_head.min_x_buf_id = buffer_id;
	}
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե��ؤΥѥ����󰵽̤β���(�Τ�)��ǧ����
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * COMPACT_NG  : �����Բ�ǽ ( == UL_MAX )
 * COMPPRES_OK  : ���̲�ǽ ( == 0 )
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
	Ulong i = 0; /* ���̥Хåե�����(Comp_head.width)�Υ����� */

	char *func_name = "compact_buffer_enter_pattern";

	test_null_variable ( cc_pat, FMT_CC_PATTERN, func_name );

	pattern_a = cc_pat->pattern_a;
	pattern_b = cc_pat->pattern_b;

	test_null_variable ( pattern_a, FMT_Ulong_P, func_name );
	test_null_variable ( pattern_b, FMT_Ulong_P, func_name );
	
	
	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )

		���������ꤵ����bit�黻<>
		   X <> X ( a : 0 <> 0 , b : 0 <> 0 �� OK )
		   X <> 0 ( a : 0 <> 0 , b : 0 <> 1 �� OK )
		   X <> 1 ( a : 0 <> 1 , b : 0 <> 0 �� OK )
		   0 <> X ( a : 0 <> 1 , b : 1 <> 0 �� OK )
		   0 <> 0 ( a : 0 <> 0 , b : 1 <> 1 �� OK )
		   0 <> 1 ( a : 0 <> 1 , b : 1 <> 0 �� NG )
		   1 <> X ( a : 1 <> 0 , b : 0 <> 0 �� OK )
		   1 <> 0 ( a : 1 <> 0 , b : 0 <> 1 �� NG )
		   1 <> 1 ( a : 1 <> 1 , b : 0 <> 0 �� OK )

		*/

		if ( ( Comp_head.buf_a[ buffer_id ][ i ] & pattern_b[ i ] ) ||
			 ( Comp_head.buf_b[ buffer_id ][ i ] & pattern_a[ i ] ) ) {
			/* ����ȯ��, ���̤Ǥ��ʤ� */
			return ( COMPACT_NG );
		}
	}
	/* �ɤ�����ͤ�ȯ�����Ƥ��ʤ� -> ����OK */
	return ( COMPACT_OK );
}

/********************************************************************************
 * << Function >>
 *		���̥Хåե����X�κǾ��ͥѥ���������
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 * name			(I/O)	type	    description
 * num_x_state   I      Ulong       �ѥ��������X�ο��������
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

	Ulong i; /* ���̥Хåե�����(Comp_head.width)�Υ����� */

	/* char *func_name = "compact_search_x_state"; */

	Comp_head.min_x_buf_id  = 0; /* ����� */
	Comp_head.min_n_x_state = Comp_head.num_x_state[ 0 ]; /* ����� */

	/* ������ : ���٤�buuffer���Ȥ�X�ο��ϵ�ޤäƤ��� */
	for ( i = 1 ; i < Comp_head.buf_size ; i++ ) {
		if ( Comp_head.min_n_x_state > Comp_head.num_x_state[ i ] ) {
			/* �������� */
			Comp_head.min_n_x_state  = Comp_head.num_x_state[ i ];
			Comp_head.min_x_buf_id = i;
		}
	}
}

/********************************************************************************
 * << Function >>
 *		ATPG�����������ѥ����������
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
 * cc_pat        I/O    CC_PATTERN * �ȹ礻��ϩ�ѥ�����
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
	Ulong   i; /* �������Ͽ��Υ����� */
	LINE    *pi = NULL; /* �������Ϥ�LINE�ݥ��� */
	STATE_3 state3 = STATE3_C;

	char *func_name = "pattern_get_from_generate";

	test_null_variable ( cc_pat, FMT_CC_PATTERN_P, func_name );

	pat_a = cc_pat->pattern_a;
	pat_b = cc_pat->pattern_b;

	/* all X�˥ꥻ�å� */
	pat_a[ width_count ] = 0;
	pat_b[ width_count ] = 0;

	for ( i = 0 ; i < Line_info.n_pi ; i++, shift_count++ ) {

		pi = Pi_head[ i ];

		/* PI�������ͤ���� */
		state3 = atpg_get_state9_2_normal_state3 ( pi->state9 );

		if ( shift_count == Fsim_head.bit_size ) {
			shift_count = 0;
			width_count++;

			if ( width_count > Comp_head.width ) {
				printf("width is bad width_count %ld Comp_head.width %ld\n",
					   width_count, Comp_head.width);
			}

			/* all X�˥ꥻ�å� */
			pat_a[ width_count ] = 0;
			pat_b[ width_count ] = 0;
		}


		/*
		  ���ѿ��ȥѥ�����δط�

		  pi_count(i) 0123 4567 ...                        32 33        ...
		  shift_count 0123 4567 ...                        32 0123 4567 ...
		  width_count 0000      ...                      0000 1111 1111 ...

		        pat_a 0100 0010 0100 0000 0000 0100 0000 0000 0000 0010 ...
				pat_b 0010 0100 0001 0110 0001 0000 0100 0100 0000 0100 ...

			PI state3 X10X X01X X1X0 X00X XXX0 X1XX X0XX X0XX XXXX X01X ...

		*/

		/* $$$ �ؿ��Υݥ��󥿲���ɬ�פ��� $$$ */
		switch ( state3 ) {
		case STATE3_0:
			/* default��X(00)�ʤΤǡ�0(01)�ˤʤ�褦�ˡ�b����bit���Ѳ������� */
			pat_b[ width_count ] |= ( BIT_1_ON << shift_count ) ;
			break;
		case STATE3_1:
			/* default��X(00)�ʤΤǡ�1(01)�ˤʤ�褦�ˡ�a����bit���Ѳ������� */
			pat_a[ width_count ] |= ( BIT_1_ON << shift_count ) ;
			break;
		case STATE3_X:
		case STATE3_U: /* ñ������Ϥ�U�ǤϤʤ�����¬�Ǥ��ʤ�PI�ξ�硢U�ˤʤ� */
			break;
			/* default��X(00)�ʤΤǤʤˤ⤷�ʤ� */
		default:
			topgun_error( FEC_PRG_LINE_STAT, func_name );
		}
	}
}

/********************************************************************************
 * << Function >>
 *		ATPG�����������ѥ����������
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
 * cc_pat        I/O    CC_PATTERN * �ȹ礻��ϩ�ѥ�����
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
	Ulong   i; /* �������Ͽ��Υ����� */
	LINE    *pi = NULL; /* �������Ϥ�LINE�ݥ��� */

	char *func_name = "pattern_get_from_generate";

	Ulong check_bit = PARA_V1_BIT & bit_1_set;

	test_null_variable ( cc_pat, FMT_CC_PATTERN_P, func_name );

	pat_a = cc_pat->pattern_a;
	pat_b = cc_pat->pattern_b;

	/* all X�˥ꥻ�å� */
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

			/* all X�˥ꥻ�å� */
			pat_a[ width_count ] = 0;
			pat_b[ width_count ] = 0;
		}


		/*
		  ���ѿ��ȥѥ�����δط�

		  pi_count(i) 0123 4567 ...                        32 33        ...
		  shift_count 0123 4567 ...                        32 0123 4567 ...
		  width_count 0000      ...                      0000 1111 1111 ...

		        pat_a 0100 0010 0100 0000 0000 0100 0000 0000 0000 0010 ...
				pat_b 0010 0100 0001 0110 0001 0000 0100 0100 0000 0100 ...

			PI state3 X10X X01X X1X0 X00X XXX0 X1XX X0XX X0XX XXXX X01X ...

		*/

		/* $$$ �ؿ��Υݥ��󥿲���ɬ�פ��� $$$ */
		/* PI�������ͤ���� */
		if ( pi->n_val_a & check_bit ) {
			//STATE3_1:
			//printf("1");
			
			/* default��X(00)�ʤΤǡ�1(01)�ˤʤ�褦�ˡ�a����bit���Ѳ������� */
			pat_a[ width_count ] |= ( BIT_1_ON << shift_count ) ;
		}
		else {
			//STATE3_0:
			//printf("0");
			/* default��X(00)�ʤΤǡ�0(01)�ˤʤ�褦�ˡ�b����bit���Ѳ������� */
			pat_b[ width_count ] |= ( BIT_1_ON << shift_count ) ;
		}
	}
	//printf("\n");
}

/********************************************************************************
 * << Function >>
 *		�ȹ礻�ѥ�����ν����
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cc_pat        I/O    CC_PATERN * �������/���������
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
	Ulong i; /* ���̥Хåե��� */

	char *func_name = "compact_cc_pat_initialize";

	test_null_variable( cc_pat, FMT_CC_PATTERN, func_name );
	
	/* �Ȥꤢ���������� "X" �˽���� */
	cc_pat->pattern_a = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );
	cc_pat->pattern_b = ( Ulong *)topgunMalloc
		( FMT_Ulong, sizeof(  Ulong ), Comp_head.width, func_name );

	for ( i = 0 ; i < Comp_head.width ; i++ ) {

		/* X(a = 0, b = 0) �ǽ�������� */
		cc_pat->pattern_a[ i ] = 0;
		cc_pat->pattern_b[ i ] = 0;
	}
}


/********************************************************************************
 * << Function >>
 *		�ȹ礻�ѥ�����ν����
 *
 * << Function ID >>
 *	   	?-2
 *
 * << Return >>
 *      void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * comp_info     I/O    COMP_INFO * ���̥Хåե��˴ؤ�������
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

	Ulong i; /* bit_size������ */
	
	char *func_name = "compact_info_initialize";

	test_null_variable ( comp_info, FMT_GENE_INFO, func_name );

	/* comp_info */
	comp_info->mode     = COMP_NO_INFO;
	comp_info->n_cc_pat = Fsim_head.bit_size;

	/* �ѥ����������Ȥ��Ȥ� */

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
 *		�ȹ礻�ѥ����󤬤��٤�X�������å�����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      COMP_PAT_ALL_X     : �ѥ����󤬤��٤�X�Ǥ���
 *      COMP_PAT_NOT_X : �ѥ����󤬤��٤�X�Ǥʤ�
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cc_pattern    I      cc_pattern* �ȹ礻��ϩ�ѥ�����
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

	Ulong i = 0; /* width������ */
	Ulong pattern = 0; 
	
	char *func_name = "compact_check_pattern_all_x";

	test_null_variable ( cc_pattern, FMT_CC_PATTERN, func_name );
	
	for ( i = 0 ; i < Comp_head.width ; i++ ) {
		/* X�ʤ�Ф��٤�0�ˤʤ� */
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
 *		���̥Хåե����������֤�
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      buf_size : ���̥Хåե�������( Ulong )
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
 *		Comp_head�¹ԥѥ�᡼��������
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
	/* $$$ 100�ϥǥե������ $$$ */
	Comp_head.buf_size = ( 31 );
	utility_renew_parameter_with_env ( "TOPGUN_COM_BUF_SIZE", &( Comp_head.buf_size ), ( 31 ) );
	//printf("Comp_head.buf_size %ld\n", Comp_head.buf_size ) ;
	
}

/********************************************************************************
 * << Function >>
 *		Compact�μ¹Գ��ϥե饰
 *
 * << Function ID >>
 *	   	2-?
 *
 * << Return >>
 * COMP_ON_START : ���̳���
 * COMP_ON_STOP  : ���̳��Ϥ���
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
 *		���̥Хåե�����ѥ���ξ㥷�ߥ�졼������ѤΥѥ�������ꤹ��
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
	
	Ulong    i, j; /* ������ */

	topgun_2_utility_enter_start_time ( &Time_head.compact );

	fsim_info->n_cc_pat = Fsim_head.bit_size;
	
	/* ���̥Хåե�����Ф��ѥ���Υ��ԡ� */
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
		/* ����X�ξ��ʤ��ѥ���������Ф��� */
		/* �ѥ�������äƤ��� */


		mini_x_id = Comp_head.min_x_buf_id;

		//printf(" %5lu", mini_x_id );
		
		compact_get_pattern_with_id ( comp_info->cc_pat_wodge[ i ], mini_x_id );

		topgun_print_mes_compact_pattern
			( tmp_pat.pattern_a, tmp_pat.pattern_b, i, Comp_head.buf_size);
		topgun_print_mes_n_compact();
		
		/* �������Хåե��Ϥ��٤�X���᤹ */
		for ( j = 0 ; j < Comp_head.width ; j++ ) {
			Comp_head.buf_a[ mini_x_id ][ j ] = 0;
			Comp_head.buf_b[ mini_x_id ][ j ] = 0;
		}

		/* min_x_buf��Ʒ׻����� */
		compact_min_x_state_recalc ( mini_x_id ) ;
			
		/* ������min_x_buf_id��Ʒ׻����� */
		compact_search_min_x_state();
	}
	//printf("\n");
	
	topgun_2_utility_enter_end_time ( &Time_head.compact );
}

/* End of File */
