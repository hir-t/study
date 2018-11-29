/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 信号線の状態を扱う関数群
	
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
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h" /* FEC_XX */
#include "topgunTest.h"  /* topgun_test_xxx() */ 

extern	LINE_INFO	Line_info;
extern	LINE		*Line_head;

static IMP_STATE Judge_ass44[4][4] =
/* state\assign  X         0         1         Y   */
/* X */ {{IMP_KEEP, IMP_UPDA, IMP_UPDA, IMP_CONF},
/* 0 */  {IMP_KEEP, IMP_KEEP, IMP_CONF, IMP_CONF},
/* 1 */  {IMP_KEEP, IMP_CONF, IMP_KEEP, IMP_CONF},
/* Y */  {IMP_KEEP, IMP_CONF, IMP_CONF, IMP_CONF}};

#ifdef TOPGUN_NOUSE
static int Judge_ass[16][16] =
/* stat\ass   XX        0X        1X     UX     X0        00        10     U0     X1        01        11     U1     XU     0U    1U      UU */
/* XX */ {{ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_KEEP, ERROR,    ERROR, ERROR, ERROR,    ASS_KEEP, ERROR, ERROR, ERROR, ERROR, ERROR},
/* 0X */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_KEEP, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* 1X */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_CONF, ERROR,    ERROR, ERROR, ERROR,    ASS_KEEP, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* UX */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_CONF, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* X0 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_KEEP, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR},
/* 00 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_KEEP, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* 10 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_CONF, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* U0 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_CONF, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* X1 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_KEEP, ERROR,    ERROR, ERROR, ERROR,    ASS_KEEP, ERROR, ERROR, ERROR, ERROR, ERROR},
/* 01 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_KEEP, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* 11 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_CONF, ERROR,    ERROR, ERROR, ERROR,    ASS_KEEP, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* U1 */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ASS_CONF, ERROR,    ERROR, ERROR, ERROR,    ASS_CONF, ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* XU */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR, ERROR, ERROR},
/* 0U */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* 1U */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR, ERROR, ERROR}, 
/* UU */  {ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR,    ERROR,    ERROR, ERROR, ERROR, ERROR, ERROR}}; 
#endif /* TOPGUN_NOUSE */

#ifdef NOUSE
static Ulong No_failure_stat[16] =
           /* XX */ { STATE9_XX,
		   /* 0X */   STATE9_00,
		   /* 1X */   STAT_11,
		   /* UX */   STAT_UU,
		   /* X0 */   STATE9_00,
		   /* 00 */   STATE9_00,
		   /* 10 */   STAT_CF,
		   /* U0 */   STAT_CF,
		   /* X1 */   STAT_11,
		   /* 01 */   STAT_CF,
		   /* 11 */   STAT_11,
		   /* U1 */   STAT_CF,
		   /* XU */   STAT_UU,
		   /* 0U */   STAT_CF,
		   /* 1U */   STAT_CF,
		   /* UU */   STAT_UU };
#endif /* NOUSE */

static Ulong Reflesh_stat[ COND_NORMAL+1 ] =
	{ STATE9_XX,	/* no use  0 */
	  STATE9_UX,  /* STAT_F  1 */
	  STATE9_XX,  /* no use  2 */
	  STATE9_XX,  /* no use  3 */
	  STATE9_XU   /* STAT_N  4 */
	};

static INDIRECT_IMP_INFO Indirect_imp_info;


/* function list */

/* original function */
IMP_RESULT gene_enter_state ( ASS_LIST *, Ulong );
void    atpg_reflesh_state( LINE *, Uint );
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9値から正常値を取り出す */
STATE_3 atpg_get_state9_2_failure_state3 ( STATE_9 ); /* 9値から故障値を取り出す */
void    atpg_get_chara_stat ( STATE_3 , char * );
STATE_9 atpg_state3_with_condition_2_state9( STATE_3, Uint );/* 3値からconditionによって9値を取り出す */

/* topgun_state.hのマクロ */
//STATE_3 atpg_get_state9_with_condition_2_state3( STATE_9, Uint );


int     atpg_check_state_n_f( Ulong ); /* stateが正常値か故障値かを判断する */
void  atpg_enter_implication_list_input ( LINE * , STATE_3, Uint, Ulong, Ulong ); /* 入力方向への含意操作を含意操作リストへ登録する */
void  atpg_enter_implication_list_output (  LINE *, STATE_3, Uint, Ulong ); /* 出力方向へ含意操作リストを登録する */
int   atpg_check_ent_stat( ASS_LIST *);
Ulong atpg_imp_make_enter_stat ( Ulong, Uint );
IMP_RESULT atpg_update_static_learn_state ( LEARN_LIST * );
IMP_RESULT atpg_update_indirect_state( LEARN_LIST *, Ulong );
static IMP_RESULT gene_check_enter_state ( ASS_LIST * );
static IMP_RESULT gene_check_enter_state_normal( LEARN_LIST * );
static LEARN_LIST *gene_copy_static_learn_state( LEARN_LIST * );
static ASS_LIST *gene_make_learn_list_2_ass_list( LEARN_LIST * );
static void gene_enter_list_by_static_learn( LEARN_LIST * );
static LEARN_LIST *gene_get_list_by_static_learn( void );

