/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 故障リストからパターン生成対象故障を選択する関する関数群
	
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
 *		ATPGの対象となる故障を選択する
 *
 * << Function ID >>
 *	   	8-1
 *
 * << Return >>
 *  fault   : data of selected fault for ATPG
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault         O      FLIST*  ATPGの対象となる故障データ
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

	char  *func_name = "select_fault"; /* 関数名 */

	topgun_3_utility_enter_start_time ( &Time_head.select );
	
	test_null_variable ( gene_info, FMT_GENE_INFO, func_name );
	
	/* debag message */
	topgun_print_mes( 2, 2 );
	
	if ( fault_id == NULL ) {
		/* 普通のモード */

		/* 乱数で故障を探す場所を選択 */

		start =( Ulong )( rand() % ( Line_info.n_line * 2 ));

		/* 選択した数から数字が大きい方向に未処理故障を探す */
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
		/* ある信号線に対するモード */

		if ( fault_id != NULL ) {

			line_id = atol ( fault_id );
			topgun_print_mes ( 19, 1 );
			topgun_print_mes_n ( );
		
			/* 選択した数から数字が大きい方向に未処理故障を探す */

			gene_info->fault = select_fault_search_id( line_id );
		}
	}

	if ( gene_info->fault == NULL ) {
		/* 故障が選択できない場合 */
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

    Ulong i = 0; /* 最大設定故障数 */


	char  *func_name = "select_fault_search"; /* 関数名 */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 未分類の故障？                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

    /* startから大きいがわにstopまで未処理故障を探す */
    for ( i = start; i < stop; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) || /* 分類されていない */
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) || /* 過去にパタン生成の対象となっていない */
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) || /* 過去にパタン生成の対象となっていない */
				 ( Flist_head[ i ]->info & TOPGUN_DTX )  ) ) { /* FsimVal3で検出されていない */  				 

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 未分類故障を選択                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

			/* 未処理&未検出の故障の場合 */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {
				/* 等価故障解析のflistの先頭をわたす */
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

	/* 最後行っても見付からない場合、最初から探す */
    for ( i = 0; i < start; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) ||
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTX ) ) ) {

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 未分類故障を選択                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			
			/* 未処理&未検出の故障の場合 */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {
				/* 等価故障解析のflistの先頭をわたす */
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
	/* 故障がみつからない */
	/*
	topgun_error( FEC_CIRCUIT_NO_FAULT , func_name );
	*/
	/* 来るはずはない */
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

    Ulong i = 0; /* 最大設定故障数 */

	/* char  *func_name = "select_fault_search_id"; *//* 関数名 */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 未分類の故障？                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	
	while (  Flist_head[ i ] != NULL ) {

		if ( Flist_head[ i ]->line->line_id == line_id ) {
			if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) ||
					 ( Flist_head[ i ]->info & TOPGUN_EXE ) ) ) {

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 未分類故障を選択                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

			/* 未処理&未検出の故障の場合 */
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

    Ulong i = 0; /* 最大設定故障数 */
	Ulong level = 0; /* 故障のレベル数 */


	char  *func_name = "select_fault_search"; /* 関数名 */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 未分類の故障？                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

    /* startから大きいがわにstopまで未処理故障を探す */
    for ( i = start; i < stop; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) || /* 分類されていない */
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) || /* 過去にパタン生成の対象となっていない */
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) || /* randomパタンで検出されていない */
				 ( Flist_head[ i ]->info & TOPGUN_DTX )  ) ) { /* FsimVal3で検出されていない */  				 

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 未分類故障を選択                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

			/* 未処理&未検出の故障の場合 */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->line->lv_po > level ) {
			
				if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {

					/* 等価故障解析のflistの先頭をわたす */
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
					/* 現在のlevelより大きい場合選択する */
					level =	Flist_head[ i ]->line->lv_po;
					return_flist = flist;
				}
				test_flist ( flist, func_name );
			}
		}
	}

	/* 最後行っても見付からない場合、最初から探す */
    for ( i = 0; i < start; i++ ) {

		if ( ! ( ( Flist_head[ i ]->info & TOPGUN_DCD ) ||
				 ( Flist_head[ i ]->info & TOPGUN_EXE ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTR ) ||
				 ( Flist_head[ i ]->info & TOPGUN_DTX ) ) ) {

			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			/* $$$ 2-2-2 未分類故障を選択                                   $$$ */
			/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
			
			/* 未処理&未検出の故障の場合 */
			topgun_print_mes_result_2_2( Flist_head[ i ] );

			if ( Flist_head[ i ]->line->lv_po > level ) {
				
				if ( Flist_head[ i ]->info & TOPGUN_SA0 ) {
					/* 等価故障解析のflistの先頭をわたす */
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
					/* 現在のlevelより大きい場合選択する */
					level =	Flist_head[ i ]->line->lv_po;
					return_flist = flist;
				}
				test_flist ( flist, func_name );
			}

		}
    }
	//printf("%3lu ",level);
	return( return_flist );

	/* 故障がみつからない */
	/*
	topgun_error( FEC_CIRCUIT_NO_FAULT , func_name );
	*/
	/* 来るはずはない */
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

	/* char  *func_name = "select_fault_search_id"; *//* 関数名 */
	if ( line->flist[ FSIM_SA0 ] != NULL ) {
		sa0_info = line->flist[ FSIM_SA0 ]->info;

		if ( sa0_info & TOPGUN_DTR ) {
			/* ランダムパタンで検出されている場合は選択しない */
		}
		else if ( sa0_info & TOPGUN_DTS ) {
			/* 2値パタンで検出されている場合は選択しない */
		}
		else if ( sa0_info & TOPGUN_DCD ) {
			/* ATPGで故障分類されている場合は選択しない */
		}
		else if ( sa0_info & TOPGUN_DTX ) {
			/* Xsimで検出されている場合は選択しない */
		}
		else {
			/* この場合だけ選択する */
			result |= SELECT_ON_SA0;
		}
	}
	if ( line->flist[ FSIM_SA1 ] != NULL ) {
		sa1_info = line->flist[ FSIM_SA1 ]->info;

		if ( sa1_info & TOPGUN_DTR ) {
			/* ランダムパタンで検出されている場合は選択しない */
		}
		else if ( sa1_info & TOPGUN_DTS ) {
			/* 2値パタンで検出されている場合は選択しない */
		}
		else if ( sa1_info & TOPGUN_DCD ) {
			/* ATPGで故障分類されている場合は選択しない */
		}
		else if ( sa1_info & TOPGUN_DTX ) {
			/* Xsimで検出されている場合は選択しない */
		}
		else {
			/* この場合だけ選択する */
			result |= SELECT_ON_SA1;
		}
	}

	if ( result == SELECT_DEFAULT ) {
		/* どちらも選択できない場合 */
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

	/* char  *func_name = "select_fault_search_id"; *//* 関数名 */
	if ( line->flist[ FSIM_SA0 ] != NULL ) {
		
		sa0_info = line->flist[ FSIM_SA0 ]->info;

		if ( sa0_info & TOPGUN_DTS ) {
				/* 2値パタンで検出されている場合は選択しない */
		}
		else if ( sa0_info & TOPGUN_RED ) {
			/* ATPGで冗長故障に分類されている場合は選択しない */
			/* ATPGでUNTEST故障に分類されている場合は選択しない */
		}
		else {
			/* この場合だけ選択する */
			result |= SELECT_ON_SA0;
		}
	}
	if ( line->flist[ FSIM_SA1 ] != NULL ) {
		
		sa1_info = line->flist[ FSIM_SA1 ]->info;

		if ( sa1_info & TOPGUN_DTS ) {
			/* 2値パタンで検出されている場合は選択しない */
		}
		else if ( sa1_info & TOPGUN_RED ) {
			/* ATPGで故障分類されている場合は選択しない */
		}
		else {
			/* この場合だけ選択する */
			result |= SELECT_ON_SA1;
		}
	}

	if ( result == SELECT_DEFAULT ) {
		/* どちらも選択できない場合 */
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

    Ulong i = 0; /* 最大設定故障数 */

	//char  *func_name = "select_fault_search"; /* 関数名 */

	//printf(" real_start %6u ", real_start );
	

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2-1 未分類の故障？                                             $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	if ( ( start % 2 )  == 0 ) {

		flist = Line_head[ real_start ].flist[ FSIM_SA0 ];
		if (  flist != NULL ) {
			
			flist_info = flist->info;

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_faultで定義されている故障は選択しない
			}
			else {
				/* 選択した故障を返り値とする */
				//printf(" direct select sa0\n");
				return ( flist );
			}
		}
	}
	flist = Line_head[ real_start ].flist[ FSIM_SA1 ];
	if (  flist != NULL ) {
		flist_info = flist->info;

		if ( flist_info & Basis_select_fault ) {
			// Basis_select_faultで定義されている故障は選択しない
		}
		else {
			// 選択した故障を返り値とする
			//printf(" direct select sa1\n");
			return ( flist );
		}
	}

	//printf(" for start ");
    // startから大きいがわにstopまで未処理故障を探す
    for ( i = real_start + 1 ; i < stop; i++ ) {

		flist = Line_head[ i ].flist[ FSIM_SA0 ];
		
		if (  flist != NULL ) {
			flist_info = flist->info;
			if ( flist_info & Basis_select_fault ) {
				// Basis_select_faultで定義されている故障は選択しない
			}
			else {
				/* 選択した故障を返り値とする */
				//printf(" select %6u sa0\n",i);
				return ( flist );
			}
		}
		flist = Line_head[ i ].flist[ FSIM_SA1 ]; 
		if (  flist != NULL ) {
			flist_info = flist->info;

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_faultで定義されている故障は選択しない
			}
			else {
				// 選択した故障を返り値とする
				//printf(" select %6u sa1\n",i);
				return ( flist );
			}
		}
	}
	//printf(" for start 0 ",i);
    // 0から大きいがわにstopまで未処理故障を探す
    for ( i = 0 ; i < real_start ; i++ ) {

		flist = Line_head[ i ].flist[ FSIM_SA0 ];
		if (  flist != NULL ) {
			flist_info = flist->info;

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_faultで定義されている故障は選択しない
			}
			else {
				// 選択した故障を返り値とする 
				//printf(" select %6u sa0\n",i);
				return ( flist );
			}
		}
		
		flist = Line_head[ i ].flist[ FSIM_SA1 ]; 
		if (  flist  != NULL ) {
			flist_info = flist->info; 

			if ( flist_info & Basis_select_fault ) {
				// Basis_select_faultで定義されている故障は選択しない
			}
			else {
				/* 選択した故障を返り値とする */
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
			// Basis_select_faultで定義されている故障は選択しない
		}
		else {
			/* 選択した故障を返り値とする */
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

	// ランダムパタンで検出されている場合は選択しない
	Basis_select_fault |= TOPGUN_DTR;

	// ATPGで故障分類されている場合は選択しない
	Basis_select_fault |= TOPGUN_DCD;

	// 3値パタンで検出されている場合は選択しない
	Basis_select_fault |= TOPGUN_DTX;

	// ATPGを実施している場合は選択しない
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

	// ランダムパタンで検出されている場合は基準外!!
	//Basis_select_fault |= TOPGUN_DTR;

	// ATPGで故障分類されている場合は選択しない
	Basis_select_fault |= TOPGUN_DCD;

	// 3値パタンで検出されている場合は選択しない
	Basis_select_fault |= TOPGUN_DTX;

	// ATPGを実施している場合は選択しない
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
	//Basis_select_faultの初期化
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


	// ATPGで故障分類されている場合は選択しない
	rand_on |= TOPGUN_DCD;

	// 3値パタンで検出されている場合は選択しない
	rand_on |= TOPGUN_DTX;

	// ATPGを実施している場合は選択しない
	rand_on |= TOPGUN_EXE;

	rand_off = rand_on;
	
	// ランダムパタンで検出されている場合は基準外!!
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
