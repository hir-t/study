/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 静的学習に関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunState.h" /* STATE_3, STATE_9 */
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunLearn.h"
#include "topgunMemory.h"
#include "topgunError.h"
#include "topgunTime.h"


extern  LINE_INFO       Line_info;
extern  LINE            ***Lv_pi_head;
extern	GENE_HEAD       Gene_head;

extern  void static_learn( void );
static  void static_learn_socrates( Ulong );
static  void static_learn_ichihara( Ulong );
extern  void static_learn_make_learn_list( LINE *, STATE_3, LEARN_MODE );
static  void static_learn_line ( ASS_LIST *, STATE_3, LEARN_MODE );
extern  void topgun_print_mes_dag_fix( void );



static  LEARN_LIST *static_learn_cut_surplusage_learn_list( LINE *, STATE_3, LEARN_LIST * );
extern  LEARN_TYPE static_learn_line_type_check(LINE_TYPE );
extern  LEARN_TYPE static_learn_check_learn_list ( LEARN_LIST *, LINE *, STATE_3 );
extern  void       static_learn_check_learn_list_puls ( LEARN_LIST *, LINE *, STATE_3 );
extern  LEARN_TYPE static_learn_equal_learn_list ( LEARN_LIST *, LINE *, STATE_3 );

extern  LEARN_TYPE static_learn_line_check ( LINE *, LEARN_MODE );

extern  void static_learn_line_count ( ASS_LIST *, STATE_3 );
extern  void static_learn_count_learn_list( LINE *, STATE_3, char * );
extern  void static_learn_print_learn_result( void );
extern  void static_learn_line_count_indirect( ASS_LIST *,  STATE_3 );

/* no use */
extern  void static_learn_cut_line ( ASS_LIST *, STATE_3 );

/* topgun_implication.c */
IMP_RESULT implication ( void );  /* 含意操作関数 */

/* topgun_reflesh.c */
void atpg_imptrace_reflesh ( void );

/* state function */
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9値から正常値を取り出す */
IMP_RESULT gene_enter_state ( ASS_LIST *, Ulong );
STATE_3 atpg_invert_state3 ( STATE_3 );
void    atpg_reflesh_state( LINE *, Uint );


extern  void utility_renew_parameter_with_env ( char *, Ulong *, Ulong );
/* topgun_print.c */
void    topgun_print_mes ( Ulong, Ulong );
void    topgun_print_mes_learn( LINE * );
void    topgun_print_mes_learn_state3( LINE *, STATE_3, LINE *, STATE_3);
void    topgun_print_mes_cut_learn_state3( LINE *, STATE_3, LINE *, STATE_3);
void    topgun_print_mes_learn_list_result ( LINE *, STATE_3 );

/* topgun_learn_dag.c */
void static_learn_print_learn_result_dag ( void );

/* topgun_test.c */
void    test_all_state_xx ( void );
void    test_line_out_null ( LINE *, char * );

extern  void static_learn_dag ( Ulong );
extern  void static_learn_kajihara( Ulong );
extern  void static_learn_kajihara_d( Ulong );
extern  void static_learn_line_kajihara( ASS_LIST *, STATE_3 , LEARN_MODE );


Ulong  learn_mode = 0;
Ulong  learn_iteration = 1;


/*!
  @brief 静的学習用のマネージャ

  @param [in] void
  @return void

*/

void static_learn
(
 void
 ){
	
	char *func_name = "static_learn"; /* 関数名 */

	topgun_3_utility_enter_start_time ( &Time_head.learn );
	
	/* $$$ 0はデフォルト値(OFF) $$$ */
	utility_renew_parameter_with_env ( "TOPGUN_LEARN_MODE", &learn_mode, 0 );
	//printf("Learn_mode %ld\n",learn_mode);
	
	utility_renew_parameter_with_env ( "TOPGUN_LEARN_ITERATION", &learn_iteration, 1 );
	//printf("Learn_iteration %ld\n",learn_iteration);
	
	switch ( learn_mode ) {
	case LEARN_MODE_OFF: //0
		break;
	case LEARN_MODE_SOCRATES: //1
		static_learn_socrates( learn_iteration );
		break;
	case LEARN_MODE_ICHIHARA://2
		static_learn_ichihara( learn_iteration );
		break;
	case LEARN_MODE_KAJIHARA://3
		static_learn_kajihara( learn_iteration );
		break;
	case LEARN_MODE_KAJIHARA_D://4
		static_learn_kajihara_d( learn_iteration );
		break;
	case LEARN_MODE_DAG://5
		static_learn_dag( learn_iteration );
		break;
	default:
		topgun_error ( FEC_PRG_LINE_STAT, func_name );		
	}
	
	topgun_3_utility_enter_end_time ( &Time_head.learn );
	
	
#ifdef LEARN_OUTPUT

	{
		static_learn_print_learn_result_dag( );
#ifdef LEARN_IMP_COUNT
		{
		Ulong i,j;
		ASS_LIST   ass_list;
		
		ass_list.condition = COND_NORMAL; /* 学習は正常値に対してするので */
		ass_list.next      = NULL;

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				/* 静的学習の対象となる信号線 */
				ass_list.line = Lv_pi_head[ i ][ j ];

				static_learn_line_count_indirect( &ass_list, STATE3_0 );
				static_learn_line_count_indirect( &ass_list, STATE3_1 );
			}
		}
		topgun_print_mes_dag_fix();
		}
#endif /* LEARN_IMP_COUNT */
		exit(0);
	}

#endif /* LEARN_OUTPUT */


}

