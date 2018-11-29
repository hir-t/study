/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief generate��λ��˿������ξ��֤ʤɤ򸵤ˤ�ɤ��ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include "topgun.h"
#include "topgunState.h" //! STATE_9
#include "topgunLine.h" 
#include "topgunAtpg.h"
#include "topgunTime.h"
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h" /* FEC_XX */

extern	GENE_HEAD Gene_head;

/* function list */

/* original function */
void atpg_reflesh ( void );
void atpg_flag_reflesh ( void );
void atpg_reflesh_justify_flag_top ( void );
void atpg_dfront_reflesh( void );
void atpg_imptrace_reflesh ( void );
void atpg_fp_iap_flag_reflesh ( LINE * );
void atpg_btree_reflesh( void );
void atpg_btree_state_reflesh ( BTREE * );
void atpg_reflesh_justify_flag ( JUST_INFO * );

/* other file function */

/* topgun_state.c */
void atpg_reflesh_state( LINE *, Uint );

/* topgun_dfront.c */
void gene_reflesh_state_dfront( DFRONT * );

/* topgun_btree.c */
void atpg_btree_stat_reflesh( BTREE * );

/* topgun_justified.c */
void atpg_btree_just_info_reflesh ( JUST_INFO * ); /* just_info��õ�� */

/* print function */
void topgun_print_mes( int, int );
void topgun_print_mes_id2( LINE * );
void topgun_print_mes_id_stat( LINE *, Ulong, Ulong );
void topgun_print_mes_del_just_flag_opening( void );
void topgun_print_mes_del_just_flag( LINE * );
void topgun_print_mes_id_pre_state9 ( LINE *, CONDITION );
void topgun_print_mes_id_after_state9 ( LINE *, CONDITION );
void topgun_print_mes_reflesh_normal( Ulong , Ulong);
void topgun_print_mes_reflesh_failure( Ulong , Ulong);

/* test function list */
void  test_line_in_null ( LINE * , char * );
void  test_dfront_next_null ( DFRONT *, char * );
void  test_line_flag ( char * ); /* line->flag�����٤�0��Ĵ�٤� */

/********************************************************************************
 * << Function >>
 *		check termination conditions of atpg
 *
 * << Function ID >>
 *	   	13-1
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/4/18
 *
 ********************************************************************************/

void atpg_reflesh
(
 void
){
	char *func_name = "atpg_reflesh"; /* �ؿ�̾ */

	topgun_3_utility_enter_start_time ( &Time_head.reflesh );
	
	topgun_print_mes(21,0);

	/* btree�򤿤ɤäƿ������ξ��֤򸵤��᤹ */
	atpg_btree_reflesh();

	/* dfront�򤿤ɤäƿ������ξ��֤򸵤��᤹ */
	atpg_dfront_reflesh();

	/* imp_trace�򤿤ɤäƿ������ξ��֤򸵤��᤹ */
	atpg_imptrace_reflesh();

	/* flag�⸵�ˤ�ɤ� */
	atpg_flag_reflesh();

	/* line->flag�ξ��֤�����å����� for debug */
	test_line_flag( func_name );

	topgun_3_utility_enter_end_time ( &Time_head.reflesh );
}


/********************************************************************************
 * << Function >>
 *		check termination conditions of atpg
 *
 * << Function ID >>
 *	   	13-6
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/4/18
 *
 ********************************************************************************/

void atpg_flag_reflesh
(
 void
){
	topgun_print_mes( 21, 3 );

	/* �ξ�flag */
	Gene_head.fault_line->flag &= ~LINE_FLT;

	/* ���½�λflag */
	Gene_head.flag &= ~ATPG_PROPA_E;

	/* ����������flag */
	Gene_head.flag &= ~ATPG_JUSTI_S;

	/* FP_IAP flag */
	atpg_fp_iap_flag_reflesh( Gene_head.fault_line );

	/* Gene_head clear */
	Gene_head.flag       = 0;
	Gene_head.fault_line = NULL;
	Gene_head.propa_line = NULL;
	Gene_head.dfront_t   = NULL;
	Gene_head.dfront_c   = NULL;
#ifndef OLD_IMP
	Gene_head.n_enter_imp_list = 0;
#else	
	Gene_head.imp_f      = NULL;
	Gene_head.imp_l      = NULL;
#endif /* OLD_IMP */
	Gene_head.btree      = NULL;
#ifndef OLD_IMP2
#else	
	Gene_head.imp_trace  = NULL;
#endif /* OLD_IMP2 */	

}

