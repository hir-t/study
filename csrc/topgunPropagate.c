/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 故障伝搬に関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include "topgun.h"
#include "topgunState.h"  /* STATE_3, STATE_9 */
#include "topgunLine.h"   /* LINE, LINE_TYPE, LINE_INFO */
#include "topgunAtpg.h"
#include "topgunFlist.h"  /* FLIST */
#include "topgunTime.h"
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h"  /* FEC_XX */

extern	GENE_HEAD	Gene_head;

GENE_RESULT atpg_propagate_fault ( FLIST *fault, Ulong ); /* 故障を伝搬させるための値割り当てを求める */
void      gene_reflesh_state_dfront( DFRONT * ); /* stateをもどす */
int       atpg_get_d_state9( STATE_9 );

BACK_RESULT gene_back_track ( Ulong ) ; /* 割り当てた値を戻す */

ASS_LIST *atpg_get_propa_list( void );


/* on other file function */
/* topgun_state.c */
STATE_9 atpg_get_state3_with_condition_2_state9( STATE_3, Uint );
extern IMP_RESULT  gene_enter_state( ASS_LIST *, Ulong ); /* stateを設定し、含意操作リストへ登録する */

/* topgun_state.hのマクロ */
//STATE_3 atpg_get_state9_with_condition_2_state3( STATE_9, Uint );

/* topgun_justified.c */
void    atpg_startlist_justified( void );

/* topgun_dfront.c */
extern GENE_RESULT atpg_drive_dfront( void );
extern void gene_imp_trace_2_dfront( Ulong );
extern void atpg_ent_propa_dfront();

GENE_BACK gene_back_track_propagate( void );
GENE_BACK gene_back_track_justified( Ulong );


/* tachyon_implication.c */
IMP_RESULT implication( void ); /* 含意操作を実行する */

/* test function */
void test_line_in_null ( LINE *, char * );
void test_state_n_f ( STATE_9, char * );
void test_state_normal ( STATE_3, char * );
void test_state_d_nd ( STATE_9, char * );
void test_ass_list ( ASS_LIST *, char * );
void test_condition ( Uint, char * );

/* print function */
void  topgun_print_mes( int, int );
void  topgun_print_mes_sp( int );
void  topgun_print_mes_id( LINE * );
void  topgun_print_mes_state9( STATE_9 );
void  topgun_print_mes_stat_n( Ulong );
void  topgun_print_mes_condition ( Uint );
void  topgun_print_mes_state9_new( STATE_9, Uint );
void  topgun_print_mes_n( void );
void topgun_print_mes_dfront_condition ( void );

/********************************************************************************
 * << Function >>
 *		propagate fault
 *
 * << Function ID >>
 *    	14-1
 *
 * << Return >>
 *	DRIVE_CONT	: D-Drive cotinue
 *	DRIVE_END	: D-Drive end
 *	DRIVE_IMPOS : D-Drive impossible
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fault		 I		FLIST *	target fault data
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

GENE_RESULT gene_propagate_fault
(
 FLIST *fault,
 Ulong imp_id
 ){

	GENE_RESULT result; /* 各関数の結果を受ける */
	IMP_RESULT imp_result; /* 各関数の結果を受ける */

	ASS_LIST *propa_list;

	char    *func_name = "atpg_propagate_fault"; /* 関数名 */

	topgun_3_utility_enter_start_time ( &Time_head.propagate );

	/* select dfrontier */

	/* $$$ いずれはFFRもしくは拡張FFRまで一度に伝搬させる $$$ */

	/* $$$ x-path チェック $$$ */

	/* D-Driveを実施 */

	/* $$$ 使用書には未定義 $$$ */
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-4-1 故障を伝搬させる                                         %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	result = atpg_drive_dfront();

	/* imp_traceの引渡し */

	switch ( (int) result ) {
	case DRIVE_CONT:
		/* 後の処理をする */
	    break;
	case DRIVE_END:
	    /* 故障伝搬完了 */
	    topgun_3_utility_enter_end_time ( &Time_head.propagate );
	    return( PROPA_PASS ) ;
	case DRIVE_IMPOS:
	    /* 故障伝搬失敗 */
	    topgun_3_utility_enter_end_time ( &Time_head.propagate );
	    return( PROPA_FAIL_DRIVE ) ;
	default:
	    topgun_error( FEC_PRG_DFRONT, func_name );
	}

	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-4-2 伝搬に必要な次の値を求める                               %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	propa_list = atpg_get_propa_list();
	if ( propa_list == NULL ) {
		topgun_error( FEC_PRG_DFRONT, func_name );
	}
	test_ass_list ( propa_list, func_name );


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-4-4 back trace tree (dfrontier tree)に登録                   %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	atpg_ent_propa_dfront ( propa_list );

	topgun_print_mes_dfront_condition();
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-4-3 lineにstateを設定                                        %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    // 値割り当てなのでmax_imp_id 0を設定する
	imp_result = gene_enter_state( propa_list, 0 );

	if ( imp_result.i_state == IMP_CONF) {
		/* 衝突する設定値の場合 ASS_CONF -> PROPA_FAILへ変換  */
		topgun_3_utility_enter_end_time ( &Time_head.propagate );
		return ( PROPA_FAIL_ENTER );
	}

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-X 含意操作                                                     %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    imp_result = implication( );

	/* imp_traceをdfrontへ移動する */
	gene_imp_trace_2_dfront( imp_id );

	topgun_3_utility_enter_end_time ( &Time_head.propagate );
	
    if ( imp_result.i_state == IMP_CONF ) {
		/* 含意操作で衝突した場合 IMP_CONF -> PROPA_FAILへ変換  */
		return( PROPA_FAIL_IMP );
    }
    return( PROPA_CONT );
}