void topgun_print_mes_dag_fix
(
 void
 ){
#ifdef _DEBUG_PRT_LEARN_
	LINE_STACK *fix_list = NULL;

	fix_list = Gene_head.cir_fix;
	
	while ( fix_list ){
		switch ( fix_list->line->state9 ){
		case STATE9_0X:
			printf("FIX : %8ld state0\n",fix_list->line->line_id);
			break;
		case STATE9_1X:
			printf("FIX : %8ld state1\n",fix_list->line->line_id);
			break;
		default:
			printf("FIX : %8ld state$$\n",fix_list->line->line_id);
		}
		fix_list = fix_list->next;
	}
#endif /* _DEBUG_PRT_LEARN_ */
 }


void static_learn_ichihara
(
 Ulong itration
 ){

	Ulong i; /* 信号線のレベル数 */
	Ulong j; /* あるレベル中の信号線数 */
	Ulong it; /* 繰り返し回数 */
	
	ASS_LIST  ass_list;

	ass_list.condition = COND_NORMAL; /* 学習は正常値に対してするので */
	ass_list.next      = NULL;

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			/* 静的学習の対象となる信号線 */
			ass_list.line = Lv_pi_head[ i ][ j ];

			//printf("Lv %lu/%lu No %lu/%lu\n",i,Line_info.max_lv_pi,j,Line_info.n_lv_pi[i]);
			
			/* 学習する */
			static_learn_line_kajihara ( &ass_list, STATE3_0, LEARN_MODE_ICHIHARA );
			static_learn_line_kajihara ( &ass_list, STATE3_1, LEARN_MODE_ICHIHARA );
		}
	}

	for ( it = 1 ; it < itration ; it++ ) {

		printf("2-L-%ld\n",it);

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				/* 静的学習の対象となる信号線 */
				ass_list.line = Lv_pi_head[ i ][ j ];

				/* 学習する */
				static_learn_line_kajihara ( &ass_list, STATE3_0, LEARN_MODE_ICHIHARA );
				static_learn_line_kajihara ( &ass_list, STATE3_1, LEARN_MODE_ICHIHARA );
			}
		}
	}
	static_learn_print_learn_result( );
}


 /*!
  @brief 静的学習用のマネージャ

  @param [in] void
  @return void

*/
void static_learn_socrates
(
 Ulong iteration
 ){
	Ulong i; /* 信号線のレベル数 */
	Ulong j; /* あるレベル中の信号線数 */
	Ulong it; 

	ASS_LIST  ass_list;

	ass_list.condition = COND_NORMAL; /* 学習は正常値に対してするので */
	ass_list.next      = NULL;

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			/* 静的学習の対象となる信号線 */
			ass_list.line = Lv_pi_head[ i ][ j ];

			/* 学習する */
			static_learn_line ( &ass_list, STATE3_0, LEARN_MODE_SOCRATES );
			static_learn_line ( &ass_list, STATE3_1, LEARN_MODE_SOCRATES );
			
		}
	}
	
	for ( it = 1 ; it < iteration ; it++ ) {
	
		static_learn_print_learn_result( );

		printf("2-L-%ld\n",it);
		
		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				/* 静的学習の対象となる信号線 */
				ass_list.line = Lv_pi_head[ i ][ j ];

				/* 学習する */
				static_learn_line ( &ass_list, STATE3_0, LEARN_MODE_SOCRATES );
				static_learn_line ( &ass_list, STATE3_1, LEARN_MODE_SOCRATES );
				
			}
		}
	}

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			/* 静的学習の対象となる信号線 */
			ass_list.line = Lv_pi_head[ i ][ j ];

			/* 削除する */
			//static_learn_cut_line ( &ass_list, STATE3_0 );
			//static_learn_cut_line ( &ass_list, STATE3_1 );

		}
	}

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			/* 静的学習の対象となる信号線 */
			ass_list.line = Lv_pi_head[ i ][ j ];
			/* 学習結果を出力する */
			topgun_print_mes_learn_list_result ( ass_list.line, STATE3_0 );
			topgun_print_mes_learn_list_result ( ass_list.line, STATE3_1 );
		}
	}

	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			/* 静的学習の対象となる信号線 */
			ass_list.line = Lv_pi_head[ i ][ j ];

			static_learn_line_count_indirect( &ass_list, STATE3_0 );
			static_learn_line_count_indirect( &ass_list, STATE3_1 );

		}
	}

}

/*!
  @brief ある信号線に対する静的学習を行う

  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] ass_state3 静的学習の対象となる含意操作する値
  @return void

*/

void static_learn_line
(
 ASS_LIST *ass_list,
 STATE_3 ass_state3,
 LEARN_MODE mode
 ){

	IMP_RESULT imp_result;
	LEARN_LIST *learn_list;
	LEARN_TYPE learn_type;

	// initial imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;

	learn_type = static_learn_line_type_check ( ass_list->line->type );
	if ( learn_type == LEARN_ON ) {

		//!< learn_listの設定
		if ( ass_state3 == STATE3_0 ) {
			learn_list = ass_list->line->imp_0;
		}
		else {
			learn_list = ass_list->line->imp_1;
		}
	
		ass_list->ass_state3 = ass_state3;

        // learnなので0でよい
		imp_result = gene_enter_state ( ass_list, 0 );

		if ( IMP_CONF != imp_result.i_state ) {

			imp_result = implication();

			if ( IMP_CONF != imp_result.i_state ) {

				/* 含意操作の結果より、必要なものを覚える */
				static_learn_make_learn_list( ass_list->line, ass_state3, mode );

				/* 含意操作の結果より、過剰なものを削除する */
				//learn_list = static_learn_cut_surplusage_learn_list( ass_list->line, ass_state3, learn_list );

				/* 自身の学習結果を元に戻す */
			}
		}
		if ( ass_state3 == STATE3_0 ) {
			ass_list->line->imp_0 = learn_list;
		}
		else {
			ass_list->line->imp_1 = learn_list;
		}

		/* 含意操作の結果をrefleshする */
		//printf("2-L imp state free start\n");
		atpg_imptrace_reflesh();
#ifndef OLD_IMP2
#else		
		Gene_head.imp_trace = NULL;
#endif /* OLD_IMP2 */
		
		/* 値割り当ての結果を元にもどす */
		atpg_reflesh_state( ass_list->line, ass_list->condition );
		
		test_all_state_xx();

	}
	else {
		/* 学習しないゲート型であればしない */
	}		
}