/********************************************************************************
 * << Function >>
 *		reflesh JUSTIFY flag
 *
 * << Function ID >>
 *    	13-7
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
 *
 * << Date >>
 *		2005/4/19
 *
 ********************************************************************************/

void atpg_reflesh_justify_flag_top
(
 void
){

	JUST_INFO *just_info = NULL;

	char      *func_name = "atpg_reflesh_justify_flag_top";  /* �ؿ�̾ */

	if ( Gene_head.propa_line != NULL ) {
		
		Gene_head.propa_line->flag &= ~JUSTIFY_NO;
		Gene_head.propa_line->flag &= ~JUSTIFY_FL;
	
		just_info = ( JUST_INFO * )topgunMalloc( FMT_JUST_INFO, sizeof( JUST_INFO ),
											  1, func_name );
		just_info->condition = 0;
		just_info->line      = Gene_head.propa_line;
		just_info->next      = NULL;

		atpg_reflesh_justify_flag ( just_info );

		topgunFree ( just_info, FMT_JUST_INFO, 1, func_name );
	}
}

/********************************************************************************
 * << Function >>
 *		reflesh dfrontier
 *
 * << Function ID >>
 *    	13-5
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type		description
 *	ATPG_HEAD	Gene_head	infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/20
 *
 ********************************************************************************/

void atpg_dfront_reflesh
(
 void
){

	Ulong       i; /* ��������ʬ���� */

	DFRONT_LIST *dl, *tmp_dl, *free_dl; /* dfront tree��é�뤿�� */
	DFRONT      *dfront;

	char        *func_name = "atpg_dfront_reflesh"; /* �ؿ�̾ */

	/* �ޤ�stat�򸵤ˤ�ɤ� */

	/* dfrontier tree �� top���� */
	dfront = Gene_head.dfront_c;

	/* state���᤹���� */
	/* imp_trace �� ass_list��free���� */
	while( dfront ){ 
		/* �ƻޤ��Ф����᤹������Ԥ� */
		gene_reflesh_state_dfront( dfront );
		/* root�Ρ��ɤޤǷ����֤��Ԥ� */
		dfront = dfront->prev;
	}

	/* dfrontier tree�� free���� */

	/* dfrontier tree �� top���� */
	dfront = Gene_head.dfront_t;

	if(dfront == NULL){
		return ;
	}

	/* stack�ν���� */
	dl = NULL;

	test_dfront_next_null ( dfront, func_name );
	
	/* dfrontier tree��é�� */
	for ( i = 0; i < dfront->line->n_out; i++ ) {

		tmp_dl = ( DFRONT_LIST * )topgunMalloc( FMT_DFRONT_LIST,
												 sizeof( DFRONT_LIST ),
												 1, func_name );

		tmp_dl->dfront = dfront->next[ i ];
		tmp_dl->next   = dl;
		dl             = tmp_dl;
	}

	while ( dl ) {
		dfront  = dl->dfront;
		free_dl = dl;
		dl      = dl->next;

		/* dfront�����Ĥʤ��ʤ���礬����(�̥롼�ȤǤ��ɤ��夤�����ʤ�) */
		if ( dfront->next == NULL ) {
		}
		else{
			
			test_dfront_next_null ( dfront, func_name );
			
			for( i = 0; i < dfront->line->n_out; i++ ) {

				tmp_dl = ( DFRONT_LIST * )topgunMalloc( FMT_DFRONT_LIST,
														 sizeof( DFRONT_LIST ),
														 1, func_name );
				tmp_dl->dfront = dfront->next[ i ];
				tmp_dl->next   = dl;
				dl             = tmp_dl;
			}
			topgunFree ( dfront->next, FMT_DFRONT_P, dfront->line->n_out, func_name );
		}
		
		topgunFree ( dfront, FMT_DFRONT, 1, func_name );
		topgunFree( free_dl, FMT_DFRONT_LIST, 1, func_name );
	}
	
	dfront = Gene_head.dfront_t;
	gene_reflesh_state_dfront( dfront );
	topgunFree ( dfront->next, FMT_DFRONT_P, dfront->line->n_out, func_name );
	topgunFree ( dfront, FMT_DFRONT, 1, func_name );
	Gene_head.dfront_t = NULL;
	Gene_head.dfront_c = NULL;
}

