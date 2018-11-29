/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 梶原方式静的学習に関する関数群
	
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

extern  void static_learn_kajihara( Ulong );
extern  void static_learn_kajihara_d( Ulong );
extern  void static_learn_line_kajihara ( ASS_LIST *, STATE_3, LEARN_MODE );
static  void static_learn_line_kajihara_d ( ASS_LIST *, STATE_3);
static  LEARN_TYPE static_learn_line_type_state_check( LINE *, STATE_3  );


extern LEARN_TYPE static_learn_line_type_check( LINE_TYPE );
extern IMP_RESULT gene_enter_state ( ASS_LIST * );
extern IMP_RESULT implication ( void );  /* 含意操作関数 */
extern void static_learn_make_learn_list( LINE *, STATE_3, LEARN_MODE );
static void static_learn_make_learn_list_kajihara_d( LINE *, STATE_3 );
extern void static_learn_check_learn_list_puls ( LEARN_LIST *, LINE *, STATE_3 );

extern void atpg_imptrace_reflesh ( void );
extern void atpg_reflesh_state( LINE *, Uint );
extern void topgun_print_mes_learn_list_result ( LINE *, STATE_3 );
extern STATE_3 atpg_invert_state3 ( STATE_3 );
extern LEARN_TYPE static_learn_line_check ( LINE * );
extern STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9値から正常値を取り出す */
extern  LEARN_TYPE static_learn_check_learn_list ( LEARN_LIST *, LINE *, STATE_3 );
extern void    topgun_print_mes_learn_state3( LINE *, STATE_3, LINE *, STATE_3);
extern  void static_learn_print_learn_result( void );
extern  void static_learn_line_count_indirect( ASS_LIST *,  STATE_3 );

extern void test_all_state_xx();

/*!
  @brief 梶原方式 静的学習用のマネージャ

  @param [in] void
  @return void

*/
void static_learn_kajihara
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

				/* 学習する */
				static_learn_line_kajihara ( &ass_list, STATE3_0, LEARN_MODE_KAJIHARA );
				static_learn_line_kajihara ( &ass_list, STATE3_1, LEARN_MODE_KAJIHARA );

		}
	}

	for ( it = 1 ; it < itration ; it++ ) {
		static_learn_print_learn_result( );

		printf("2-L-%ld\n",it);

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				/* 静的学習の対象となる信号線 */
				ass_list.line = Lv_pi_head[ i ][ j ];

				/* 学習する */
				static_learn_line_kajihara ( &ass_list, STATE3_0, LEARN_MODE_KAJIHARA );
				static_learn_line_kajihara ( &ass_list, STATE3_1, LEARN_MODE_KAJIHARA );

			}
		}
	}
	
#ifdef LEARN_OUTPUT
	{
		LEARN_TYPE learn_type;

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
	}
#endif /* LEARN_OUTPUT */

}

void static_learn_kajihara_d
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

			/* 学習する */
			static_learn_line_kajihara_d ( &ass_list, STATE3_0 );
			static_learn_line_kajihara_d ( &ass_list, STATE3_1 );

		}
	}

	for ( it = 1 ; it < itration ; it++ ) {
		printf("2-L-%ld\n",it);

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				/* 静的学習の対象となる信号線 */
				ass_list.line = Lv_pi_head[ i ][ j ];

				/* 学習する */
				static_learn_line_kajihara_d ( &ass_list, STATE3_0 );
				static_learn_line_kajihara_d ( &ass_list, STATE3_1 );

			}
		}
	}

#ifdef LEARN_OUTPUT
	{
		LEARN_TYPE learn_type;

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				/* 静的学習の対象となる信号線 */
				ass_list.line = Lv_pi_head[ i ][ j ];

				learn_type = static_learn_line_type_check ( ass_list.line->type ) ;

				/* 学習結果を出力する */
				topgun_print_mes_learn_list_result ( ass_list.line, STATE3_0 );
				topgun_print_mes_learn_list_result ( ass_list.line, STATE3_1 );
			}
		}

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
	}
#endif 	/* LEARN_OUTPUT */
	
}

/*!
  @brief ある信号線に対する静的学習を行う

  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] ass_state3 静的学習の対象となる含意操作する値
  @return void

*/

