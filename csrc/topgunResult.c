/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief パターン生成終了後に故障の分類結果を故障リストへ登録する関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunFlist.h" /* FLIST & FLIST->info */
#include "topgunMemory.h" /* FMT_XX */

extern	LINE_INFO	Line_info;

/* topgun_print.c */
void topgun_print_mes();

extern void test_null_variable( void *, TOPGUN_MEMORY_TYPE_ID, char *);

/********************************************************************************
 * << Function >>
 *		enter result of atpg
 *
 * << Function ID >>
 *	   	10-1
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault         I      FLIST * fault for atpg
 * result        I      Ulong   atpg result
 *
 * << extern >>
 * name			(I/O)	type	description
 * Line_info   I      HEADER  Topgun main header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/25
 *
 ********************************************************************************/

void fault_classify_with_generate
(
 GENE_INFO *gene_info
 ){

	FLIST  *flist   = NULL;
	Ulong   result  = TOPGUN_EXE;
	F_CLASS f_class = NO_DEFINE;
	static char *func_name = "fault_classify_with_generate";

	test_null_variable ( gene_info, FMT_GENE_INFO, func_name );

	flist   = gene_info->fault;
	f_class = gene_info->f_class;

	topgun_print_mes( 2, 8 );

	switch ( f_class ) {
	case GENE_DET_A:
		result |= TOPGUN_DTA;
		result |= TOPGUN_DCD;
		/* $$$ APTG DET => "DET"はコマンドで制御する $$$ */
		/*
		result |= TOPGUN_DTS;
		*/
		break;
	case GENE_RED:
		result |= TOPGUN_DCD;
		result |= TOPGUN_RED;
		break;
	case GENE_UNT:
		result |= TOPGUN_DCD;
		break;
	case GENE_ABT:
		result |= TOPGUN_ABT;
	default:
		break;
	}
	
	/* ATPGの結果(result)を故障データリストへ登録 */
	while ( flist ) {
		flist->info |= result;
		flist        = flist->next;

		/* ATPGの対象故障数を減らす */
	}

	if ( result & TOPGUN_DCD ) {
		Line_info.n_fault_atpg_count--;
		/*
		  {
		  printf("C-G   : %8ld ", gene_info->fault->line->line_id);
		  if ( gene_info->fault->info & TOPGUN_SA0 ) {
		  printf("sa0\n");
		  }
		  else {
		  printf("sa1\n");
		  }
		  }
		*/
	}
}

/* End of File */