/********************************************************************************
 * << Function >>
 *		reflesh imp_trace (on conflict)
 *
 * << Function ID >>
 *    	13-2
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type		description
 *	ATPG_HEAD	Gene_head	infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/2
 *
 ********************************************************************************/

void atpg_imptrace_reflesh
(
 void
){

	/* �ޤ�state�򸵤ˤ�ɤ� */
#ifndef OLD_IMP2
	Ulong i;
	Ulong pos;


	for ( pos = Gene_head.last_n, i = 0 ; i < Gene_head.cnt_n; i++, pos-- ) {

		topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL );
			
		Gene_head.imp_list_n[ pos ]->state9 &= STATE9_XU;

		topgun_print_mes_id_after_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL);
	}

	/* �����󥿤δ��ᤷ */
	Gene_head.cnt_n  = 0;
	Gene_head.last_n = pos;
		
	for ( pos = Gene_head.last_f, i = 0 ; i < Gene_head.cnt_f; i++, pos-- ) {
			
		topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );
			
		Gene_head.imp_list_f[ pos ]->state9 &= STATE9_UX;
			
		topgun_print_mes_id_after_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );
	}
	/* �����󥿤δ��ᤷ */
	Gene_head.cnt_f = 0;
	Gene_head.last_f = pos;
	
#else	
	IMP_TRACE *imp_trace;     /* Gene_head.im_plist�򤿤ɤ� */
	IMP_TRACE *tmp_imp_trace; /* free�ѥݥ��� */

	char *func_name = "atpg_implist_stat"; /* �ؿ�̾ */
	
	imp_trace = Gene_head.imp_trace;

	/* ���� */
	while ( imp_trace ) {

		atpg_reflesh_state( imp_trace->line, imp_trace->condition );


		/* ���ν��� */
		tmp_imp_trace = imp_trace;
		imp_trace     = imp_trace->next;

		topgunFree( tmp_imp_trace, FMT_IMP_TRACE, 1, func_name );
	}
#endif /* OLD_IMP2 */	
}

/********************************************************************************
 * << Function >>
 *		reflesh FP_IAP & FP_VSS flag
 *
 * << Function ID >>
 *    	13-8
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 * line          I      LINE *      line pointer for inserted fault
 *
 * << extern >>
 * name			(I/O)	type		description
 *
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/2
 *
 ********************************************************************************/

