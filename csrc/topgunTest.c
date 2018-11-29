/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief debug用に変数の状態をチェックする扱う関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunState.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunCell.h"
#include "topgunFlist.h"  /* FLIST */
#include "topgunMemory.h"
#include "topgunError.h" /* FEC_XX */

extern	LINE_INFO Line_info;
extern	LINE      *Line_head;
extern  JUST_INFO **Lv_head;

void test_line_flag ( char * ); /* line->flagがすべて0か調べる */

void topgun_error( int, char * ); /* error出力関数 */
void topgun_error_o( int, char *, char * ); /* error出力関数 */
void topgun_error_type(  TOPGUN_MEMORY_TYPE_ID, char * ); /* error出力関数 */
void test_imp_trace ( IMP_TRACE *, char * );
void test_state9_n_f( STATE_9 , char * );
extern void test_flist( FLIST *, char * );


extern void test_null_variable( void *, TOPGUN_MEMORY_TYPE_ID, char *);

void topgun_print_mes_id_type_state9( LINE * );


/********************************************************************************
 * << Function >>
 *		line pointer NULL check
 *
 * << Function ID >>
 *	   	T-1
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void extern inline test_line_null
(
 LINE *line,
 char *org_function_name
){
#ifndef TOPGUN_TEST	
	char     *func_name = "test_line_null"; /* 関数名 */

	if ( line == NULL ) {
		topgun_error_o( FEC_CIRCUIT_NO_LINE, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line pointer NULL check
 *
 * << Function ID >>
 *	   	T-L-2
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void extern inline test_line_in_null
(
 LINE *line,
 char *org_function_name
){
	
#ifndef TOPGUN_TEST
	Ulong    i   = 0;    /* lineの入力数カウンタ */
	char     *func_name = "test_line_in_null"; /* 関数名 */

	if ( line == NULL ) {
		topgun_error_o( FEC_CIRCUIT_NO_LINE, func_name, org_function_name );
	}
	
	for ( i = 0; i < line->n_in ; i++ ) {
		if ( line->in[ i ] == NULL ) {
			topgun_error_o( FEC_CIRCUIT_NO_LINE, func_name, org_function_name );
		}
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line pointer NULL check
 *
 * << Function ID >>
 *	   	T-L-2
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/16
 *
 ********************************************************************************/

void extern inline test_line_out_null
(
 LINE *line,
 char *org_function_name
){
	
#ifndef TOPGUN_TEST
	Ulong    i   = 0;    /* lineの入力数カウンタ */
	char     *func_name = "test_line_in_null"; /* 関数名 */

	if ( line == NULL ) {
		topgun_error_o( FEC_CIRCUIT_NO_LINE, func_name, org_function_name );
	}
	
	for ( i = 0; i < line->n_out ; i++ ) {
		if ( line->out[ i ] == NULL ) {
			topgun_error_o( FEC_CIRCUIT_NO_LINE, func_name, org_function_name );
		}
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line gate type check
 *
 * << Function ID >>
 *	   	T-L-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_line_type
(
 LINE *line,
 char *org_function_name
){
	
#ifndef TOPGUN_TEST
	char     *func_name = "test_line_check"; /* 関数名 */

	if ( ! ( line->type < TOPGUN_NUM_PRIM ) ) {
		topgun_error_o( FEC_PRG_LINE_TYPE, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line gate type check for only one type
 *
 * << Function ID >>
 *	   	T-L-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * line              I  LINE *      line pointer for NULL test
 * type              I  LINE_TYPE   line gate type
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/2
 *
 ********************************************************************************/

void test_line_one_type
(
 LINE *line,
 LINE_TYPE type,
 char *org_function_name
){
#ifndef TOPGUN_TEST
	char     *func_name = "test_line_one_type"; /* 関数名 */

	if ( line->type != type ) {
		topgun_error_o( FEC_PRG_LINE_TYPE, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line flag JUSTIFIY_NO or JUSTIFIY_FL
 *
 * << Function ID >>
 *	   	T-L-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * line              I  LNE *       line pointer
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/21
 *
 ********************************************************************************/

void test_line_flag
(
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_line_flag"; /* 関数名 */

	Ulong i; /* 信号線数 */
	Ulong n_flag; /* フラグがたっている信号線数 */

	void topgun_print_mes_id_type_stat();
	void topgun_print_mes_id_type_flag();

	n_flag = 0;

	for( i = 0; i < Line_info.n_line; i++ ) {
		if ( Line_head[i].flag != 0 ) {
			n_flag++;
			topgun_print_mes_id_type_flag( &Line_head[ i ] );
		}
	}
	if( n_flag != 0 ) {
		topgun_error_o( FEC_PRG_LINE_FLAG, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line flag JUSTIFIY_NO or JUSTIFIY_FL
 *
 * << Function ID >>
 *	   	T-L-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * line              I  LNE *       line pointer
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/21
 *
 ********************************************************************************/

void test_line_flag_justify
(
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_line_flag_justify"; /* 関数名 */

	Ulong i; /* 信号線数 */
	Ulong n_flag; /* フラグがたっている信号線数 */

	void topgun_print_mes_id_type_stat();
	void topgun_print_mes_id_type_flag();

	n_flag = 0;

	for( i = 0; i < Line_info.n_line; i++ ) {
		if ( ( Line_head[i].flag & JUSTIFY_NO )  ||
			 ( Line_head[i].flag & JUSTIFY_FL ) ) {
			n_flag++;
			topgun_print_mes_id_type_flag( &Line_head[ i ] );
		}
	}
	if( n_flag != 0 ) {
		topgun_error_o( FEC_PRG_LINE_FLAG, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		size check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * small           I    Ulong       small value
 * big             I    Ulong       big value
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/03/13
 *
 ********************************************************************************/

void test_ulong_small_big
(
 Ulong small,
 Ulong big,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_ulong_small_big"; /* 関数名 */

	if( small > big ) {
		topgun_error_o( FEC_PRG_LINE_FLAG, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}


/********************************************************************************
 * << Function >>
 *		line state only normal or only failure check
 *
 * << Function ID >>
 *	   	T-L-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_state9_n_f
(
 STATE_9 state9,
 char *org_function_name
){
	
#ifndef TOPGUN_TEST
	char     *func_name = "test_line_state9_n_f"; /* 関数名 */

	switch ( state9 ) {
	case STATE9_0X:
	case STATE9_1X:
	case STATE9_UX:
	case STATE9_X0:
	case STATE9_X1:
	case STATE9_XU:
		break;
	default:
		topgun_error_o( FEC_PRG_LINE_STAT, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line state only normal or only failure check or all X state
 *
 * << Function ID >>
 *	   	T-L-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/20
 *
 ********************************************************************************/

void test_state9_n_f_x
(
 STATE_9 state9,
 char *org_function_name
){
	
#ifndef TOPGUN_TEST
	char     *func_name = "test_line_state_n_f_x"; /* 関数名 */

	switch ( state9 ) {
	case STATE9_XX:
	case STATE9_0X:
	case STATE9_1X:
	case STATE9_UX:
	case STATE9_X0:
	case STATE9_X1:
	case STATE9_XU:
		break;
	default:
		topgun_error_o( FEC_PRG_LINE_STAT, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line state D/ND check
 *
 * << Function ID >>
 *	   	T-S-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * state             I  Ulong   line state for D/ND test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_state_d_nd
(
 STATE_9 state9,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	char     *func_name = "test_state_d_nd"; /* 関数名 */

	if ( !( ( state9 == STATE9_D  ) ^
			( state9 == STATE9_ND ) ) ) {
		topgun_error_o( FEC_PRG_LINE_STAT, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line state normal check
 *
 * << Function ID >>
 *	   	T-S-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * state             I  Ulong   line state for D/ND test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_state_normal
(
 STATE_3 state3,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	char     *func_name = "test_state_normal"; /* 関数名 */

	if ( state3 > STATE3_U ) {
		topgun_error_o( FEC_PRG_LINE_STAT, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line state failure check
 *
 * << Function ID >>
 *	   	T-S-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * state             I  Ulong   line state for D/ND test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_state_failure
(
 Ulong state,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	char     *func_name = "test_state_failure"; /* 関数名 */

	switch ( state ) {
	case STATE9_X0:
	case STATE9_X1:
	case STATE9_XU:
		break;
	default:
		topgun_error_o( FEC_PRG_LINE_STAT, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line state normal check
 *
 * << Function ID >>
 *	   	T-S-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * state             I  Ulong   line state for D/ND test
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_state3
(
 STATE_3 state3,
 char    *org_function_name
){
	
#ifndef TOPGUN_TEST
	char     *func_name = "test_state3"; /* 関数名 */

	if ( state3 > STATE3_U ) {
		topgun_error_o( FEC_PRG_LINE_STAT, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		condition check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * condition         I  Uint    STAT_N or STAT_F
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_condition
(
 Uint  condition,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_condition"; /* 関数名 */

	if ( ! ( ( condition == COND_NORMAL ) ^ ( condition == COND_FAILURE ) ) ) {
		topgun_error_o( FEC_PRG_IMPLICATION, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		condition check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	description
 * condition         I  Uint    STAT_N or STAT_F
 * org_function_name I  char *  function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/19
 *
 ********************************************************************************/

void test_flist_info_fault
(
 Ulong info,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_flist_info_fault"; /* 関数名 */

	if ( ! ( ( info & TOPGUN_SA0 ) ^ ( info & TOPGUN_SA1 ) ) ) {
		topgun_error_o( FEC_PRG_FLIST, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		assign list check ( ASS_LIST )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * ass_list        I    ASS_LIST*   assign list
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/21
 *
 ********************************************************************************/

void test_ass_list
(
 ASS_LIST *ass_list,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	/* static char *func_name = "test_ass_list"; *//* 関数名 */

	while ( ass_list ) {
		test_line_null ( ass_list->line, org_function_name );
		test_state_normal ( ass_list->ass_state3, org_function_name );
		test_condition ( ass_list->condition, org_function_name );
		ass_list = ass_list->next ;
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		assign list next null check ( ASS_LIST )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * ass_list        I    ASS_LIST*   assign list
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/4
 *
 ********************************************************************************/

void test_ass_list_next_null
(
 ASS_LIST *ass_list,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	 static char *func_name = "test_ass_list_next_null"; /* 関数名 */

	if ( ass_list->next != NULL ) {
		topgun_error_o( FEC_PRG_ASS_LIST, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		assign list check ( ASS_LIST )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * ass_list        I    ASS_LIST*   assign list
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/21
 *
 ********************************************************************************/

void test_dfront
(
 DFRONT *dfront,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_dfront"; /* 関数名 */

	if ( dfront == NULL ) {
		topgun_error_o( FEC_PRG_DFRONT, func_name, org_function_name );
	}
#ifndef OLD_IMP2
#else	
	test_imp_trace ( dfront->imp_trace, org_function_name );
#endif /* OLD_IMP2 */	
	test_ass_list ( dfront->ass_list, org_function_name );

#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		assign list check ( ASS_LIST )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * dfront            I  DFRONT *    dfrontier tree
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/21
 *
 ********************************************************************************/

void test_dfront_next
(
 DFRONT *dfront,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	Ulong i;

	test_dfront ( dfront, org_function_name );

	for ( i = 0 ; i < dfront->line->n_out ; i++ ) {
		test_dfront ( dfront->next[ i ], org_function_name ); 
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		assign list check ( ASS_LIST )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * dfront            I  DFRONT *    dfrontier tree
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/21
 *
 ********************************************************************************/

void test_dfront_next_null
(
 DFRONT *dfront,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	Ulong i;
	static char *func_name = "test_dfront_next_null"; /* 関数名 */

	if ( dfront == NULL ) {
		topgun_error_o( FEC_PRG_DFRONT, func_name, org_function_name );
	}
	for ( i = 0 ; i < dfront->line->n_out ; i++ ) {
		if ( dfront->next[ i ] == NULL ) {
			topgun_error_o( FEC_PRG_DFRONT, func_name, org_function_name );
		}
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		imp trace infomation check ( IMP_TRACE )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * imp_trace         I  IMP_TRACE * implication back trace infomation
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/23
 *
 ********************************************************************************/

void test_imp_trace
(
 IMP_TRACE *imp_trace,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_imp_trace"; /* 関数名 */

	if ( imp_trace == NULL ) {
		topgun_error_o( FEC_PRG_IMPLICATION, func_name, org_function_name );
	}
	test_line_null ( imp_trace->line, org_function_name );
	test_condition ( imp_trace->condition, org_function_name );

#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		imp trace infomation check ( IMP_TRACE )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * flist             I  FLIST *     fault infomation
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/24
 *
 ********************************************************************************/

void test_flist
(
 FLIST *flist,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST

	static char *func_name = "test_flist"; /* 関数名 */

	if ( flist == NULL ) {

		printf("null??\n");
		
		topgun_error_o( FEC_PRG_FLIST, func_name, org_function_name );
	}
	if ( flist->f_id >= Line_info.n_fault ) {
		
		printf("sonnnan arahen !?\n");
		
		topgun_error_o( FEC_PRG_FLIST, func_name, org_function_name );
	}
	if ( ! ( ( flist->info & TOPGUN_SA0 ) ^ ( flist->info & TOPGUN_SA1 )  ) ) {

		printf("hen yade !?\n");
		topgun_error_o( FEC_PRG_FLIST, func_name, org_function_name );
	}
	test_line_null ( flist->line, org_function_name );
#endif /* TOPGUN_TEST */
}


/********************************************************************************
 * << Function >>
 *		btree infomation check ( BTREE )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * btree             I  BTREE *     justified infomation
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/10/24
 *
 ********************************************************************************/

void test_btree_null
(
 BTREE *btree,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST

	static char *func_name = "test_btree_null"; /* 関数名 */

	if ( btree == NULL ) {
		topgun_error_o( FEC_PRG_BTREE, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		Lv_head check ( JUST_INFO )
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type	    description
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Lv_head       I      JUST_INFO**
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/09
 *
 ********************************************************************************/

void test_lv_head_null
(
 char  *org_function_name
){
#ifndef TOPGUN_TEST

	Ulong  i;
	
	static char *func_name = "test_lv_head_null"; /* 関数名 */

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		if ( Lv_head[ i ] != NULL ) {
			topgun_error_o( FEC_PRG_LV_HEAD, func_name, org_function_name );
		}
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		just_info check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * just_info         I  JUST_INFO * justified infomation
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/09
 *
 ********************************************************************************/

void test_just_info_null
(
 JUST_INFO *just_info,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	static char *func_name = "test_just_info_null"; /* 関数名 */
	
	if ( just_info == NULL ) {
		topgun_error_o( FEC_PRG_JUST_INFO, func_name, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		just_info check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * just_info         I  JUST_INFO * justified infomation
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/09
 *
 ********************************************************************************/

void test_just_info
(
 JUST_INFO *just_info,
 char  *org_function_name
){
	
#ifndef TOPGUN_TEST
	/* static char *func_name = "test_just_info"; *//* 関数名 */
	test_just_info_null ( just_info, org_function_name );
	test_line_null ( just_info->line, org_function_name );
	test_condition ( just_info->condition, org_function_name );
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		cell name check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * name              I  char *      name for hash value
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/28
 *
 ********************************************************************************/

void test_cell_nul
(
 CELL *cell,
 char *org_function_name
){
#ifndef TOPGUN_TEST
	if ( cell == NULL ) {
		topgun_error ( FEC_PRG_CELL, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		cell name check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * name              I  char *      name for hash value
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/28
 *
 ********************************************************************************/

void test_null_variable
(
 void *pointer,
 TOPGUN_MEMORY_TYPE_ID type_id,
 char *org_function_name
){
#ifndef TOPGUN_TEST
	if ( pointer == NULL ) {
		printf("NULL-!\n");
		topgun_error_type ( type_id, org_function_name );
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		cell name check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * name              I  char *      name for hash value
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/28
 *
 ********************************************************************************/

void test_cell_name
(
 char *name,
 char *org_function_name
){
	
#ifndef TOPGUN_TEST


#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		cell io check
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * cell_io           I  CELL_IO *   cell_io pointer
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/28
 *
 ********************************************************************************/

void test_cell_io_null
(
 CELL_IO *cell_io,
 char *org_function_name
){
	
#ifndef TOPGUN_TEST

	if ( cell_io == NULL ){
		/* cell_ioがNULLの場合 */
		topgun_error( FEC_PRG_CELL_IO, org_function_name );
	}

#endif /* TOPGUN_TEST */
}


/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_pi
(
 LINE *line, 
 char *org_function_name
){
#ifndef TOPGUN_TEST

	if ( line->n_out == 0 ) {
		topgun_error ( FEC_PRG_LINE_OUT , org_function_name );
	}
	if ( line->n_in != 0 ) {
		topgun_error ( FEC_PRG_LINE_IN , org_function_name );
	}
	test_line_out_null ( line, org_function_name );
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_po
(
 LINE *line, 
 char *org_function_name
){
#ifndef TOPGUN_TEST

	if ( line->n_in == 0 ) {
		topgun_error ( FEC_PRG_LINE_OUT , org_function_name );
	}
	if ( line->n_out != 0 ) {
		topgun_error ( FEC_PRG_LINE_IN , org_function_name );
	}
	test_line_in_null ( line, org_function_name );
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_br
(
 LINE *line, 
 char *org_function_name
){
#ifndef TOPGUN_TEST

	if ( line->n_out != 1 ) {
		topgun_error ( FEC_PRG_LINE_OUT , org_function_name );
	}
	if ( line->n_in != 1 ) {
		topgun_error ( FEC_PRG_LINE_IN , org_function_name );
	}
	test_line_out_null ( line, org_function_name );
	test_line_in_null  ( line, org_function_name );
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_in1
(
 LINE *line, 
 char *org_function_name
){
#ifndef TOPGUN_TEST

	/* BUF/INV */

	if ( line->n_out < 1 ) {
		topgun_error ( FEC_PRG_LINE_OUT , org_function_name );
	}
	if ( line->n_in != 1 ) {
		topgun_error ( FEC_PRG_LINE_IN , org_function_name );
	}
	test_line_out_null ( line, org_function_name );
	test_line_in_null  ( line, org_function_name );
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_and_nand_or_nor
(
 LINE *line, 
 char *org_function_name
){
#ifndef TOPGUN_TEST

	/* BUF/INV/BR */

	if ( line->n_out == 0 ) {
		topgun_error ( FEC_PRG_LINE_OUT , org_function_name );
	}
	if ( line->n_in == 0 ) {
		topgun_error ( FEC_PRG_LINE_IN , org_function_name );
	}
	test_line_out_null ( line, org_function_name );
	test_line_in_null  ( line, org_function_name );
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_blki
(
 LINE *line, 
 char *org_function_name
){
#ifndef TOPGUN_TEST

	if ( ( line->n_in != 0 ) ||
		 ( line->n_out != 1 ) ) {
		topgun_error ( FEC_PRG_LINE_IN , org_function_name );
	}
	test_line_out_null ( line, org_function_name );
		
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * line              I  LINE *  line pointer for NULL test
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_blko
(
 LINE *line, 
 char *org_function_name
){
#ifndef TOPGUN_TEST

	if ( ( line->n_out != 0 ) ||
		 ( line->n_in  != 1 ) ) {
		topgun_error ( FEC_PRG_LINE_OUT , org_function_name );
	}
	test_line_in_null  ( line, org_function_name );
		
#endif /* TOPGUN_TEST */
}


/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_line_structure_all
(
 char *org_function_name
){
#ifndef TOPGUN_TEST

	Ulong i;
	LINE *line;

	for ( i = 0; i < Line_info.n_line; i++ ) {
		line = &( Line_head[ i ] );

		switch ( line->type ) {
		case TOPGUN_PI:
			test_line_structure_pi ( line, org_function_name );
			break;
		case TOPGUN_PO:
			test_line_structure_po ( line, org_function_name );
			break;
		case TOPGUN_BR:
			test_line_structure_br ( line, org_function_name );
			break;
		case TOPGUN_INV:
		case TOPGUN_BUF:			
			test_line_structure_in1 ( line, org_function_name );
			break;
		case TOPGUN_AND:
		case TOPGUN_NAND:			
		case TOPGUN_OR:
		case TOPGUN_NOR:			
			test_line_structure_and_nand_or_nor ( line, org_function_name );
			break;
		case TOPGUN_BLKI:			
			test_line_structure_blki ( line, org_function_name );
		case TOPGUN_BLKO:			
			test_line_structure_blko ( line, org_function_name );
			break;
#ifdef TOPGUN_XOR_ON	
		case TOPGUN_XOR:
		case TOPGUN_XNOR:
#endif /* TOPGUN_XOR_ON */
		default:			
			topgun_error ( FEC_PRG_LINE_TYPE, org_function_name );
			break;
		}
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_lv_pi_head
(
 Ulong *cnt_lv,
 char *org_function_name
){
#ifndef TOPGUN_TEST

	Ulong i;
	for ( i = 0 ; i < Line_info.max_lv_pi; i++ ) {
		if ( cnt_lv[i]  != Line_info.n_lv_pi[i] ) {
			topgun_error ( FEC_PRG_LV_PI_HEAD, org_function_name );
		}
	}

#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		line structure
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * org_function_name I  char *      function name of calling
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void test_number_equal_ulong
(
 Ulong n_1,
 Ulong n_2,
 char *org_function_name
){
#ifndef TOPGUN_TEST

	if ( n_1 != n_2 ) {
		topgun_error ( FEC_PRG_ERROR, org_function_name );
	}

#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		check state X on all line
 *
 * << Function ID >>
 *    	17-1
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
 * Line_head     I      LINE*   Line Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *
 ********************************************************************************/

void test_all_state_xx
(
 void
){
#ifndef TOPGUN_TEST	

	Ulong i;       /* 信号線数 */
	Ulong n_stat;  /* stateがXでない信号線数 */

	n_stat = 0;

	for( i = 0; i < Line_info.n_line; i++ ) {
		if (! ( Line_head[i].flag & CIR_FIX ) ) {
			if ( Line_head[i].state9 != STATE9_XX ) {
				if ( n_stat == 0 ) {
					printf( "???????????????????????????????????\n" );
				}
				n_stat++;
				topgun_print_mes_id_type_state9( &Line_head[ i ] );
			}
		}
	}
	if( n_stat != 0 ){
		printf("???????????????????????????????????\n");
	}
#endif /* TOPGUN_TEST */	
}


/********************************************************************************
 * << Function >>
 *		check line->flag on all line
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	0			: state cleare flag
 *	ERROR		: other state
 *
 * << Argument >>
 * args			(I/O)	type		description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type		description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/3
 *
 ********************************************************************************/

void test_all_line_flag
(
 void
){
#ifndef TOPGUN_TEST	

	Ulong i; /* 信号線数 */
	Ulong n_flag; /* フラグがたっている信号線数 */

	void topgun_print_mes_id_type_stat();
	void topgun_print_mes_id_type_flag();

	n_flag = 0;

	for( i = 0; i < Line_info.n_line; i++ ) {
		if ( Line_head[i].flag != 0 ) {
			if ( n_flag == 0 ) {
				printf( "###################################\n" );
			}
			n_flag++;
			topgun_print_mes_id_type_flag( &Line_head[ i ] );
		}
	}
	if( n_flag != 0 ) {
		printf( "###################################\n" );
	}
#endif /* TOPGUN_TEST */	
}

/********************************************************************************
 * << Function >>
 *		check termination conditions of atpg
 *
 * << Function ID >>
 *	   	7-2
 *
 * << Return >>
 *     Void
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
 *		2005/5/12
 *
 ********************************************************************************/

/* static unsigned long cnt = 0; */
void test_state 
(
 Ulong stat
 ){

#ifdef TOPGUN_TEST
	
    char *func_name = "check_stat";	/* 関数名 */

	if ( ( stat > STATE9_UU ) ||
		 ( stat < STATE9_XX ) ) {
		topgun_error( FEC_PRG_LINE_STAT, func_name ); 
	}
#endif /* TOPGUN_TEST */
}

/********************************************************************************
 * << Function >>
 *		check value for fault simulater
 *
 * << Function ID >>
 *	   	7-?
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * value         I      Ulong   state value
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/1/8
 *
 ********************************************************************************/

void test_line_value
(
 Ulong value
 ){

#ifdef TOPGUN_TEST
	
    char *func_name = "test_line_value";	/* 関数名 */
	
    //if ( value < 0 ) {
	topgun_error( FEC_PRG_LINE_STAT, func_name );
    //}
#endif /* TOPGUN_TEST */
}

