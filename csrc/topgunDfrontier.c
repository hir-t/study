/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief D�ե��ƥ����˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2005 / 01 / 20   initialize
		2007 / 06 / 01   imp_id_n & imp_id_f
		2016 / 12 / 12   initialize
*/

#include<stdio.h>
#include "topgun.h"
#include "topgunState.h" /* STATE_3 , STATE_9 */
#include "topgunLine.h"  /* LINE , LINE_TYPE, LINE_INFO */
#include "topgunAtpg.h"  /* LINE_LIST */
#include "topgunTm.h"    /* TM_XXX */
#include "topgunMemory.h" /* FMT_DFORNT */
#include "topgunError.h"  /* FEC_XXXX */

extern	GENE_HEAD	Gene_head;


/* function list */
extern void   geneg_init_dfront ( LINE *fault_line ); /* dfront tree�ν���� */
extern GENE_RESULT atpg_drive_dfront( void );
extern void atpg_ent_propa_dfront( ASS_LIST * );
extern void gene_reflesh_state_dfront( DFRONT * );
extern void gene_imp_trace_2_dfront( Ulong );


/* other function list */
void   atpg_reflesh_state( LINE *, int );
int    atpg_get_d_state9( STATE_9 );
GENE_RESULT atpg_make_dfront( DFRONT * );
ASS_LIST *utility_combine_ass_list( ASS_LIST *, ASS_LIST *);

/* print function */
void   topgun_print_mes_id_type_state9( LINE * );
void   topgun_print_mes( int, int );
void   topgun_print_mes_n();
void   topgun_print_mes_result_line( int, int, LINE * );
void   topgun_print_mes_id_pre_state9 ( LINE *, CONDITION );
void   topgun_print_mes_id_after_state9 ( LINE *, CONDITION );
void   topgun_print_mes_reflesh_normal( Ulong , Ulong);
void   topgun_print_mes_reflesh_failure( Ulong , Ulong);

/* test function */
void  test_line_null ( LINE *, char * );
void  test_ass_list ( ASS_LIST *, char * );
void  test_dfront ( DFRONT *, char  * );


/********************************************************************************
 * << Function >>
 *		enter D-frontier
 *
 * << Function ID >>
 *    	18-1
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line          I      LINE *  line poiter of d-frontier
 *
 * << extern >>
 * name			(I/O)	type	    description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/20
 *
 ********************************************************************************/

void gene_init_dfront
(
 LINE *fault_line        
 ){

	Ulong     i = 0;             /* �������ν��Ͽ������� */
	DFRONT 	  *dfront    = NULL; /* dfront�ѥ���ݥ��� */
	DFRONT    *child     = NULL; /* ������dfront�ѥ���ݥ��� */
	DFRONT    **children = NULL; /* ������dfront�ѥ���ݥ��󥿤Υݥ��� */
	
	char     *func_name = "gene_init_dfront"; /* �ؿ�̾ */

	topgun_print_mes_result_line( 23, 0, fault_line );

	test_line_null ( fault_line, func_name );

    /* D-frontier��top�Ρ��ɥ������ */
	dfront = ( DFRONT * )topgunMalloc( FMT_DFRONT, sizeof( DFRONT ),
										1, func_name );

	/* dfront �ν���� */
	dfront->flag      = DF_ROOT;
    dfront->line      = fault_line;
    dfront->ass_list  = NULL;
	dfront->last_n    = 0;
	dfront->last_f    = 0;
	dfront->cnt_n     = 0;
	dfront->cnt_f     = 0;
	dfront->prev      = NULL;
	dfront->next      = NULL;


	/* ����������õ������D-Frontier�Υ������� */
	children = ( DFRONT ** )topgunMalloc( FMT_DFRONT_P, sizeof( DFRONT * ),
										   fault_line->n_out, func_name );

	for ( i = 0; i < fault_line->n_out; i++ ) {

		/* �Ҷ���D-frontier�ѥ������ */
		child = ( DFRONT * )topgunMalloc( FMT_DFRONT, sizeof( DFRONT ),
										   1, func_name );

		/* child �ν���� */
		/* $$$ branch�ν��֤��Ĵ�¬��ν�ˤʤäƤ�г� $$$ */
		child->flag      = DF_NOCHECK;
		child->line      = fault_line->out[ i ];
		child->ass_list  = NULL;
		child->last_n    = 0;
		child->last_f    = 0;
		child->cnt_n     = 0;
		child->cnt_f     = 0;
		child->prev      = dfront;
		child->next      = NULL;

		children[ i ]   = child;
	}
	dfront->next = children;

	/* D-frontier �ꥹ�Ȥ���Ͽ */

#ifndef TOPGUN_TEST
    if ( Gene_head.dfront_t != NULL ) {
		topgun_error( FEC_PRG_ATPG_HEAD, func_name );
	}
#endif /* TOPGUN_TEST */

	Gene_head.dfront_t  = dfront;
	Gene_head.dfront_c  = dfront;
	/* ���̤����פΤϤ� */
	Gene_head.last_n    = 0;
	Gene_head.last_f    = 0;
	Gene_head.cnt_n     = 0;
	Gene_head.cnt_f     = 0;

	/* back trace tree ����Ͽ */
	/* dfrontier��assign����Ͽ��ˡ���ѹ������������� 2005/04/07
	if(atpg_ent_dfront_btree(dfront) == ERROR){
		return(ERROR);
	}
	*/
}

