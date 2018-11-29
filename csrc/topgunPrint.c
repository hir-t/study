/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief debug用のメッセージを出力する関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <string.h>

#include "topgun.h"
#include "topgunState.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunOut.h"
#include "topgunTm.h"
#include "topgunBench.h"
#include "topgunCell.h"
#include "topgunFsim.h"
#include "topgunCompact.h"
#include "topgunFlist.h"
#include "topgunError.h"
#include "topgunTime.h"

extern  LINE_INFO       Line_info;
extern  FSIM_HEAD       Fsim_head;
extern  LINE            *Line_head;
extern  GENE_HEAD       Gene_head;
extern  LINE_STACK      **Lv_head;
extern  LINE            **Pi_head;
extern  char            **Tp_head;


/* function list */
extern STATE_9 atpg_get_state3_with_condition_2_state9 ( STATE_3, CONDITION );

/* print function */
void topgun_print_mes( int, int );
void topgun_print_mes_result_2_2( FLIST * );
void topgun_print_mes_result_state9( int, int, STATE_9 );
void topgun_print_mes_result_line( int, int, LINE * );
void topgun_print_mes_id( LINE * );
void topgun_print_mes_id2 (LINE * );
void topgun_print_mes_id_only( LINE * );
void topgun_print_mes_type( LINE * );
void topgun_print_mes_state9( STATE_9 );
void topgun_print_mes_state9_only( STATE_9 );
void topgun_print_mes_state9_new( STATE_9, Uint );
void topgun_print_mes_state3_n( STATE_9 );
void topgun_print_mes_lv( LINE * );
void topgun_print_mes_n_f( LINE * );
void topgun_print_mes_n( void );
void topgun_print_mes_star ( void );
void topgun_print_mes_sp( Ulong );
void topgun_print_mes_id_type_state9( LINE * );
void topgun_print_mes_id_type_state9_lv( LINE * );
void topgun_print_mes_just( LINE *);
void topgun_print_mes_can_just( LINE * );
void topgun_print_mes_id_state9_0( LINE *, STATE_9, Uint );
void topgun_print_mes_id_type_flag( LINE * );
void topgun_print_mes_flag( int );
void topgun_print_mes_gate_info( LINE * );
void topgun_print_mes_gate_state9( LINE * );
void topgun_print_mes_gate_state9_new ( LINE *, Ulong, Uint, int );
void topgun_print_mes_gate_state9_only( LINE * );
void topgun_print_mes_imp_info_new( IMP_INFO * );
void topgun_print_mes_condition ( Uint );
void topgun_print_mes_imp_trace ( IMP_TRACE * );
void topgun_print_mes_learn_state3( LINE *, STATE_3, LINE *, STATE_3);
void topgun_print_mes_cut_learn_state3( LINE *, STATE_3, LINE *, STATE_3);
	
void topgun_print_mes_ass_list_n ( ASS_LIST * );
void topgun_print_mes_ass_list ( ASS_LIST * );
void topgun_print_mes_dfront ( DFRONT *, Ulong, Ulong );
void topgun_print_mes_dfront_t ( void ) ;
	
void topgun_print_mes_val2( Ulong );

/* all function for debug */
void topgun_print_imp_all( void );
void topgun_print_line_all( void );

void topgun_print_mes_compact_pattern( Ulong *, Ulong *, Ulong, Ulong );

#ifdef NOUSE
void topgun_print_mes_result_X( State9 );
void topgun_print_mes_enter_X(LINE *, Ulong );
void topgun_print_mes_result_Z( int );
void topgun_print_mes_id_pre_state9( LINE *,  CONDITION );
void topgun_print_mes_id_after_state9(LINE *, CONDITION );
void topgun_print_mes_fp_iap_on( LINE *);

#endif /* NOUSE */


/* other file function */
/* topgun_state.c */
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 );
STATE_3 atpg_get_state9_2_failure_state3 ( STATE_9 );
void    atpg_get_chara_stat ( STATE_3 , char * );

/* topgun_chk.c */
Uint    atpg_check_state9_n_f ( STATE_9 );

#ifdef _DEBUG_PRT_ATPG_
static char print_mes_0[2][30] = {  /* common message */
	" =>  end\n",					/* 0 end */
	" =>  continue\n",				/* 1 continue */
};

static char print_mes_1[1][2] = { 	/* chapter 1 */
	"",
};
static char print_mes_2[9][30] = {	/* chapter 2 */
	"",								/* 0 */
	"2-1   Check the end of atpg",	/* 1 2-1 */
	"2-2   Select fault of atpg",	/* 2 2-2 */
	"2-3   Generate pattern",		/* 3 2-3 */
    "2-3-1 fault insert",			/* 4 2-3-1 */
	"      =>  insert fault",		/* 5 2-3-1-1 */
	"      =>  cannot insert fault",/* 6 2-3-1-2 */
	"      --> check line stat",	/* 7 2-3-1-3 */
	"2-3-Z Generate compliete",     /* 8 2-3-Z */
};

static char print_mes_L[12][30] = {	/* chapter L (12)*/
	"2-L-0 initail",                /* 0 initailze */                 
	"2-L-1 topological sort",       /* 1 direct list */
	"2-L-2 direct list line ->",    /* 2 direct list from line->*/
	"2-L-3 search from line",       /* 3 search line & state */
	"2-L-4 NOT need indirect",      /* 4 possible direct implication */
	"2-L-5 already learn indirect", /* 5 already learning indirect */
	"2-L-6 Learn indirect",         /* 6 indirect implacation learn */
	"2-L-7 Learn result",           /* 7 indirect implication result */
	"2-L-8 Learn result print",     /* 8 indirect implication result print */
	"2-L-9 Iteration",              /* 9 indirect implication result print */
	"2-L-A redo sort",              /* A indirect implication result print */
	"2-L-M Merge node",             /* M indirect implication result print */
};


static char print_mes_P[7][30] = {	/* chapter P (17)*/
	"2-P-0 n-state with rereverse", /* 0 reverse state search */
	"2-P-1 n and f common state ",  /* 1 n and f enter state */
};

static char print_mes_R[8][30] = {	/* chapter R (18)*/
	"2-R-0 reverse state search",   /* 0 reverse state search */
	"2-R-1 reverse OK",             /* 1 OK */
	"2-R-2 NG already set",         /* 2 NG */
	"2-R-3 NG don't make reverse",  /* 3 NG */
	"2-R-4 reverse implication NG", /* 4 reverse implication NG */
	"2-R-5 reverse implication OK", /* 5 reverse implication NG */	
	"2-R-6 btree one back stem",    /* 6 btree one back */
	"2-R-7 can't insert r-state",   /* 7 insert r-state NG */
};


static char print_mes_S[6][30] = {	/* chapter S (19) */
	"2-S-0 select fault id",	    /* 0 FP_IAP flag */
	"2-S-1 setting fault id",       /* 1 flag on */
};


static char print_mes_T[6][30] = {	/* chapter T (20) */
	"2-T-0 FP_IAP flag",   		    /* 0 FP_IAP flag */
	"2-T-1 FP_IAP flag on",         /* 1 flag on */
};

static char print_mes_U[6][30] = {	/* chapter U (21) */
	"2-U-0 reflesh start",   		/* 0 reflesh start  */
	"2-U-1 D-Tree ",                /* 1 dtree reflesh */
	"2-U-2 B-Tree ",                /* 2 btree reflesh */
	"2-U-3 flag",                   /* 3 flag etc */
	"2-U-4 implication",            /* 4 implication result */
	"2-U-5 assignment",             /* 5 assignment value */
};

static char print_mes_V[5][30] = {	/* chapter V (22) */
	"2-V-0 B-Tree entry ",   		/* 0 btree entry  */
	"2-V-1 B-Tree delete ",         /* 1 btree delete */
	"2-V-2 B-Tree entry imp_trace", /* 2 btree entry imp_trace */
	"2-V-3 B-Tree entry D-front ",  /* 3 btree entry dfront */
	"2-V-4 B-Tree entry just_list ",/* 4 btree entry just_list */
};

static char print_mes_W[8][30] = {	/* chapter W (23) */
	"2-W-0 D-front entry  ",   		/* 0 dfront entry  */
	"2-W-1 D-front delete ",   		/* 1 dfront delete */
	"2-W-2 D-front check ",   		/* 2 dfront check */
	"2-W-3 D-front search ",   		/* 3 dfront search */
	"2-W-4 D-front no output ",		/* 4 dfront no output */
	"2-W-5 D-front drive",			/* 5 dfront no output */
	"2-W-6 D-front multi output",	/* 6 dfront multi output */
	"2-W-7 D-front infomation",	    /* 7 dfront infomation */
};

static char print_mes_X[10][30] = {	/* chapter X (24) */
	"2-X   Implication",			/* 0 */
	"2-X-0 enter implication",		/* 1 */
	"2-X-1 implication select",		/* 2 */
	"      =>  Conflict",			/* 3 */
	"      =>  Keep",				/* 4 */
	"      =>  Update",				/* 5 */	
	"      enter reverse state",	/* 6 */
	"      only normal state",	    /* 7 */
	"      no imp fault line",	    /* 8 */
	"2-X-2 learn update",	     	/* 9 */
};

static char print_mes_Y[9][30] = {	/* chapter Y */
	"2-Y   Justified",				/* 0 */
	"2-Y-1 search not justify line",/* 1 */
	"2-Y-2 justify candidate list ",/* 2 */
	"2-Y-3 select justify line",	/* 3 */
	"2-Y-4 enter",					/* 4 */
	"2-Y-5 no candidate",			/* 5 */
	"2-Y-6 candidate enter",		/* 6 */
	"2-Y-7 Justified complite",		/* 7 */
	"2-Y-8 Justified No Entry",		/* 8 */
};