/*!
  @brief ある信号線に対する静的学習の削減を行う

  @param [in] *ass_list  静的学習の削減対象となる信号線へのポインタ
  @param [in] ass_state3 静的学習の削減対象となる含意操作する値
  @return void

*/

void static_learn_cut_line
(
 ASS_LIST *ass_list,
 STATE_3 ass_state3     
 ){

	IMP_RESULT imp_result;
	LEARN_LIST *learn_list;
	LEARN_TYPE learn_type;

	// initial imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;
	
	learn_type = static_learn_line_type_check ( ass_list->line->type );
	
	if ( learn_type == LEARN_ON ) {

		//!< learn_listの設定と自身の学習結果を一時退避
		if ( ass_state3 == STATE3_0 ) {
			learn_list = ass_list->line->imp_0;
			ass_list->line->imp_0 = NULL;
		}
		else {
			learn_list = ass_list->line->imp_1;
			ass_list->line->imp_1 = NULL;
		}
	
		ass_list->ass_state3 = ass_state3;

        // learnなので0でよい
		imp_result = gene_enter_state ( ass_list, 0 );

		if ( IMP_CONF == imp_result.i_state ) {
			return;
		}

		imp_result = implication();

		if ( IMP_CONF == imp_result.i_state ){
			return;
		}
		
		/* 含意操作の結果より、過剰なものを削除する */
		learn_list = static_learn_cut_surplusage_learn_list( ass_list->line, ass_state3, learn_list );

		/* 自身の学習結果を元に戻す */
		if ( ass_state3 == STATE3_0 ) {
			ass_list->line->imp_0 = learn_list;
		}
		else {
			ass_list->line->imp_1 = learn_list;
		}

		/* 含意操作の結果をrefleshする */
		atpg_imptrace_reflesh();

#ifndef OLD_IMP2
#else		
		Gene_head.imp_trace = NULL;
#endif /* OLD_IMP2 */		

		/* 値割り当ての結果を元にもどす */
		atpg_reflesh_state( ass_list->line, ass_list->condition );
		
		test_all_state_xx();

	}
	else {
		/* 学習しないゲート型であればしない */
	}		
}

/*!
  @brief ある信号線に対する静的学習の対象となる信号線の数を数える

  @note まず直接含意だけ
  
  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] state3 静的学習の対象となる含意操作する値
  @return void

*/

void static_learn_line_count_direct
(
 ASS_LIST *ass_list,
 STATE_3 state3
 ){

	IMP_RESULT imp_result;
	LEARN_LIST *learn_list;
	LEARN_TYPE learn_type;

	// initial imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;
	
	learn_type = static_learn_line_type_check ( ass_list->line->type );
	if ( learn_type == LEARN_ON ) {

		//!< learn_listの設定
		if ( state3 == STATE3_0 ) {
			learn_list = ass_list->line->imp_0;
		}
		else {
			learn_list = ass_list->line->imp_1;
		}
	
		ass_list->ass_state3 = state3;

        // learnなので0でよい
		imp_result = gene_enter_state ( ass_list, 0 );

		if ( IMP_CONF == imp_result.i_state ) {
			return ;
		}

		imp_result = implication();

		if ( IMP_CONF == imp_result.i_state ) {
			return ;
		}

		/* 含意操作の結果より、数える */
		static_learn_count_learn_list( ass_list->line, state3, "direct" );

		/* 自身の学習結果を元に戻す */
		if ( state3 == STATE3_0 ) {
			ass_list->line->imp_0 = learn_list;
		}
		else {
			ass_list->line->imp_1 = learn_list;
		}

		/* 含意操作の結果をrefleshする */
		atpg_imptrace_reflesh();
#ifndef OLD_IMP2
#else		
		Gene_head.imp_trace = NULL;
#endif /* OLD_IMP2 */		

		/* 値割り当ての結果を元にもどす */
		atpg_reflesh_state( ass_list->line, ass_list->condition );
		
		test_all_state_xx();

	}
	else {
		/* 学習しないゲート型であればしない */
	}		
}

/*!
  @brief ある信号線に対する静的学習の対象となる信号線の数を数える

  @note 間接含意も含む
  
  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] state3 静的学習の対象となる含意操作する値
  @return void

*/