void static_learn_line_kajihara
(
 ASS_LIST *ass_list,
 STATE_3 ass_state3,
 LEARN_MODE mode
 ){

	IMP_RESULT imp_result;
	LEARN_LIST *learn_list;
	LEARN_TYPE learn_type;

	// initail imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;

	learn_type = static_learn_line_type_state_check ( ass_list->line, ass_state3 );
	if ( learn_type == LEARN_ON ) {

		//!< learn_listの設定
		if ( ass_state3 == STATE3_0 ) {
			learn_list = ass_list->line->imp_0;
		}
		else {
			learn_list = ass_list->line->imp_1;
		}
	
		ass_list->ass_state3 = ass_state3;

		imp_result = gene_enter_state ( ass_list );

		if ( IMP_CONF != imp_result.i_state ) {
		
			topgun_4_utility_enter_start_time ( &Time_head.learn_imp );
			imp_result = implication();
			topgun_4_utility_enter_end_time ( &Time_head.learn_imp );
			
			if ( IMP_CONF != imp_result.i_state ) {

			/* 含意操作の結果より、必要なものを覚える */
				static_learn_make_learn_list( ass_list->line, ass_state3, mode );

			/* 含意操作の結果より、過剰なものを削除する */
			//learn_list = static_learn_cut_surplusage_learn_list( ass_list->line, ass_state3, learn_list );

			}
		}

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
  @brief ある信号線に対する静的学習を行う

  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] ass_state3 静的学習の対象となる含意操作する値
  @return void

*/

void static_learn_line_ichihara
(
 ASS_LIST *ass_list,
 STATE_3 ass_state3,
 LEARN_MODE mode
 ){

	IMP_RESULT imp_result;
	LEARN_LIST *learn_list;
	LEARN_TYPE learn_type;

	// initail imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;
	
	/* ANDゲートの入力0や、ORゲートの入力1や、分岐などが対象 */
	learn_type = static_learn_line_type_state_check ( ass_list->line, ass_state3 );
	if ( learn_type == LEARN_ON ) {

		//!< learn_listの設定
		if ( ass_state3 == STATE3_0 ) {
			learn_list = ass_list->line->imp_0;
		}
		else {
			learn_list = ass_list->line->imp_1;
		}
	
		ass_list->ass_state3 = ass_state3;

		imp_result = gene_enter_state ( ass_list );

		if ( IMP_CONF != imp_result.i_state ) {
		
			topgun_4_utility_enter_start_time ( &Time_head.learn_imp );
			imp_result = implication();
			topgun_4_utility_enter_end_time ( &Time_head.learn_imp );
			
			if ( IMP_CONF != imp_result.i_state ) {

			/* 含意操作の結果より、必要なものを覚える */
				static_learn_make_learn_list( ass_list->line, ass_state3, mode );

			/* 含意操作の結果より、過剰なものを削除する */
			//learn_list = static_learn_cut_surplusage_learn_list( ass_list->line, ass_state3, learn_list );

			}
		}

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
  @brief ある信号線に対する静的学習を行う

  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] ass_state3 静的学習の対象となる含意操作する値
  @return void

*/

void static_learn_line_kajihara_d
(
 ASS_LIST *ass_list,
 STATE_3 ass_state3
 ){

	IMP_RESULT imp_result;
	LEARN_LIST *learn_list;
	LEARN_TYPE learn_type;

	// initail imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;
	
	learn_type = static_learn_line_type_state_check ( ass_list->line, ass_state3 );
	if ( learn_type == LEARN_ON ) {

		//!< learn_listの設定
		if ( ass_state3 == STATE3_0 ) {
			learn_list = ass_list->line->imp_0;
		}
		else {
			learn_list = ass_list->line->imp_1;
		}
	
		ass_list->ass_state3 = ass_state3;

		imp_result = gene_enter_state ( ass_list );

		if ( IMP_CONF != imp_result.i_state ) {

			utility_enter_start_time ( &Time_head.learn_imp );	
			imp_result = implication();
			utility_enter_end_time ( &Time_head.learn_imp );
			
			if ( IMP_CONF != imp_result.i_state ) {

				/* 含意操作の結果より、必要なものを覚える */
				static_learn_make_learn_list_kajihara_d( ass_list->line, ass_state3 );

				/* 含意操作の結果より、過剰なものを削除する */
				//learn_list = static_learn_cut_surplusage_learn_list( ass_list->line, ass_state3, learn_list );
			}
		}

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
  @brief 静的学習を行う信号線かを判断する

  @param [in] *line   静的学習の対象となる信号線
  @param [in] ass_state3  静的学習の対象となる信号線の値
  @return LEARN_TYPE

*/

LEARN_TYPE static_learn_line_type_state_check
(
 LINE    *line,
 STATE_3  ass_state3
 ){
	char *func_name = "static_learn_line_type_state_check"; /* 関数名 */
	
	switch ( line->type ) {
	case TOPGUN_BR:
	case TOPGUN_INV:
	case TOPGUN_BUF:
	case TOPGUN_PO:
	case TOPGUN_BLKI:
	case TOPGUN_BLKO:
	case TOPGUN_UNK:
		return ( LEARN_OFF ) ;
	case TOPGUN_AND:
	case TOPGUN_NOR:
		if ( ass_state3 == STATE3_0 ) {
			return ( LEARN_OFF );
		}
		else {
			return ( LEARN_ON );
		}
		break;
	case TOPGUN_OR:
	case TOPGUN_NAND:
		if ( ass_state3 == STATE3_1 ) {
			return ( LEARN_OFF );
		}
		else {
			  return ( LEARN_ON );
		}
		break;
	case TOPGUN_PI:
		return( LEARN_ON );
		if ( line->n_out > 1 ) {
			return ( LEARN_ON );
		}
		else {
			return ( LEARN_OFF );
		}
		break;
	default:
		topgun_error ( FEC_PRG_LINE_STAT, func_name );
	}
	return ( LEARN_OFF );
}

/*!
  @brief ある信号線に対する静的学習を行う

  @param [in] *line  静的学習の対象となる信号線へのポインタ
  @param [in] ass_state3 静的学習の対象となる値割り当ての値
  @return void

*/

void static_learn_make_learn_list_kajihara_d
(
 LINE *start_line,
 STATE_3 start_state3
 ){
#ifndef OLD_IMP2
	Ulong      pos;
	Ulong      i;
	LINE       *imp_line;
	
	STATE_3    direct_state3;
	STATE_3    start_invert_state3 = atpg_invert_state3 ( start_state3 );
	LEARN_LIST *learn_list = NULL;
	LEARN_TYPE learn_type;

	char *func_name = "static_learn_make_learn_list"; /* 関数名 */
	
	for ( pos = Gene_head.last_n, i = 0 ; i < Gene_head.cnt_n; i++, pos-- ) {

		imp_line = Gene_head.imp_list_n[ pos ];
		learn_type = static_learn_line_check ( imp_line );

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


					/* 古い消せるノードがあるかを調べる */
					if ( learn_list->ass_state3 == STATE3_0 ) {
						static_learn_check_learn_list_puls ( learn_list->line->imp_0, imp_line, STATE3_1 );
					}
					else {
						static_learn_check_learn_list_puls ( learn_list->line->imp_1, imp_line, STATE3_1 );
					}
						
					
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

					/* 古い消せるノードがあるかを調べる */
					if ( learn_list->ass_state3 == STATE3_0 ) {
						static_learn_check_learn_list_puls ( learn_list->line->imp_0, imp_line, STATE3_0 );
					}
					else {
						static_learn_check_learn_list_puls ( learn_list->line->imp_1, imp_line, STATE3_0 );
					}
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
	
	while ( imp_trace ) {

		learn_type = static_learn_line_check (imp_trace->line );

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


					/* 古い消せるノードがあるかを調べる */
					if ( learn_list->ass_state3 == STATE3_0 ) {
						static_learn_check_learn_list_puls ( learn_list->line->imp_0, imp_trace->line, STATE3_1 );
					}
					else {
						static_learn_check_learn_list_puls ( learn_list->line->imp_1, imp_trace->line, STATE3_1 );
					}
						
					
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

					/* 古い消せるノードがあるかを調べる */
					if ( learn_list->ass_state3 == STATE3_0 ) {
						static_learn_check_learn_list_puls ( learn_list->line->imp_0, imp_trace->line, STATE3_0 );
					}
					else {
						static_learn_check_learn_list_puls ( learn_list->line->imp_1, imp_trace->line, STATE3_0 );
					}
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

/* End of File */