/********************************************************************************
 * << Function >>
 *		dfrontier root check
 *
 * << Function ID >>
 *      18-2
 *
 * << Return >>
 *	DRIVE_CONT	: D-Drive cotinue
 *	DRIVE_END	: D-Drive end
 *	DRIVE_IMPOS : D-Drive impossible
 *
 * << Argument >>
 * args			(I/O)	type			description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/25
 *
 ********************************************************************************/

GENE_RESULT atpg_drive_dfront
(
 void
){

	GENE_RESULT result; /* �ξ����¤η�� */
	DFRONT      *df;    /* dforntier */

	char    *func_name = "atpg_drive_dfront"; /* �ؿ�̾ */

	topgun_print_mes( 23, 2 );

	/* dfrontier�ꥹ�Ȥκ��� */
	if ( Gene_head.dfront_c == NULL ) {
		/* dfrontier������ */
		/* ��Ĺ�⤷����untest */
		return( DRIVE_IMPOS );
	}

	while ( 1 ) {
		
		df = Gene_head.dfront_c;

		topgun_print_mes_id_type_state9( df->line );

		/* ���ߤ�dfrontier��state��Ĵ�٤� */

		result = atpg_get_d_state9( df->line->state9 );

		if ( result == (GENE_RESULT) DRIVE_POINT ) {
			/* df->line�ˤ�D�����¤��Ƥ��� */
			/* ���·�ϩ���ɤäơ�D�����ֿʤ�Ǥ���Ȥ����õ�� */

			topgun_print_mes( 23, 3 );

			/* $$$ �����å�������֤ˤϲ��ꤢ�� $$$ */
			if ( df->line->n_out == 0 ) {
				/* dfrontier������ */
				if ( df->line->type == TOPGUN_PO ) {
					/* df(dfrontier)���������Ϥ���ã = �ξ����´�λ */
					Gene_head.dfront_c->line = df->line;
					Gene_head.propa_line = df->line;
					Gene_head.flag |= ATPG_PROPA_E;
					return ( DRIVE_END );
				}
				else if ( df->line->type == TOPGUN_BLKO ) {
					return( DRIVE_IMPOS );
				}
				/* $$$ �̤Υ��顼�β�ǽ������ $$$ */
				topgun_print_mes( 23, 4 );
				return( DRIVE_IMPOS );
			}
			else if( df->line->n_out > 1 ){

				/* ʣ������� */
				topgun_print_mes( 23, 6 );

				/* dfront->next�����������ʤ����� */
				result = atpg_make_dfront( df );
				if ( result == (GENE_RESULT) DRIVE_IMPOS ) {
					return( DRIVE_IMPOS );
				}
			}
			else if( df->line->n_out == 1 ) {

				test_line_null ( df->line->out[0], func_name );

				/* ñ�����ξ�� */
				topgun_print_mes( 23, 5 );

				/* ����Dfrontier��ʤ�� */
				result = atpg_make_dfront( df );
				if ( result == (GENE_RESULT)DRIVE_IMPOS ) {
					return( DRIVE_IMPOS ) ;
				}
				topgun_print_mes_id_type_state9( df->line );
			}
			else{
				/* 0�Ǥ�1�ʾ�Ǥ�1�Ǥ�ʤ���� */
				topgun_error( FEC_CIRCUIT_NO_LINE, func_name );
			}
		}
		else{
		    /* result ==DRIVE_CONT
			   df->line�ˤ�D�����¤��Ƥʤ�
			   �����¤�����սꤽ�Τ�� */
			/* result ==DRIVE_IMPOS
			   �����ͤ����ꡢ���¤Ǥ��ʤ� */
			return( result );
		}
	}
}