void gene_learn_list_initalize( void ); /* 間接含意による含意用 */

/* call function */
void  atpg_enter_imp_trace ( LINE *, Uint );
Ulong atpg_normal_failure_stat( Ulong, Ulong );
void  gene_update_state ( LINE *, STATE_3, Uint );
Uint  atpg_reverse_condition ( Uint );
LEARN_LIST *utility_combine_learn_list ( LEARN_LIST *, LEARN_LIST * );
ASS_LIST *utility_combine_ass_list ( ASS_LIST *, ASS_LIST * );
void gene_reflesh_imp_list( void );

extern void (* Switch_enter_imp_trace[ TOPGUN_NUM_COND ] )( LINE *);

/* print function */
void topgun_print_mes ( Ulong, Ulong );
void topgun_print_mes_n ();
void topgun_print_mes_condition ( Uint );
void topgun_print_mes_id_pre_state9( LINE *, Uint );
void topgun_print_mes_id_after_state9( LINE *, Uint );
void topgun_print_mes_id_type_state9( LINE * );
void topgun_print_mes_gate_info ( LINE * );
void topgun_print_mes_gate_state9_new ( LINE *, Ulong, Ulong, Ulong );
void topgun_print_mes_gate_info_learn( void );
void topgun_print_mes_gate_state9_line( LINE * );
void topgun_print_mes_gate_state9_learn ( LEARN_LIST * );

/* test function */
void  test_state9_n_f ( STATE_9, char * );
void  test_state9_n_f_x ( STATE_9, char * );
void  test_state_normal ( STATE_3, char * );
void  test_state_failure ( Ulong, char * );
void  test_line_null ( LINE *, char * );
void  test_condition ( Uint, char * );
void  test_ass_list ( ASS_LIST *, char * );

/********************************************************************************
 * << Function >>
 *		get state at normal
 *
 * << Function ID >>
 *    	17-2
 *
 * << Return >>
 *	stat		: state at normal
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stat          I		Ulong	line state
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/14
 *
 ********************************************************************************/

/* normal mode */
STATE_3 atpg_get_state9_2_normal_state3
(
 STATE_9 state9
 ){
	STATE_3 state3;

	state3 = state9 & STATE9_NO;

	return( state3 );
}

/********************************************************************************
 * << Function >>
 *		get state at failure
 *
 * << Function ID >>
 *    	17-3
 *
 * << Return >>
 *	stat		: state at failure
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stat          I		Ulong	line state
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/14
 *
 ********************************************************************************/


/* failure to normal with 2bit right shift */
STATE_3 atpg_get_state9_2_failure_state3
(
 STATE_9 state9
 ){
	STATE_3 state3;

	state3 = ( ( state9 / 4 ) & STATE9_NO )  ;
	return( state3 ) ;
}

/********************************************************************************
 * << Function >>
 *		get state at normal/failure by flag
 *
 * << Function ID >>
 *    	17-?(no use)
 *
 * << Return >>
 *	stat		: state at normal/failure
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stat          I		Ulong	line state
 * flag          I		int     normal(4) or failure(1)
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/2
 *
 ********************************************************************************/


/* failure to normal with 2bit right shift */
/* topgun_state.hのマクロ */
/*
STATE_3 atpg_get_state9_with_condition_2_state3
(
 STATE_9 state9,
 Uint condition
 ){
	STATE_3 state3;

	state3 = ( ( ( state9 * condition ) / 4 ) & STATE9_NO )  ;

	return( state3 );
}
*/
/* condition */
/* failure 1 */
/* normal  4 */


/********************************************************************************
 * << Function >>
 *		make state at normal/failure by flag
 *
 * << Function ID >>
 *    	17-?(no use)
 *
 * << Return >>
 *	stat		: state at normal/failure
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stat          I		Ulong	line state
 * flag          I		int     normal(4, STAT_N) or failure(1, STAT_F)
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/2
 *
 ********************************************************************************/


STATE_9 extern inline atpg_get_state3_with_condition_2_state9
(
 STATE_3 state3,
 Uint condition
 ){
	STATE_9 state9;

	state9 = ( ( state3 * 4 ) / condition ) ;
	return( state9 );
}

/* flag      */
/* failure 1 STAT_F */
/* normal  4 STAT_N */


/********************************************************************************
 * << Function >>
 *		exchange state to character
 *
 * << Function ID >>
 *    	17-10
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stat          I      Ulong   state of line
 * moji          I/O    char *  state of character
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/14
 *
 ********************************************************************************/

void atpg_get_chara_stat
(
 STATE_3 state3,
 char  moji[]
 ){

	char *func_name = "atpg_get_char_stat"; /* 関数名 */

	switch ( state3 ) {
	case STATE3_0:
		strcpy( moji, "0" );
		break;
	case STATE3_1:
		strcpy( moji, "1" );
		break;
	case STATE3_X:
		strcpy( moji, "X" );
		break;
	case STATE3_U:
		strcpy( moji, "U" );
		break;
	default:
		topgun_error( FEC_PRG_LINE_STAT, func_name );
	}
}

