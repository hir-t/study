/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ξ�ꥹ�Ȥ���ѥ����������оݸξ�����򤹤�ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h>

#include "topgun.h"
#include "topgunState.h" /* STATE_3, STATE_9 */
#include "topgunAtpg.h" /* STATE_3, STATE_9 */
#include "topgunLine.h"  /* LINE, LINE_TYPE, LINE_INFO */
#include "topgunFlist.h" /* FLIST & FLIST->info */
#include "topgunTime.h"
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h" /* FEC_XX */

extern	LINE_INFO      Line_info;
extern	FLIST	       **Flist_head;
extern  LINE           *Line_head; 

static  Ulong          Basis_select_fault = 0;

/* function list */
extern void   select_fault ( GENE_INFO *, char * );

static FLIST  *select_fault_search( Ulong );
FLIST  *select_fault_search_level( Ulong, Ulong );
static FLIST  *select_fault_search_id( long );


Ulong select_fault_fsim_parallel ( LINE * );
Ulong select_fault_fsim_random ( LINE * );

/* other function list */

/* test function */
void test_flist ( FLIST *, char * );

/* topgun_print.c */
void topgun_print_mes( Ulong, Ulong );
void topgun_print_mes_id( LINE *);
void topgun_print_mes_n( );
void topgun_print_mes_result_2_2( FLIST * );

/* test_function */
void test_null_variable( void *, TOPGUN_MEMORY_TYPE_ID, char *);

/********************************************************************************
 * << Function >>
 *		ATPG���оݤȤʤ�ξ�����򤹤�
 *
 * << Function ID >>
 *	   	8-1
 *
 * << Return >>
 *  fault   : data of selected fault for ATPG
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault         O      FLIST*  ATPG���оݤȤʤ�ξ�ǡ���
 *
 * << extern >>
 * name			(I/O)	type	description
 * Topgun_head   I      HEADER  Topgun main Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/13
 *
 ********************************************************************************/

void select_fault
(
 GENE_INFO *gene_info,
 char *fault_id
){

    Ulong start = 0;
	long  line_id = 0;

	char  *func_name = "select_fault"; /* �ؿ�̾ */

	topgun_3_utility_enter_start_time ( &Time_head.select );
	
	test_null_variable ( gene_info, FMT_GENE_INFO, func_name );
	
	/* debag message */
	topgun_print_mes( 2, 2 );
	
	if ( fault_id == NULL ) {
		/* ���̤Υ⡼�� */

		/* ����Ǹξ��õ���������� */

		start =( Ulong )( rand() % ( Line_info.n_line * 2 ));

		/* ���򤷤�������������礭��������̤�����ξ��õ�� */
		gene_info->fault = select_fault_search( start );
		//gene_info->fault = select_fault_search_level( start, Line_info.n_fault_atpg );
		if ( gene_info->fault != NULL ) {
			topgun_print_mes ( 19, 0 );
			topgun_print_mes_id ( gene_info->fault->line );
			topgun_print_mes_n ( );
			//printf("select fault %lu \n",gene_info->fault->line->line_id );
		}
		else {
			//printf("select fault missing\n");
		}
	}
	else {
		/* ���뿮�������Ф���⡼�� */

		if ( fault_id != NULL ) {

			line_id = atol ( fault_id );
			topgun_print_mes ( 19, 1 );
			topgun_print_mes_n ( );
		
			/* ���򤷤�������������礭��������̤�����ξ��õ�� */

			gene_info->fault = select_fault_search_id( line_id );
		}
	}

	if ( gene_info->fault == NULL ) {
		/* �ξ㤬����Ǥ��ʤ���� */
		gene_info->f_class = GENE_NO_FAULT;

	}
	else {
		gene_info->f_class = NO_DEFINE;
	}

	topgun_3_utility_enter_end_time ( &Time_head.select );
}