void atpg_fp_iap_flag_reflesh
(
 LINE *line
 ){

	Ulong      i; /* ��������ʬ���� */
	Ulong      j; /* line�����Ͽ� */
	LINE_STACK *lstk, *tmp_lstk;
	LINE       *lp, *out;

	char       *func_name = "atpg_fp_iap_flag_reflesh"; /* �ؿ�̾ */

	test_line_in_null ( line , func_name );
	
	/* �����Υե饰�򸵤��᤹ */
	line->flag &= ~FP_IAP;
	line->flag &= ~FV_ASS;
	lstk = NULL;

	/*
	topgun_print_mes_id2( line );
	topgun_print_mes_n();
	*/

	for ( j = 0; j < line->n_in; j++ ) {
		
		line->in[ j ]->flag &= ~FV_ASS;

	}
	

	for( i = 0; i < line->n_out; i++ ) {
		out = line->out[ i ];

		test_line_in_null ( out , func_name );

		if ( out->flag & FP_IAP ) {
				out->flag &= ~FP_IAP;
				out->flag &= ~FV_ASS;

				for ( j = 0; j < out->n_in; j++ ) {
					
					out->in[ j ]->flag &= ~FV_ASS;

				}

				tmp_lstk = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK,
														  sizeof( LINE_STACK ),
														  1, func_name );
				tmp_lstk->line = out;
				tmp_lstk->next = lstk;
				lstk           = tmp_lstk;
		} 
	}
	while ( lstk ) {

		lp = lstk->line;
		tmp_lstk = lstk;
		lstk = lstk->next;
		
		topgunFree( tmp_lstk, FMT_LINE_STACK, 1, func_name );

		for( i = 0; i < lp->n_out; i++ ) {
			out = lp->out[ i ];

			test_line_in_null ( out , func_name );

			/* FP_IAP�ե饰�����äƤ����� */
			if ( out->flag & FP_IAP ) {

				out->flag &= ~FP_IAP;
				out->flag &= ~FV_ASS;

				for ( j = 0; j < out->n_in; j++ ) {
					out->in[ j ]->flag &= ~FV_ASS;
					
				}

				tmp_lstk = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK,
														  sizeof( LINE_STACK ),
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
 *		reflesh all state
 *
 * << Function ID >>
 *    	13-3
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type		description
 *	ATPG_HEAD	Gene_head	infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/13
 *
 ********************************************************************************/

void atpg_btree_reflesh
(
 void
){

	BTREE *btree;
	BTREE *tmp_btree;

	
	char  *func_name = "atpg_btree_reflesh"; /* �ؿ�̾ */

	/* FILO����᤹ 1��ƻ������� */
	topgun_print_mes( 21, 2 );

	btree = Gene_head.btree;
	while ( btree != NULL ) {
		/* ����btree��reflesh���� */
		atpg_btree_state_reflesh( btree );

		atpg_btree_just_info_reflesh ( btree->just_e );
		btree->just_e = NULL;
		
		atpg_btree_just_info_reflesh ( btree->just_d );
		btree->just_d = NULL;

		atpg_btree_just_info_reflesh ( btree->just_flag );
		
		/* ���ν��� */
		tmp_btree = btree;
		btree = btree->prev;

		topgunFree( tmp_btree, FMT_BTREE, 1, func_name );

	}
	Gene_head.btree = NULL;
}

/********************************************************************************
 * << Function >>
 *		reflesh state on btree
 *
 * << Function ID >>
 *    	13-4
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 * btree         I      BTREE *     back trace infomation
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/14
 *
 ********************************************************************************/

void atpg_btree_state_reflesh
(
 BTREE *btree
 ){
#ifndef OLD_IMP2
	Ulong i;
	Ulong pos;

	topgun_print_mes( 21, 4 );

	if ( btree->cnt_n != 0 ) {

		topgun_print_mes_reflesh_normal (btree->last_n, btree->cnt_n );

		for ( pos = btree->last_n, i = 0 ; i < btree->cnt_n; i++, pos-- ) {
			
			topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL );

			Gene_head.imp_list_n[ pos ]->state9 &= STATE9_XU;
		
			topgun_print_mes_id_after_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL );
		}
		/* �����󥿤δ��ᤷ */
		Gene_head.last_n  = pos;

		btree->last_n  = 0;
		btree->cnt_n   = 0;
	}
	
	if ( btree->cnt_f != 0 ) {

		topgun_print_mes_reflesh_failure (btree->last_f, btree->cnt_f );
		
		for ( pos = btree->last_f, i = 0 ; i < btree->cnt_f; i++, pos-- ) {

			topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );

			Gene_head.imp_list_f[ pos ]->state9 &= STATE9_UX;

			topgun_print_mes_id_after_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );
		}
		/* �����󥿤δ��ᤷ */
		Gene_head.last_f  = pos;
		
		btree->last_f  = 0;
		btree->cnt_f   = 0;
	}

	topgun_print_mes( 21, 5 );