/* failure to normal with 2bit right shift */
Ulong atpg_get_common_stat
(
 Ulong state
 ){
	return( ( state * 4 ) | state );
}

/********************************************************************************
 * << Function >>
 *		entry state on line
 *
 * << Function ID >>
 *    	17-5
 *
 * << Return >>
 *	ASS_CONF	: assign conflict (= IMP_CONF)
 *	ASS_KEEP	: normal end (!= ASS_CONF)
 *
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * ass_list      I      ASS_LIST *  assign list
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/01/19
 *		2007/06/04  imp_id 
 *
 ********************************************************************************/

/* visio 2006/04/24 */

IMP_RESULT gene_enter_state
(
 ASS_LIST *ass_list,
 Ulong max_imp_id
 ){

	LINE     *current_line = NULL;
	CONDITION current_condition = COND_UNKNOWN;
	CONDITION reverse_condition = COND_UNKNOWN;
	ASS_LIST  *add_list = NULL;
	ASS_LIST  *top_ass_list = ass_list;
	
	STATE_3  current_state3 = STATE3_C;
	STATE_3  r_state3 = STATE3_C;
	
	IMP_RESULT imp_result;
	IMP_RESULT final_result;

	//char  *func_name = "atpg_enter_state"; /* 関数名 */

	/* 値設定前にすべて考慮されている->しない */
	//test_ass_list ( ass_list, func_name );


	// initail imp_result & final_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;
	final_result.i_state = IMP_KEEP;
	final_result.imp_id  = 0;
	

	// 設定されたすべての値に対して
	while ( ass_list ) {

		/* 1設定された値 */
		current_line      = ass_list->line;
		current_condition = ass_list->condition;
		current_state3    = ass_list->ass_state3;
		
		if ( ! ( current_line->flag & FV_ASS ) ) {
			/* 故障値が設定できない */
			/* 正常値として値を設定 */
			if ( current_condition == COND_FAILURE ){
				topgun_print_mes ( 17, 0 );
				topgun_print_mes_n ();
			}
			current_condition = COND_NORMAL;
		}
		
		/* 値設定による衝突をチェックする */
		imp_result = gene_check_enter_state ( ass_list );
		
		if ( IMP_CONF == imp_result.i_state ) {
			final_result.i_state = IMP_CONF;

			if ( current_condition == COND_NORMAL ) {
				final_result.imp_id = ass_list->line->imp_id_n;
			}
			else {
				final_result.imp_id = ass_list->line->imp_id_f;
			}

			/* Gene_head.imp_f & imp_l を開放する */
			break;

		}
		else if ( IMP_KEEP == imp_result.i_state ) {
			;
		}
		else {
			/* 更新する場合 */
			final_result.i_state = IMP_UPDA;

			/* state の更新 */
			gene_update_state( current_line, current_state3, current_condition );

			/* 値更新に伴う含意操作を登録する */
		
			/* imp_infoへの登録 */
			/* 入力側へ */
			atpg_enter_implication_list_input
				( current_line, current_state3, current_condition, OUT_PIN_NUM, max_imp_id );
		
			/* 出力側へ */
			atpg_enter_implication_list_output
				( current_line, current_state3, current_condition, max_imp_id );

			/* assgignはあとで戻せるので */
			Switch_enter_imp_trace[ current_condition ] ( current_line );

			if ( current_condition == COND_NORMAL ) {
				/* 値設定のときに間接含意をする */		
				/* 2006/04/21追加 */
				if ( current_state3 == STATE3_0 ) {
					/* ass_listに連結する */
					if ( current_line->imp_0 != NULL ) {
						topgun_print_mes( 24, 9 );
						topgun_print_mes_gate_info_learn();
						topgun_print_mes_gate_state9_line( current_line );
						add_list = gene_make_learn_list_2_ass_list ( current_line->imp_0 );
						ass_list = utility_combine_ass_list ( ass_list, add_list );
					}
				}
				else {
					if ( current_line->imp_1 != NULL ) {
						topgun_print_mes( 24, 9 );
						topgun_print_mes_gate_info_learn();
						topgun_print_mes_gate_state9_line( current_line );
						add_list = gene_make_learn_list_2_ass_list ( current_line->imp_1 );
						ass_list = utility_combine_ass_list ( ass_list, add_list );
					}
				}
			}
		
			if ( current_line->flag & FV_ASS ) {

				if ( ! ( current_line->flag & FP_IAP ) ) {

					/* 2. 故障値は設定できるが故障伝搬経路でない場合、逆値が設定できる */

					reverse_condition = atpg_reverse_condition ( current_condition );

					/* $$$ r_condition とass_state3で作成可能 $$$ */
					r_state3 = atpg_get_state9_with_condition_2_state3
						( current_line->state9, reverse_condition );
					if ( STATE3_X == r_state3 ) {
						/* 逆の値はXである場合、逆の値も一緒に更新 */
						/* ass_listに追加する */
						topgun_print_mes ( 17, 1 );
						topgun_print_mes_condition ( reverse_condition );
						topgun_print_mes_n ( );

						gene_update_state
							( current_line, current_state3, reverse_condition );
						atpg_enter_implication_list_input
							( current_line, current_state3, reverse_condition, OUT_PIN_NUM, max_imp_id );
						atpg_enter_implication_list_output
							( current_line, current_state3, reverse_condition, max_imp_id );
						Switch_enter_imp_trace[ reverse_condition ] ( current_line );

						/*
						  ass_reverse = ( ASS_LIST * )topgunMalloc
						  ( FMT_ASS_LIST, sizeof( ASS_LIST ), 1, func_name );

						  ass_reverse->ass_state3 = ass_list->ass_state3;
						  ass_reverse->condition  = reverse_condition;
						  ass_reverse->line       = ass_line;
						  ass_reverse->next       = ass_list->next;
						  ass_list->next          = ass_reverse;
						*/

						/* 逆値が設定できた場合は、必ず正常値を含む */
						if ( reverse_condition == COND_NORMAL ) {
							/* 2006/04/21追加 */
							if ( current_state3 == STATE3_0 ) {
								/* ass_listに連結する */
								if ( current_line->imp_0 != NULL ) {
									topgun_print_mes( 24, 9 );
									topgun_print_mes_gate_info_learn();
									topgun_print_mes_gate_state9_line( current_line );
									add_list = gene_make_learn_list_2_ass_list ( current_line->imp_0 );
									ass_list = utility_combine_ass_list ( ass_list, add_list );
								}
							}
							else {
								if ( current_line->imp_1 != NULL ) {
									topgun_print_mes( 24, 9 );
									topgun_print_mes_gate_info_learn();
									topgun_print_mes_gate_state9_line( current_line );
									add_list = gene_make_learn_list_2_ass_list ( current_line->imp_1 );
									ass_list = utility_combine_ass_list ( ass_list, add_list );
								}
							}
						}
					}
				}
			}
		}
		ass_list = ass_list->next;
	}

	ass_list = top_ass_list;
	/* 様子見
	while ( ass_list ) {

		top_ass_list = ass_list;
		ass_list = ass_list->next;

		topgunFree ( tmp_ass_list , FMT_ASS_LIST, 1, func_name );
	}
	*/
	return( final_result );
}

