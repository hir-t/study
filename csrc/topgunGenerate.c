/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ѥ����������ޥ͡�����˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunFlist.h" /* FLIST */
#include "topgunCompact.h"
#include "topgunTime.h"
#include "topgunMemory.h"
#include "topgunError.h"

extern  GENE_HEAD Gene_head;
extern  LINE_INFO Line_info;

/* function list */
void     generate ( GENE_INFO *, char * );

/* topgun_select.c */
extern void     select_fault( GENE_INFO *, char * );
extern void     select_fault_search_basis_random_det( void );

/* topgun_insert.c */
GENE_RESULT gene_insert_fault( FLIST *, Ulong );

/* topgun_chk.c */
int      atpg_check_finish_generate ( Ulong );

/* topgun_select.c */
Ulong select_fault_search_basis_random_det_rest_calc( void );


/* topgun_propagate.c */
int      gene_propagate_fault ( FLIST *fault, Ulong ); /* �ξ�����¤����뤿����ͳ�����Ƥ���� */
BACK_RESULT gene_back_track ( void ) ; /* ������Ƥ��ͤ��᤹ */

/* topgun_justified.c */
int      atpg_justified ( Ulong ); /* ���������뿮�����������ӡ���������Ԥ� */

/* topgun_output.c */
void     one_pat_output ( void ); /* 1�ѥ�������Ϥ��� */

/* topgun_class.c */
int      atpg_untest_redun ( void ); /* ��Ĺ�ξ㤫�ƥ���������ǽ�ξ㤫��Ƚ�Ǥ��� */

/* topgun_fsim.c */
void     pattern_bridge_by_random( void );

/* topgun_compress.c */
void     pattern_get_from_generate ( CC_PATTERN * );

/* topgun_reflesh.c */
void     atpg_reflesh ( void );

/* topgun_uty.c */
void utility_renew_parameter_with_env ( char *, Ulong *, Ulong );


/* topgun_error.c */
void     topgun_error( TOPGUN_ERROR_CODE, char * ); /* ���顼��å���������Ϥ��� */



/* print function */
void     topgun_print_mes( int, int );
void     topgun_print_mes_dfront_t( void );
void     topgun_print_mes_atpg_result_all( FLIST *, int );

void utility_enter_end_time_gene( PROCESS_TIME *, GENE_INFO * );    //!<

extern void test_null_variable( void *, TOPGUN_MEMORY_TYPE_ID, char *);

/********************************************************************************
 * << Function >>
 *		manger of generate pattern
 *
 * << Function ID >>
 *    	2-3
 *
 * << Return >>
 *	result		: result of generate pattern (bit)
 *                TOPGUN_DTA ( on: ATPG det, off: can't generate pattern )
 *                TOPGUN_RED ( on: redundant fault,  off: untestable fault )
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault		 I		FLIST *	target fault data
 *
 * << extern >>
 * name			(I/O)	type	description
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/13
 *		2005/ 8/24 << Modified ���ѽ�ˤ����褦���ѹ�(M.Yoshimura) >> 
 *
 ********************************************************************************/

