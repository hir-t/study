/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ATPGの終了/未終了を確認する関数
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#include <stdio.h>
#include "topgun.h"
#include "topgunState.h" /* STATE_3 , STATE_9 */
#include "topgunLine.h"  /* LINE_INFO, LINE, LINE_TYPE */
#include "topgunAtpg.h"
#include "topgunTime.h"
#include "topgunError.h" /* FEC_XX */

extern	LINE_INFO Line_info;
extern  GENE_HEAD Gene_head;

/* function list */
GENE_RESULT atpg_check_finish_generate ( Ulong );

/* topgun_propagate.c */
extern int  atpg_check_finish_propagate();

/* topgun_justified.c */
extern int  atpg_check_finish_justified();


/* test function */
CONDITION   test_state9_n_f( STATE_9 , char * );

/********************************************************************************
 * << Function >>
 *		check termination conditions of atpg
 *
 * << Function ID >>
 *	   	7-1
 *
 * << Return >>
 *	ATPG_CONT	: continue
 *	ATPG_NOFLT	: condition of no fault
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
 *		2004/12/13
 *
 ********************************************************************************/

int check_end
(
 void
){
	void topgun_print_mes();

	/* debug print function */
	topgun_print_mes( 2, 1 );

	/* ATPGの終了条件を確認する */
	/* 現在は残り対象故障が0の場合のみ */

    if ( Line_info.n_fault_atpg_count == 0 ) {

		/* 残りATPG対象故障がないため終了 */

		/* debug print function */
		topgun_print_mes( 0, 0 );

		return( ATPG_NOFLT );
    }

	/* 残りATPG対象故障がまだあるから継続 */
	/* debug print function */
    topgun_print_mes( 0, 1 );

    return( ATPG_CONT );
}


/********************************************************************************
 * << Function >>
 *		check termination of generate pattern
 *
 * << Function ID >>
 *	   	7-3
 *
 * << Return >>
 * args			(I/O)	type	description
 * result        O      int     statement (以下の何れか)
 *                              JUSTI_END
 *                              JUSTI_KEEP
 *                              PROPA_CONT
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stat          I      Ulong   line stat
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/8/24
 *
 ********************************************************************************/

GENE_RESULT atpg_check_finish_generate
(
 Ulong count_back
 ){
	GENE_RESULT  result;

	char *func_name = "atpg_check_finish_generate"; /* 関数名 */

	topgun_4_utility_enter_start_time ( &Time_head.check_gene );

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-3-1 故障値が外部出力まで伝搬したか?                          %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if ( count_back >= Gene_head.limit_back ) {
		/* back trackの上限値の為終了 */
		return ( LIM_BACK_END );
	}

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-3-1 故障値が外部出力まで伝搬したか?                          %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	result = atpg_check_finish_propagate();

	switch ( result ) {

	case PROPA_CONT:
		break;
	case PROPA_PASS:
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-3-3-2 未正当化信号線は存在するか?                          %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		result = atpg_check_finish_justified();
		break;
	default:
		topgun_error( FEC_PRG_JUDGE_GENE_END, func_name );
	}

	topgun_4_utility_enter_end_time ( &Time_head.check_gene );

	return( result );
}

/* End of File */

/********************************************************************************
 * << Function >>
 *		check state with normal or failure state
 *
 * << Function ID >>
 *	   	7-?
 *
 * << Return >>
 *     STAT_N : normal  state
 *     STAT_F : failure state
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stat          I      Ulong   line stat for checked
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/13
 *
 ********************************************************************************/

CONDITION atpg_check_state9_n_f
(
 STATE_9 state9
 ){

	static char *func_name = "atpg_check_state_n_f"; 	/* 関数名 */

	test_state9_n_f( state9, func_name );

	if ( state9 < STATE9_X0 ) {
		return ( COND_NORMAL );
	}
	else {
		return ( COND_FAILURE ); 
	}
}