static char print_mes_Z[5][30] = {	/* chapter Z */
	"2-Z   Propagate",				/* 0 */
	"2-Z-1 search propagate line",	/* 1 */
	"2-Z-2 finish to propagate",	/* 2 */
	"2-Z-3 don't finish to propa",	/* 3 */
	"STAT ",	                    /* 4 */
};

static char print_mes_type[TOPGUN_NUM_PRIM][5] = {	/* chapter Z */
	"PI  ",						/*  0 */
	"PO  ",						/*  1 */
	"BR  ",						/*  2 */
	"INV ",						/*  3 */
	"BUF ",						/*  4 */	
	"AND ",						/*  5 */
	"NAND",						/*  6 */
	"OR  ",						/*  7 */
	"NOR ",						/*  8 */	
	"XOR ",						/*  9 */
	"XNOR",						/* 10 */	
	"BLK ",						/* 11 */
	"UNK ",						/* 12 */
};
#endif /* _DEBUG_PRT_ATPG_ */


#ifdef _DEBUG_INPUT_
static char print_mes_bench_0[2][30] = {  /* 0-common message */
	"bench net type analyze start",		 /* 0 start */
	"bench net type analyze end",	     /* 1 end   */
};
static char print_mes_bench_1[5][30] = {  /* 1-header analyze */
	"header -> comment line",            /* 0 comment */
	"header -> end of file",             /* 1 EOF */
	"header -> input",                   /* 2 input */
	"header -> output",                  /* 3 output */
	"header -> gate",                    /* 4 gate */
};
static char print_mes_bench_2[2][30] = {  /* 2-gate analyze */
	"gate type ",                        /* 0 type */
	"gate table ",                       /* 1 table */
};
static char print_mes_bench_3[1][30] = {  /* 3-make new cell */
	"make new cell",                     /* 0 mes */
};
static char print_mes_bench_4[4][30] = {  /* 4-make po */
	"PO input gate",                     /* 0 mes */
	"make new po input gate",            /* 1 make new cell */
	"made already po input gate",        /* 2 alredy made cell */
	"make new po gate",                  /* 3 make new cell */
};
static char print_mes_bench_5[1][30] = {  /* 5-make pi */
	"PI info",                           /* 0 mes */
};
static char print_mes_bench_6[4][30] = {  /* 6-make fanout */
	"insert fanout on stem",             /* 0 mes */
	"branch name",                       /* 1 make branch name */
	"branch connect",                    /* 2 connect branch */
	"result of insert fanout",           /* 3 result */
};
static char print_mes_bench_7[3][30] = {  /* 7-make and conect between cells */
	"connect gate",                      /* 0 mes */
	"new cell",                          /* 1 mes */
	"existing",                          /* 2 mes */
};
#endif /* _DEBUG_INPUT_ */

