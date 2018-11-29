/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ξ������˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include<stdio.h>

#include "topgun.h"
#include "topgunState.h"  /* STATE_3, STATE_9 */
#include "topgunLine.h"   /* LINE, LINE_TYPE, LINE_INFO */
#include "topgunAtpg.h" // IMP_RESULT, IMP_STATE
#include "topgunFlist.h"  /* FLIST */
#include "topgunTime.h"
#include "topgunMemory.h" /* FMT_XX */

extern	GENE_HEAD	Gene_head;

/* Function list */
GENE_RESULT gene_insert_fault( FLIST *, Ulong );
static ASS_LIST *atpg_make_ass_list_for_fault( FLIST * );
static void     atpg_enter_ass_list_insert_fault ( ASS_LIST * );
void     atpg_init_fault_propagation_potential ( LINE * );
IMP_RESULT implication ( ) ; /* �ް����¹Դؿ� */
void     atpg_init_btree( void );

/* state function */
extern STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 );
extern IMP_RESULT atpg_update_static_learn_state ( LEARN_LIST * );
extern IMP_RESULT gene_enter_state( ASS_LIST *, Ulong ); /* state���ͤ����ꤹ��ؿ� */

/* dfrontier tree function */
extern void gene_imp_trace_2_dfront ( Ulong );
extern void gene_init_dfront ( LINE * );


LEARN_LIST *gene_uniq ( LINE * );

/* print function */
void topgun_print_mes( int, int );
void topgun_print_mes_result_state9( int, int, STATE_9 );
void topgun_print_mes_result_line( int, int, LINE * );
void topgun_print_mes_id2( void );
void topgun_print_mes_n( void );

/* test function */
void test_line_in_null ( LINE *, char * );
void test_flist ( FLIST * , char * );
void test_flist_info_fault ( Ulong, char * );
void test_ass_list ( ASS_LIST *, char * );


/********************************************************************************
 * << Function >>
 *		insert target fault
 *
 * << Function ID >>
 *    	12-1
 *
 * << Return >>
 *	INSERT_PASS	: Terminated in normal
 *	INSERT_FAIL	: can't insert fault
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault		 I		FLIST *	target fault data
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/6
 *
 ********************************************************************************/

GENE_RESULT gene_insert_fault
(
 FLIST *fault,
 Ulong imp_id
 ){

	ASS_LIST *ass_insert_fault = NULL;
	IMP_RESULT  imp_result;    /* implication�¹Է�� */
	//LEARN_LIST *uniq_list = NULL;

	char *func_name = "gene_insert_fault"; /* �ؿ�̾ */

	topgun_3_utility_enter_start_time ( &Time_head.insert_fault );
	
	test_flist ( fault , func_name );

	// initial imp_result ( imp_id & imp_result )
	imp_result.i_state = 0;
	//imp_result.imp_id  = 0;
	

	// ass_list�����
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% 2-3-1-1 �ξ��ͤ�����                                             %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	ass_insert_fault = atpg_make_ass_list_for_fault( fault );

	if ( ass_insert_fault == NULL ) {
		/* �ξ㤬����Ǥ��ʤ��ä���� */

		topgun_3_utility_enter_end_time ( &Time_head.insert_fault );
		return( INSERT_FAIL );
	}
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-1-2 LINE_FLT & FP_IAP & FV_ASS �ե饰��Ω�Ƥ�                %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	atpg_init_fault_propagation_potential ( fault->line );

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-1-2 D-frontier������                                       %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	gene_init_dfront ( fault->line );

	/* ���FP_IAP �� FV_ASS�����ꤹ��ɬ�פ��� */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-1-3 �ξ��ް����ꥹ�Ȥ���Ͽ                               %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    // �ͳ�����ƤʤΤ�max_imp_id��0���ɤ�
	imp_result = gene_enter_state ( ass_insert_fault, 0 ); 

	test_ass_list ( ass_insert_fault, func_name );

	/* �Хå��ȥ졼�����Ѥ� */
	atpg_enter_ass_list_insert_fault ( ass_insert_fault );
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-X �ް����                                                     %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* $$$ ���ˤޤȤ�Ƥ⤤������ $$$ */

	if ( IMP_CONF != imp_result.i_state ) {
		imp_result = implication ();
	}
    if ( IMP_CONF == imp_result.i_state ) {

		/* imp_trace��dfront tree����Ͽ */
		gene_imp_trace_2_dfront( imp_id );
		
		topgun_3_utility_enter_end_time ( &Time_head.insert_fault );
		/* �ξ㤬����Ǥ��ʤ��ä���� IMP_CONF->INSERT_FAIL���Ѵ� */
		return( INSERT_FAIL );
    }

	/* ��ճ����� */
	/* �����Ȥ櫓��ɬ�פ����뤫? */
	// -> CONF���Ϥκݤˡ��嵯������CONF����ճ�������CONF�����ڤ�ʬ����ҤĤ褦������(2007/06/05)
	//uniq_list = gene_uniq ( fault->line );

	/* imp_trace��dfront tree����Ͽ */
	gene_imp_trace_2_dfront( imp_id );

	topgun_3_utility_enter_end_time ( &Time_head.insert_fault );
	
    if ( IMP_CONF == imp_result.i_state ) {
		/* �ξ㤬����Ǥ��ʤ��ä���� IMP_CONF->INSERT_FAIL���Ѵ� */
		return( INSERT_FAIL );
    }
    return( INSERT_PASS );
}

