/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief  バックトラック操作とバックトレースツリーに関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunMemory.h" /* FMT_BTREE */
#include "topgunError.h"  /* FEC_XXX */

extern	GENE_HEAD	Gene_head;

/* function list */
extern void atpg_btree_enter_just_flag( JUST_INFO * );
extern void atpg_ent_btree( ASS_LIST *ass_list );
extern void gene_imp_trace_2_btree( Ulong );


/* print function */
void     topgun_print_mes( int, int );
void     topgun_print_mes_id_state9_0( LINE *, STATE_9, Uint );

/* test function */
void     test_ass_list ( ASS_LIST *, char * );
void     test_btree_null ( BTREE *, char * );
void     test_ass_list_next_null ( ASS_LIST *, char * );
void     test_just_info ( JUST_INFO *just_info, char  *org_function_name );

/********************************************************************************
 * << Function >>
 *		enter new back trace tree
 *
 * << Function ID >>
 *	   	19-4
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	        description
 * ass_list      I      ASS_LIST*       assign list for enter btree
 *
 * << extern >>
 * name			(I/O)	type            description
 * Gene_head     I		ATPG_HEAD		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/13
 *
 ********************************************************************************/

void gene_ent_btree
(
 ASS_LIST *ass_list /* B-Treeへ登録する情報(line, state)*/
 )
{
	BTREE   *ent_btree;         /* btreeへのポインタ */
	STATE_3 state3;

    char    *func_name = "atpg_ent_btree";	/* 関数名 */

	topgun_print_mes( 22, 0 );
	test_ass_list ( ass_list, func_name );
	test_ass_list_next_null ( ass_list, func_name );

	state3 = ass_list->ass_state3;

	ent_btree = Gene_head.btree;

	/* 値割り当てたflag */
	/* $$$ flagのデータ構造および 求め方に再考の余地あり $$$ */

	switch ( ass_list->condition ) {
	case COND_NORMAL:
		if ( state3 == STATE3_0 ) {
			ent_btree->flag |= ASS_N_0;
		}
		if ( state3 == STATE3_1 ) {
			ent_btree->flag |= ASS_N_1;
		}
		break;
	case COND_FAILURE:
		if ( state3 == STATE3_0 ) {
			ent_btree->flag |= ASS_F_0;
		}
		if ( state3 == STATE3_1 ) {
			ent_btree->flag |= ASS_F_1;
		}
		break;
	case COND_REVERSE:
	default:
		topgun_error( FEC_NO_ERROR_CODE, func_name);
		break;
	}

	ent_btree->condition  = ass_list->condition;
	ent_btree->ass_state3 = ass_list->ass_state3;
	ent_btree->line       = ass_list->line;

}

/********************************************************************************
 * << Function >>
 *		enter new root back trace tree
 *
 * << Function ID >>
 *	   	19-1
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/13
 *
 ********************************************************************************/

void atpg_init_btree
(
 void
 ){

	BTREE *ent_btree;

	char *func_name = "atpg_init_btree"; /* 関数名 */

	topgun_print_mes( 22, 0 );


	/* メモリの確保 */
	ent_btree = (BTREE *)topgunMalloc( FMT_BTREE, sizeof( BTREE ),
										1, func_name );

	ent_btree->flag       = 0;
	ent_btree->condition  = 0;

	ent_btree->ass_state3 = STATE3_X; /* btreeのrootは使用しない */
	ent_btree->line       = NULL;
#ifndef OLD_IMP2
	ent_btree->last_n     = 0;
	ent_btree->last_f     = 0;
	ent_btree->cnt_n      = 0;
	ent_btree->cnt_f      = 0;
#else	
	ent_btree->imp_trace  = NULL;
#endif /* OLD_IMP2 */
	ent_btree->just_e     = NULL;
	ent_btree->just_d     = NULL;
	ent_btree->just_flag  = NULL;
	ent_btree->next       = NULL;
	ent_btree->prev       = NULL;

	/* btreeの先頭とする */
	Gene_head.btree      = ent_btree;
}

/********************************************************************************
 * << Function >>
 *		enter single assign line and state of line in back trace tree
 *
 * << Function ID >>
 *	   	19-? (no use / comment out)
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type			description
 * line			I		LINE *			line pointer
 * stat			I		Ulong           line stat
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
#ifdef TOPGUN_NOUSE
void atpg_ent_single_stat_btree
(
 LINE *line,
 Ulong stat
 ){
	char *func_name = "atpg_ent_single_stat_btree"; /* 関数名 */

#ifndef TOPGUN_TEST
	if ( Gene_head.btree == NULL ) {
		topgun_error( FEC_PRG_BTREE, func_name );
	}
#endif /* TOPGUN_NULL */

	/* btreeへ登録 */
	Gene_head.btree->line     = line;

	test_state( Gene_head.btree->pre_stat );

	Gene_head.btree->pre_stat = line->stat;
	Gene_head.btree->ass_stat = stat;

}
#endif /* TOPGUN_NOUSE */


