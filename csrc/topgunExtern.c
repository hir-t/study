/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief グローバル変数の設定と初期化
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#include <stdio.h>
#include "topgun.h"
#include "topgunState.h" /* STATE_3, STATE_9 */
#include "topgunLine.h" /* LINE, LINE_TYPE */
#include "topgunExtern.h"
#include "topgunAtpg.h"
#include "topgunFlist.h" /* for FLIST */
#include "topgunTime.h"
#include "topgunMemory.h"

LINE 	    *Line_head    = NULL;
LINE 	    **Pi_head     = NULL;
LINE 	    **Po_head     = NULL;
LINE 	    **BBPi_head   = NULL;
LINE 	    **BBPo_head   = NULL;
LINE 	    ***Lv_pi_head = NULL;
JUST_INFO 	**Lv_head	  = NULL;
FLIST 	    **Flist_head  = NULL;
TIME_HEAD   Time_head;

LINE_INFO	Line_info = {
    0,		/* n_line */
    0,		/* n_pi */
    0,		/* n_po */
    0,		/* n_bbpi */
    0,		/* n_bbpo */
    0,		/* n_fault */
    0,		/* n_fault_atpg */
	0,		/* n_fault_atpg_count */
    0,		/* max_lv_pi */
    0,		/* max_lv_po */	
    NULL,	/* n_lv_pi */
};

GENE_HEAD Gene_head = {
    0,		/* flag */
	ATPG_LIMIT_BACK, /* limit_back */
	0,	    /* back_all */
	0,	    /* back_drive */
	0,	    /* back_drive_just */
	NULL,	/* fault_line */
	NULL,	/* propa_line */
    NULL,	/* dfront_t */
    NULL,	/* dfront_c */
#ifndef OLD_IMP
	0,      /* n_imp_info_list */
	0,      /* n_enter_imp_list */
    NULL,   /* imp_info_list */
#else
	NULL,   /* imp_f */
    NULL,   /* imp_l */
#endif /* OLD_IMP */
    NULL,	/* btree */
#ifndef OLD_IMP2
#ifndef OLD_IMP3	
	0,        /* last_n 0は使わない*/
#else	
	FL_ULMAX, /* last_n 0は使う */
#endif /* OLD_IMP3 */	
	0,        /* cnt_n */
#ifndef OLD_IMP3
	0,        /* last_f 0は使わない*/
#else	
	FL_ULMAX, /* last_f 0は使う */
#endif /* OLD_IMP3 */	
	0,        /* to_f */	
	NULL,     /* imp_list_n */
	NULL,     /* imp_list_f */
#else	
    NULL,	/* imp_trace */
#endif /* OLD_IMP2 */	
	NULL,	/* cir_fix */
};

char TOPGUN_MEMORY_TYPE_NAME[FMT_NUM_MEMORY_TYPE][20] = {
	"Char",
	"Char *",
	"Ulong",
	"Ulong *",	
	"LINE",
	"LINE *",
	"LINE **",
	"FLIST",
	"FLIST *",
	"LINE_STACK",
	"LINE_STACK *",
	"BTREE",
	"DFRONT",
	"DFRONT *",
	"DFRONT_LIST",
	"ASS_LIST",
	"IMP_LIST",
	"IMP_INFO",
	"IMP_TRACE",
	"JUST_INFO",
	"JUST_INFO *",
	"CELL",
	"CELL *",
	"CELL_IO",
	"ENENVT_LIST",
	"CC_PATTERN",
	"CC_PATTERN_P",
	"LEARN_LIST",
	"LEARN_LIST_P",
	"LEARN_NODE",
	"LEARN_NODE_P",
	"LEARN_NODE_PP",
	"LEARN_BRANCH",
	"LEARN_BRANCH_P",
	"LEARN_S_LIST",
	"LEARN_S_LIST2",
	"LEARN_B_LIST",
	"LEARN_TOPO",
	"LEARN_NODE_LIST",
	"LEARN_EQ_INFO",
	"LEARN_EQ_HEAD",
	"GENE_INFO",	
	"STOCK_PAT",
	"STOCK_PAT_P",
	"STOCK_FAULT",
	"STOCK_FAULT_P",
	"FFRG_NODE",
	"FFRG_NODE_P",
};