/********************************************************************************
 * << Function >>
 *		search target fault
 *
 * << Function ID >>
 *    	8-2
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * start		I		unsigned long	search start point
 * stop			I		unsigned long	search stop point
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

FLIST *select_fault_search_old
(
 Ulong start,
 Ulong stop
 ){

	FLIST *flist = NULL;

    Ulong i = 0; /* ��������ξ�� */


	char  *func_name = "select_fault_search"; /* �ؿ�̾ */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 ̤ʬ��θξ㡩                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

    /* start�����礭�������stop�ޤ�̤�����ξ��õ�� */
    for ( i = start; i < stop; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) || /* ʬ�व��Ƥ��ʤ� */
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) || /* ���˥ѥ����������оݤȤʤäƤ��ʤ� */
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) || /* ���˥ѥ����������оݤȤʤäƤ��ʤ� */
				 ( Flist_head[ i ]->info & TOPGUN_DTX )  ) ) { /* FsimVal3�Ǹ��Ф���Ƥ��ʤ� */  				 

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 ̤ʬ��ξ������                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

			/* ̤����&̤���Фθξ�ξ�� */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {
				/* �����ξ���Ϥ�flist����Ƭ��錄�� */
				flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];

				if ( flist == NULL ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
				}
				else if ( ! ( ( flist->info &  TOPGUN_DCD ) ||
							  ( flist->info &  TOPGUN_EXE ) ||
							  ( flist->info &  TOPGUN_DTR ) ||
							  ( flist->info &  TOPGUN_DTX ) ) ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
				}
					
			}
			else {
				flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];

				if ( flist == NULL ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
				}
				else if ( ! ( ( flist->info &  TOPGUN_DCD ) ||
							  ( flist->info &  TOPGUN_EXE ) ||
							  ( flist->info &  TOPGUN_DTR ) ||
							  ( flist->info &  TOPGUN_DTX ) ) ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
				}
				
			}

			test_flist ( flist, func_name );
			
			return( flist );
		}
	}

	/* �Ǹ�ԤäƤ⸫�դ���ʤ���硢�ǽ餫��õ�� */
    for ( i = 0; i < start; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) ||
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTX ) ) ) {

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 ̤ʬ��ξ������                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			
			/* ̤����&̤���Фθξ�ξ�� */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {
				/* �����ξ���Ϥ�flist����Ƭ��錄�� */
				flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
				
				if ( flist == NULL ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
				}
				else if ( ! ( ( flist->info &  TOPGUN_DCD ) ||
							  ( flist->info &  TOPGUN_EXE ) ||
							  ( flist->info &  TOPGUN_DTR ) ||
							  ( flist->info &  TOPGUN_DTX ) ) ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
				}
			}
			else {
				flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];

				if ( flist == NULL ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
				}
				else if ( ! ( ( flist->info &  TOPGUN_DCD ) ||
							  ( flist->info &  TOPGUN_EXE ) ||
							  ( flist->info &  TOPGUN_DTR ) ||
							  ( flist->info &  TOPGUN_DTX ) ) ) {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
				}
			}

			test_flist ( flist, func_name );

			return( flist );
		}
    }
	/* �ξ㤬�ߤĤ���ʤ� */
	/*
	topgun_error( FEC_CIRCUIT_NO_FAULT , func_name );
	*/
	/* ���Ϥ��Ϥʤ� */
	return( NULL );
}

/********************************************************************************
 * << Function >>
 *		search target fault with Line ID
 *
 * << Function ID >>
 *    	8-3
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * line_id	 	 I		Ulong	        line id of fault
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

FLIST *select_fault_search_id
(
 long line_id
 ){

    Ulong i = 0; /* ��������ξ�� */

	/* char  *func_name = "select_fault_search_id"; *//* �ؿ�̾ */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 ̤ʬ��θξ㡩                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	
	while (  Flist_head[ i ] != NULL ) {

		if ( Flist_head[ i ]->line->line_id == line_id ) {
			if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) ||
					 ( Flist_head[ i ]->info & TOPGUN_EXE ) ) ) {

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 ̤ʬ��ξ������                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

			/* ̤����&̤���Фθξ�ξ�� */
				topgun_print_mes_result_2_2( Flist_head[ i ] );
				return( Flist_head[ i ] );
			}
		}
		i++;
	}
	return ( NULL );
}