/********************************************************************************
 * << Function >>
 *		update state on line
 *
 * << Function ID >>
 *    	17-?
 *
 * << Return >>
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * line          I/O    LINE *      line updated of state
 * state         I      Ulong       update state
 * condition     I      Uint        normal or failure flag
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

void gene_update_state
(
 LINE    *line,
 STATE_3 state3,
 Uint    condition
 ){
	STATE_9 state9;
	
	/* char  *func_name = "atpg_update_state"; *//* 関数名 */

	topgun_test_state_normal ( state3, func_name );
	topgun_test_state9_n_f_x ( line->state9, func_name );

	/* X/? と ?/X (?は0 or 1 or U)の組み合わせのみなので|演算でOK */
	/* Xは00でコード化されている */
	state9 = atpg_get_state3_with_condition_2_state9( state3, condition );
	line->state9 |= state9;
}

/********************************************************************************
 * << Function >>
 *		entry state on line
 *
 * << Function ID >>
 *    	17-5
 *
 * << Return >>
 *	ASS_CONF	: assign conflict (= IMP_CONF)
 *	ASS_KEEP	: normal end (!= ASS_CONF)
 *
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * ass_list      I      ASS_LIST *  assign list
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

IMP_RESULT atpg_update_indirect_state
(
 LEARN_LIST *org_learn_list,
 Ulong max_imp_id
 ){

	LINE     *current_line;
	STATE_3  current_state3;
	STATE_3  r_state3;
	LEARN_LIST *learn_list = NULL;
	IMP_RESULT imp_result;

	//char  *func_name = "atpg_update_indirect_state"; /* 関数名 */

	/* ほとんどenter_state */
	/* conditionがnormal限定 */

	//printf("update_indirect_state\n");

	Indirect_imp_info.enter_id = 0;
	Indirect_imp_info.get_id = 0;

	//printf("UIS enter %lu\n",Indirect_imp_info.enter_id);
	//printf("UIS get   %lu\n",Indirect_imp_info.get_id);
	
	/* enterして、getしても同じなので直接入力する */
	learn_list = org_learn_list;

	// initail imp_result
	imp_result.i_state = IMP_UPDA;
	imp_result.imp_id  = 0;

	while ( learn_list ) {
		while ( learn_list ) {

			topgun_print_mes_gate_state9_learn ( learn_list );

			/* 値設定による衝突をチェックする */
			imp_result = gene_check_enter_state_normal ( learn_list );

			if ( IMP_CONF == imp_result.i_state ) {
				
				topgun_print_mes( 24, 3 );
				return ( imp_result );
			}
			else if ( IMP_KEEP == imp_result.i_state ) {

				learn_list = learn_list->next;
				topgun_print_mes( 24, 4 );
			}
			else {
				topgun_print_mes( 24, 5 );
				
				/* 衝突しない場合 */
				/* 1. conditionに設定された値 */
				current_line      = learn_list->line;
				current_state3    = learn_list->ass_state3;
		
				learn_list = learn_list->next;

				/* state の更新 */
				gene_update_state( current_line, current_state3, COND_NORMAL );

				/* 値更新に伴う含意操作を登録する */
				/* imp_infoへの登録 */
				/* 入力側へ */
				atpg_enter_implication_list_input
					( current_line, current_state3, COND_NORMAL, OUT_PIN_NUM, max_imp_id );
		
				/* 出力側へ */
				atpg_enter_implication_list_output
					( current_line, current_state3, COND_NORMAL, max_imp_id );

#ifndef OLD_IMP2
				Switch_enter_imp_trace[ COND_NORMAL ] ( current_line );
#else		
				atpg_enter_imp_trace ( current_line, COND_NORMAL );
#endif /* OLD_IMP2 */			

				if ( current_line->flag & FV_ASS ) {

					if ( ! ( current_line->flag & FP_IAP ) ) {

						/* 2. 故障値は設定できるが故障伝搬経路でない場合、逆値が設定できる */
						r_state3 = atpg_get_state9_2_failure_state3	( current_line->state9 );
				
						if ( STATE3_X == r_state3 ) {
							/* 逆の値はXである場合、逆の値も一緒に更新 */
							/* ass_listに追加する */
							topgun_print_mes ( 17, 1 );
							topgun_print_mes_condition ( COND_FAILURE );
							topgun_print_mes_n ( );

							gene_update_state
								( current_line, current_state3, COND_FAILURE );
							atpg_enter_implication_list_input
								( current_line, current_state3, COND_FAILURE, OUT_PIN_NUM, max_imp_id );
							atpg_enter_implication_list_output
								( current_line, current_state3, COND_FAILURE, max_imp_id );
                            
							Switch_enter_imp_trace[ COND_FAILURE ] ( current_line );
						}
					}
				}
				
				/* 間接含意のリストをコピーする */
				if ( current_state3 == STATE3_0 ) {
					if ( ( current_line->imp_0 ) != NULL ){
						gene_enter_list_by_static_learn ( current_line->imp_0 );
						//printf("UIS enter %lu\n",Indirect_imp_info.enter_id);
					}
				}
				else {
					if ( ( current_line->imp_1 ) != NULL ){
						gene_enter_list_by_static_learn ( current_line->imp_1 );
						//printf("UIS enter %lu\n",Indirect_imp_info.enter_id);
					}
				}
			}
		}
		learn_list = gene_get_list_by_static_learn();
		//printf("UIS get   %lu\n",Indirect_imp_info.get_id);
	}
	
	imp_result.i_state = IMP_UPDA;
	return( imp_result );
}