/********************************************************************************
 * << Function >>
 *		search list of propagation line
 *
 * << Function ID >>
 *    	14-4
 *
 * << Return >>
 *	propa		: assign list for propagation
 *
 * << Argument >>
 * args			(I/O)	type			description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Gene_head     I      ATPG_HEAD imformation for ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/ 1/18
 *		2005/ 8/30 << stateを正常値と故障値でわけて考える M.Yoshimura >>
 *
 ********************************************************************************/

ASS_LIST  *atpg_get_propa_list
(
 void
){

	int      result       = 0;        /* get_d_frontの実行結果 */
	Uint     condition    = 0;        /* 正常値か故障値の割り当てを示す */
	LINE     *line        = NULL;     /* driveする信号線のポインタ */
	STATE_9  d_state9;                /* driveする信号線のstate */
	Ulong    i            = FL_ULMAX; /* カウンタ(入力信号線数) */
	Ulong    propa_in     = FL_ULMAX; /* 故障値が伝搬する信号線番号 */
	STATE_3  ass_state3;              /* 故障値が伝搬するために必要な値割り当て */
	STATE_3  check_ass_state3;        /* 故障値が伝搬するために必要な値割り当てが既にあるかを確認する */
	STATE_9  pre_ass_state9;          /* 故障値が伝搬するために必要な値割り当てをする前の状態(XOR/XNOR用) */
	ASS_LIST *propa         = NULL;   /* 求める伝搬するために必要な値とその信号線 */
	ASS_LIST *tmp_propa     = NULL;   /* メモリ確保用 */

#ifdef TOPGUN_XOR_ON	
	Ulong    ass_in       = FL_ULMAX; /* 故障値が伝搬しない信号線番号(XOR/XNOR用) */
#endif /* TOPGUN_XOR_ON */
	

	char     *func_name = "atpg_get_propa_list"; /* 関数名 */

	line   = Gene_head.dfront_c->line;

	d_state9       = STATE9_CF;
	ass_state3     = STATE3_C;
	pre_ass_state9 = STATE9_CF;

	topgun_print_mes( 26, 1 );

	/* 引数のNULLチェック */
	test_line_in_null ( line, func_name );
	
	for ( i = 0 ; i < line->n_in ; i++ ) {
		/* 入力iのstateをチェック */
		result = atpg_get_d_state9 ( line->in[ i ]->state9 );
		
		if ( DRIVE_POINT == result ) {
			propa_in = i;
			break;
		}
		/*
		if ( DRIVE_IMPOS == result ) {
			return ( NULL );
		}
		*/
	}
	d_state9 = line->in[ propa_in ]->state9;

	/* d_statのチェック */
	test_state_d_nd ( d_state9 , func_name );

	/* stateの決定 */
	/* $$$ 通常時は正常値だけでよいはず $$$ */
	/* $$$ 梶原先生から否定あり(2005/8/29) $$$ */

	switch ( line->type ) {
	case TOPGUN_AND:
	case TOPGUN_NAND:
		/* AND, NAND ならば1を伝搬することを考える */
		if( d_state9 == STATE9_01 ) {
			condition = COND_FAILURE;
		}
		else {
			/* 必ず d_stat == STATE9_10 */
			condition = COND_NORMAL;
		}
		ass_state3 = STATE3_1;
		break;

	case TOPGUN_OR:
	case TOPGUN_NOR:
		/* OR, NOR ならば0を伝搬することを考える */
		if ( d_state9 == STATE9_01 ) {
			condition = COND_NORMAL;
		}
		else {
			/* 必ず d_stat == STATE9_10 */
			condition = COND_FAILURE;
		}
		ass_state3 = STATE3_0;
		break;


#ifdef TOPGUN_XOR_ON
	case TOPGUN_XOR:
	case TOPGUN_XNOR:
		/* XOR, XNORならば簡単な方 */
		/* (01 10) (01 01) (10 10) (10 01)はすべて故障が伝搬できない */

		if ( propa_in == 0 ) {
			ass_in = 1;
			pre_ass_state9 = line->in[ 1 ]->state9;
		}
		else {
			ass_in = 0;
			pre_ass_state9 = line->in[ 0 ]->state9;
		}

		if ( pre_ass_state9 == STATE9_xx ) {
			if ( line->in[ ass_in ]->tm_c0 >
				 line->in [ass_in ]->tm_c1 ) {
				/* 両方一気にわりあてる */
				/* $$$ 逆値がとれない $$$ */
				ass_state3 = STATE3_1;
			}
			else{
				/* とりあえず正常値優先で */
				ass_state3 = STATE3_0;
				/* $$$ 逆値がとれない $$$ */
			}

			condition = STAT_N;

			/* propagate 用メモリ確保 */
			tmp_propa = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ), 1, func_name );
			tmp_propa->line       = line->in[ ass_in ];
			tmp_propa->ass_state3 = ass_state3;
			tmp_propa->condition  = condition;
			
			/* 一つずらす */			
			tmp_propa->next      = propa;
			propa = tmp_propa;

			topgun_print_mes_sp( 5 );
			topgun_print_mes_id( propa->line );
			fprint_print_mes( 26, 4);
			topgun_print_mes_state9_new
				( atpg_get_state3_with_condition_2_state9(ass_state3, condition), condition );
			topgun_print_mes_n();


			if ( line->in[ ass_in ]->flag & FP_IAP ) {

				condition = STAT_F;

				/* propagate 用メモリ確保 */
				tmp_propa = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ), 1, func_name );
				tmp_propa->line       = line->in[ ass_in ];
				tmp_propa->ass_state3 = ass_state3;
				tmp_propa->condition  = STAT_F;
			
				/* 一つずらす */			
				tmp_propa->next      = propa;
				propa = tmp_propa;

				topgun_print_mes_sp( 5 );
				topgun_print_mes_id( propa->line );
				fprint_print_mes( 26, 4);				
				topgun_print_mes_state9_new
					( atpg_get_state3_with_condition_2_state9(ass_state3, condition), condition );
				topgun_print_mes_n();
						
				test_ass_list ( propa, func_name );
			}
			return( propa );

		}
		else {
			
			/*
			  else if ( ( pre_ass_stat == STATE9_1x ) ||
			  ( pre_ass_stat == STATE9_x1 ) ) {
			  ass_stat = STATE9_11;
			  }
			*/
			if ( pre_ass_state9 == STATE9_0x ) {
				ass_state3 = STATE3_0;
				condition = STAT_F;
			}
			else if ( pre_ass_state9 == STATE9_x0 ) {
				ass_state3 = STATE3_0;
				condition = STAT_N;
			}
			else if ( pre_ass_state9 == STATE9_1x ) {
				ass_state3 = STATE3_1;
				condition = STAT_F;
			}
			else if ( pre_ass_state9 == STATE9_X1 ) {
				ass_state3  = STATE3_1;
				condition = STAT_N;
			}
			else {
				topgun_error( FEC_PRG_LINE_TYPE, func_name );
			}
		}
		break;