void static_learn_line_count_indirect
(
 ASS_LIST *ass_list,
 STATE_3 state3
 ){

	IMP_RESULT enter_imp_result;
	IMP_RESULT imp_result;
	LEARN_TYPE learn_type;
	ASS_LIST   *free_ass_list;
	ASS_LIST   *tmp_ass_list;

	char *func_name = "static_learn_line_count_indirect"; /* 関数名 */

	// initial imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;

	// initial enter_imp_result
	enter_imp_result.i_state = IMP_KEEP;
	enter_imp_result.imp_id  = 0;

	
	learn_type = static_learn_line_type_check ( ass_list->line->type );
	if ( learn_type == LEARN_ON ) {

		ass_list->ass_state3 = state3;

        // learnなので0でよい
		enter_imp_result = gene_enter_state ( ass_list, 0 );

		if ( IMP_UPDA == enter_imp_result.i_state ) {

			imp_result = implication();

			if ( IMP_CONF != imp_result.i_state ) {

				/* 含意操作の結果より、数える */
				static_learn_count_learn_list( ass_list->line, state3, "indirect" );
			}
			else {
				static_learn_count_learn_list( ass_list->line, state3, "imp_conf" );
			}
		}
		else {
			static_learn_count_learn_list( ass_list->line, state3, "ins_conf" );
		}

		/* 含意操作の結果をrefleshする */
		atpg_imptrace_reflesh();
#ifndef OLD_IMP2
#else		
		Gene_head.imp_trace = NULL;
#endif /* OLD_IMP2 */		

		/* 値割り当ての結果を元にもどす */
		if ( IMP_UPDA == enter_imp_result.i_state ) {
			atpg_reflesh_state( ass_list->line, ass_list->condition );
		}
		
		test_all_state_xx();


		tmp_ass_list = ass_list->next;
		ass_list->next = NULL;
		while ( tmp_ass_list ) {
			free_ass_list = tmp_ass_list;
			tmp_ass_list = tmp_ass_list->next;
			topgunFree( free_ass_list, FMT_ASS_LIST, 1, func_name );			
		}
	}
	else {
		/* 学習しないゲート型であればしない */
		//static_learn_count_learn_list( ass_list->line, state3, "nolearn" );
	}		
}
/*!
  @brief ある信号線に対する静的学習の削減を行う

  @param [in] LEARN_LIST *learn_list  静的学習の削減対象となる値割り当ての値リスト
  @return LEARN_LIST *new_list 剰余部分を削減された静的学習の割当リスト

*/

LEARN_LIST *static_learn_cut_surplusage_learn_list
(
 LINE *ass_line,    
 STATE_3 ass_state3,
 LEARN_LIST *learn_list
 ){
	LEARN_LIST *check_list = learn_list;
	LEARN_LIST *new_list = NULL;
	STATE_3    imp_result_state3;
	
	char *func_name = "static_learn_cut_surplusage_learn_list"; /* 関数名 */

	while ( learn_list ) {

		check_list = learn_list;
		learn_list = learn_list->next;
		
		imp_result_state3 = atpg_get_state9_2_normal_state3 ( check_list->line->state9 ); /* 9値から正常値を取り出す */

		if ( imp_result_state3 == check_list->ass_state3 ) {
			/* 静的学習なしでも含意できた */

			topgun_print_mes_cut_learn_state3( check_list->line, check_list->ass_state3, ass_line, ass_state3 );
			
			/* リストを除去する */
			topgunFree( check_list, FMT_LEARN_LIST, 1, func_name );
			
		}
		else {
			/* 新なリストに追加する */
			check_list->next = new_list;
			new_list         = check_list ;
		}
	}

	return ( new_list );
}

/*!
  @brief ある信号線に対する静的学習を行う

  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] ass_state3 静的学習の対象となる値割り当ての値
  @return void

*/