void extern inline topgun_print_mes(int chapter, int section){
#ifdef _DEBUG_PRT_ATPG_
	
	switch(chapter){
	case 0:
		topgun_print_mes_sp(5);
		printf("%s",print_mes_0[section]);
		break;
	case 1:
		printf("%s\n",print_mes_1[section]);
		break;
	case 2:
		printf("%s\n",print_mes_2[section]);
		break;
	case 12:
		printf("%s",print_mes_L[section]);
		break;
	case 17:
		printf("%s\n",print_mes_P[section]);
		break;
	case 18:
		printf("%s\n",print_mes_R[section]);
		break;
	case 19:
		printf("%s",print_mes_S[section]);
		break;		
	case 20:
		printf("%s\n",print_mes_T[section]);
		break;
	case 21:
		printf("%s\n",print_mes_U[section]);
		break;
	case 22:
		printf("%s\n",print_mes_V[section]);
		break;
	case 23:
		printf("%s\n",print_mes_W[section]);
		break;
	case 24:
		printf("%s\n",print_mes_X[section]);
		break;
	case 25:
		printf("%s\n",print_mes_Y[section]);
		break;
	case 26:
		printf("%s\n",print_mes_Z[section]);
		break;
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_result_2_2(FLIST *fault){
#ifdef _DEBUG_PRT_ATPG_

	topgun_print_mes_sp(5);	
	topgun_print_mes_id(fault->line);

    if(fault->info & TOPGUN_SA0){
			topgun_print_mes_state9( STATE9_10 );
    }
    else if(fault->info & TOPGUN_SA1){
			topgun_print_mes_state9( STATE9_01 );
    }
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_result_state9(int chapter, int section, STATE_9 state9){
#ifdef _DEBUG_PRT_ATPG_

	topgun_print_mes(chapter,section);
	topgun_print_mes_sp(5);
	topgun_print_mes_state9(state9);
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_result_line(int chapter, int section, LINE *line){
#ifdef _DEBUG_PRT_ATPG_

	topgun_print_mes(chapter,section);
	topgun_print_mes_id_type_state9(line);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_id(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	printf(" ID %5ld",line->line_id);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_id2(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	printf(" %5ld",line->line_id);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_id_only(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	printf("%8ld",line->line_id);
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_type(LINE *line){
#if _DEBUG_PRT_ATPG_ | _DEBUG_PRT_FSIM_
	printf(" TYPE %s",print_mes_type[line->type]);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_type_only(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	printf(" %s",print_mes_type[line->type]);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_state9( STATE_9 state9 ){
#ifdef _DEBUG_PRT_ATPG_
	char normal[2];
	char failure[2];

	atpg_get_chara_stat((atpg_get_state9_2_normal_state3 ( state9)),normal);	
	atpg_get_chara_stat((atpg_get_state9_2_failure_state3 ( state9)),failure);	

	printf(" STAT %s/%s",normal,failure);
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_state3( STATE_3 state3 ){
#ifdef _DEBUG_PRT_ATPG_
	char normal[2];
	
	atpg_get_chara_stat( state3 ,normal);	

	printf(" STAT %s",normal);
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_state3_only( STATE_3 state3 ){
#ifdef _DEBUG_PRT_ATPG_
	char normal[2];
	
	atpg_get_chara_stat( state3 ,normal);	

	printf("%s",normal);
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_state9_only( STATE_9 state9){
#ifdef _DEBUG_PRT_ATPG_
	char normal[2];
	char failure[2];

	atpg_get_chara_stat((atpg_get_state9_2_normal_state3 ( state9)),normal);	
	atpg_get_chara_stat((atpg_get_state9_2_failure_state3 ( state9)),failure);	

	printf("%s/%s",normal,failure);
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_state9_n( STATE_9 state9 ){
#ifdef _DEBUG_PRT_ATPG_
	char normal[2];

	atpg_get_chara_stat((atpg_get_state9_2_normal_state3 ( state9)),normal);

	printf(" %s",normal);
#endif /* _DEBUG_PRT_ATPG_ */	
}


void extern inline topgun_print_mes_lv(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	printf(" %5ld",line->lv_pi);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_n_f(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	if ( line->flag & JUSTIFY_NO ) {
		printf(" Normal  ");
	}
	if ( line->flag & JUSTIFY_FL ) {
		printf(" Failure ");
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_n(){
#if _DEBUG_PRT_ATPG_ | _DEBUG_PRT_FSIM_
	printf("\n");
#endif /* _DEBUG_PRT_ATPG_ | _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_star(){
#ifdef _DEBUG_PRT_ATPG_	
	printf("*");
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_sp( Ulong cnt){
#if _DEBUG_PRT_ATPG_ | _DEBUG_PRT_FSIM_
	Ulong i;
	for(i=0;i<cnt;i++){
		printf(" ");
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_comma(){
#ifdef _DEBUG_PRT_ATPG_
	printf(",");
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_id_type_state9(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	topgun_print_mes_sp(5);	
	topgun_print_mes_id(line);
	topgun_print_mes_type(line);
	topgun_print_mes_state9(line->state9);
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_id_type_state9_lv( LINE *line ){
#ifdef _DEBUG_PRT_ATPG_
	topgun_print_mes_sp(5);	
	topgun_print_mes_id(line);
	topgun_print_mes_type(line);
	topgun_print_mes_state9(line->state9);
	topgun_print_mes_lv(line);
	topgun_print_mes_n_f(line);
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_just(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	topgun_print_mes(25,1);
	topgun_print_mes_id_type_state9(line);
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_can_just(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	topgun_print_mes(25,3);
	topgun_print_mes_id_type_state9_lv(line);
#endif /* _DEBUG_PRT_ATPG_ */		
}


void extern inline topgun_print_mes_id_state9_0(LINE *line, STATE_9 state9, Uint condition){
#ifdef _DEBUG_PRT_ATPG_
	
	topgun_print_mes_id(line);
	topgun_print_mes_sp(1);
	topgun_print_mes_state9_new(state9, condition);
	topgun_print_mes_n();
	
#endif /* _DEBUG_PRT_ATPG_ */		
}


void extern inline topgun_print_mes_id_type_flag(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	
	topgun_print_mes_sp(5);	
	topgun_print_mes_id(line);
	topgun_print_mes_type(line);
	topgun_print_mes_flag(line->flag);
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_flag(int flag){
#ifdef _DEBUG_PRT_ATPG_
	
	int i;
	for (i = 0; i < 15; i++ ) {
		/* 1bit目のフラグ */
		if ( flag & LINE_SA0 ) {
			printf("%d ",i+1);
		}
		flag /= 2;
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}


void extern inline topgun_print_mes_gate_info(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	topgun_print_mes_sp(21);
	printf("ID");
	topgun_print_mes_sp(6);
	printf("OUT   ");

	for ( i = 0 ; i < line->n_in  ; i++ ) {
		topgun_print_mes_sp(5);
		printf("ID");
		topgun_print_mes_sp(6);	
		printf("IN%-2ld  ",i);
	}
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */		
}

void extern inline topgun_print_mes_gate_state9(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	/* OUT */
	topgun_print_mes_sp(5);	
	topgun_print_mes_type(line);
	topgun_print_mes_id_only(line);
	topgun_print_mes_sp(5);	
	topgun_print_mes_state9_only(line->state9);
	topgun_print_mes_sp(1);

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		topgun_print_mes_id_only(line->in[i]);
		topgun_print_mes_sp(5);	
		topgun_print_mes_state9_only(line->in[i]->state9);
		topgun_print_mes_sp(1);
	}
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */		
}

void extern inline topgun_print_mes_exclamation_condition( Uint condition ){
#ifdef _DEBUG_PRT_ATPG_
	
	if (condition == COND_NORMAL ){
		topgun_print_mes_sp(1);
		printf("!");
		topgun_print_mes_sp(3);
	}
	else {
		topgun_print_mes_sp(3);
		printf("!");
		topgun_print_mes_sp(1);
	}

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_at_condition( Uint condition){
#ifdef _DEBUG_PRT_ATPG_
	
	if (condition == COND_NORMAL ){
		topgun_print_mes_sp(1);
		printf("@");
		topgun_print_mes_sp(3);
	}
	else {
		topgun_print_mes_sp(3);
		printf("@");
		topgun_print_mes_sp(1);
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_end_condition( Uint condition){
#ifdef _DEBUG_PRT_ATPG_
	
	if (condition == COND_NORMAL ){
		topgun_print_mes_sp(1);
		printf("#");
		topgun_print_mes_sp(2);
	}
	else {
		topgun_print_mes_sp(2);
		printf("#");
		topgun_print_mes_sp(1);
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_next_condition( LINE *line, Ulong pin_num, Uint condition){
#ifdef _DEBUG_PRT_ATPG_

	if ( condition == COND_NORMAL ) {
		topgun_print_mes_sp(1);

		if ( line->in[pin_num-1]->flag & JUSTIFY_NO ) {
			printf("-");
		}
		else {
			printf("+");
		}
		topgun_print_mes_sp(3);
	}
	else {
		if ( ( line->in[pin_num-1]->flag & FP_IAP ) &&
			 ( ! ( line->in[pin_num-1]->flag & LINE_FLT ) ) ) {

			/* この場合は故障値可 */
			
			topgun_print_mes_sp(3);

			if ( line->in[pin_num-1]->flag & JUSTIFY_FL ) {
				printf("-");
			}
			else {
				printf("+");
			}			
			topgun_print_mes_sp(1);
		}
		else {

			if ( line->in[pin_num-1]->flag & LINE_FLT ) {
			
				/* 故障値不可 */
				topgun_print_mes_sp(1);

				if ( line->in[pin_num-1]->flag & JUSTIFY_NO ) {
					printf("-<F");
				}
				else {
					printf("+<F");
				}							
				
				topgun_print_mes_sp(1);
			}
			else {
				/* 故障値不可 */
				topgun_print_mes_sp(1);

				if ( line->in[pin_num-1]->flag & JUSTIFY_NO ) {
					printf("-<-");
				}
				else {
					printf("+<-");
				}							
				topgun_print_mes_sp(1);
			}
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}


void extern inline topgun_print_mes_gate_at_condition
(
 LINE  *line,
 Ulong pin_num,
 Uint  condition
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	topgun_print_mes_sp(28);
		
	if ( OUT_PIN_NUM == pin_num ) {
		topgun_print_mes_at_condition( condition ) ;
	}
	else {
		topgun_print_mes_sp(4);
	}

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		topgun_print_mes_sp(13);
		if ( ( i + 1 ) == pin_num ) {
			topgun_print_mes_at_condition( condition ) ;
		}
		else {
			topgun_print_mes_sp(4);
		}
		topgun_print_mes_sp(2);
	}
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_gate_exclamation_condition
(
 LINE  *line,
 Ulong pin_num,
 Uint  condition
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	topgun_print_mes_sp(30);
		
	if ( OUT_PIN_NUM == pin_num ) {
		topgun_print_mes_exclamation_condition( condition ) ;
	}
	else {
		topgun_print_mes_sp(4);
	}

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		topgun_print_mes_sp(13);
		if ( ( i + 1 ) == pin_num ) {
			topgun_print_mes_exclamation_condition( condition ) ;
		}
		else {
			topgun_print_mes_sp(4);
		}
		topgun_print_mes_sp(2);
	}
	topgun_print_mes_n();
	
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_gate_end_condition
(
 LINE  *line,
 Ulong pin_num,
 Uint  condition
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	topgun_print_mes_sp(28);
		
	if ( OUT_PIN_NUM == pin_num ) {
		topgun_print_mes_end_condition( condition ) ;
	}
	else {
		topgun_print_mes_sp(4);
	}

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		topgun_print_mes_sp(13);
		if ( ( i + 1 ) == pin_num ) {
			topgun_print_mes_end_condition( condition ) ;
		}
		else {
			topgun_print_mes_sp(4);
		}
		topgun_print_mes_sp(2);
	}
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_gate_next_condition
(
 LINE  *line,
 Ulong pin_num,
 Uint  condition
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	topgun_print_mes_sp(30);
		
	if ( OUT_PIN_NUM == pin_num ) {
		topgun_print_mes_next_condition( line, pin_num, condition );
	}
	else {
		topgun_print_mes_sp(4);
	}

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		topgun_print_mes_sp(13);
		if ( ( i + 1 ) == pin_num ) {
			topgun_print_mes_next_condition( line, pin_num, condition ) ;
		}
		else {
			topgun_print_mes_sp(4);
		}
		topgun_print_mes_sp(2);
	}
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_gate_state9_new
(
 LINE *line,
 Ulong pin_num,
 Uint  condition,
 int  flag
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	/* OUT */
	if ( flag  == 0 ) {
		topgun_print_mes_sp(5);	
		topgun_print_mes_type(line);
		topgun_print_mes_id_only(line);
		topgun_print_mes_sp(5);
	}
	else {
		topgun_print_mes_sp(28);
	}
		
	if ( OUT_PIN_NUM == pin_num ) {
		topgun_print_mes_state9_new(line->state9, condition);
	}
	else {
		topgun_print_mes_state9_new(line->state9, 0);
	}
	topgun_print_mes_sp(1);

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		if ( flag == 0 ) {
			topgun_print_mes_id_only(line->in[i]);
		}
		else {
			topgun_print_mes_sp(8);
		}
		topgun_print_mes_sp(5);
		if ( ( i + 1 ) == pin_num ) {
			topgun_print_mes_state9_new(line->in[i]->state9, condition);
		}
		else {
			topgun_print_mes_state9_new(line->in[i]->state9, 0);
		}
		topgun_print_mes_sp(1);
	}
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_gate_info_learn
(
 ){
#ifdef _DEBUG_PRT_ATPG_
	topgun_print_mes_sp(21);
	printf("ID");
	topgun_print_mes_sp(6);
	printf("LINE");
	topgun_print_mes_sp(7);
	printf("ID");
	topgun_print_mes_sp(6);
	printf("INDIRECT");
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_gate_state9_line
(
 LINE *line
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	topgun_print_mes_sp(5);	
	topgun_print_mes_type(line);
	topgun_print_mes_id_only(line);
	topgun_print_mes_sp(5);
	topgun_print_mes_state9_new(line->state9, COND_NORMAL);
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_gate_state9_learn
(
 LEARN_LIST *learn_list
 ){
#ifdef _DEBUG_PRT_ATPG_
	STATE_9 state9;
	
	topgun_print_mes_sp(34);
	topgun_print_mes_id_only(learn_list->line);
	topgun_print_mes_sp(5);
	state9 = atpg_get_state3_with_condition_2_state9 ( learn_list->ass_state3, COND_NORMAL);
	topgun_print_mes_state9_new( state9, COND_NORMAL);
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */	
}


void extern inline topgun_print_mes_gate_state9_new_lv
(
 LINE *line,
 Ulong pin_num,
 Uint  condition,
 int  flag
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	/* OUT */
	if ( flag  == 0 ) {
		topgun_print_mes_sp(4);
		topgun_print_mes_lv(line);
		topgun_print_mes_type_only(line);
		topgun_print_mes_id_only(line);
		topgun_print_mes_sp(5);
	}
	else {
		topgun_print_mes_sp(28);
	}
		
	if ( OUT_PIN_NUM == pin_num ) {
		topgun_print_mes_state9_new(line->state9, condition);
	}
	else {
		topgun_print_mes_state9_new(line->state9, 0);
	}
	topgun_print_mes_sp(1);

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		if ( flag == 0 ) {
			topgun_print_mes_id_only(line->in[i]);
		}
		else {
			topgun_print_mes_sp(8);
		}
		topgun_print_mes_sp(5);
		if ( ( i + 1 ) == pin_num ) {
			topgun_print_mes_state9_new(line->in[i]->state9, condition);
		}
		else {
			topgun_print_mes_state9_new(line->in[i]->state9, 0);
		}
		topgun_print_mes_sp(1);
	}
	topgun_print_mes_n();
	
#endif /* _DEBUG_PRT_ATPG_ */	
}


void extern inline topgun_print_mes_gate_state9_only(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong   i;

	/* OUT */
	topgun_print_mes_sp(28);	
	topgun_print_mes_state9_only(line->state9);
	topgun_print_mes_sp(1);

	/* IN? */
	for ( i = 0; i < line->n_in ; i++ ){
		topgun_print_mes_sp(13);	
		topgun_print_mes_state9_only(line->in[i]->state9);
		topgun_print_mes_sp(1);
	}
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_imp_info_new(IMP_INFO *imp_info){
#ifdef _DEBUG_PRT_ATPG_	

	Ulong   i;

	topgun_print_mes_sp(28);

	for ( i = 0; i < imp_info->pin_num ; i++ ) {
			topgun_print_mes_sp(17);
	}
	if ( imp_info->condition == COND_FAILURE ) { 
		topgun_print_mes_sp(2);
	}
	printf("~");
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */
}





void extern inline topgun_print_mes_state9_new( STATE_9 state9, Uint condition){
#ifdef _DEBUG_PRT_ATPG_		

	switch ( condition ) {
	case COND_NORMAL:
		topgun_print_mes_star();
		topgun_print_mes_state9_only(state9);
		topgun_print_mes_sp(1);
		break;
	case COND_FAILURE:
		topgun_print_mes_sp(1);
		topgun_print_mes_state9_only(state9);
		topgun_print_mes_star();
		break;
	default:
		topgun_print_mes_sp(1);
		topgun_print_mes_state9_only(state9);
		topgun_print_mes_sp(1);
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}



void extern inline topgun_print_mes_condition ( Uint condition ){
#ifdef _DEBUG_PRT_ATPG_
	
	switch ( condition ) {
	case COND_NORMAL:
		printf(" N ");
		break;
	case COND_FAILURE:
		printf(" F ");
		break;
	default:
		printf(" * ");
	}
#endif /* _DEBUG_PRT_ATPG_ */		
}

void extern inline topgun_print_mes_imp_trace ( IMP_TRACE *imp_trace ){
#ifdef _DEBUG_PRT_ATPG_
	Ulong i = 1;

	while ( imp_trace ) {
		if ( i == 1 ) {
			printf("imp_trace %3ld ",i );
		}
		else {
			printf("          %3ld ",i );
		}
		i++;
		topgun_print_mes_id2 ( imp_trace->line );
		topgun_print_mes_condition ( imp_trace->condition );
		topgun_print_mes_n ();
		imp_trace = imp_trace->next;
	}
#endif /* _DEBUG_PRT_ATPG_ */		
}

void extern inline topgun_print_mes_ass_list_n ( ASS_LIST *ass_list ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong i = 1;

	while ( ass_list ) {
		if ( i == 1 ) {
			printf("ass_list  %3ld ",i );
		}
		else {
			printf("          %3ld ",i );
		}
		i++;
		topgun_print_mes_id2 ( ass_list->line );
		topgun_print_mes_condition ( ass_list->condition );
		topgun_print_mes_state3 ( ass_list->ass_state3 );
		topgun_print_mes_n ();
		ass_list = ass_list->next;
	}
#endif /* _DEBUG_PRT_ATPG_ */			
}

void extern inline topgun_print_mes_ass_list ( ASS_LIST *ass_list ){
#ifdef _DEBUG_PRT_ATPG_
	
	printf(" ass ");
	while ( ass_list ) {
		topgun_print_mes_id2 ( ass_list->line );
		topgun_print_mes_condition ( ass_list->condition );
		topgun_print_mes_state3 ( ass_list->ass_state3 );
		topgun_print_mes_sp (1);
		ass_list = ass_list->next;
	}

#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_dfront ( DFRONT * dfront, Ulong i, Ulong j ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong k;

	printf("DFRONT %8ld-%2ld flag : ",i,j);
	topgun_print_mes_flag(dfront->flag);
	topgun_print_mes_n ();
	topgun_print_mes_sp (11);
	topgun_print_mes_id(dfront->line);
	topgun_print_mes_sp (4);
	topgun_print_mes_state9_only(dfront->line->state9);
	topgun_print_mes_n ();
	topgun_print_mes_ass_list_n ( dfront->ass_list );
#ifndef OLD_IMP2
#else	
	topgun_print_mes_imp_trace ( dfront->imp_trace );
#endif /* OLD_IMP2 */


	for ( k = 0; k < dfront->line->n_out ; k++ ) {

		if ( dfront->next == NULL ) {
			break;
		}
		if ( dfront->next[k] == NULL ) {
			break;
		}
		topgun_print_mes_dfront ( dfront->next[ k ], i+1, k+1 );
	}
	
#endif /* _DEBUG_PRT_ATPG_ */
}
	
void extern inline topgun_print_mes_dfront_t( void ) {
#ifdef _DEBUG_PRT_ATPG_
	
	DFRONT *dfront = NULL;
	Ulong  i = 1; /* 世代数 */
	Ulong  j = 1; /* 兄弟数 */
	
	dfront = Gene_head.dfront_t;

	printf("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n");
	topgun_print_mes_dfront ( dfront, i, j );
	printf("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n");

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_flag_dfront(int flag){
#ifdef _DEBUG_PRT_ATPG_
	
	if ( flag & DF_DEADEND ) {
		printf(" xxx ");
	}
	else if ( flag & DF_ROOT ) {
		printf(" ooo ");
	}
	else {
		printf(" ??? ");
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_dfront_no_imp ( DFRONT * dfront, Ulong i, Ulong j ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong k;

	/* prim & tm_co */

	printf("%3ld-%ld ",i,j);
	topgun_print_mes_flag_dfront(dfront->flag);
	topgun_print_mes_id(dfront->line);
	topgun_print_mes_sp (1);
	topgun_print_mes_state9_only(dfront->line->state9);
	topgun_print_mes_ass_list ( dfront->ass_list );
	topgun_print_mes_n ();


	for ( k = 0; k < dfront->line->n_out ; k++ ) {

		if ( dfront->next == NULL ) {
			break;
		}
		if ( dfront->next[k] == NULL ) {
			break;
		}
		topgun_print_mes_dfront_no_imp ( dfront->next[ k ], i+1, k+1 );
	}

#endif /* _DEBUG_PRT_ATPG_ */		
}


void extern inline topgun_print_mes_dfront_condition ( void ) {
#ifdef _DEBUG_PRT_ATPG_
	
	DFRONT *dfront;
	Ulong  i = 1;
	Ulong  j = 1;

	topgun_print_mes ( 23, 7 );
	
	dfront = Gene_head.dfront_t;
	topgun_print_mes_dfront_no_imp ( dfront, i, j );

#endif /* _DEBUG_PRT_ATPG_ */			
}

void extern inline topgun_print_mes_justified_start ( Ulong start_lv ) {
#ifdef _DEBUG_PRT_ATPG_
	printf(" start level %ld\n", start_lv );
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_justified_search ( LINE *line, int condition ) {
#ifdef _DEBUG_PRT_ATPG_
	topgun_print_mes_gate_info ( line );
	topgun_print_mes_gate_state9_new_lv( line, 0, condition, 0 );
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_just_e (){
#ifdef _DEBUG_PRT_ATPG_

	JUST_INFO *just_info;

	topgun_print_mes ( 25, 2 );
	
	just_info = Gene_head.btree->just_e;
	topgun_print_mes_gate_info ( just_info->line );
	
	while(just_info){
		topgun_print_mes_gate_state9_new_lv
			( just_info->line, 0, just_info->condition, 0 );
		just_info = just_info->next;
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_ent_just_flag(){
#ifdef _DEBUG_PRT_ATPG_

	JUST_INFO *flag = Gene_head.btree->just_flag;
	
	printf("entry just flag list\n");
	
	while(flag){
		topgun_print_mes_id(flag->line);
		topgun_print_mes_n();
		flag = flag->next;
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_del_just_flag_opening(){
#ifdef _DEBUG_PRT_ATPG_
	
	printf("delete just flag list\n");

#endif /* _DEBUG_PRT_ATPG_ */	
}
void extern inline topgun_print_mes_del_just_flag(LINE *line ){
#ifdef _DEBUG_PRT_ATPG_
	
	topgun_print_mes_id(line);
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */	
}


void extern inline topgun_print_mes_j_ass_list (LINE *line, ASS_LIST *ass_list){
#ifdef _DEBUG_PRT_ATPG_

	Ulong i;
	
	topgun_print_mes_gate_info ( line );
	if ( line->flag & JUSTIFY_NO ) {
		topgun_print_mes_gate_state9_new_lv( line, 0, COND_NORMAL, 0 );
	}
	if ( line->flag & JUSTIFY_FL ) {
		topgun_print_mes_gate_state9_new_lv( line, 0, COND_FAILURE, 0 );
	}

	for ( i = 0 ; i < line->n_in ; i++ ) {
		if ( line->in[i]->line_id == ass_list->line->line_id ) {
			topgun_print_mes_gate_exclamation_condition ( line, i+1, ass_list->condition );
			break;
		}
	}

#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_lv_head()
{
#ifndef TOPGUN_TEST
#ifdef _DEBUG_PRT_ATPG_
	Ulong      i,j;
	LINE_STACK *stk;

	printf("lv_head\n");
	for ( i = 0 ; i < Line_info.max_lv_pi ; i ++ ) {
		stk = Lv_head[i];
		j = 0;
		while(stk){
			if(stk->line == NULL){
				printf(" %ld %ld N\n",i,j);
			}
			else{
				printf(" %ld %ld id %5ld\n",i,j,stk->line->line_id);
			}
			j++;
			stk = stk->next;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */	
#endif /* TOPGUN_TEST */
}

void extern inline topgun_print_mes_btree_flag( Ulong flag ){
#ifdef _DEBUG_PRT_ATPG_
	
	printf("   flag: N ");
	if ( flag & ASS_N_0 ) {
		printf(" 0 ");
	}
	else {
		printf("   ");
	}
	if ( flag & ASS_N_1 ) {
		printf(" 1 ");
	}
	else {
		printf("   ");
	}
	printf(" F ");
	if ( flag & ASS_F_0 ) {
		printf(" 0 ");
	}
	else {
		printf("   ");
	}
	if ( flag & ASS_F_1 ) {
		printf(" 1 ");
	}
	else {
		printf("   ");
	}
	printf("\n");
	
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_btree_ass_flag( BTREE *btree ) {
#ifdef _DEBUG_PRT_ATPG_
	
	if ( btree->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		if ( ( btree->flag & ASS_N_0 ) &&
			 ( btree->flag & ASS_N_1 ) ) {
			printf(" xx ");
		}
		else {
			printf("    ");
		}
	}
	else {
		/* COND_FAILURE */
		if ( ( btree->flag & ASS_F_0 ) &&
			 ( btree->flag & ASS_F_1 ) ) {
			printf(" xx ");
		}
		else {
			printf("    ");
		}
		printf("   ");
	}
	
#endif /* _DEBUG_PRT_ATPG_ */		
}

void extern inline topgun_print_mes_btree_assign ( BTREE *btree ) {
#ifdef _DEBUG_PRT_ATPG_
	
	printf("ass_list  ");

	if ( btree->line == NULL ) {
		printf(" NULL\n");
	}
	else {
		topgun_print_mes_id2 ( btree->line );
		topgun_print_mes_condition ( btree->condition );
		topgun_print_mes_state3 ( btree->ass_state3 );
		topgun_print_mes_btree_ass_flag ( btree );
		topgun_print_mes_n ();
	}
	
#endif /* _DEBUG_PRT_ATPG_ */			
}

void extern inline topgun_print_mes_btree_just_e( JUST_INFO *just_e ){
#ifdef _DEBUG_PRT_ATPG_
	while ( just_e ) {
		topgun_print_mes_gate_state9_new_lv( just_e->line, 0, just_e->condition, 0 );
		just_e = just_e->next;
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_btree( BTREE *btree ){
#ifdef _DEBUG_PRT_ATPG_
	
	topgun_print_mes_btree_assign ( btree );
	topgun_print_mes_btree_flag ( btree->flag );
#ifndef OLD_IMP2
#else	
	topgun_print_mes_imp_trace ( btree->imp_trace );
#endif /* OLD_IMP2 */	
	topgun_print_mes_btree_just_e ( btree->just_e );

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_all_btree( void ){
#ifdef _DEBUG_PRT_ATPG_
	
	BTREE *btree;
	BTREE *top_btree;
	
	btree = Gene_head.btree;

	while( btree->prev ){
		btree = btree->prev;
	}

	top_btree = btree;

	while ( btree ) {
		topgun_print_mes_btree ( btree );
		btree = btree->next;
	}

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_btree_ass_only( void ){
#ifdef _DEBUG_PRT_ATPG_
	
	BTREE *btree;
	BTREE *top_btree;
	
	btree = Gene_head.btree;

	while( btree->prev ){
		btree = btree->prev;
	}

	top_btree = btree;

	if ( btree == NULL ) {
		;
	}
	else {
		while ( btree ) {
			topgun_print_mes_btree_assign ( btree );
			btree = btree->next;
		}
	}

#endif /* _DEBUG_PRT_ATPG_ */	
}


void extern inline topgun_print_mes_id_pre_state9(LINE *line,  Uint condition )
{
#ifdef _DEBUG_PRT_ATPG_
	
	topgun_print_mes_id(line);
	topgun_print_mes_sp(1);	
	topgun_print_mes_state9_new(line->state9, condition);
	printf(" -> ");
	
#endif /* _DEBUG_PRT_ATPG_ */		
}
void extern inline topgun_print_mes_id_after_state9(LINE *line, Uint condition )
{
#ifdef _DEBUG_PRT_ATPG_
	
	topgun_print_mes_state9_new(line->state9,condition);
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */	
}

/* all function for debug */
void extern inline topgun_print_imp_all( void ){
#ifdef _DEBUG_PRT_ATPG_
#ifdef OLD_IMP	
	Ulong    i = 0;
	IMP_INFO *stk;
	
	stk = Gene_head.imp_l;

	while(stk){
		printf("%ld ID %5ld\n",i++,stk->line->line_id);
		stk = stk->next;
	}
#endif /* OLD_IMP */
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_line_all( void ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong i, j;

	for(i=0;i<Line_info.n_line;i++){
		printf("ID: %5ld tpye %2d n_in %2ld n_out %2ld"
			   ,Line_head[i].line_id,Line_head[i].type
			   ,Line_head[i].n_in,Line_head[i].n_out);
		if(Line_head[i].n_in != 0){
			printf(" in:");
			for(j=0;j<Line_head[i].n_in;j++){
				printf("%ld ",Line_head[i].in[j]->line_id);
			}
		}
		if(Line_head[i].n_out != 0){
			printf(" out:");
			for(j=0;j<Line_head[i].n_out;j++){
				printf("%ld ",Line_head[i].out[j]->line_id);
			}
		}
		printf("\n");
	}
#endif /* _DEBUG_PRT_ATPG_ */		
}


void extern inline topgun_print_mes_sa0_sa1( Ulong info ){
#ifdef _DEBUG_PRT_ATPG_
	
	if ( info & TOPGUN_SA0 ) {
		printf(" 1/0");
	}
	else {
		printf(" 0/1");
	}

#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_atpg_result_pat( F_CLASS result ){
#ifdef _DEBUG_PRT_ATPG_
	
	Ulong i;
	STATE_3 state3;
	char  normal[2]; /* パターンのキャラクタ */
	
	if ( result == GENE_DET_A ) {
		for ( i = 0; i < Line_info.n_pi; i++ ) {

			state3 = atpg_get_state9_2_normal_state3 ( Pi_head[ i ]->state9 );
			atpg_get_chara_stat( state3, normal );
		
			printf( "%s", normal );
			topgun_print_mes_comma();
		}
	}
	
#endif /* _DEBUG_PRT_ATPG_ */		
}

void extern inline topgun_print_mes_atpg_result( F_CLASS f_class ){
#ifdef _DEBUG_PRT_ATPG_
	
	switch ( f_class ) {
	case GENE_DET_A:
	case GENE_DET_S:
		printf( " %s", TOPGUN_OUT_DET );
		break;
	case GENE_RED:
		printf( " %s",TOPGUN_OUT_REDUN );
		break;
	case GENE_ABT:
		printf( " %s",TOPGUN_OUT_ABORT );
		break;
	case GENE_IUN:
		printf( " %s",TOPGUN_OUT_INITUNT );
		break;
	case GENE_UNT:
		printf( " %s",TOPGUN_OUT_UNTEST);
		break;
	default:
		printf( " %s",TOPGUN_OUT_ABORT);
		break;
	}

#endif /* _DEBUG_PRT_ATPG_ */
}
 
void extern inline topgun_print_mes_atpg_result_all( FLIST *fault, F_CLASS f_class ){
#ifdef _DEBUG_PRT_ATPG_
	
	/* ヘッダ */
	printf("### ");
	topgun_print_mes_comma();
	
	/* 対象故障の情報 */
	/* 信号線ID */
	topgun_print_mes_id_only(fault->line);
	topgun_print_mes_comma();
	/* 0/1 or 1/0 */
	topgun_print_mes_sa0_sa1(fault->info);
	topgun_print_mes_comma();

	/* 故障の分類の表示 */
	topgun_print_mes_atpg_result( f_class );
	topgun_print_mes_comma();

	/* back trackの回数 */
	printf("%ld", Gene_head.back_all);
	topgun_print_mes_comma();

	/* 伝搬のback trackの回数 */
	printf("%ld", Gene_head.back_drive);
	topgun_print_mes_comma();

	/* 正当化のback trackの回数 */
	printf("%ld", Gene_head.back_all - Gene_head.back_drive);
	topgun_print_mes_comma();
	
	/* 伝搬したパスで正当化が失敗した回数 */
	printf("%ld", Gene_head.back_drive_just);
	topgun_print_mes_comma();

	topgun_print_mes_sp(1);	
	topgun_print_mes_comma();	

	/* パターンの表示 */
	topgun_print_mes_atpg_result_pat( f_class );

	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */	
}




void extern inline topgun_print_mes_fault_list(){
#ifdef _DEBUG_PRT_ATPG_

	Ulong i,j;
	FLIST *fl;
	LINE  *line;

    char *func_name = "topgun_print_mes_fault_list"; /* 関数名 */

	for ( i = 0; i < Line_info.n_line; i++ ) {
		line = &(Line_head[ i ]);

		fl = line->flist[FSIM_SA0];
		if ( fl != NULL ) {

			printf("o %9ld ", fl->f_id );

			printf("%9ld ", fl->line->line_id );

			if ( fl->info & TOPGUN_SA0 ) {
				printf("%s ", TOPGUN_OUT_SA0 );
			}
			else if ( fl->info & TOPGUN_SA1 ) {
				printf("%s ", TOPGUN_OUT_SA1 );
			}
			else{
				topgun_error( FEC_PRG_FLIST, func_name );
			}
			printf("\n" );

			for ( j = 0 ; j < ( fl->n_eqf - 1 ) ; j++ ) {
				printf("e %9ld\n", fl->eqf[ j ] );
			}

		}
		fl = line->flist[FSIM_SA1];
		if ( fl != NULL ) {
		
			printf("o %9ld ", fl->f_id );

			printf("%9ld ", fl->line->line_id );

			if ( fl->info & TOPGUN_SA0 ) {
				printf("%s ", TOPGUN_OUT_SA0 );
			}
			else if ( fl->info & TOPGUN_SA1 ) {
				printf("%s ", TOPGUN_OUT_SA1 );
			}
			else{
				topgun_error( FEC_PRG_FLIST, func_name );
			}
			printf("\n" );
		 
			for ( j = 0 ; j < ( fl->n_eqf - 1 ) ; j++ ) {
				printf("e %9ld\n", fl->eqf[ j ] );
			}

		}
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_char( char *ch ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("%s",ch);
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_ulong_2( Ulong value ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("%2ld",value );
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_bench( int chapter, int section ){
#ifdef _DEBUG_INPUT_

	switch ( chapter ) {
	case 0:
		printf("BEN %s\n",print_mes_bench_0[section]);
		break;
	case 1:
		printf("BEN %s\n",print_mes_bench_1[section]);
		break;
	case 2:
		printf("BEN %s",print_mes_bench_2[section]);
		break;
	case 3:
		printf("BEN %s",print_mes_bench_3[section]);
		break;
	case 4:
		printf("BEN %s",print_mes_bench_4[section]);
		break;
	case 5:
		printf("BEN %s",print_mes_bench_5[section]);
		break;
	case 6:
		printf("BEN %s",print_mes_bench_6[section]);
		break;
	case 7:
		printf("BEN %s",print_mes_bench_7[section]);
		break;
	}
#endif /*  _DEBUG_INPUT_ */
}

void extern inline topgun_print_mes_bench_char( int chapter, int section, char *mes ){
#ifdef _DEBUG_INPUT_
	topgun_print_mes_bench( chapter, section );
	printf(" %s\n", mes);
#endif /*  _DEBUG_INPUT_ */	
}

void extern inline topgun_print_mes_bench_char_char( int chapter, int section, char *mes1, char *mes2 ){
#ifdef _DEBUG_INPUT_
	topgun_print_mes_bench( chapter, section );
	printf(" %s -> %s\n", mes1, mes2);
#endif /*  _DEBUG_INPUT_ */	
}

void extern inline topgun_print_mes_bench_fanout_info( CELL *stem ) {
#ifdef _DEBUG_INPUT_

	CELL_IO *tmp_out;
	Ulong   i = 1;

	topgun_print_mes_bench_char( 6, 0, stem->name );
	
	tmp_out = stem->out;
	while(tmp_out){
		printf("BEN %ld %s\n",i++,tmp_out->cell->name);
		tmp_out = tmp_out->next;
	}
	
#endif /*  _DEBUG_INPUT_ */	
}

void extern inline topgun_print_mes_bench_fanout_result( CELL *stem) {
#ifdef _DEBUG_INPUT_

	CELL_IO *tmp_out;
	Ulong   i = 1;

	topgun_print_mes_bench_char( 6, 3, stem->name );
	tmp_out = stem->out;
	while(tmp_out){
		printf("BEN %ld %s -> %s\n",i++
			   ,tmp_out->cell->name
			   ,tmp_out->cell->out->cell->name);
		tmp_out = tmp_out->next;
	}
	
#endif /* _DEBUG_INPUT_ */
}

void extern inline topgun_print_mes_tm_con_calc ( LINE *lp ){
#ifdef _DEBUG_PRT_ATPG_

	printf("TM_CON LV %3ld gate %8ld is C0 %8ld C1 %8ld\n",lp->lv_pi, lp->line_id, lp->tm_c0, lp->tm_c1);

#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_obs_calc ( LINE *lp ) {
#ifdef _DEBUG_PRT_ATPG_

	printf("TM_OBS LV %3ld gate %8ld is OBS %8ld\n",lp->lv_pi, lp->line_id,lp->tm_co);

#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_pi ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is PI gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_inv ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is INV gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_in1 ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is in1 gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_and ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is and gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_nand ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is nand gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_or ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is or  gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_nor ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is nor gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_xor ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is xor gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_tm_con_xnor ( LINE *line_tm ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("TM_CON gate %8ld is xnor gate ",line_tm->line_id);
	printf("C0 %2ld C1 %2ld\n", line_tm->tm_c0, line_tm->tm_c1);
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_tm_calc_result ( void ) {
#ifdef _DEBUG_PRT_ATPG_

	Ulong i;
	LINE  *lp;

	for ( i = 0; i < Line_info.n_line; i++ ) {

		lp = &(Line_head[i]);
		
		printf("TM_RES id %5ld pr %2d lv %3ld c0 "
			   ,lp->line_id,lp->type,lp->lv_pi );
		if ( lp->tm_c0 == TM_INF ){
			printf("  +00");
		}
		else {
			printf("%5ld",lp->tm_c0);
		}
		printf(" c1 ");
		if ( lp->tm_c1 == TM_INF ){
			printf("  +00");
		}
		else {
			printf("%5ld",lp->tm_c1);
		}
		printf(" co ");
		if ( lp->tm_c1 == TM_INF ){
			printf("   00\n");
		}
		else {
			printf("%5ld\n",lp->tm_co);
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_lv_calc_result ( LINE *lp ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("LV_CAL id %5ld lv_pi %3ld\n", lp->line_id, lp->lv_pi);
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_n_lv_calc_result ( Ulong lv ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("LV_CAL lv %2ld %5ld\n", lv, Line_info.n_lv_pi[ lv ] );
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_cell_fault ( CELL *cell, LINE *line, Ulong id ) {
#ifdef _DEBUG_PRT_ATPG_
	printf("CELLID %8ld name %20s line_id %8ld f_id %8ld sa0\n", cell->id, cell->name, line->line_id, id );
	printf("CELLID %8ld name %20s line_id %8ld f_id %8ld sa1\n", cell->id, cell->name, line->line_id, id + 1  );
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_next_fault (){
#ifdef _DEBUG_PRT_ATPG_
		printf("==============================================\n");
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_pat_cnt( Ulong p_cnt ){
#ifdef _DEBUG_PRT_ATPG_
	printf( "%8ld: ", p_cnt );
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_pat( char *normal ){
#ifdef _DEBUG_PRT_ATPG_
	printf( "%s", normal );
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_logic_sim_val2_line( LINE *lp ){
#ifdef _DEBUG_PRT_FSIM_
	Ulong i;

	printf("L_SIM LV %3ld ID %8ld "
		   ,lp->lv_pi, lp->line_id );
	topgun_print_mes_type( lp );
	printf("NVAL0 ");
	topgun_print_mes_val2(lp->n_val_a);
	topgun_print_mes_n();
	
	for ( i = 0 ; i < lp->n_in ; i++ ) {
		printf("L_SIM");
		topgun_print_mes_sp(30);
		printf(" IN%2ld ",i);
		topgun_print_mes_val2(lp->in[i]->n_val_a);
		topgun_print_mes_n();
	}
#endif /* _DEBUG_PRT_FSIM_ */
}
void extern inline topgun_print_mes_logic_sim_val3_line( LINE *lp ){
#ifdef _DEBUG_PRT_FSIM_
	Ulong i;

	printf("L_SIM LV %3ld ID %8ld "
		   ,lp->lv_pi, lp->line_id );
	topgun_print_mes_type( lp );
	printf("NVAL0 ");
	topgun_print_mes_val3(lp->n_val_a, lp->n_val_b);
	topgun_print_mes_n();
	
	for ( i = 0 ; i < lp->n_in ; i++ ) {
		printf("L_SIM");
		topgun_print_mes_sp(30);
		printf(" IN%2ld ",i);
		topgun_print_mes_val3(lp->in[i]->n_val_a, lp->in[i]->n_val_b);
		topgun_print_mes_n();
	}
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_val2( Ulong value ){
#ifdef _DEBUG_PRT_FSIM_
	Ulong i ;
	for ( i = 0 ; i < Fsim_head.bit_size ; i++ ) {

		if ( value % 2  == BIT0_ON ) {
			printf("1");
		}
		else {
			printf("0");
		}
		value /= 2;
	}
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_val3( Ulong value_a, Ulong value_b ){
#ifdef _DEBUG_PRT_FSIM_
	Ulong i ;
	for ( i = 0 ; i < Fsim_head.bit_size ; i++ ) {

		if ( value_a % 2  == BIT0_ON ) {
			printf("1");
		}
		else {
			if ( value_b % 2 == BIT0_ON ) {
				printf("0");
			}
			else {
				printf("X");
			}
		}
		value_a /= 2;
		value_b /= 2;
	}
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_fsim_enter_line( LINE *line ){
#ifdef _DEBUG_PRT_FSIM_
	printf("F_SIM Enter ID %8ld LV %3ld ",line->line_id, line->lv_pi);
	topgun_print_mes_type( line );
	topgun_print_mes_n( );
#endif /* _DEBUG_PRT_FSIM_ */
}


void extern inline topgun_print_mes_fsim_state( Ulong times, Ulong max, Ulong n_rest ) {
#ifdef _DEBUG_PRT_FSIM_ 
	printf("F_SIM STATE PAT %8ld / %8ld Fault %8ld\n", times, max, n_rest );
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_fsim_end_state( Ulong max, Ulong n_rest ) {
#ifdef _DEBUG_PRT_FSIM_ 
	printf("F_SIM STATE PAT      End / %8ld Fault %8ld\n", max, n_rest );
#endif /* _DEBUG_PRT_FSIM_ */
}


void extern inline topgun_print_mes_fsim_cannot_enter_line( LINE *line, Ulong flag ){
#ifdef _DEBUG_PRT_FSIM_ 
	printf("F_SIM NotEnter %8ld LV %3ld ",line->line_id, line->lv_pi);
	topgun_print_mes_type( line );
	if ( flag == FSIM_SA0 ) {
		printf("sa0");
	}
	else { 
		printf("sa1");
	}
	topgun_print_mes_n( );
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_fsim_exec_line( LINE *line ){
#ifdef _DEBUG_PRT_FSIM_
	printf("F_SIM Exec  ID %8ld LV %3ld ",line->line_id, line->lv_pi);
	topgun_print_mes_type( line );
	topgun_print_mes_sp( 1 );
	if ( line->n_val_a % 2  == BIT0_ON ) {
		printf("1");
	}
	else {
		printf("0");
	}
	printf(" / ");
	if ( line->f_val_a % 2  == BIT0_ON ) {
		printf("1");
	}
	else {
		printf("0");
	}
	topgun_print_mes_n( );
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_fsim3_exec_line( LINE *line ){
#ifdef _DEBUG_PRT_FSIM_
	printf("F_SIM Exec  ID %8ld LV %3ld ",line->line_id, line->lv_pi);
	topgun_print_mes_type( line );
	topgun_print_mes_sp( 1 );
	if ( ( line->n_val_a % 2  == BIT0_ON ) &&
		 ( ! ( line->n_val_b % 2  == BIT0_ON ) ) ) {
		printf("1");
	}
	else {
		if ( ( ! ( line->n_val_a % 2  == BIT0_ON ) ) &&
			 ( ( line->n_val_b % 2  == BIT0_ON ) ) ) {
			printf("0");
		}		
		else {
			printf("X");
		}
	}
	printf(" / ");
	if ( ( line->f_val_a % 2  == BIT0_ON ) &&
		 ( ! ( line->f_val_b % 2  == BIT0_ON ) ) ) {
		printf("1");
	}
	else {
		if ( ( ! ( line->f_val_a % 2  == BIT0_ON ) ) &&
			 ( ( line->f_val_b % 2  == BIT0_ON ) ) ) {
			printf("0");
		}		
		else {
			printf("X");
		}
	}
	topgun_print_mes_n( );
#endif /* _DEBUG_PRT_FSIM_ */
}


void extern inline topgun_print_mes_fsim_detect( LINE *line, Ulong flag ){
#ifdef _DEBUG_PRT_FSIM_
	printf("F_SIM R_Detect    %8ld ",line->line_id);
	if ( flag == FSIM_SA0 ) {
		printf("sa0");
	}
	else { 
		printf("sa1");
	}
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_fsim_not_detect(LINE *line, Ulong flag){
#ifdef _DEBUG_PRT_FSIM_
	printf("F_SIM R_Notdetect %8ld ",line->line_id);
	if ( flag == FSIM_SA0 ) {
		printf("sa0");
	}
	else { 
		printf("sa1");
	}
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_FSIM_ */
}


void extern inline topgun_print_mes_fsim_not_insert(LINE *line){
#ifdef _DEBUG_PRT_ATPG_
	printf("F_SIM R_NotInsert %8ld",line->line_id);
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_learn( LINE *line ){
#ifdef _DEBUG_PRT_ATPG_

	//topgun_print_mes_learn_state3( line, STATE3_0 );
	//topgun_print_mes_learn_state3( line, STATE3_1 );

#endif /* _DEBUG_PRT_ATPG_ */
}



void extern inline topgun_print_mes_learn_list_result
(
 LINE *ass_line,    
 STATE_3 ass_state3 
 ){
	//#ifdef _DEBUG_PRT_ATPG_
	LEARN_LIST *learn_list = NULL;


	//char *func_name = "topgun_print_mes_learn_list_result"; /* 関数名 */
	
	if ( ass_state3 == STATE3_0 ) {
		learn_list = ass_line->imp_0;
	}
	else {
		learn_list = ass_line->imp_1;
	}
	while ( learn_list ) {
		/* メッセージ出力 */
		if ( ass_state3 == STATE3_0 ) {
			printf("LEARN_RES %8ld   state0 ",ass_line->line_id);
			if ( learn_list->ass_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",learn_list->line->line_id);
			}
			else {
				printf("-> %8ld state1\n",learn_list->line->line_id);
			}
		}
		else {
			printf("LEARN_RES %8ld   state1 ",ass_line->line_id);
			if ( learn_list->ass_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",learn_list->line->line_id);
			}
			else {
				printf("-> %8ld state1\n",learn_list->line->line_id);
			}
		}
		
		learn_list = learn_list->next;
	}
	//#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_learn_state3( LINE *check_line, STATE_3 check_state3, LINE *ass_line, STATE_3 invert_state3 ){
#ifdef _DEBUG_PRT_ATPG_

	if ( check_state3 == STATE3_0 ) {
		printf("LEARN     %8ld   state0 ",check_line->line_id);
		if ( invert_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",ass_line->line_id);
		}
		else {
			printf("-> %8ld state1\n",ass_line->line_id);
		}
	}
	else {
		printf("LEARN     %8ld   state1 ",check_line->line_id);
		if ( invert_state3 == STATE3_0 ) {
			printf("-> %8ld state0\n",ass_line->line_id);
		}
		else {
			printf("-> %8ld state1\n",ass_line->line_id);
		}
	}
	
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_cut_learn_state3( LINE *check_line, STATE_3 check_state3, LINE *ass_line, STATE_3 ass_state3 ){
#ifdef _DEBUG_PRT_ATPG_

	if ( ass_state3 == STATE3_0 ) {
		printf("LEARN_CUT %8ld   state0 ",ass_line->line_id);
		if ( check_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",check_line->line_id);
		}
		else {
			printf("-> %8ld state1\n",check_line->line_id);
		}
	}
	else {
		printf("LEARN_CUT %8ld   state1 ",ass_line->line_id);
		if ( check_state3 == STATE3_0 ) {
			printf("-> %8ld state0\n",check_line->line_id);
		}
		else {
			printf("-> %8ld state1\n",check_line->line_id);
		}
	}
	
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_line_stack( LINE *check_line, STATE_3 check_state3, LINE *ass_line, STATE_3 invert_state3 ){
#ifdef _DEBUG_PRT_ATPG_

	if ( check_state3 == STATE3_0 ) {
		printf("LEARN     %8ld   state0 ",check_line->line_id);
		if ( invert_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",ass_line->line_id);
		}
		else {
			printf("-> %8ld state1\n",ass_line->line_id);
		}
	}
	else {
		printf("LEARN     %8ld   state1 ",check_line->line_id);
		if ( invert_state3 == STATE3_0 ) {
			printf("-> %8ld state0\n",ass_line->line_id);
		}
		else {
			printf("-> %8ld state1\n",ass_line->line_id);
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

/*!
  @brief  Line_stackの表示
  @param [in] *line_stack
  @return Void
*/


void extern inline topgun_print_mes_uniq_learn_list
(
 LEARN_LIST *org_learn_list
){
#if _DEBUG_PRT_ATPG_ || _DEBUG_PRT_ATPG_UNIQ_
	LEARN_LIST *learn_list = org_learn_list;
	
	while ( learn_list ) {

		printf("UNIQ_RES ");
		topgun_print_mes_id ( learn_list->line );
		topgun_print_mes_n();
			
		learn_list = learn_list->next;
	}
#endif /* _DEBUG_PRT_ATPG_ */
}


void extern inline topgun_print_mes_compare_ulong
( Ulong from,
  Ulong to
  ){
#ifdef _DEBUG_PRT_ATPG_
	if ( from < to ) {
		printf("<");
	}else if ( from > to ) {
		printf(">");
	}
	else {
		printf("=");
	}
		
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_reflesh_normal
(
 Ulong last,
  Ulong cnt
  ){
#ifdef _DEBUG_PRT_ATPG_
	printf("reflesh normal  last %3ld for %3ld times\n", last, cnt );
#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_reflesh_failure
(
 Ulong last,
 Ulong cnt
  ){
#ifdef _DEBUG_PRT_ATPG_
	printf("reflesh failure last %3ld for %3ld times\n", last, cnt );
#endif /* _DEBUG_PRT_ATPG_ */
}



void extern inline topgun_print_mes_fsim_input_pattern( Ulong N_tp, Ulong cnt ){
#ifdef _DEBUG_PRT_FSIM_

	Ulong i,j;
	
	printf("F_SIM RP ");
	
	for ( i = 0 ; i < N_tp ; i++ ) {
		printf(" %5ld : ",cnt);
		for ( j = 0 ; j < Line_info.n_pi ; j++ ) {
			if ( Tp_head[ i ][ j ] == STATE_0_CODE ) {
				printf("0");
			}
			else {
				printf("1");
			}
		}
		printf("\n");
	}
#endif /* _DEBUG_PRT_FSIM_ */
}


void extern inline topgun_print_mes_fsim_start( ) {
#ifdef _DEBUG_PRT_FSIM_
	printf("# F_SIM REST FAULT    %8ld ->\n", Line_info.n_fault_atpg_count );
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_fsim_end( ) {
#ifdef _DEBUG_PRT_FSIM_
	printf("# F_SIM REST FAULT -> %8ld\n", Line_info.n_fault_atpg_count );
#endif /* _DEBUG_PRT_FSIM_ */
}

void extern inline topgun_print_mes_compact_pattern( Ulong *pattern_a, Ulong *pattern_b, Ulong cnt, Ulong master ) {
#ifdef _DEBUG_PRT_COMPACT_

	Ulong i; /* 外部入力のカウンタ */
	Ulong shift_count = 0;
	Ulong width_count = 0; 

	if ( cnt == master ) {
		printf("# COMPA GENE  :");
	}
	else if ( ( cnt + 1) == master ) {
		printf("# COMPA    -> :");
	}
	else {
		printf("# COMPA %5ld :",cnt);
	}

	for ( i = 0 ; i < Line_info.n_pi ; i++, shift_count++ ) {
		if ( shift_count == Fsim_head.bit_size ) {
			shift_count = 0;
			width_count++;
		}

		if ( pattern_a[ width_count ] & ( BIT_1_ON << shift_count ) ){
			printf("1");
		}
		else if ( pattern_b[ width_count ] & ( BIT_1_ON << shift_count ) ){
			printf("0");
		}
		else {
			printf("X");
		}
	}
#endif /* _DEBUG_PRT_COMPACT_ */
}

void extern inline topgun_print_mes_compact_posibbility( Ulong result ) {
#ifdef _DEBUG_PRT_COMPACT_
	if ( result == COMPACT_OK ) {
		printf(" OK \n");
	}
	else {
		printf(" NG \n");
	}
#endif /* _DEBUG_PRT_COMPACT_ */
}



void extern inline topgun_print_mes_min_check ( Ulong pat, Ulong buf) {
#ifdef _DEBUG_PRT_COMPACT_
	if ( buf < pat ) {
		printf("# COMPA buf %8ld <  pat %8ld -> compact pattern out\n",buf,pat);
	}
	else {
		printf("# COMPA buf %8ld >= pat %8ld -> generate pattern out\n",buf,pat);
	}
#endif /* _DEBUG_PRT_COMPACT_ */
}

void extern inline topgun_print_mes_n_compact ( void ) {
#ifdef _DEBUG_PRT_COMPACT_
	printf("\n");
#endif /* _DEBUG_PRT_COMPACT_ */
}

void extern inline topgun_print_mes_compact_count_x_in_buf ( Ulong buffer_id, Ulong num_x_state) {
#ifdef _DEBUG_PRT_COMPACT_
	printf("# COMPA id %3ld x %4ld\n",buffer_id,num_x_state);
#endif /* _DEBUG_PRT_COMPACT_ */
}

void extern inline topgun_print_mes_compact_count_x_in_pat ( Ulong num_x_state) {
#ifdef _DEBUG_PRT_COMPACT_
	printf("# COMPA new     x %4ld\n",num_x_state);
#endif /* _DEBUG_PRT_COMPACT_ */
}

	
void topgun_print_mes_opening( ) {

	fprintf( stdout, "******************************************************************\n");
	fprintf( stdout, "                              TOPGUN                              \n");
	fprintf( stdout, "                                                                  \n");
	fprintf( stdout, "                All Right Reversed, Copyright                     \n");
        fprintf( stdout, "           (C) Yoshimura Lab.,Kyoto Sangyo University             \n");
	fprintf( stdout, "******************************************************************\n");

     
}

void topgun_print_mes_process_time
(
 PROCESS_TIME *operate,
 char *name,
 Ulong level
 ) {
	signed long space = 0;
	size_t length = strlen( name );
	Ulong i,j;

	if ( level == 0 ) {
		fprintf( stdout, "TTT %-16s ",name);
	}
	else {
		fprintf( stdout, "TTT ");
		for ( i = 1; i < level ; i++ ) {
			fprintf( stdout, "  ");
		}
		fprintf( stdout, "+ %s ",name);
		space = 16 - (level * 2 ) - length ;
		if ( space > 0 ) {
			for ( j = 0; j < space ; j++ ) {
				fprintf( stdout, " ");
			}
		}
	}
	fprintf( stdout, " %11.3f ",operate->total.user);
	fprintf( stdout, " %11.3f ",operate->total.system);
	fprintf( stdout, " %11.3f ",operate->total.real);
	fprintf( stdout, "(sec) / %12ld\n",operate->count);
}

void topgun_print_mes_process_time_all
(
 void
 ) {
	fprintf(stdout,"TTT ============================================================================\n");
	fprintf(stdout,"TTT process name            user        system          real      /        count\n");
	fprintf(stdout,"TTT ----------------------------------------------------------------------------\n");	
	topgun_print_mes_process_time ( &Time_head.total, "total", 0);
	fprintf(stdout,"TTT ----------------------------------------------------------------------------\n");	
	topgun_print_mes_process_time ( &Time_head.main_initialize, "initialize", 0);
	fprintf(stdout,"TTT   --------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.learn, "learn",1);
	fprintf(stdout,"TTT     ------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.learn_init, "initial",2);
	topgun_print_mes_process_time ( &Time_head.learn_dynamic_resort, "resort",2);
	topgun_print_mes_process_time ( &Time_head.learn_direct_node, "d_node",3);
	topgun_print_mes_process_time ( &Time_head.learn_upflag, "upflag",4);
	topgun_print_mes_process_time ( &Time_head.learn_downflag, "downflag",4);
	topgun_print_mes_process_time ( &Time_head.learn_imp, "impli",4);
	topgun_print_mes_process_time ( &Time_head.learn_loop_check, "loop ck",4);
	topgun_print_mes_process_time ( &Time_head.learn_loop_mk, "make",5);
	topgun_print_mes_process_time ( &Time_head.learn_loop_ope, "loop ope",4);
	topgun_print_mes_process_time ( &Time_head.learn_no_reach, "no reach",4);
	topgun_print_mes_process_time ( &Time_head.learn_contra, "contra",4);
	topgun_print_mes_process_time ( &Time_head.learn_add_br, "add br",4);
	fprintf(stdout,"TTT     ------------------------------------------------------------------------\n");	
	topgun_print_mes_process_time ( &Time_head.learn_search, "search",4);
	topgun_print_mes_process_time ( &Time_head.learn_finish, "clearn",4);
	fprintf(stdout,"TTT ----------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.atpg, "atpg", 0);
	fprintf(stdout,"TTT   --------------------------------------------------------------------------\n");	
	topgun_print_mes_process_time ( &Time_head.generate, "generate",1);
	fprintf(stdout,"TTT     ------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.gene_det, "gene_det",2);
	topgun_print_mes_process_time ( &Time_head.gene_redun, "gene_redun",2);
	topgun_print_mes_process_time ( &Time_head.gene_abort, "gene_abort",2);
	fprintf(stdout,"TTT     ------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.select, "select",2);
	topgun_print_mes_process_time ( &Time_head.insert_fault, "insert",2);
	topgun_print_mes_process_time ( &Time_head.check_gene, "check end",2);
	topgun_print_mes_process_time ( &Time_head.propagate, "propagate",2);
	topgun_print_mes_process_time ( &Time_head.justified, "justified",2);
	topgun_print_mes_process_time ( &Time_head.justified_spt, "just_spt",3);
	topgun_print_mes_process_time ( &Time_head.back_track, "back track",2);
	topgun_print_mes_process_time ( &Time_head.reflesh, "reflesh",2);
	fprintf(stdout,"TTT     ------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.implication, "implication",2);
	fprintf(stdout,"TTT   --------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.compact, "compaction", 1);
	fprintf(stdout,"TTT   --------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.sim, "simulation",1);
	fprintf(stdout,"TTT     ------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.logic_sim, "logic sim",2);
	topgun_print_mes_process_time ( &Time_head.fault_sim, "fault sim",2);
	fprintf(stdout,"TTT     ------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.random_sim, "random sim",2);
	topgun_print_mes_process_time ( &Time_head.sim_val3, "f_sim val3",2);
	topgun_print_mes_process_time ( &Time_head.sim_val2, "f_sim val2",2);
	fprintf(stdout,"TTT ----------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.stock_sim, "stock",2);
	fprintf(stdout,"TTT ----------------------------------------------------------------------------\n");
	topgun_print_mes_process_time ( &Time_head.output, "output",0);
	fprintf(stdout,"TTT ============================================================================\n");

}

void extern inline topgun_print_mes_process_time_random_pattern
(
 void
 ){
	
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifdef NOUSE
void extern inline topgun_print_mes_result_X( STATE_9 state9 ){
#ifdef _DEBUG_PRT_ATPG_
	topgun_print_mes_sp(24);	
	topgun_print_mes_state9(state9);
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_ATPG_ */	
}

void extern inline topgun_print_mes_enter_X(LINE *line, Ulong stat ){
#ifdef _DEBUG_PRT_ATPG_

	topgun_print_mes(24,6);
	topgun_print_mes_sp(5);
	topgun_print_mes_id(line);
	topgun_print_mes_state9(line->state9);
	if(stat & IMP_FLG_FRONT){
		printf(" F");
	}
	if(stat & IMP_FLG_BACK){
		printf(" B");
	}
	topgun_print_mes_n();

#endif /* _DEBUG_PRT_ATPG_ */
}

void extern inline topgun_print_mes_result_Z(int line_id){
#ifdef _DEBUG_PRT_ATPG_
	printf(" --> entry D-frontia   line_id %5d\n",line_id);
#endif /* _DEBUG_PRT_ATPG_ */
}
void extern inline topgun_print_mes_fp_iap_on(LINE *line){

	topgun_print_mes_sp(1);
	topgun_print_mes_id(line);
}


#endif /* NOUSE */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