#endif /* TOPGUN_XOR_ON */
		
	case TOPGUN_PI:
	case TOPGUN_PO:
	case TOPGUN_BR:
	case TOPGUN_INV:
	case TOPGUN_BLKI:
	case TOPGUN_BLKO:		
		/* ありえないprimitiveたち */
	default:
		/* pointerを返す関数なので ERROR->NULL */
		topgunFree( propa, FMT_ASS_LIST, 1, func_name );
		topgun_error( FEC_PRG_LINE_TYPE, func_name );
	}

	test_state_normal ( ass_state3, func_name );
	test_condition ( condition, func_name );
	
	/* 多入力ゲートへの対応 */

	propa = NULL;

	
	for ( i = 0 ; i < line->n_in ; i++ ) {

		if ( i != propa_in ) {

			/* 設定しなくてもよい入力があるを確認
			 3入力以上のgateで必要 ただしAND/NAND/OR/NOR用 */
			check_ass_state3 = atpg_get_state9_with_condition_2_state3
				( line->in[ i ]->state9, condition );
			
			if ( check_ass_state3 == STATE3_X ) {
			
				/* propagate 用メモリ確保 */
				tmp_propa = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ), 1, func_name );
				tmp_propa->line       = line->in[ i ];
				tmp_propa->ass_state3 = ass_state3;
				tmp_propa->condition  = condition;
			

				/* 一つずらす */			
				tmp_propa->next      = propa;
				propa = tmp_propa;

				topgun_print_mes_sp( 5 );
				topgun_print_mes_id( propa->line );
				topgun_print_mes ( 26, 4 );
				topgun_print_mes_state9_new
					( atpg_get_state3_with_condition_2_state9(ass_state3, condition), condition );
				topgun_print_mes_n();
			}
		}
	}

	test_ass_list ( propa, func_name );
	
	return( propa );
}