/********************************************************************************
 * << Function >>
 *		search target fault
 *
 * << Function ID >>
 *    	8-2
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * start		I		unsigned long	search start point
 * stop			I		unsigned long	search stop point
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

FLIST *select_fault_search_level
(
 Ulong start,
 Ulong stop
 ){

	FLIST *flist = NULL;
	FLIST *return_flist = NULL;

    Ulong i = 0; /* ��������ξ�� */
	Ulong level = 0; /* �ξ�Υ�٥�� */


	char  *func_name = "select_fault_search"; /* �ؿ�̾ */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 ̤ʬ��θξ㡩                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

    /* start�����礭�������stop�ޤ�̤�����ξ��õ�� */
    for ( i = start; i < stop; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) || /* ʬ�व��Ƥ��ʤ� */
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) || /* ���˥ѥ����������оݤȤʤäƤ��ʤ� */
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) || /* random�ѥ���Ǹ��Ф���Ƥ��ʤ� */
				 ( Flist_head[ i ]->info & TOPGUN_DTX )  ) ) { /* FsimVal3�Ǹ��Ф���Ƥ��ʤ� */  				 

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 ̤ʬ��ξ������                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

			/* ̤����&̤���Фθξ�ξ�� */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->line->lv_po > level ) {
			
				if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {

					/* �����ξ���Ϥ�flist����Ƭ��錄�� */
					flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];

					if ( flist == NULL ) {
						flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
					}
					else if ( !( ( flist->info &  TOPGUN_DCD ) ||
								 ( flist->info &  TOPGUN_EXE ) ||
								 ( flist->info &  TOPGUN_DTR ) ||
								 ( flist->info &  TOPGUN_DTX ) ) ){
						flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
					}
					
				}
				else {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];

					if ( flist == NULL ) {
						flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
					}
					else if ( ! ( ( flist->info &  TOPGUN_DCD ) ||
								  ( flist->info &  TOPGUN_EXE ) ||
								  ( flist->info &  TOPGUN_DTR ) ||
								  ( flist->info &  TOPGUN_DTX ) ) ) {
						flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
					}
					
				}
				if ( flist != NULL ) {
					/* ���ߤ�level����礭��������򤹤� */
					level =	Flist_head[ i ]->line->lv_po;
					return_flist = flist;
				}
				test_flist ( flist, func_name );
			}
		}
	}

	/* �Ǹ�ԤäƤ⸫�դ���ʤ���硢�ǽ餫��õ�� */
    for ( i = 0; i < start; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) ||
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTX ) ) ) {

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 ̤ʬ��ξ������                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			
			/* ̤����&̤���Фθξ�ξ�� */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->line->lv_po > level ) {
				
				if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {
					/* �����ξ���Ϥ�flist����Ƭ��錄�� */
					flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
					
					if ( flist == NULL ) {
						flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
					}
					else if ( ! ( ( flist->info &  TOPGUN_DCD ) ||
								  ( flist->info &  TOPGUN_EXE ) ||
								  ( flist->info &  TOPGUN_DTR ) ||
								  ( flist->info &  TOPGUN_DTX ) ) ){
						flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];
					}
				}
				else {
					flist = Flist_head[ i ]->line->flist[ FSIM_SA1 ];

					if ( flist == NULL ) {
						flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
					}
					else if ( ! ( ( flist->info &  TOPGUN_DCD ) ||
								  ( flist->info &  TOPGUN_EXE ) ||
								  ( flist->info &  TOPGUN_DTR ) ||
								  ( flist->info &  TOPGUN_DTX ) ) ){
						flist = Flist_head[ i ]->line->flist[ FSIM_SA0 ];
					}
				}
				if ( flist != NULL ) {
					/* ���ߤ�level����礭��������򤹤� */
					level =	Flist_head[ i ]->line->lv_po;
					return_flist = flist;
				}
				test_flist ( flist, func_name );
			}

		}
    }
	//printf("%3lu ",level);
	return( return_flist );

	/* �ξ㤬�ߤĤ���ʤ� */
	/*
	topgun_error( FEC_CIRCUIT_NO_FAULT , func_name );
	*/
	/* ���Ϥ��Ϥʤ� */
	return( NULL );
}


/********************************************************************************
 * << Function >>
 *		select fault for random pattern fault simulation
 *
 * << Function ID >>
 *    	8-?
 *
 * << Return >>
 *	SELECT_ON_SA0  : selectable fault of only sa0
 *	SELECT_ON_SA1  : selectable fault of only sa1
 *	SELECT_ON_SA01 : selectable fault of sa0 and sa1
 *	SELECT_OFF     : no selectable fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * line  	 	 I		LINE *	        line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/10/05
 *
 ********************************************************************************/