/********************************************************************************
 * << Function >>
 *		enter target fault
 *
 * << Function ID >>
 *    	12-2
 *
 * << Return >>
 *	GENE_CONT   : Terminated in normal
 *	GENE_END    : generate end
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault		 I		FLIST *	target fault data
 *
 * << extern >>
 * name			(I/O)	type	    description
 *	ATPG_HEAD	 I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

ASS_LIST *atpg_make_ass_list_for_fault
(
 FLIST *fault
 ){

	STATE_3   n_state3;     /* �������������� */
	LINE     *line = NULL;  /* �ξ㿮�����Υݥ��� */

	ASS_LIST *normal  = NULL; /* �����ͤ��ͳ������ */
	ASS_LIST *failure = NULL; /* �ξ��ͤ��ͳ������ */
	
    char *func_name = "atpg_make_ass_list_for_fault";	/* �ؿ�̾ */

    /* line ������ */
    line = fault->line;

	/* Gene_head.fault_line������ */
	Gene_head.fault_line = line;


    /* �̾����stat�γ�ǧ */
	/* �ξ�����ͤϳ�ǧ���ʤ��Ƥ�褤���� */
	n_state3 = atpg_get_state9_2_normal_state3 ( line->state9 );

    topgun_print_mes( 2, 4 );
    topgun_print_mes_result_state9( 2, 7, line->state9 );

	test_flist_info_fault ( fault->info, func_name );


	/* �ް����ꥹ����Ͽ�ѥ������ */
	normal = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ),
										  1, func_name );
	failure = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ),
										   1, func_name );

	/* initialize */
	normal->ass_state3  = STATE3_C;
	normal->condition   = COND_NORMAL;
	normal->line        = line;
	normal->next        = failure;  // Ϣ�뤵����
		
	failure->ass_state3 = STATE3_C;
	failure->condition  = COND_FAILURE;
	failure->line       = line;
	failure->next       = NULL;
	
	/* �ξ����� */
	if ( fault->info & TOPGUN_SA0 ) {
		if ( n_state3 == STATE3_0 ) {
			/* sa0�λ��������ͤ����0�Ǥ���иξ������Ǥ��ʤ� */
			topgun_print_mes( 2, 6 );

			topgunFree( normal,  FMT_ASS_LIST, 1, func_name );
			topgunFree( failure, FMT_ASS_LIST, 1, func_name );
			
			return( NULL );
		}
		/* �ξ��ͤ򿮹��������� */
		normal->ass_state3  = STATE3_1;
		failure->ass_state3 = STATE3_0;
	}
	else {
		/* �¼���fault->info & TOPGUN_SA1�ξ�� */
		/* sa0�λ��������ͤ����1�Ǥ���иξ������Ǥ��ʤ� */
		if ( n_state3 == STATE3_1 ) {
			topgun_print_mes( 2, 6 );

			topgunFree( normal,  FMT_ASS_LIST, 1, func_name );
			topgunFree( failure, FMT_ASS_LIST, 1, func_name );
			
			return( NULL );
		}
		/* �ξ��ͤ򿮹��������� */
		normal->ass_state3  = STATE3_0;
		failure->ass_state3 = STATE3_1;
	}
	test_ass_list ( normal, func_name ) ;

	topgun_print_mes_result_line( 2, 5, line );


	/* �����˱�����assign���Ȥ߹�碌��ư��������ˤ�?
	 -> ���줾��δؿ���Ĥ��뤷���ʤ��ΤǤ�?
	 -> �����ñ���state�δؿ������ */

	/* back trace tree���� root�������� */
	/*
	atpg_init_btree( );
	*/

	return( normal );
}