/********************************************************************************
 * << Function >>
 *		enter line of dfrontier in back trace tree
 *
 * << Function ID >>
 *	   	19-? (nouse)
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type			description
 * dfront		I		DFRONT *		dfront poitner
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
#ifdef TOPGUN_NOUSE
void atpg_ent_dfront_btree
(
 DFRONT *dfront
 ){

	DFRONT_LIST *dfront_list, *dfront_e;

	char *func_name = "atpg_ent_dfront_btree"; /* 関数名 */

	topgun_print_mes(22,3);

	dfront_list = (DFRONT_LIST *)topgunMalloc( FMT_DFRONT_LIST,
												sizeof( DFRONT_LIST ),
												1, func_name );

	dfront_list->dfront = dfront;
	dfront_list->next   = NULL;

	dfront_e = Gene_head.btree->dfront_e;

	if(dfront_e == NULL){
		Gene_head.btree->dfront_e = dfront_list;
	}
	else{
		dfront_list->next = dfront_e;
		Gene_head.btree->dfront_e = dfront_list;
	}
}
#endif /* TOPGUN_NOUSE */

/********************************************************************************
 * << Function >>
 *		enter line of justified in back trace tree
 *
 * << Function ID >>
 *	    19-3
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 * line			I		LINE *		line poitner
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

void atpg_ent_just_btree
(
 JUST_INFO *just_info
 ){
	char       *func_name = "atpg_ent_just_btree"; /* 関数名 */

	test_btree_null ( Gene_head.btree, func_name );
	test_just_info ( just_info, func_name );

	if ( Gene_head.btree->just_e == NULL ) {
		Gene_head.btree->just_e = just_info;
	}
	else {
		just_info->next = Gene_head.btree->just_e;
		Gene_head.btree->just_e = just_info;
	}
}

/********************************************************************************
 * << Function >>
 *		enter line of justified in back trace tree
 *
 * << Function ID >>
 *	    19-3
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 * line			I		LINE *		line poitner
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

void atpg_btree_enter_just_flag
(
 JUST_INFO *just_info
 ){
	char       *func_name = "atpg_btree_enter_just_info"; /* 関数名 */

	test_btree_null ( Gene_head.btree, func_name );
	test_just_info ( just_info, func_name );

	just_info->next = Gene_head.btree->just_flag;
	Gene_head.btree->just_flag = just_info;
}

/********************************************************************************
 * << Function >>
 *		make back trace tree and enter Gene_head
 *
 * << Function ID >>
 *	    19-3
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
 *		2005/11/8
 *
 ********************************************************************************/

void atpg_make_btree
(
 void
 ){
	BTREE *btree;

	char  *func_name = "atpg_make_btree"; /* 関数名 */

	btree = ( BTREE * )topgunMalloc( FMT_BTREE, sizeof( BTREE ),
										  1, func_name );

	btree->flag              = 0;
	btree->condition         = 0;
	btree->ass_state3        = STATE3_X;
	btree->line              = NULL;
#ifndef OLD_IMP2	
	btree->last_n            = 0;
	btree->last_f            = 0;
	btree->cnt_n             = 0;
	btree->cnt_f             = 0;
#else
	btree->imp_trace         = NULL;
#endif /* OLD_IMP2 */
	btree->just_e            = NULL;
	btree->just_d            = NULL;
	btree->just_flag         = NULL;
	btree->next              = NULL;
	btree->prev              = Gene_head.btree;

	if ( Gene_head.btree == NULL ) {
		Gene_head.btree = btree;
	}
	else {
		Gene_head.btree->next = btree;
		Gene_head.btree = btree;
	}
}

/********************************************************************************
 * << Function >>
 *		imp_trace move to Back trace Tree
 *
 * << Function ID >>
 *	    19-3
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
 *		2005/11/08
 *		2007/06/05     entery imp_id to line->imp_id
 *
 ********************************************************************************/

void gene_imp_trace_2_btree
(
 Ulong imp_id
 ){

	Ulong pos = 0;
	Ulong i = 0;
	
	// imp_idの登録
	// normal state
	for ( pos = Gene_head.last_n, i = 0; i < Gene_head.cnt_n; i++, pos-- ) {
		Gene_head.imp_list_n[ pos ]->imp_id_n = imp_id;
	}
	// failure state
	for ( pos = Gene_head.last_f, i = 0; i < Gene_head.cnt_f; i++, pos-- ) {
		Gene_head.imp_list_f[ pos ]->imp_id_f = imp_id;
	}
	
	/* 含意操作結果を正当化ツリーへの引渡し */
	Gene_head.btree->last_n    = Gene_head.last_n;
	Gene_head.btree->cnt_n     = Gene_head.cnt_n;
	Gene_head.btree->last_f    = Gene_head.last_f;
	Gene_head.btree->cnt_f     = Gene_head.cnt_f;

	Gene_head.cnt_n = 0;
	Gene_head.cnt_f = 0;
}
/* End of File */