/********************************************************************************
 * << Function >>
 *		check finished to propagate fault
 *
 * << Function ID >>
 *    	14-2
 *
 * << Return >>
 *	PROPA_END	: finish to propagate fault
 *	PROPA_CONT	: don't finish to propagate fault
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Gene_head     I      ATPG_HEAD imformation for ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/18
 *
 ********************************************************************************/

int atpg_check_finish_propagate
(
 void
){

	if ( Gene_head.flag & ATPG_PROPA_E ) {
		/* dfrontの最前線がPOであれば伝搬したとみなす */
		topgun_print_mes( 26, 2 );

		/* check_finish_justと同じ条件 */
		if ( ! (Gene_head.flag & ATPG_JUSTI_S ) ) {

			/* 故障伝搬個所を正当化候補個所として登録する */
			/* atpg_justified の中ですることに変更 */
			/* atpg_startlist_justified(); */

			/* justified を開始したflag */
			Gene_head.flag |= ATPG_JUSTI_S;
		}
		return( PROPA_PASS );
	}
	else {
		/* 念のため */
		topgun_print_mes( 26, 3 );
		return ( PROPA_CONT );
	}
}

/********************************************************************************
 * << Function >>
 *		back tracking
 *
 * << Function ID >>
 *    	14-3
 *
 * << Return >>
 *	GENE_BACK_END  : back track 終了(やりつくした)
 *	GENE_BACK_CONT : dfront tree一段戻った
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/26
 *
 ********************************************************************************/

BACK_RESULT gene_back_track
(
 Ulong imp_id
){
	BACK_RESULT  b_result;

	b_result.result = GENE_BACK_END;
	b_result.back_imp_id = 0;

	topgun_3_utility_enter_start_time ( &Time_head.back_track );

	/* 今の状態をpropagate or justified 判断する */
	if ( ( Gene_head.flag & ATPG_PROPA_E ) &&
	     ( Gene_head.flag & ATPG_JUSTI_S ) ) {

		b_result.result = gene_back_track_justified( imp_id );

		Gene_head.back_all++;

		if ( b_result.result == GENE_BACK_CONT ) {

			topgun_3_utility_enter_end_time ( &Time_head.back_track );
			b_result.result = GENE_BACK_CONT;
			return(	b_result );
		}
		/* propaを元にもどすため */
		Gene_head.flag &= ~ATPG_PROPA_E;
		Gene_head.flag &= ~ATPG_JUSTI_S;
		Gene_head.propa_line = NULL;

		Gene_head.back_drive_just++;
	}

	Gene_head.back_all++;
	Gene_head.back_drive++;

	b_result.result = gene_back_track_propagate();

	topgun_3_utility_enter_end_time ( &Time_head.back_track );
	return( b_result );
}

/********************************************************************************
 * << Function >>
 *		back tracking on propagate
 *
 * << Function ID >>
 *    	14-5
 *
 * << Return >>
 *	GENE_BACK_END  : back track 終了(やりつくした)
 *	GENE_BACK_CONT : dfront tree一段戻った
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/28
 *
 ********************************************************************************/

GENE_BACK gene_back_track_propagate
(
 void
){

	ASS_LIST *ass_list;
	ASS_LIST *tmp_ass_list;
	
	char *func_name = "gene_back_track_propagate"; /* 関数名 */
	

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-7-2-1 値割り当てとそれに伴う含意操作の結果を元にもどす       %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* 必ずdfront_cは空ではない */
	gene_reflesh_state_dfront( Gene_head.dfront_c );

	ass_list = Gene_head.dfront_c->ass_list;

	while ( ass_list ) {
		tmp_ass_list = ass_list;
		ass_list = ass_list->next;
		topgunFree( tmp_ass_list, FMT_ASS_LIST, 1, func_name );
	}
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-7-2-2 dfrontier treeの探索フラグを探索済にする               %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	Gene_head.dfront_c->flag &= ~DF_NOCHECK;
	Gene_head.dfront_c->flag |= DF_DEADEND;

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-7-2-3 dfrontier treeを一段遡る                               %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	Gene_head.dfront_c = Gene_head.dfront_c->prev;

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-7-2-4 dfrontier treeを空か?                                  %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	if ( Gene_head.dfront_c == NULL ) {
		return( GENE_BACK_END );
	}
	return( GENE_BACK_CONT );
}

/* End of File */