/********************************************************************************
 * << Function >>
 *		initailize LINE_FLT & FP_IAP & FV_ASS flag
 *
 * << Function ID >>
 *    	12-3
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault		 I		LINE *	line pointer for target fault
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/10
 *
 ********************************************************************************/

void atpg_init_fault_propagation_potential
(
 LINE *line
 ){

	Ulong      i = 0; /* line�ν��Ͽ� */
	Ulong      j = 0; /* line�����Ͽ� */
	LINE_STACK *lstk = NULL;
	LINE_STACK *tmp_lstk = NULL;
	LINE       *lp = NULL;
	LINE       *out = NULL;

	char *func_name = "atpg_init_fault_propagation_potential"; /*  �ؿ�̾        */

	topgun_print_mes( 20, 0 );
	topgun_print_mes( 20, 1 );

	test_line_in_null ( line, func_name );

	/* line->flag�˸ξ������ */
	/* �ް����ǻ��� */
	line->flag |= LINE_FLT;

	
	line->flag |= FP_IAP;
	line->flag |= FV_ASS;
	lstk = NULL;

	for ( j = 0; j < line->n_in; j++ ) {

		line->in[ j ]->flag |= FV_ASS;
	}

	

	/* �ξ����¥ե饰�ȸξ�������ե饰�򤿤Ƥ� */
	for ( i = 0; i < line->n_out; i++ ) {
		out = line->out[ i ];

		test_line_in_null ( out, func_name );
		
		out->flag |= FP_IAP;
		out->flag |= FV_ASS;

		for ( j = 0; j < out->n_in; j++ ) {
			
			out->in[ j ]->flag |= FV_ASS;

		}
			
		tmp_lstk = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK, sizeof( LINE_STACK ),
												 1, func_name );
		tmp_lstk->line = out;
		tmp_lstk->next = lstk;
		lstk           = tmp_lstk;

	}

	while ( lstk ) {

		lp       = lstk->line;
		tmp_lstk = lstk;
		lstk     = lstk->next;

		topgunFree( tmp_lstk, FMT_LINE_STACK, 1, func_name );

		for( i = 0; i < lp->n_out; i++ ) {
			out = lp->out[ i ];

			test_line_in_null ( out, func_name );

			/* FP_IAP�ե饰�����äƤ��ʤ���� */
			if ( ! ( out->flag & FP_IAP ) ) {

				out->flag |= FP_IAP;
				out->flag |= FV_ASS;

				for ( j = 0; j < out->n_in; j++ ) {
					out->in[ j ]->flag |= FV_ASS;

				}
				tmp_lstk = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK, sizeof( LINE_STACK ),
														 1, func_name );

				tmp_lstk->line = out;
				tmp_lstk->next = lstk;
				lstk           = tmp_lstk;
			}
		}
	}
}

/********************************************************************************
 * << Function >>
 *		initailize FP_IAP flag
 *
 * << Function ID >>
 *    	12-3
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type			description
 * ass_list  	 I		ASS_LIST *		assignment for insert fault
 *
 * << extern >>
 * name			(I/O)	type	        description
 * Gene_head     I      GENE_HEAD
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/24
 *
 ********************************************************************************/

void atpg_enter_ass_list_insert_fault
(
 ASS_LIST *ass_list 
 ){

	char *func_name = "atpg_enter_ass_list_insert_fault"; /* �ؿ�̾ */

	test_ass_list ( ass_list, func_name );

	/* ass_list ��dfront_c��ass_list�κǸ�����­�� */
	if ( Gene_head.dfront_t->ass_list == NULL ) {
		Gene_head.dfront_t->ass_list = ass_list;
	}
	else{
		ass_list->next = Gene_head.dfront_t->ass_list;
		Gene_head.dfront_t->ass_list = ass_list;
	}
}


/* End of File */