/********************************************************************************
 * << Function >>
 *		make new D-frontier data structure
 *
 * << Function ID >>
 *    	18-4
 *
 * << Return >>
 *	DRIVE_IMPOS : D-Drive impossible
 *	DRIVE_CONT	: D-Drive cotinue
 *
 * << Argument >>
 * args			(I/O)	type			description
 * dfront		 I		DFRONT *		making dfrontier point
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/28
 *
 ********************************************************************************/

GENE_RESULT atpg_make_dfront
(
 DFRONT *dfront
 ){

	Ulong  min_tm_co; /* branch�Τʤ��ǺǾ��βĴ�¬�� */
	Ulong  min_cnt_propa; /* branch�Τʤ��ǺǾ������²�� */
	Ulong  br;        /* branch�ν��� */
	Ulong  i;        /* buranch�ο� */

	DFRONT *d_next; /* ̤������Dfrontier�ؤΥݥ��� */
	DFRONT *child;     /* �Ҷ���dfront�ؤΥݥ��� */
	DFRONT **children; /* �Ҷ���dfront��«�ͤ�ݥ��� */

	char   *func_name = "atpg_make_dfront"; /*  �ؿ�̾                          */

	min_tm_co	  = TM_MAX;
	min_cnt_propa = FL_ULMAX;
	br 			  = FL_ULMAX;

	/* branch�ν��֤�co��ˤʤ��ɬ�פʤ� */
	/* �������� */

#ifdef PROPA_COUNT
	// ��������դ�dfront�⡼��(�Хå��ȥ�å������������Ω���ʤ�)
	for ( i = 0; i < dfront->line->n_out ; i++){

		d_next = dfront->next[ i ];

		/* D-frontier��̤Ĵ���� */
		if ( d_next->flag & DF_NOCHECK ) {
			/* TMŪ�˴�¬�Ǥ����ǽ�������뤫? */
			if ( d_next->line->tm_co != TM_INF ) {
				if ( d_next->line->cnt_propa < min_cnt_propa ) {
					min_cnt_propa = d_next->line->cnt_propa;
					min_tm_co      = d_next->line->tm_co;
					br = i;
				}
				else {
					if ( d_next->line->cnt_propa == min_cnt_propa ) {
						if ( d_next->line->tm_co < min_tm_co ) {
							/* �Ǿ��βĴ�¬���̤Ĵ��flag����Ļޤ�õ�� */
							min_tm_co = d_next->line->tm_co;
							br        = i;
						}
					}
				}
			}
		}
	}
	if ( br != FL_ULMAX ) {
		dfront->line->out[ br ]->cnt_propa++;
	}
#else
	// TM���¤��Ȥ���˹Ԥ�����
	for ( i = 0; i < dfront->line->n_out ; i++){

		d_next = dfront->next[ i ];

		/* D-frontier��̤Ĵ���� */
		if ( d_next->flag & DF_NOCHECK ) {

			if ( d_next->line->tm_co < min_tm_co ) {
				/* �Ǿ��βĴ�¬���̤Ĵ��flag����Ļޤ�õ�� */
				min_tm_co = d_next->line->tm_co;
				br        = i;

			}
		}
	}
#endif /* PROPA_NORMAL */
	

	if ( br == FL_ULMAX ) {
		/* �Ĵ�¬��̵����ʳ��λޤ��ʤ� */
		/* Dfront���ʤ���� */
		/* ñ��˸ξ�򸡽ФǤ��ʤ��������� */
		/* $$$ ��backtrack $$$ */
		return( DRIVE_IMPOS );
	}
	/* �����ޤ� */



	
	/* dfront_c�ι��� */
	dfront = dfront->next[ br ];

	/* 2005/11/8 ����о� */
	/*
	Gene_head.dfront_c->imp_trace = Gene_head.imp_trace;
	Gene_head.imp_trace           = NULL;
	   2005/11/18 �����ޤ� */

	Gene_head.dfront_c = dfront;

	dfront->flag |= DF_ROOT;


	/* ������D-Frontier�ν���¦�ؤΥǡ������� */
	children = ( DFRONT ** )topgunMalloc( FMT_DFRONT_P, sizeof( DFRONT * ),
										   dfront->line->n_out, func_name );

	for ( i = 0 ; i < dfront->line->n_out ; i++ ) {
    	/* �Ҷ���D-frontier�ѥ������ */
		child = ( DFRONT * )topgunMalloc( FMT_DFRONT, sizeof( DFRONT ),
										   1, func_name);

		/* child �ν���� */
		/* $$$ branch�ν��֤��Ĵ�¬��ν�ˤʤäƤ�г� $$$ */
		child->flag      = DF_NOCHECK;
		child->line      = dfront->line->out[ i ];
		child->ass_list  = NULL;
#ifndef OLD_IMP2
		child->last_n    = 0;
		child->last_f    = 0;
		child->cnt_n     = 0;
		child->cnt_f     = 0;
#else		
		child->imp_trace = NULL;
#endif /* OLD_IMP2 */
		child->prev      = dfront;
		child->next      = NULL;

		children[ i ] = child;
	}
	dfront->next = children;

	return( DRIVE_CONT );
}