#else 	
	IMP_TRACE *imp_trace; /* �ް����򸵤��᤹����Υǡ��� */
	IMP_TRACE *tmp_imp_trace; /* ���곫���� */


	char     *func_name = "atpg_btree_state_reflesh"; /* �ؿ�̾ */

	/* �����Ƥ��ͤ򸵤��᤹ */

	/* �ް����η�̤򸵤��᤹ */
	topgun_print_mes( 21, 4 );
	imp_trace = btree->imp_trace;

	while ( imp_trace ) {

		/* �ºݤ��ͤ��᤹ */
		atpg_reflesh_state( imp_trace->line, imp_trace->condition );

		/* �����᤹����ν��� */
		tmp_imp_trace = imp_trace;
		imp_trace     = imp_trace->next;
		
		topgunFree( tmp_imp_trace, FMT_IMP_TRACE, 1, func_name );
	}
	btree->imp_trace = NULL;

	/* *** �ץ����å� *** */
	topgun_print_mes( 21, 5 );

	/* ����imp_trace�ˤϤ���Τ����� 2006/05/25 */
	if ( btree->line != NULL ) {
		//atpg_reflesh_state( btree->line, btree->condition );
	}
#endif /* OLD_IMP2 */	
}

/********************************************************************************
 * << Function >>
 *		reflesh JUSTIFY flag
 *
 * << Function ID >>
 *    	13-?
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
 *
 * << Date >>
 *		2005/11/18
 *
 ********************************************************************************/

void atpg_reflesh_justify_flag
(
 JUST_INFO *just_info
){

	Ulong      i; /* ��������ʬ���� */
	LINE       *line, *in;
	LINE_STACK *stk = NULL;
	LINE_STACK *tmp_stk;

	char     *func_name = "atpg_reflesh_justify_flag"; /* �ؿ�̾ */

	topgun_print_mes_del_just_flag_opening();
	
	while ( just_info ) {
		/* just_info����Ͽ����Ƥ��뿮������flag���ᤵ�ʤ� */
		tmp_stk = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK, sizeof( LINE_STACK ),
											 1, func_name );

		tmp_stk->line = just_info->line;
		tmp_stk->next = stk;
		stk           = tmp_stk;

		just_info     = just_info->next;
	}

	/* ����¦��flag���ݤ������ */
	while ( stk ) {

		tmp_stk = stk;
		line    = stk->line;
		stk     = stk->next;

		topgunFree( tmp_stk, FMT_LINE_STACK, 1, func_name );

		test_line_in_null ( line, func_name );

		for( i = 0; i < line->n_in; i++){

			in = line->in[ i ];
			
			if ( ( in->flag & JUSTIFY_NO ) ||
				 ( in->flag & JUSTIFY_FL ) ) {
				/* ���ˤ�ɤ� */
				in->flag &= ~JUSTIFY_NO;
				in->flag &= ~JUSTIFY_FL;

				topgun_print_mes_del_just_flag(in);
				
				/* ���ν��� */
				tmp_stk = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK,
														 sizeof( LINE_STACK ),
														 1, func_name );
				tmp_stk->line = in;
				tmp_stk->next = stk;
				stk           = tmp_stk;
			}
		}
	}
}

/********************************************************************************
 * << Function >>
 *		implication list
 *
 * << Function ID >>
 *    	13-?
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
 *	GENE_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/6/13
 *
 ********************************************************************************/
#ifdef OLD_IMP
void gene_reflesh_imp_list
(
 void
 ){

	IMP_INFO *imp_info;
	IMP_INFO *free_imp_info;
	
	char     *func_name = "atpg_reflesh_imp_list"; /* �ؿ�̾ */
	
	imp_info = Gene_head.imp_l;

	while ( imp_info  ){

		free_imp_info = imp_info;
		imp_info = imp_info->next;

		topgunFree ( free_imp_info , FMT_IMP_INFO, 1, func_name );
	}
	Gene_head.imp_l = NULL;
	Gene_head.imp_f = NULL;
}
#endif /* OLD_IMP */
/* End of File */