Ulong select_fault_fsim_random
(
 LINE *line
 ){

	Ulong sa0_info = 0;
	Ulong sa1_info = 0;
	Ulong result   = SELECT_DEFAULT;

	/* char  *func_name = "select_fault_search_id"; *//* �ؿ�̾ */
	if ( line->flist[ FSIM_SA0 ] != NULL ) {
		sa0_info = line->flist[ FSIM_SA0 ]->info;

		if ( sa0_info & TOPGUN_DTR ) {
			/* ������ѥ���Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else if ( sa0_info & TOPGUN_DTS ) {
			/* 2�ͥѥ���Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else if ( sa0_info & TOPGUN_DCD ) {
			/* ATPG�Ǹξ�ʬ�व��Ƥ���������򤷤ʤ� */
		}
		else if ( sa0_info & TOPGUN_DTX ) {
			/* Xsim�Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else {
			/* ���ξ��������򤹤� */
			result |= SELECT_ON_SA0;
		}
	}
	if ( line->flist[ FSIM_SA1 ] != NULL ) {
		sa1_info = line->flist[ FSIM_SA1 ]->info;

		if ( sa1_info & TOPGUN_DTR ) {
			/* ������ѥ���Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else if ( sa1_info & TOPGUN_DTS ) {
			/* 2�ͥѥ���Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else if ( sa1_info & TOPGUN_DCD ) {
			/* ATPG�Ǹξ�ʬ�व��Ƥ���������򤷤ʤ� */
		}
		else if ( sa1_info & TOPGUN_DTX ) {
			/* Xsim�Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else {
			/* ���ξ��������򤹤� */
			result |= SELECT_ON_SA1;
		}
	}

	if ( result == SELECT_DEFAULT ) {
		/* �ɤ��������Ǥ��ʤ���� */
		result = SELECT_OFF;
	}

	return ( result );
}

/********************************************************************************
 * << Function >>
 *		select fault for random pattern fault simulation
 *
 * << Function ID >>
 *    	8-?
 *
 * << Return >>
 *	SELECT_ON_SA0  : selectable fault of only sa0
 *	SELECT_ON_SA1  : selectable fault of only sa1
 *	SELECT_ON_SA01 : selectable fault of sa0 and sa1
 *	SELECT_OFF     : no selectable fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * line  	 	 I		LINE *	        line pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/10/05
 *
 ********************************************************************************/

Ulong select_fault_fsim_parallel
(
 LINE *line
 ){

	Ulong sa0_info = 0;
	Ulong sa1_info = 0;
	Ulong result   = SELECT_DEFAULT;

	/* char  *func_name = "select_fault_search_id"; *//* �ؿ�̾ */
	if ( line->flist[ FSIM_SA0 ] != NULL ) {
		
		sa0_info = line->flist[ FSIM_SA0 ]->info;

		if ( sa0_info & TOPGUN_DTS ) {
				/* 2�ͥѥ���Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else if ( sa0_info & TOPGUN_RED ) {
			/* ATPG�Ǿ�Ĺ�ξ��ʬ�व��Ƥ���������򤷤ʤ� */
			/* ATPG��UNTEST�ξ��ʬ�व��Ƥ���������򤷤ʤ� */
		}
		else {
			/* ���ξ��������򤹤� */
			result |= SELECT_ON_SA0;
		}
	}
	if ( line->flist[ FSIM_SA1 ] != NULL ) {
		
		sa1_info = line->flist[ FSIM_SA1 ]->info;

		if ( sa1_info & TOPGUN_DTS ) {
			/* 2�ͥѥ���Ǹ��Ф���Ƥ���������򤷤ʤ� */
		}
		else if ( sa1_info & TOPGUN_RED ) {
			/* ATPG�Ǹξ�ʬ�व��Ƥ���������򤷤ʤ� */
		}
		else {
			/* ���ξ��������򤹤� */
			result |= SELECT_ON_SA1;
		}
	}

	if ( result == SELECT_DEFAULT ) {
		/* �ɤ��������Ǥ��ʤ���� */
		result = SELECT_OFF;
	}

	return ( result );
}