/********************************************************************************
 * << Function >>
 *		entry propagate list on dfrontier tree
 *
 * << Function ID >>
 *    	18-3
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type			description
 * propa_list	 I		ASS_LIST *		assignment for propagating fault
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/7
 *
 ********************************************************************************/

void atpg_ent_propa_dfront
(
 ASS_LIST *propa_list
 ){

	char *func_name = "atpg_ent_propa_dfront"; /* �ؿ�̾ */

	test_ass_list ( propa_list, func_name );

	/* propa_list ��dfront_c��ass_list�κǸ�����­�� */
	if ( Gene_head.dfront_c->ass_list == NULL ) {
		Gene_head.dfront_c->ass_list = propa_list;
	}
	else{
		utility_combine_ass_list ( Gene_head.dfront_c->ass_list, propa_list ) ;
		/*
		propa_list->next = Gene_head.dfront_c->ass_list;
		Gene_head.dfront_c->ass_list = propa_list;
		*/
	}
}

/********************************************************************************
 * << Function >>
 *		reflesh state on dfront
 *
 * << Function ID >>
 *    	18-5
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 * dfront        I      DFRONT *    dfrontier infomation
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/04/14
 *		2007/06/01  imp_id_n & imp_id_f Masayoshi Yoshimura (Kyushu-u)
 *
 ********************************************************************************/