void static_learn_make_learn_list
(
 LINE *start_line,
 STATE_3 start_state3,
 LEARN_MODE mode
 ){
#ifndef OLD_IMP2
	Ulong      i = 0;
	Ulong      pos = 0;
	LINE       *imp_line = NULL;
	
	STATE_3    direct_state3;
	STATE_3    start_invert_state3 = atpg_invert_state3 ( start_state3 );
	LEARN_LIST *learn_list = NULL;
	LEARN_TYPE learn_type;

	char *func_name = "static_learn_make_learn_list"; /* 関数名 */

	for ( pos = Gene_head.last_n, i = 0 ; i < Gene_head.cnt_n; i++, pos-- ) {
		
		imp_line = Gene_head.imp_list_n [ pos ];
		
		learn_type = static_learn_line_check ( imp_line, mode ) ;

		if ( learn_type == LEARN_ON ) {

			direct_state3 = atpg_get_state9_2_normal_state3 ( imp_line->state9 ); /* 9値から正常値を取り出す */
		
			switch ( direct_state3 ) {
			case STATE3_0:

				learn_type = static_learn_check_learn_list
					( imp_line->imp_1, start_line, start_invert_state3 ); /* 間接含意の間接含意でできるかをチェック */

				if ( learn_type == LEARN_ON ) { 
				
					/* 対偶を覚える */
					learn_list = ( LEARN_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					
					learn_list->line       = start_line;
					learn_list->ass_state3 = start_invert_state3;
			
					learn_list->next       = imp_line->imp_1;
					imp_line->imp_1        = learn_list;

					topgun_print_mes_learn_state3( imp_line, STATE3_1, start_line, start_invert_state3 );
				}
				else {
					/* すでにできる == 覚えなくてよい */
					;
				}
				break;

			case STATE3_1:

				learn_type = static_learn_check_learn_list
					( imp_line->imp_0, start_line, start_invert_state3 ); /* 間接含意の間接含意でできるかをチェック */

				if ( learn_type == LEARN_ON ) {
					learn_list = ( LEARN_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					
					learn_list->line       = start_line;
					learn_list->ass_state3 = start_invert_state3;
					
					/* 対偶を覚える */
					learn_list->next       = imp_line->imp_0;
					imp_line->imp_0        = learn_list;

					topgun_print_mes_learn_state3( imp_line, STATE3_0, start_line, start_invert_state3 );
				}
				else {
					/* すでにできる == 覚えなくてよい */
					;
				}
					break;
			default:
				topgun_error ( FEC_PRG_LINE_STAT, func_name );
			}
		}
		
	}

#else	
	STATE_3    direct_state3;
	STATE_3    start_invert_state3 = atpg_invert_state3 ( start_state3 );
	IMP_TRACE  *imp_trace = Gene_head.imp_trace;
	LEARN_LIST *learn_list = NULL;
	LEARN_TYPE learn_type;

	char *func_name = "static_learn_make_learn_list"; /* 関数名 */
	
	while ( 1 ) {

		if ( imp_trace == NULL || imp_trace->next == NULL ) {
			break;
		}

		learn_type = static_learn_line_check (imp_trace->line, mode );

		if ( learn_type == LEARN_ON ) {

			direct_state3 = atpg_get_state9_2_normal_state3 ( imp_trace->line->state9 ); /* 9値から正常値を取り出す */
		
			switch ( direct_state3 ) {
			case STATE3_0:

				learn_type = static_learn_check_learn_list
					( imp_trace->line->imp_1, start_line, start_invert_state3 ); /* 間接含意の間接含意でできるかをチェック */

				if ( learn_type == LEARN_ON ) { 
				
					/* 対偶を覚える */
					learn_list = ( LEARN_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					
					learn_list->line       = start_line;
					learn_list->ass_state3 = start_invert_state3;
			
					learn_list->next       = imp_trace->line->imp_1;
					imp_trace->line->imp_1 = learn_list;

					topgun_print_mes_learn_state3( imp_trace->line, STATE3_1, start_line, start_invert_state3 );
				}
				else {
					/* すでにできる == 覚えなくてよい */
					;
				}
				break;

			case STATE3_1:

				learn_type = static_learn_check_learn_list
					( imp_trace->line->imp_0, start_line, start_invert_state3 ); /* 間接含意の間接含意でできるかをチェック */

				if ( learn_type == LEARN_ON ) {
					learn_list = ( LEARN_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					
					learn_list->line       = start_line;
					learn_list->ass_state3 = start_invert_state3;
					
					/* 対偶を覚える */
					learn_list->next       = imp_trace->line->imp_0;
					imp_trace->line->imp_0 = learn_list;

					topgun_print_mes_learn_state3( imp_trace->line, STATE3_0, start_line, start_invert_state3 );
				}
				else {
					/* すでにできる == 覚えなくてよい */
					;
				}
					break;
			default:
				topgun_error ( FEC_PRG_LINE_STAT, func_name );
			}
		}
		
		atpg_reflesh_state( imp_trace->line, imp_trace->condition );
		imp_trace = imp_trace->next;
	}
#endif /* OLD_IMP2 */ 	
}



/*!
  @brief ある信号線に対する静的学習の対象となる信号線の数を数える

  @note まず直接含意だけ
  
  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] state3 静的学習の対象となる含意操作する値
  @return void

*/

void static_learn_count_learn_list
(
 LINE *line,    
 STATE_3 state3,
 char *mes
 ){

	Ulong      on_count = 0;
	Ulong      off_count = 0;
	LEARN_TYPE learn_type;

#ifndef OLD_IMP2

	Ulong      i;
	Ulong      pos;

	for ( pos = Gene_head.last_n, i = 0 ; i < Gene_head.cnt_n; i++, pos-- ) {
	
		learn_type = static_learn_line_type_check
			( Gene_head.imp_list_n[ pos ]->type ) ;

		//if ( ( line->line_id == 187 ) && ( state3 == STATE3_0 ) ){
		//printf("2-L-D line ID %8ld State %2d\n",imp_trace->line->line_id, imp_trace->line->state9);
		//}
		

		if ( learn_type == LEARN_ON ) {
			on_count++;
		}
		else {
			off_count++;
		}
	}
	for ( pos = Gene_head.last_f, i = 0 ; i < Gene_head.cnt_f; i++, pos-- ) {
	
		learn_type = static_learn_line_type_check
			( Gene_head.imp_list_f[ pos ]->type ) ;

		//if ( ( line->line_id == 187 ) && ( state3 == STATE3_0 ) ){
		//printf("2-L-D line ID %8ld State %2d\n",imp_trace->line->line_id, imp_trace->line->state9);
		//}
		

		if ( learn_type == LEARN_ON ) {
			on_count++;
		}
		else {
			off_count++;
		}
	}
	printf(" %5ld",line->line_id);
	printf(" %8s %5ld on %5ld off %5ld\n", mes, on_count+off_count, on_count, off_count );
#else 	
	IMP_TRACE  *imp_trace = Gene_head.imp_trace;

	//char *func_name = "static_learn_count_learn_list"; /* 関数名 */
	
	while ( imp_trace ) {

		learn_type = static_learn_line_type_check ( imp_trace->line->type ) ;

		//if ( ( line->line_id == 187 ) && ( state3 == STATE3_0 ) ){
		//printf("2-L-D line ID %8ld State %2d\n",imp_trace->line->line_id, imp_trace->line->state9);
		//}
		

		if ( learn_type == LEARN_ON ) {
			on_count++;
		}
		else {
			off_count++;
		}
		imp_trace = imp_trace->next;
	}
	printf(" %5ld",line->line_id);
	printf(" %8s %5ld on %5ld off %5ld\n", mes, on_count+off_count, on_count, off_count );
#endif /* OLD_IMP2 */	
}


/*!
  @brief 静的学習を行う信号線の型かを判断する

  @param [in] type   静的学習の対象となる信号線のタイプ
  @return LEARN_TYPE

*/

LEARN_TYPE static_learn_line_type_check
(
 LINE_TYPE type
 ){
	switch ( type ) {
	case TOPGUN_BR:
	case TOPGUN_INV:
	case TOPGUN_BUF:
	case TOPGUN_PO:
	case TOPGUN_BLKI:
	case TOPGUN_BLKO:
	case TOPGUN_UNK:
		return ( LEARN_OFF ) ;
	default:
		return ( LEARN_ON );
	}
}

/*!
  @brief 静的学習を行う信号線かを判断する

  @param [in] *line   静的学習の対象となる信号線
  @return LEARN_TYPE

*/

LEARN_TYPE static_learn_line_check
(
 LINE *line,
 LEARN_MODE mode
 ){

	STATE_3 line_state3;  //!< 現在の値

	STATE_3 out_state3 = STATE3_C;
	STATE_3 out_in_state3 = STATE3_C;
	STATE_3 in_state3;   

	LINE *out = NULL;
	LINE *out_in = NULL;

	Ulong   i;  /* n_in , n_out のカウンタ */
	int     front_flag = 0;
	int     back_flag = 0;

	char *func_name = "static_learn_line_check"; /* 関数名 */
	
	test_line_out_null ( line, func_name );

	line_state3 = atpg_get_state9_2_normal_state3 ( line->state9 );

	switch ( line->n_out ) {
	case 0:
		out = NULL;
		out_state3 = STATE3_C;
		break;
	case 1:
		out = line->out[ 0 ];
		out_state3 = atpg_get_state9_2_normal_state3 ( out->state9 );
		break;
	default:
		for ( i = 0 ; i < line->n_out ; i++ ) {
			out = line->out[ i ];
			out_state3 = atpg_get_state9_2_normal_state3 ( out->state9 );

			if ( ( out_state3 == STATE3_0 ) ||
				 ( out_state3 == STATE3_1 ) ) {
				break;
			}
		}
		break;
	}

	

	/* 前向き時 */
	/* 条件1 出力側の信号線の値によって決定できない
	         1-1と1-2のいずれかの条件が必要
	         -> 1-1 出力側がXである
             -> 1-2 出力側が制御値であり、自分の入力以外が非制御値が存在する
	*/

	switch ( out_state3 ) {
	case STATE3_X:
		front_flag++; /* 条件1-1 クリア */
		break;
		
	case STATE3_0:

		if ( out->type == TOPGUN_AND ) { /* ANDの場合 */

			for ( i = 0 ; i < out->n_in ; i++ ) {
				
				out_in = out->in[ i ];

				if ( out_in->line_id != line->line_id ) {

					out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
					if ( out_in_state3 == STATE3_0 ) {
						front_flag++; /* 条件1-2 クリア */
						break;
					}
				}
			}
		}
		else if ( out->type == TOPGUN_NOR ) { /* NORの場合 */

			for ( i = 0 ; i < out->n_in ; i++ ) {
				
				out_in = out->in[ i ];

				if ( out_in->line_id != line->line_id ) {

					out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
					if ( out_in_state3 == STATE3_1 ) {
						front_flag++; /* 条件1-2 クリア */
						break;
					}
				}
			}
		}
		break;
		
	case STATE3_1:
		
		if ( out->type == TOPGUN_NAND ) { /* NANDの場合 */

			for ( i = 0 ; i < out->n_in ; i++ ) {
				
				out_in = out->in[ i ];

				if ( out_in->line_id != line->line_id ) {

					out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
					if ( out_in_state3 == STATE3_0 ) {
						front_flag++; /* 条件2 クリア */
						break;
					}
				}
			}
		}
		else if ( out->type == TOPGUN_OR ) { /* ORの場合 */

			for ( i = 0 ; i < out->n_in ; i++ ) {
				
				out_in = out->in[ i ];

				if ( out_in->line_id != line->line_id ) {

					out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
					if ( out_in_state3 == STATE3_1 ) {
						front_flag++; /* 条件1-2 クリア */
						break;
					}
				}
			}
		}
		break;
	case STATE3_U:
	default:
		front_flag++; /* 条件1-1 クリア */
	}

	/* 前向き時 */
	/* 条件2 入力側の信号線の値によって非制御値が決定される
	*/

	if ( front_flag == 1 ) {
		/* 条件1が成立しているとき */
		
		switch ( line->type ) {
		case TOPGUN_AND:
		case TOPGUN_NOR:
			/* type AND : out = 1 , in_0 = 1, in_1 = 1, ... , in_n = 1  */
			/* type NOR : out = 1 , in_0 = 0, in_1 = 0, ... , in_n = 0  */
			if ( line_state3 == STATE3_1 ) {
				return ( LEARN_ON ) ;
			}
			break;
		case TOPGUN_NAND:
		case TOPGUN_OR:
			/* type NAND: out = 0 , in_0 = 1, in_1 = 1, ... , in_n = 1  */
			/* type OR  : out = 0 , in_0 = 0, in_1 = 0, ... , in_n = 0  */
			if ( line_state3 == STATE3_0 ) {
				return ( LEARN_ON ) ;
			}
		default:
			// error
			break;
		}
	}

	/* 市原先生文献用->SLDM比較 */
	if ( mode == LEARN_MODE_ICHIHARA ) {
		return ( LEARN_OFF );
	}

	
	/* 後向き時 */
	/* 条件1 1-1 出力が制御値 */
	/*       1-2 自分も制御値 */
	/*       1-3 他の入力はすべて制御値でない */

	if ( out == NULL ) {
		return ( LEARN_OFF );
	}

	switch ( out->type ) {
	case TOPGUN_AND:
		if ( out_state3 == STATE3_0 ) {
			
			if ( line_state3 == STATE3_0 ) {

				back_flag++; /* 仮成立 */
				
				for ( i = 0 ; i < out->n_in ; i++ ) {
				
					out_in = out->in[ i ];
					
					if ( out_in->line_id != line->line_id ) {

						out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
						if ( out_in_state3 == STATE3_0 ) {
							back_flag = 0; /* 仮成立条件に反する */
							break;
						}
					}
				}	
			}
		}
		break;
	case TOPGUN_NOR:
		if ( out_state3 == STATE3_0 ) {

			if ( line_state3 == STATE3_1 ) {
				
				back_flag++;  /* 仮成立 */
				
				for ( i = 0 ; i < out->n_in ; i++ ) {
				
					out_in = out->in[ i ];
					
					if ( out_in->line_id != line->line_id ) {

						out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
						if ( out_in_state3 == STATE3_1 ) {
							back_flag = 0; /* 仮成立条件に反する */
							break;
						}
					}
				}	
			}
		}
		break;
	case TOPGUN_NAND:
		if ( out_state3 == STATE3_1 ) {
			
			if ( line_state3 == STATE3_0 ) {
				
				back_flag++; /* 仮成立 */
				
				for ( i = 0 ; i < out->n_in ; i++ ) {
				
					out_in = out->in[ i ];
					
					if ( out_in->line_id != line->line_id ) {

						out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
						if ( out_in_state3 == STATE3_0 ) {
							back_flag = 0; /* 仮成立条件に反する */
							break;
						}
					}
				}
			}
		}
		break;
	case TOPGUN_OR:
		if ( out_state3 == STATE3_1 ) {
			if ( line_state3 == STATE3_1 ) {
				back_flag++; /* 仮成立 */
				
				for ( i = 0 ; i < out->n_in ; i++ ) {
				
					out_in = out->in[ i ];
					
					if ( out_in->line_id != line->line_id ) {

						out_in_state3 = atpg_get_state9_2_normal_state3 ( out_in->state9 );
						if ( out_in_state3 == STATE3_1 ) {
							back_flag = 0; /* 仮成立条件に反する */
							break;
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}

	if ( back_flag == 1 ) {
		/* 条件1が成立したとき */

		/* 条件2 前向き含意できない */
		/*       1入力の場合、Xである */
		/*       他入力の場合、Xである */
		
		switch ( line->type ) {

		case TOPGUN_BR:
		case TOPGUN_INV:
		case TOPGUN_BUF: /* 単一入力の場合 */
			in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ 0 ]->state9 );
			if ( in_state3 == STATE3_X ) {

				return ( LEARN_ON ) ;

			}
			break;
		case TOPGUN_AND:
			if ( line_state3 == STATE3_1 ) {

				for ( i = 0; i < line->n_in ; i++ ){
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );
					if ( in_state3 != STATE3_1 ) {
						return ( LEARN_ON );
					}
				}
				return ( LEARN_OFF );
			}
			else if ( line_state3 == STATE3_0 ) {
				for ( i = 0; i < line->n_in ; i++ ){
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );
					if ( in_state3 == STATE3_0 ) {
						return ( LEARN_OFF );
					}
				}
				return ( LEARN_ON );
			}
			break;
		case TOPGUN_NAND:
			if ( line_state3 == STATE3_0 ) {

				for ( i = 0; i < line->n_in ; i++ ){
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );
					if ( in_state3 != STATE3_1 ) {
						return ( LEARN_ON );
					}
				}
				return ( LEARN_OFF );
			}
			else if ( line_state3 == STATE3_0 ) {
				for ( i = 0; i < line->n_in ; i++ ){
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );
					if ( in_state3 == STATE3_0 ) {
						return ( LEARN_OFF );
					}
				}
				return ( LEARN_ON );
			}
			break;
			
		case TOPGUN_OR:
			if ( line_state3 == STATE3_1 ) {

				for ( i = 0; i < line->n_in ; i++ ){
					
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );

					if ( in_state3 != STATE3_0 ) {

						return ( LEARN_ON );
						
					}
				}
				return ( LEARN_OFF );
			}
			else if ( line_state3 == STATE3_0 ) {
				
				for ( i = 0; i < line->n_in ; i++ ){
					
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );
					
					if ( in_state3 == STATE3_1 ) {
						
						return ( LEARN_OFF );
					}
				}
				return ( LEARN_ON );
			}
			break;
		case TOPGUN_NOR:
			
			if ( line_state3 == STATE3_1 ) {

				for ( i = 0; i < line->n_in ; i++ ){
					
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );

					if ( in_state3 != STATE3_0 ) {
						
						return ( LEARN_ON );
						
					}
				}
				return ( LEARN_OFF );
			}
			else if ( line_state3 == STATE3_0 ) {
				
				for ( i = 0; i < line->n_in ; i++ ){
					
					in_state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 );
					
					if ( in_state3 == STATE3_1 ) {
						
						return ( LEARN_OFF );
						
					}
				}
				return ( LEARN_ON );
			}
			break;
		case TOPGUN_BLKI:
		case TOPGUN_BLKO:
		case TOPGUN_UNK:
		default:
			break;
		}

		/* type AND : out = 0 , in_0 = 1, in_1 = 1, in_a = 0, ... , in_n = 1  */
		/* type NAND: out = 1 , in_0 = 1, in_1 = 1, in_a = 0, ... , in_n = 1  */
		/* type OR  : out = 1 , in_0 = 0, in_1 = 0, in_a = 1, ... , in_n = 0  */
		/* type NOR : out = 0 , in_0 = 0, in_1 = 0, in_a = 1, ... , in_n = 0  */
	
	}

	return ( LEARN_OFF );
}