void generate
(
 GENE_INFO *gene_info,
 char *fault_id
 ){

	BACK_RESULT b_result;
	Ulong n_rand_fault = 0;
	Ulong imp_id = 0; // �������������ܤ��ͳ�����ƤǴްդ��줿����
	                  // gene_info�ˤ⤿���Ƥ��ɤ�����

	static int  select_fault_mode = 2;   // 0 : �����ม�Ф����򤷤ʤ�
	                                     // 1 : �����ม�Ф����򤹤�

	char *func_name = "generate"; /* �ؿ�̾ */

	topgun_2_utility_enter_start_time ( &Time_head.generate );

	b_result.result = 1;   /* $$$ ?? $$$ */	/* �����(!=0)�Ǥ�����ɤ� */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2 �ξ�����                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	select_fault( gene_info, fault_id );

	if ( gene_info->f_class == GENE_NO_FAULT ){
		/* �����ܤξ��ϥ�����ѥ���Ǹ��Ф����ξ������پ�˺ܤ��� */
		if ( select_fault_mode == 0 ) {
			select_fault_mode = 1;
			select_fault_search_basis_random_det();

			n_rand_fault = select_fault_search_basis_random_det_rest_calc();
			printf("Virtual Detect %lu\n",n_rand_fault );
			Line_info.n_fault_atpg_count += n_rand_fault;
		
			select_fault( gene_info, fault_id );
		}
	}

	if ( gene_info->f_class == GENE_NO_FAULT ){
		/* �ξ㤬����Ǥ��ʤ���� */
		topgun_2_utility_enter_end_time ( &Time_head.generate );
		return;
	}
	topgun_print_mes( 2, 3 );
	
	
	Gene_head.back_all = 0;
	Gene_head.back_drive = 0;
	Gene_head.back_drive_just = 0;

	test_null_variable ( gene_info, FMT_GENE_INFO, func_name );

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-1 �ξ������                                                 %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    gene_info->result = gene_insert_fault ( gene_info->fault, ++imp_id );

    if ( gene_info->result == INSERT_FAIL ) {
		/* �ξ������˼��Ԥ������ */
		/* ���¤��������򤻤���atpg_reflesh()�򤷤ƥѥ�����������λ */
		b_result.result = 0;
    }

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-2 ��ճ�����                                                 %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	//gene_info->result = gene_uniq ( gene_info->fault->line );
	
	
	/* ̤���� (2005/8/25) */

    /* �ξ����� */
	while ( b_result.result ) {

		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-3-3 �ѥ�����������λ?                                      %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		gene_info->result = atpg_check_finish_generate( Gene_head.back_all );

		switch ( gene_info->result ) {

		case PROPA_CONT:

			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* %%% 2-3-4 �ξ����¤�ʤ��                                   %%% */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			gene_info->result = gene_propagate_fault( gene_info->fault, ++imp_id );

			if ( ( gene_info->result == PROPA_FAIL ) ||
				 ( gene_info->result == PROPA_FAIL_DRIVE ) ||
				 ( gene_info->result == PROPA_FAIL_ENTER ) ||
				 ( gene_info->result == PROPA_FAIL_IMP ) ) {
				/* ���¤˼��Ԥ����Τǡ��Хå��ȥ�å��򤹤� */

				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				/* %%% 2-3-7 �����򸵤��᤹                                 %%% */
				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				b_result = gene_back_track();
			}
			break;

		case JUSTI_CONT:

			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* %%% 2-3-5 ��������ʤ��                                     %%% */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			gene_info->result = atpg_justified( ++imp_id );

			if ( ( gene_info->result == JUSTI_FAIL ) ||
				 ( gene_info->result == JUSTI_FAIL_ENTER ) ||
				 ( gene_info->result == JUSTI_FAIL_IMP ) ) {

				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				/* %%% 2-3-7 �����򸵤��᤹                                 %%% */
				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				/* �ޤ�Ŭ�� */
				b_result = gene_back_track();
			}
			break;

		case JUSTI_PASS: /* �ξ����¤Ȥ��������⽪λ = ATPG��λ */

			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* %%% 2-3-6 1�ѥ�������Ϥ���Ϥ���                            %%% */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

			gene_info->f_class = GENE_DET_A;

			/* �ѥ�������� */
			// for b19_C��
			one_pat_output(); //<-���֤�topgun_atpg�Ǥ���٤�

			b_result.result = 0; /* loop��ȴ���Ф�(��ä��ɤ��������!!) */

			topgun_print_mes_dfront_t(); /* dfront tree ��dump */
			break;

		case LIM_BACK_END: /* back track�ξ���ͤΰٽ�λ = �Ǥ��ڤ� */

			gene_info->f_class = GENE_ABT;

			b_result.result = 0; /* loop��ȴ���Ф�(��ä��ɤ��������!!) */
			break;

		default: 
			topgun_error( FEC_PRG_JUDGE_GENE_END, func_name ); /* ��ʤ��Ϥ� */
		}
	}



	/* �ѥ�����������λ!! */

	switch ( gene_info->f_class ){
	case NO_DEFINE:
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-3-8 ��Ĺ�ξ�/Untestable��Ƚ��                              %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		if ( TOPGUN_RED == atpg_untest_redun() ) {
			/* ��Ĺ�ξ�� */
			gene_info->f_class = GENE_RED;
		}
		else {
			gene_info->f_class = GENE_UNT;
		}
		break;
	case GENE_DET_A:
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-3-8 ATPG�ˤ�ä����������ѥ����������                     %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		pattern_get_from_generate( gene_info->cc_pat );

		break;
	default:
		break;
	}

	topgun_print_mes_atpg_result_all( gene_info->fault, gene_info->f_class );
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-9 ATPG�ˤ�ä��Ѳ������������򸵤��᤹                       %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	atpg_reflesh();

	topgun_2_utility_enter_end_time_gene ( &Time_head.generate, gene_info );

}

/********************************************************************************
 * << Function >>
 *		Gene_head�¹ԥѥ�᡼��������
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
 * Gene_head     I/O    GENE_HEAD   
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/08
 *
 ********************************************************************************/

void gene_env_init
(
 void
 ){
	/* $$$ 100�ϥǥե������ $$$ */
	utility_renew_parameter_with_env ( "TOPGUN_LIM_BACK", &( Gene_head.limit_back ), 100 );
}

/* End of File */