IMP_RESULT atpg_update_indirect_state_old
(
 LEARN_LIST *org_learn_list,
 Ulong max_imp_id
 ){

	LINE     *current_line;
	STATE_3  current_state3;
	STATE_3  r_state3;
	LEARN_LIST *learn_list = NULL;
	LEARN_LIST *free_learn_list = NULL;
	LEARN_LIST *add_list = NULL;
	IMP_RESULT imp_result;

	char  *func_name = "atpg_update_indirect_state"; /* 関数名 */

	/* ほとんどenter_state */
	/* conditionがnormal限定 */

	// initail imp_result
	imp_result.i_state = IMP_UPDA;
	imp_result.imp_id = 0;

	learn_list = gene_copy_static_learn_state ( org_learn_list );
		
	while ( learn_list ) {

			topgun_print_mes_gate_state9_learn ( learn_list );

			/* 値設定による衝突をチェックする */
			imp_result = gene_check_enter_state_normal ( learn_list );

			free_learn_list = learn_list;
		
		if ( IMP_CONF == imp_result.i_state ) {
			topgun_print_mes( 24, 3 );
			while ( learn_list ) {
				free_learn_list = learn_list;
				learn_list = learn_list->next;
				topgunFree( free_learn_list, FMT_LEARN_LIST, 1, func_name );
			}
			return ( imp_result );
		}
		else if ( IMP_KEEP == imp_result.i_state ) {

			learn_list = learn_list->next;
			topgunFree( free_learn_list, FMT_LEARN_LIST, 1, func_name );
			topgun_print_mes( 24, 4 );
		}
		else {
			topgun_print_mes( 24, 5 );
				
			/* 衝突しない場合 */
			/* 1. conditionに設定された値 */
			current_line      = learn_list->line;
			current_state3    = learn_list->ass_state3;
		
			learn_list = learn_list->next;
			topgunFree( free_learn_list, FMT_LEARN_LIST, 1, func_name );

			/* state の更新 */
			gene_update_state( current_line, current_state3, COND_NORMAL );

			/* 値更新に伴う含意操作を登録する */
			/* imp_infoへの登録 */
			/* 入力側へ */
			atpg_enter_implication_list_input
				( current_line, current_state3, COND_NORMAL, OUT_PIN_NUM, max_imp_id );
		
			/* 出力側へ */
			atpg_enter_implication_list_output
				( current_line, current_state3, COND_NORMAL, max_imp_id );

			Switch_enter_imp_trace[ COND_NORMAL ] ( current_line );

			if ( current_line->flag & FV_ASS ) {

				if ( ! ( current_line->flag & FP_IAP ) ) {

					/* 2. 故障値は設定できるが故障伝搬経路でない場合、逆値が設定できる */
					r_state3 = atpg_get_state9_2_failure_state3	( current_line->state9 );
				
					if ( STATE3_X == r_state3 ) {
						/* 逆の値はXである場合、逆の値も一緒に更新 */
						/* ass_listに追加する */
						topgun_print_mes ( 17, 1 );
						topgun_print_mes_condition ( COND_FAILURE );
						topgun_print_mes_n ( );

						gene_update_state
							( current_line, current_state3, COND_FAILURE );
						atpg_enter_implication_list_input
							( current_line, current_state3, COND_FAILURE, OUT_PIN_NUM, max_imp_id );
						atpg_enter_implication_list_output
							( current_line, current_state3, COND_FAILURE, max_imp_id );
                        
						Switch_enter_imp_trace[ COND_FAILURE ] ( current_line );
					}
				}
			}
			
			/* 間接含意のリストをコピーする */
			if ( current_state3 == STATE3_0 ) {
				add_list = gene_copy_static_learn_state ( current_line->imp_0 );
			}
			else {
				add_list = gene_copy_static_learn_state ( current_line->imp_1 );
			}
			/* learn_listに連結する */
			learn_list = utility_combine_learn_list ( learn_list, add_list );
		}
	}
	imp_result.i_state = IMP_UPDA;
	return( imp_result );
}

