/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief パターン生成マネージャに関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunFlist.h" /* FLIST */
#include "topgunCompact.h"
#include "topgunTime.h"
#include "topgunMemory.h"
#include "topgunError.h"

extern  GENE_HEAD Gene_head;
extern  LINE_INFO Line_info;

/* function list */
void     generate ( GENE_INFO *, char * );

/* topgun_select.c */
extern void     select_fault( GENE_INFO *, char * );
extern void     select_fault_search_basis_random_det( void );

/* topgun_insert.c */
GENE_RESULT gene_insert_fault( FLIST *, Ulong );

/* topgun_chk.c */
int      atpg_check_finish_generate ( Ulong );

/* topgun_select.c */
Ulong select_fault_search_basis_random_det_rest_calc( void );


/* topgun_propagate.c */
int      gene_propagate_fault ( FLIST *fault, Ulong ); /* 故障を伝搬させるための値割り当てを求める */
BACK_RESULT gene_back_track ( void ) ; /* 割り当てた値を戻す */

/* topgun_justified.c */
int      atpg_justified ( Ulong ); /* 正当化する信号線を一つ選び、正当化を行う */

/* topgun_output.c */
void     one_pat_output ( void ); /* 1パターン出力する */

/* topgun_class.c */
int      atpg_untest_redun ( void ); /* 冗長故障かテスト生成不能故障かを判断する */

/* topgun_fsim.c */
void     pattern_bridge_by_random( void );

/* topgun_compress.c */
void     pattern_get_from_generate ( CC_PATTERN * );

/* topgun_reflesh.c */
void     atpg_reflesh ( void );

/* topgun_uty.c */
void utility_renew_parameter_with_env ( char *, Ulong *, Ulong );


/* topgun_error.c */
void     topgun_error( TOPGUN_ERROR_CODE, char * ); /* エラーメッセージを出力する */



/* print function */
void     topgun_print_mes( int, int );
void     topgun_print_mes_dfront_t( void );
void     topgun_print_mes_atpg_result_all( FLIST *, int );

void utility_enter_end_time_gene( PROCESS_TIME *, GENE_INFO * );    //!<

extern void test_null_variable( void *, TOPGUN_MEMORY_TYPE_ID, char *);

/********************************************************************************
 * << Function >>
 *		manger of generate pattern
 *
 * << Function ID >>
 *    	2-3
 *
 * << Return >>
 *	result		: result of generate pattern (bit)
 *                TOPGUN_DTA ( on: ATPG det, off: can't generate pattern )
 *                TOPGUN_RED ( on: redundant fault,  off: untestable fault )
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
 *		2005/ 8/24 << Modified 使用書にあうように変更(M.Yoshimura) >> 
 *
 ********************************************************************************/