/********************************************************************************
 * << Function >>
 *		search target fault
 *
 * << Function ID >>
 *    	8-2
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * start		I		unsigned long	search start point
 * stop			I		unsigned long	search stop point
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

FLIST *select_fault_search
(
 Ulong start
 ){

	const Ulong stop = Line_info.n_line;
	const Ulong real_start = ( start / 2 );
	Ulong flist_info = 0;
	FLIST *flist = NULL;

    Ulong i = 0; /* ��������ξ�� */

	//char  *func_name = "select_fault_search"; /* �ؿ�̾ */

	//printf(" real_start %6u ", real_start );
	

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 ̤ʬ��θξ㡩                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	if ( ( start % 2 )  == 0 ) {

		flist = Line_head[ real_start ].flist[ FSIM_SA0 ];
		if (  flist != NULL ) {
			
			flist_info = flist->info;

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_fault���������Ƥ���ξ�����򤷤ʤ�
			}
			else {
				/* ���򤷤��ξ���֤��ͤȤ��� */
				//printf(" direct select sa0\n");
				return ( flist );
			}
		}
	}
	flist = Line_head[ real_start ].flist[ FSIM_SA1 ];
	if (  flist != NULL ) {
		flist_info = flist->info;

		if ( flist_info & Basis_select_fault ) {
			// Basis_select_fault���������Ƥ���ξ�����򤷤ʤ�
		}
		else {
			// ���򤷤��ξ���֤��ͤȤ���
			//printf(" direct select sa1\n");
			return ( flist );
		}
	}

	//printf(" for start ");
    // start�����礭�������stop�ޤ�̤�����ξ��õ��
    for ( i = real_start + 1 ; i < stop; i++ ) {

		flist = Line_head[ i ].flist[ FSIM_SA0 ];
		
		if (  flist != NULL ) {
			flist_info = flist->info;
			if ( flist_info & Basis_select_fault ) {
				// Basis_select_fault���������Ƥ���ξ�����򤷤ʤ�
			}
			else {
				/* ���򤷤��ξ���֤��ͤȤ��� */
				//printf(" select %6u sa0\n",i);
				return ( flist );
			}
		}
		flist = Line_head[ i ].flist[ FSIM_SA1 ]; 
		if (  flist != NULL ) {
			flist_info = flist->info;

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_fault���������Ƥ���ξ�����򤷤ʤ�
			}
			else {
				// ���򤷤��ξ���֤��ͤȤ���
				//printf(" select %6u sa1\n",i);
				return ( flist );
			}
		}
	}
	//printf(" for start 0 ",i);
    // 0�����礭�������stop�ޤ�̤�����ξ��õ��
    for ( i = 0 ; i < real_start ; i++ ) {

		flist = Line_head[ i ].flist[ FSIM_SA0 ];
		if (  flist != NULL ) {
			flist_info = flist->info;

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_fault���������Ƥ���ξ�����򤷤ʤ�
			}
			else {
				// ���򤷤��ξ���֤��ͤȤ��� 
				//printf(" select %6u sa0\n",i);
				return ( flist );
			}
		}
		
		flist = Line_head[ i ].flist[ FSIM_SA1 ]; 
		if (  flist  != NULL ) {
			flist_info = flist->info; 

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_fault���������Ƥ���ξ�����򤷤ʤ�
			}
			else {
				/* ���򤷤��ξ���֤��ͤȤ��� */
				//printf(" select %6u sa1\n",i);
				return ( flist );
			}
		}
	}

	//printf(" final select \n");
	flist = Line_head[ real_start ].flist[ FSIM_SA0 ];
	if (  flist != NULL ) {
			
		flist_info = flist->info;

		if ( flist_info & Basis_select_fault ) {
			// Basis_select_fault���������Ƥ���ξ�����򤷤ʤ�
		}
		else {
			/* ���򤷤��ξ���֤��ͤȤ��� */
			return ( flist );
		}
	}
	return( NULL );
}