/*!
  @brief 間接含意の間接含意でできるかをチェック 

  @param [in] *exit  静的学習ですでに学習しているリスト
  @param [in] *new   静的学習でこれから学習しようとしているリスト
  @return void

*/


LEARN_TYPE static_learn_check_learn_list
(
 LEARN_LIST *current, /* current ->  ass_line, ass_state3へ学習しようとしている */
 LINE *ass_line,
 STATE_3 ass_state3
 ){

	LEARN_LIST *indirect;
	LEARN_TYPE learn_type;
	
	
	while ( current ) {

		/* 全く同一かを調べる */
		learn_type = static_learn_equal_learn_list ( current, ass_line, ass_state3 );

		if ( learn_type == LEARN_OFF ) {

			return ( LEARN_OFF );
		}

		if ( current->ass_state3 == STATE3_0 ) {
			indirect = current->line->imp_0;
		}
		else {
			indirect = current->line->imp_1;
		}

		/* current->imp_?と ass_line, ass_state3が同一か調べる */
		while ( indirect ) {

			/* もう少し速く判定できるはず */
			/* $$$ level順にならんでいるため $$$ */
			learn_type = static_learn_equal_learn_list ( indirect, ass_line, ass_state3 );

			if ( learn_type == LEARN_OFF ) {

				return ( LEARN_OFF ) ;

			}

			indirect = indirect->next;

		}
		current = current->next;
	}

	return ( LEARN_ON );
}