void generate
(
 GENE_INFO *gene_info,
 char *fault_id
 ){

	BACK_RESULT b_result;
	Ulong n_rand_fault = 0;
	Ulong imp_id = 0; // 信号線が何番目の値割り当てで含意されたか　
	                  // gene_infoにもたせても良いかも

	static int  select_fault_mode = 2;   // 0 : ランダム検出を選択しない
	                                     // 1 : ランダム検出を選択する

	char *func_name = "generate"; /* 関数名 */

	topgun_2_utility_enter_start_time ( &Time_head.generate );

	b_result.result = 1;   /* $$$ ?? $$$ */	/* 初期値(!=0)であれば良い */

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-2 故障選択                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	select_fault( gene_info, fault_id );

	if ( gene_info->f_class == GENE_NO_FAULT ){
		/* 一度目の場合はランダムパタンで検出した故障を再度俎上に載せる */
		if ( select_fault_mode == 0 ) {
			select_fault_mode = 1;
			select_fault_search_basis_random_det();

			n_rand_fault = select_fault_search_basis_random_det_rest_calc();
			printf("Virtual Detect %lu\n",n_rand_fault );
			Line_info.n_fault_atpg_count += n_rand_fault;
		
			select_fault( gene_info, fault_id );
		}
	}

	if ( gene_info->f_class == GENE_NO_FAULT ){
		/* 故障が選択できない場合 */
		topgun_2_utility_enter_end_time ( &Time_head.generate );
		return;
	}
	topgun_print_mes( 2, 3 );
	
	
	Gene_head.back_all = 0;
	Gene_head.back_drive = 0;
	Gene_head.back_drive_just = 0;

	test_null_variable ( gene_info, FMT_GENE_INFO, func_name );

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-1 故障の挿入                                                 %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
    gene_info->result = gene_insert_fault ( gene_info->fault, ++imp_id );

    if ( gene_info->result == INSERT_FAIL ) {
		/* 故障挿入に失敗した場合 */
		/* 伝搬と正当化をせずにatpg_reflesh()をしてパターン生成終了 */
		b_result.result = 0;
    }

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-2 一意活生化                                                 %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	//gene_info->result = gene_uniq ( gene_info->fault->line );
	
	
	/* 未実装 (2005/8/25) */

    /* 故障伝搬 */
	while ( b_result.result ) {

		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-3-3 パターン生成終了?                                      %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		gene_info->result = atpg_check_finish_generate( Gene_head.back_all );

		switch ( gene_info->result ) {

		case PROPA_CONT:

			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* %%% 2-3-4 故障伝搬を進める                                   %%% */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			gene_info->result = gene_propagate_fault( gene_info->fault, ++imp_id );

			if ( ( gene_info->result == PROPA_FAIL ) ||
				 ( gene_info->result == PROPA_FAIL_DRIVE ) ||
				 ( gene_info->result == PROPA_FAIL_ENTER ) ||
				 ( gene_info->result == PROPA_FAIL_IMP ) ) {
				/* 伝搬に失敗したので、バックトラックをする */

				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				/* %%% 2-3-7 処理を元に戻す                                 %%% */
				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				b_result = gene_back_track();
			}
			break;

		case JUSTI_CONT:

			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* %%% 2-3-5 正当化を進める                                     %%% */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			gene_info->result = atpg_justified( ++imp_id );

			if ( ( gene_info->result == JUSTI_FAIL ) ||
				 ( gene_info->result == JUSTI_FAIL_ENTER ) ||
				 ( gene_info->result == JUSTI_FAIL_IMP ) ) {

				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				/* %%% 2-3-7 処理を元に戻す                                 %%% */
				/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
				/* まだ適当 */
				b_result = gene_back_track();
			}
			break;

		case JUSTI_PASS: /* 故障伝搬とも正当化も終了 = ATPG終了 */

			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* %%% 2-3-6 1パターン出力を出力する                            %%% */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

			gene_info->f_class = GENE_DET_A;

			/* パターン出力 */
			// for b19_C用
			one_pat_output(); //<-たぶんtopgun_atpgでするべき

			b_result.result = 0; /* loopを抜け出す(もっと良いやり方が!!) */

			topgun_print_mes_dfront_t(); /* dfront tree のdump */
			break;

		case LIM_BACK_END: /* back trackの上限値の為終了 = 打ち切り */

			gene_info->f_class = GENE_ABT;

			b_result.result = 0; /* loopを抜け出す(もっと良いやり方が!!) */
			break;

		default: 
			topgun_error( FEC_PRG_JUDGE_GENE_END, func_name ); /* 来ないはず */
		}
	}



	/* パターン生成終了!! */

	switch ( gene_info->f_class ){
	case NO_DEFINE:
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-3-8 冗長故障/Untestableの判定                              %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		if ( TOPGUN_RED == atpg_untest_redun() ) {
			/* 冗長の場合 */
			gene_info->f_class = GENE_RED;
		}
		else {
			gene_info->f_class = GENE_UNT;
		}
		break;
	case GENE_DET_A:
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* %%% 2-3-8 ATPGによって生成したパターンを得る                     %%% */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		pattern_get_from_generate( gene_info->cc_pat );

		break;
	default:
		break;
	}

	topgun_print_mes_atpg_result_all( gene_info->fault, gene_info->f_class );
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-9 ATPGによって変化した信号線を元に戻す                       %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	atpg_reflesh();

	topgun_2_utility_enter_end_time_gene ( &Time_head.generate, gene_info );

}

/********************************************************************************
 * << Function >>
 *		Gene_head実行パラメータの設定
 *
 * << Function ID >>
 *	   	2-?
 *
 * << Return >>
 *      None
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Gene_head     I/O    GENE_HEAD   
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/08
 *
 ********************************************************************************/

void gene_env_init
(
 void
 ){
	/* $$$ 100はデフォルト値 $$$ */
	utility_renew_parameter_with_env ( "TOPGUN_LIM_BACK", &( Gene_head.limit_back ), 100 );
}

/* End of File */