void gene_reflesh_state_dfront
(
 DFRONT *dfront
 ){

	ASS_LIST    *ass_list;
	ASS_LIST    *tmp_ass_list;
	Ulong       i;
	Ulong       pos;

	char *func_name = "gene_reflesh_stat_dfront"; // �ؿ�̾

	topgun_print_mes( 21, 1 );
	topgun_print_mes( 21, 4 );

	// test_dfront ( dfront, func_name );
	topgun_print_mes( 21 ,5 );

	// dfrontier��ʤ�뤿����ͳ�����Ƥ�ȼ�����ް����η�̤򸵤��᤹
	if ( dfront->cnt_n != 0 ) {
		
		topgun_print_mes_reflesh_normal( dfront->last_n, dfront->cnt_n);
		
		for ( pos = dfront->last_n, i = 0 ; i < dfront->cnt_n; i++, pos-- ) {

			topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL );

			Gene_head.imp_list_n[ pos ]->state9 &= STATE9_XU;
			Gene_head.imp_list_n[ pos ]->imp_id_n = 0; // reflesh ( 0 = initial value )
			
			topgun_print_mes_id_after_state9 ( Gene_head.imp_list_n[ pos ], COND_NORMAL );
		}
		Gene_head.last_n  = pos;
		
		dfront->last_n  = 0;
		dfront->cnt_n   = 0;
	}
	if ( dfront->cnt_f != 0 ) {

		topgun_print_mes_reflesh_failure( dfront->last_f, dfront->cnt_f);
		
		for ( pos = dfront->last_f, i = 0 ; i < dfront->cnt_f; i++, pos-- ) {

			topgun_print_mes_id_pre_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );
			
			Gene_head.imp_list_f[ pos ]->state9 &= STATE9_UX;
			Gene_head.imp_list_n[ pos ]->imp_id_f = 0; // reflesh
			
			topgun_print_mes_id_after_state9 ( Gene_head.imp_list_f[ pos ], COND_FAILURE );

		}
		Gene_head.last_f = pos;

		dfront->last_f  = 0;
		dfront->cnt_f   = 0;
	}

	/* 2005/09/12 implication->assignment�˽����ѹ� */

	/* dfrontier��ʤ�뤿��˳������ͤ򸵤��᤹ */
	ass_list = dfront->ass_list;

	test_ass_list ( ass_list, func_name ); 
					
	while ( ass_list ) {

		/* ��������state�򸵤��᤹ */
		/* ����imp_trace�ˤϤ���Τ����� 2006/05/25 */
		//atpg_reflesh_state( ass_list->line, ass_list->condition );

		/* �����ͤν��� */
		tmp_ass_list = ass_list;
		ass_list     = ass_list->next;

		topgunFree( tmp_ass_list, FMT_ASS_LIST, 1, func_name );
	}
	dfront->ass_list = NULL;
}

/********************************************************************************
 * << Function >>
 *		imp_trace move to D-Frontier Tree
 *
 * << Function ID >>
 *	    ?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 *     None
 *
 * << extern >>
 * name			(I/O)	type	description
 * Gene_head	I/O		ATPG_HEAD	aptg head infomation
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/09
 *      2007/06/05      enter imp_id
 *
 ********************************************************************************/

void gene_imp_trace_2_dfront
(
 Ulong imp_id
 ){
	Ulong pos = 0;
	Ulong i = 0;

	char *func_name = "gene_imp_trace_2_dfront"; /* �ؿ�̾ */

	if ( Gene_head.dfront_c == NULL ) {
		topgun_error( FEC_PRG_DFRONT, func_name );
	}
	// imp_id ����Ͽ����
	// noraml state
	for ( pos = Gene_head.last_n , i = 0 ; i < Gene_head.cnt_n ; i++, pos-- ) {
		Gene_head.imp_list_n[ pos ]->imp_id_f = imp_id;
	}
	// failure state
	for ( pos = Gene_head.last_f , i = 0 ; i < Gene_head.cnt_f ; i++, pos-- ) {
		Gene_head.imp_list_f[ pos ]->imp_id_f = imp_id;
	}
	
	/* �ް�����̤��������ĥ꡼�ؤΰ��Ϥ� */
	Gene_head.dfront_c->last_n    = Gene_head.last_n;
	Gene_head.dfront_c->cnt_n     = Gene_head.cnt_n;
	Gene_head.dfront_c->last_f    = Gene_head.last_f;
	Gene_head.dfront_c->cnt_f     = Gene_head.cnt_f;

	/* Gene_head.last_x(�Ǹ����Ͽ�������)�Ϥ��Τޤ� */
	/* Gene_head.cnt_x �����󥿤�0���᤹ */
	Gene_head.cnt_n = 0;
	Gene_head.cnt_f = 0;
}
/* End of File */