/*
    a -> b    already 
	a -> c    new learn list

	if ( c -> b ) {
	    a -> b  is delete
	}
*/

void static_learn_check_learn_list_puls
(
 LEARN_LIST *new_learn, /* 学習した枝の先にある学習 */
 LINE *already_line, /* 学習した枝の先にある学習( */
 STATE_3 already_state3   /* 学習した枝の元の学習 */
 ){
	LEARN_TYPE learn_type;
	LEARN_LIST *already;
	LEARN_LIST *prev;
	LEARN_LIST *current_list;
	LEARN_LIST *new_learn_top = new_learn;


	char *func_name = "static_learn_check_learn_list_puls"; /* 関数名 */
	
	if ( already_state3 == STATE3_0 ){
		already = already_line->imp_0;
	}
	else {
		already = already_line->imp_1;
	}

	prev = NULL;

#ifdef _DEBUG_PRT_ATPG_
	{
		LEARN_LIST *learn_list;
		learn_list = already;
		printf("#A start\n");
		while ( learn_list ) {
			/* メッセージ出力 */
			printf("#A LEARN_RES ");
			if ( learn_list->ass_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",learn_list->line->line_id);
			}
			else {
				printf("-> %8ld state1\n",learn_list->line->line_id);
			}
			learn_list = learn_list->next;
		}
		learn_list = new_learn;	
		while ( learn_list ) {
			/* メッセージ出力 */
			printf("#N LEARN_RES ");
			if ( learn_list->ass_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",learn_list->line->line_id);
			}
			else {
				printf("-> %8ld state1\n",learn_list->line->line_id);
			}
			learn_list = learn_list->next;
		}
		printf("#N end\n");
	}
#endif /* _DEBUG_PRT_ATPG_ */

	while ( already ) {

		while ( new_learn ) {

			learn_type = static_learn_equal_learn_list ( new_learn, already->line, already->ass_state3 );
		
			if ( learn_type == LEARN_OFF ) {

				current_list = already;

				if ( prev == NULL ) {

					if ( already_state3 == STATE3_0 ){
						already_line->imp_0 = already->next;
					}
					else {
						already_line->imp_1 = already->next;
					}
				}
				else {
					prev->next = already->next;
				}
				topgunFree( current_list, FMT_LEARN_LIST, 1, func_name );
			
				/* 一つしかないはずなので */
				return ;
			}
			new_learn = new_learn->next;
		}
		prev = already;
		already = already->next;
		new_learn = new_learn_top;
	}
}