/********************************************************************************
 * << Function >>
 *		check state for assigned state
 *
 * << Function ID >>
 *    	17-7
 *
 * << Return >>
 *	ASS_CONF	: assign conflict
 *	ASS_KEEP	: normal end
 *
 * << Argument >>
 * args			(I/O)	type		description
 * ass_lst		I		ASS_LIST *	assign list
 *
 * << extern >>
 * name			(I/O)	type		description
 * Jugde_ass44   I      int[][]     judge result for assignment state
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

#ifdef NOUSE
int atpg_check_ent_stat
(
 ASS_LIST *ass_list
 ){

	Ulong org_stat; /* assignする前の値 */
	Ulong ass_stat; /* assignする値 */
	int   result;     /* checkした結果 */

	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();

	/* 表引きで対応 2005/6/27 */
	org_stat = ass_list->line->stat;
	ass_stat = atpg_state3_with_condition_2_state9 ( ass_list->ass_stat, ass_list->condition  );

	result = Judge_ass44
		[ atpg_get_normal_stat( org_stat ) ]
		[ atpg_get_normal_stat( ass_stat ) ];

	if ( result == ASS_CONF ) {
		return( ASS_CONF );
	}
	result = Judge_ass44
		[ atpg_get_failure_stat ( org_stat ) ]
		[ atpg_get_failure_stat ( ass_stat ) ];

	return(result);

}
#endif /* NOUSE */


/********************************************************************************
 * << Function >>
 *		generate normal & normal state
 *
 * << Function ID >>
 *    	17-3
 *
 * << Return >>
 *	state		: state at normal & normal
 *
 * << Argument >>
 * args			(I/O)	type		description
 * stat          I      Ulong       line state
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

/* stat -> normal & normal */
Ulong atpg_get_normal_normal_stat
(
 Ulong stat
 ){
	return( ( stat & STATE9_NO ) * 5 );
}

/********************************************************************************
 * << Function >>
 *		generate failure & failure state
 *
 * << Function ID >>
 *    	17-4
 *
 * << Return >>
 *	state		: state at normal & normal
 *
 * << Argument >>
 * args			(I/O)	type		description
 * stat          I      int	        line state
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

/* failure stat -> failure & failure */
Ulong atpg_get_failure_failure_stat
(
 Ulong stat
 ){
	return( ( stat / 4 ) * 5 );
}

/********************************************************************************
 * << Function >>
 *		check state for D-Drive
 *
 * << Function ID >>
 *    	17-4
 *
 * << Return >>
 *	DRIVE_IMPOS     : D-Drive IMPOSsible
 *	DRIVE_POINT		: D-Drive Path
 *  DRIVE_CONT      : assign POINT for D-Drive
 *
 * << Argument >>
 * args			(I/O)	type		description
 * stat          I      Ulong       line state
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

GENE_RESULT atpg_get_d_state9
(
 STATE_9 state9
 ){
	/* $$$ Uが入った場合は要注意 $$$ */
	switch( state9 ){
	case STATE9_D:
	case STATE9_ND:
		/* 故障が伝搬している */
		return( DRIVE_POINT );
	case STATE9_00:
	case STATE9_11:
		/* 故障が伝搬できない */
		return( DRIVE_IMPOS );
	default:
	/* その他 0/X, 1/X, X/0, X/1, X/X */
		return( DRIVE_CONT );
	}
}