#ifdef TOPGUN_MEMORY_LOG
FILE *Memory_log_fp; /* memory log file file pointer */

int TOPGUN_MEMORY_TYPE_SIZE[FMT_NUM_MEMORY_TYPE]= {
	0,    /* char          */
	4,    /* char *        */
	4,    /* unsinged long */
	4,    /* unsinged long**/
	60,   /* LINE          */
	4,    /* LINE*         */
	0,    /* LINE** $$$    */
	20,   /* FLIST         */
	4,    /* FLIST*        */
	8,    /* LINE_STACK    */
	4,    /* LINE_STACK *  */
	0,    /* BTREE         */
	0,    /* DFORNT        */
	4,    /* DFORNT *      */
	0,    /* DFORNT_LIST   */
	0,    /* AS_LIST       */
	0,    /* IMP_LIST      */
	0,    /* IMP_INFO      */
	0,    /* IMP_TRACE     */
	0,    /* JUST_INFO     */
	4,    /* JUST_INFO *   */
	0,    /* CELL          */
	4,    /* CELL*         */
	0,    /* CELL_IO       */
	0,    /* EVENT_LIST    */
	0,    /* CC_PATTERN    */
	4,    /* CC_PATTERN_P  */
	0,    /* LEARN_LIST    */
	4,    /* LEARN_LIST_P  */
	0,    /* LEARN_NODE    */
	4,    /* LEARN_NODE_P  */
	4,    /* LEARN_NODE_PP */
	0,    /* LEARN_BRANCH  */
	4,    /* LEARN_BRANCH_P*/
	0,    /* LEARN_S_LIST  */
	0,    /* LEARN_S_LIST2 */
	0,    /* LEARN_B_LIST  */
	0,    /* LEARN_TOPO    */
	0,    /* LEARN_NODE_LIST  */
	0,    /* LEARN_EQ_INFO */
	0,    /* LEARN_EQ_HEAD */
	0,    /* GENE_INFO */
	0,    /* STOCK_PAT */
	4,    /* STOCK_PAT_P */
	0,    /* STOCK_FAULT */
	4,    /* STOCK_FAULT_P */
	0,    /* FFRG_NODE */
	4,    /* FFRG_NODE_P */
};

#endif /* TOPGUN_MEMORY_LOG */

/* STATE 3 */
/*
STATE_3 State3_X =  {
	STAT_X,
};

STATE_3 State3_0 =  {
	STAT_0,
};

STATE_3 State3_1 =  {
	STAT_1,
};

STATE_3 State3_U =  {
	STAT_U,
};
STATE_3 State3_C =  {
	STAT_C,
};


STATE_9 State9_XX =  {
	STAT_XX,
};

STATE_9 State9_0X =  {
	STAT_0X,
};

STATE_9 State9_1X =  {
	STAT_1X,
};

STATE_9 State9_UX =  {
	STAT_UX,
};

STATE_9 State9_X0 =  {
	STAT_X0,
};

STATE_9 State9_00 =  {
	STAT_00,
};

STATE_9 State9_10 =  {
	STAT_10,
};

STATE_9 State9_U0 =  {
	STAT_U0,
};

STATE_9 State9_X1 =  {
	STAT_X1,
};

STATE_9 State9_01 =  {
	STAT_01,
};

STATE_9 State9_11 =  {
	STAT_11,
};

STATE_9 State9_U1 =  {
	STAT_U1,
};

STATE_9 State9_XU =  {
	STAT_XU,
};

STATE_9 State9_0U =  {
	STAT_0U,
};

STATE_9 State9_1U =  {
	STAT_1U,
};

STATE_9 State9_UU =  {
	STAT_UU,
};
*/