/*!
  @brief 含意の内容が同じかを確認

  @param [in] *old   静的学習ですでに学習しているリスト
  @param [in] *new   静的学習でこれから学習しようとしているリスト
  @return void

*/


LEARN_TYPE static_learn_equal_learn_list
(
 LEARN_LIST *old,
 LINE *ass_line,
 STATE_3 ass_state3
){

	if ( ( ass_line->line_id == old->line->line_id ) &&
		 ( ass_state3 == old->ass_state3 ) ) {

		return ( LEARN_OFF );
	}
	else {
		return ( LEARN_ON );

	}
}

void static_learn_print_learn_result
(
 void
 ){
	Ulong i;
	Ulong j;

	ASS_LIST ass_list;
	LEARN_TYPE learn_type;
	
	topgun_print_mes( 12, 8 );
	
	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			/* 静的学習の対象となる信号線 */
			ass_list.line = Lv_pi_head[ i ][ j ];

			learn_type = static_learn_line_type_check ( ass_list.line->type ) ;

			if ( learn_type == LEARN_ON ) {

				/* 学習結果を出力する */
				topgun_print_mes_learn_list_result ( ass_list.line, STATE3_0 );
				topgun_print_mes_learn_list_result ( ass_list.line, STATE3_1 );
			}
		}
	}
}
/* End of File */