/********************************************************************************
 * << Function >>
 *		reflesh state
 *
 * << Function ID >>
 *    	17-8
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type		description
 * stat          I      Ulong       reflesh state
 * line          I      LINE *      reflesh line pointer
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

void atpg_reflesh_state
(
 LINE *line,
 Uint condition
 ){
	char *func_name = "atpg_reflesh_state"; /* 関数名 */

	test_line_null ( line, func_name );
	test_condition ( condition, func_name );

	topgun_print_mes_id_pre_state9 ( line, condition );
	
	line->state9 &= Reflesh_stat[ condition ];

	topgun_print_mes_id_after_state9 ( line, condition );

}

/********************************************************************************
 * << Function >>
 *		check equivalence normal and fault on stat
 *
 * << Function ID >>
 *    	17-9
 *
 * << Return >>
 *	0           : normal state != failure state
 *	1           : noraml state == failure state
 *
 * << Argument >>
 * args			(I/O)	type		description
 * stat          I      Ulong       line state
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/5/31
 *		2005/7/22 : reform(誤り修正)
 *
 ********************************************************************************/

Ulong atpg_check_fault_stat
(
 Ulong stat
 ){
	/* 0 : other  */
	/* 1 : 0(X/X), 5(0/0), 10(1/1), 15(U/U) */
	
	return( 0 == ( stat % 5 ) );
}




/********************************************************************************
 * << Function >>
 *		make state noraml and failure value
 *
 * << Function ID >>
 *    	17-12
 *
 * << Return >>
 *	state		: state has both normal and failure
 *
 * << Argument >>
 * args			(I/O)	type		description
 * n_value       I      Ulong       state of normal
 * f_value       I      Ulong       state of failure
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/13
 *
 ********************************************************************************/

Ulong atpg_normal_failure_stat
(
 Ulong n_value,
 Ulong f_value
 ){
	return( n_value + ( f_value * 4 ) );
}

/********************************************************************************
 * << Function >>
 *		make state noraml and failure value
 *
 * << Function ID >>
 *    	17-12
 *
 * << Return >>
 *	state		: state has both normal and failure
 *
 * << Argument >>
 * args			(I/O)	type		description
 * n_value       I      Ulong       state of normal
 * f_value       I      Ulong       state of failure
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/13
 *
 ********************************************************************************/
#ifdef NOUSE
Ulong atpg_no_failure_stat
(
 Ulong value
 ){
	Ulong r_value;
	char  *func_name = "atpg_no_failure_stat"; /* 関数名 */
	
	r_value = No_failure_stat[ value ];

	
	
#ifndef TOPGUN_TEST	
	if ( STAT_CF == r_value ) {
		topgun_error( FEC_PRG_LINE_STAT, func_name );
	}
#endif /* TOPGUN_TEST */
	
	return( r_value );
}
#endif /* NOUSE */


/********************************************************************************
 * << Function >>
 *		make invert state 
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	inv_stat	: invert state ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * stat          I      Ulong       input state 
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/17
 *
 ********************************************************************************/

STATE_3 atpg_invert_state3
(
 STATE_3 state3
 ){
	STATE_3 invert3;
	
	Ulong tmp_t = state3 * 2;
	Ulong tmp_d = state3 / 2;
	
	/* $$$ Uの時課題 $$$ */
	/* if文は使いたくないし */

	invert3 = ( ( tmp_t | tmp_d ) & STATE9_NO );
	
	return( invert3 );
}

/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

Uint atpg_reverse_condition
(
Uint condition
 ){

	char  *func_name = "atpg_reverse_condition"; /* 関数名 */
	/* if文は使いたくないし */
	test_condition((condition  ^ COND_REVERSE ), func_name );
	
	return( (condition  ^ COND_REVERSE ) );
}



/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

IMP_RESULT gene_check_enter_state
(
 ASS_LIST *ass_list
 ){

	STATE_3 org_state3;
	IMP_RESULT imp_result;

	// initail imp_reult
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id = 0;

	/* 間接含意が随時追加されるので一つずつ衝突をチェックする */
	
	/* 信号線のstate3を作る */
	org_state3 = atpg_get_state9_with_condition_2_state3
		( ass_list->line->state9, ass_list->condition );

	/* 表でチェックする */
	imp_result.i_state = Judge_ass44[ org_state3 ][ ass_list->ass_state3 ];

	// IMP_CONFの時しかチェックしないと仮定するとif文を外せる
	if ( imp_result.i_state == IMP_CONF ) {
		imp_result.imp_id = ass_list->line->imp_id_f;
	}
	
	return ( imp_result );
}

/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

IMP_RESULT gene_check_enter_state_normal
(
 LEARN_LIST *ass_list
 ){

	STATE_3 org_state3;
	IMP_RESULT imp_result;

	// initail imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;

	/* 間接含意が随時追加されるので一つずつ衝突をチェックする */
	
	/* 信号線のstate3を作る */
	org_state3 = atpg_get_state9_2_normal_state3( ass_list->line->state9 );

	/* 表でチェックする */
	imp_result.i_state = Judge_ass44[ org_state3 ][ ass_list->ass_state3 ];
	// IMP_CONFの時しかチェックしないと仮定するとif文を外せる
	if ( imp_result.i_state == IMP_CONF ) {
		imp_result.imp_id = ass_list->line->imp_id_n;
	}

	return ( imp_result );
}