/********************************************************************************
 * << Function >>
 *		search target fault
 *
 * << Function ID >>
 *    	8-2
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * start		I		unsigned long	search start point
 * stop			I		unsigned long	search stop point
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

void select_fault_search_basis_normal
(
 void
 ){
	Basis_select_fault = 0;

	// ������ѥ���Ǹ��Ф���Ƥ���������򤷤ʤ�
	Basis_select_fault |= TOPGUN_DTR;

	// ATPG�Ǹξ�ʬ�व��Ƥ���������򤷤ʤ�
	Basis_select_fault |= TOPGUN_DCD;

	// 3�ͥѥ���Ǹ��Ф���Ƥ���������򤷤ʤ�
	Basis_select_fault |= TOPGUN_DTX;

	// ATPG��»ܤ��Ƥ���������򤷤ʤ�
	Basis_select_fault |= TOPGUN_EXE;

}

/********************************************************************************
 * << Function >>
 *		search target fault
 *
 * << Function ID >>
 *    	8-2
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * start		I		unsigned long	search start point
 * stop			I		unsigned long	search stop point
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

void select_fault_search_basis_random_det
(
 void
 ){
	Basis_select_fault = 0;

	// ������ѥ���Ǹ��Ф���Ƥ�����ϴ�೰!!
	//Basis_select_fault |= TOPGUN_DTR;

	// ATPG�Ǹξ�ʬ�व��Ƥ���������򤷤ʤ�
	Basis_select_fault |= TOPGUN_DCD;

	// 3�ͥѥ���Ǹ��Ф���Ƥ���������򤷤ʤ�
	Basis_select_fault |= TOPGUN_DTX;

	// ATPG��»ܤ��Ƥ���������򤷤ʤ�
	Basis_select_fault |= TOPGUN_EXE;

}

/********************************************************************************
 * << Function >>
 *		search target fault
 *
 * << Function ID >>
 *    	8-2
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * start		I		unsigned long	search start point
 * stop			I		unsigned long	search stop point
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

void select_init
(
 void
 ){
	//Basis_select_fault�ν����
	select_fault_search_basis_normal();
}

/********************************************************************************
 * << Function >>
 *		search target fault
 *
 * << Function ID >>
 *    	8-2
 *
 * << Return >>
 *	fault		: searched target fault
 *
 * << Argument >>
 * args			(I/O)	type			description
 * start		I		unsigned long	search start point
 * stop			I		unsigned long	search stop point
 *
 * << extern >>
 * name			(I/O)	type	description
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

Ulong select_fault_search_basis_random_det_rest_calc
(
 void
 ){
	Ulong rand_on = 0;
	Ulong rand_off = 0;
	Ulong i = 0;
	//Ulong count_0 = 0;
	Ulong count_1 = 0;
	//Ulong count_2 = 0;
	//Ulong count_3 = 0;

	FLIST *flist = NULL;


	// ATPG�Ǹξ�ʬ�व��Ƥ���������򤷤ʤ�
	rand_on |= TOPGUN_DCD;

	// 3�ͥѥ���Ǹ��Ф���Ƥ���������򤷤ʤ�
	rand_on |= TOPGUN_DTX;

	// ATPG��»ܤ��Ƥ���������򤷤ʤ�
	rand_on |= TOPGUN_EXE;

	rand_off = rand_on;
	
	// ������ѥ���Ǹ��Ф���Ƥ�����ϴ�೰!!
	rand_off |= TOPGUN_DTR;


	for ( i = 0 ; i < Line_info.n_line; i++ ) {

		flist = Line_head[ i ].flist[ FSIM_SA0 ];

		if ( flist != NULL ) {
			/*
			if ( ( flist->info & rand_on ) &&
				 ( flist->info & rand_off ) ) {
				count_0++;
			}
			*/
			if ( ( ! ( flist->info & rand_on ) ) &&
				 ( flist->info & rand_off ) ) {
				count_1++;
			}
			/*
			if ( ( ! ( flist->info & rand_on ) ) &&
				 ( ! ( flist->info & rand_off ) ) ){
				count_2++;
			}
			if ( ( flist->info & rand_on ) &&
				 ( ! ( flist->info & rand_off ) ) ){
				count_3++;
			}
			*/
		}
		
		flist = Line_head[ i ].flist[ FSIM_SA1 ];
		
		if ( flist != NULL ) {
			/*
			if ( ( flist->info & rand_on ) &&
				 ( flist->info & rand_off ) ) {
				count_0++;
			}
			*/
			if ( ( ! ( flist->info & rand_on ) ) &&
				 ( flist->info & rand_off ) ) {
				count_1++;
			}
			/*
			if ( ( ! ( flist->info & rand_on ) ) &&
				 ( ! ( flist->info & rand_off ) ) ){
				count_2++;
			}
			if ( ( flist->info & rand_on ) &&
				 ( ! ( flist->info & rand_off ) ) ){
				count_3++;
			}
			*/
		}

	}

	//printf("on %lu off %lu\n", rand_on, rand_off);
	//printf("0 : %lu, 1 : %lu, 2 : %lu, 3 : %lu\n",count_0,count_1,count_2,count_3);
	
	return ( count_1 );
}

/* End of File */