/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

LEARN_LIST *gene_copy_static_learn_state
(
 LEARN_LIST *learn_list
 ){

	LEARN_LIST *tmp_list = NULL;
	LEARN_LIST *return_list = NULL;
	
	char  *func_name = "gene_copy_static_learn_state"; /* 関数名 */
	
	while ( learn_list ) {

		tmp_list = ( LEARN_LIST * )topgunMalloc
			( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );

		tmp_list->line       = learn_list->line;
		tmp_list->ass_state3 = learn_list->ass_state3;
		tmp_list->next       = return_list;
		return_list          = tmp_list;

		learn_list = learn_list->next;

	}

	return ( return_list );
}

/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

ASS_LIST *gene_make_learn_list_2_ass_list
(
 LEARN_LIST *learn_list
 ){

	ASS_LIST *tmp_list = NULL;
	ASS_LIST *return_list = NULL;
	
	char  *func_name = "gene_make_learn_list_2_ass_list"; /* 関数名 */

	while ( learn_list ) {

		topgun_print_mes_gate_state9_learn ( learn_list );

		tmp_list = ( ASS_LIST * )topgunMalloc
			( FMT_ASS_LIST, sizeof( ASS_LIST ), 1, func_name );

		tmp_list->line       = learn_list->line;
		tmp_list->condition  = COND_NORMAL;
		tmp_list->ass_state3 = learn_list->ass_state3;
		
		tmp_list->next       = return_list;
		return_list          = tmp_list;

		learn_list = learn_list->next;
	}

	return ( return_list );
}


/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

void gene_learn_list_initalize
(
 void
 ){
	Ulong  i  = 0;
	
	char  *func_name = "gene_learn_list_initialzie"; /* 関数名 */

	Indirect_imp_info.list_size = INDIRECT_IMP_LIST_SIZE;
	Indirect_imp_info.enter_id  = 0;
	Indirect_imp_info.get_id    = 0;
	Indirect_imp_info.next      = NULL;


	Indirect_imp_info.l_list = ( LEARN_LIST ** )topgunMalloc
		( FMT_LEARN_LIST_P, sizeof( LEARN_LIST * ),
		  Indirect_imp_info.list_size, func_name );


	for ( i = 0 ; i < Indirect_imp_info.list_size ; i++ ) {
		Indirect_imp_info.l_list[ i ] = NULL;
	}
}

/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

void gene_enter_list_by_static_learn
(
 LEARN_LIST *learn_list
 ){
	//char  *func_name = "gene_enter_list_by_static_learn"; // 関数名 
	Ulong  enter_id  = Indirect_imp_info.enter_id++; /* リストの全体の番号 */
	Ulong  list_size = Indirect_imp_info.list_size;  /* リスと一つ分の大きさ */

	if ( enter_id > list_size ) {
		/* とりあえず */
		//exit(0);
	}

	Indirect_imp_info.l_list[ enter_id ] = learn_list;
	
}

/********************************************************************************
 * << Function >>
 *		make invert condition
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	reverse condition: ( STAT_XとのXOR)
 *
 * << Argument >>
 * args			(I/O)	type		description
 * condition     I      Uint        input condition
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/26
 *
 ********************************************************************************/

 LEARN_LIST *gene_get_list_by_static_learn
(
 void
 ){
	//char  *func_name = "gene_get_list_by_static_learn"; /* 関数名 */
	 Ulong  get_id    = Indirect_imp_info.get_id++; /* リストの全体の番号 */
	 Ulong  enter_id  = Indirect_imp_info.enter_id; /* リストの全体の番号 */

	if ( get_id == enter_id ) {
		return ( NULL ) ;
	}
	else {
		return ( Indirect_imp_info.l_list[ get_id ]  );
	}
}



/********************************************************************************
 * << Function >>
 *		make state value for implication with state and conditon 
 *
 * << Function ID >>
 *    	17-??
 *
 * << Return >>
 *	state		: state has both normal and failure
 *
 * << Argument >>
 * args			(I/O)	type		description
 * stat          I      Ulong       state 
 * condtion      I      int         normal or failure
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/14
 *
 ********************************************************************************/
#ifdef NOUSE
Ulong atpg_imp_make_enter_stat
(
 Ulong stat,
 Uint  condition
 ){

	Ulong atpg_normal_failure_stat();
	
	static char *func_name = "atpg_get_one_side_stat"; /* 関数名 */

	test_condition ( condition, func_name );
	test_state_normal ( stat, func_name );
	
	switch ( condition ) {
	case STAT_N:
		return(	atpg_normal_failure_stat( stat, STAT_X ) );
		
	case STAT_F:
		return( atpg_normal_failure_stat( STAT_X, stat ) );
	}
	return ( STAT_CF );
}
#endif /* NOUSE */
/* End of File */

