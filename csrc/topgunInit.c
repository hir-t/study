/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 初期化に関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include<stdio.h>

#include "topgun.h"
#include "topgunLine.h"
#include "topgunTime.h"
#include "topgunMemory.h"
#include "topgunError.h"


extern LINE_INFO Line_info;
extern LINE	     **Pi_head;
extern LINE	     **Po_head;
extern LINE	     *Line_head;

extern void topgun_init ( char * );
static void time_head_init ( void );


void  atpg_justified_init();
static void topgun_env_init( void );

/* topgun_gene.c */
void gene_env_init ( void );

/* topgun_read.c */
void read_circuit( char * );

/* topgun_level.c */
void level_init ( void );

/* topgun_tm.c */
void tm_init_scoap( void );

/* topgun_flist.c */
void flist_equivalence ( void );
void flist_make ( void );

/* topgun_select.c */
void select_init( void );

/* topgun_implication.c */
void implication_init( void );

/* topgun_justified.c */
void atpg_justified_init ( void ); /* 正当化関連の関数のポインタの初期化をする */

/* topgun_fsim.c */
void fsim_initialize( void );

/* topgun_compact.c */
void compact_buffer_initialize( void );
void comp_env_init ( void );

/* topgun_learn.c */
void select_static_learn_function();
void *static_learn( void );

/* topgun_learn_dag.c */
void static_learn_dag( void );

/* topgun_stock.c */
void stock_initialize( void );

/* topgun_uniq.c */
void gene_uniq_init( void );

/* topgun_uty.c */
void topgun_uty_process_time_init ( PROCESS_TIME * );


/********************************************************************************
 * << Function >>
 *		topgunの初期化マネージャ関数
 *
 * << Function ID >>
 *	   	2-1
 *
 * << Return >>
 *      None
 *
 * << Argument >>
 * args			(I/O)	type	description
 * bench_name    I      char *  name of circuit for atpg
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 *
 ********************************************************************************/

void topgun_init
(
 char *bench_name  /* ATPG対象回路のfile名 */
){

	/* char *func_name = "topgun_init"; *//* 関数名 */
	
	topgun_1_utility_enter_start_time ( &Time_head.main_initialize );
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 1-1 パラメータの設定                                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	topgun_env_init();

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 1-1 時間の初期化                                                 %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	time_head_init();
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 1-1 Scan Chainの認識                                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	
	/* 未完(2005/8/24) */
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-2 組合せ回路への変換                                           $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	/* 未完(2005/8/24) */
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-3 故障の設定                                                   $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	
	/* read circuit data */
	/* for temp function behind KOEI */
	/* test_read_circuit(); */
	/* pre koei */
	/* $$$ [1]は仮 $$$ */

	read_circuit( bench_name );

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-4 レベルの計算                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	level_init ();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-5 等価故障解析                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	flist_equivalence();

	flist_make();
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-5 テスタビリティメジャの計算                                   $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	tm_init_scoap();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-6 uniqの初期化                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	gene_uniq_init();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? ATPGのマネージャ初期化                                       $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* implication 関数のポインタの初期化 */
	implication_init();

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// $$$ 1-? lineの構造情報出力                                           $$$
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//info_line_structure();
	//info_line_reconvergence_structure();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-7 静的学習                                                     $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	//ffrg();

	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-6 静的学習                                                     $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	static_learn();
	//exit(0);
	
	/* make flist */
	/*
	  if(flist_init() == ERROR){
	  return(ERROR);
	  }
	*/


	/* justified 関数のポインタの初期化 */
	atpg_justified_init();

	/* select 関数のフラグの初期化 */
	select_init();
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? 故障シミュレータの初期化                                     $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	fsim_initialize();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? 圧縮バッファの初期化                                         $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* Fsim_head.bit_sizeが必要 */
	compact_buffer_initialize();

	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? stockバッファの初期化                                        $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* Fsim_head.bit_sizeが必要 */
	stock_initialize();

	topgun_1_utility_enter_end_time ( &Time_head.main_initialize );
	
}

/********************************************************************************
 * << Function >>
 *		時間計測ヘッダTime_headの初期化
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
 * Time_head     I/O    TIME_HEAD   
 *
 *
 ********************************************************************************/

static void time_head_init
(
 void
 ){
	utility_process_time_init ( &Time_head.total );
	utility_process_time_init ( &Time_head.main_initialize );
	utility_process_time_init ( &Time_head.atpg );
	utility_process_time_init ( &Time_head.sim );
	utility_process_time_init ( &Time_head.select );
	utility_process_time_init ( &Time_head.generate );
	utility_process_time_init ( &Time_head.gene_det );
	utility_process_time_init ( &Time_head.gene_redun );
	utility_process_time_init ( &Time_head.gene_abort );
	utility_process_time_init ( &Time_head.random_sim );
	utility_process_time_init ( &Time_head.sim_val3 );
	utility_process_time_init ( &Time_head.sim_val2 );
	utility_process_time_init ( &Time_head.stock_sim );
	utility_process_time_init ( &Time_head.logic_sim );
	utility_process_time_init ( &Time_head.fault_sim );
	utility_process_time_init ( &Time_head.learn );
	utility_process_time_init ( &Time_head.learn_init );
	utility_process_time_init ( &Time_head.learn_direct_node );
	utility_process_time_init ( &Time_head.learn_search );
	utility_process_time_init ( &Time_head.learn_imp );
	utility_process_time_init ( &Time_head.learn_redo_sort );
	utility_process_time_init ( &Time_head.learn_loop_check );
	utility_process_time_init ( &Time_head.learn_loop_mk );
	utility_process_time_init ( &Time_head.learn_loop_ope );
	utility_process_time_init ( &Time_head.learn_no_reach );
	utility_process_time_init ( &Time_head.learn_add_br );
	utility_process_time_init ( &Time_head.learn_select );
	utility_process_time_init ( &Time_head.learn_finish );
	utility_process_time_init ( &Time_head.learn_search );
	utility_process_time_init ( &Time_head.learn_upflag );
	utility_process_time_init ( &Time_head.learn_downflag );
	utility_process_time_init ( &Time_head.learn_contra );
	utility_process_time_init ( &Time_head.insert_fault );
	utility_process_time_init ( &Time_head.check_gene );
	utility_process_time_init ( &Time_head.propagate );
	utility_process_time_init ( &Time_head.justified );
	utility_process_time_init ( &Time_head.justified_spt );
	utility_process_time_init ( &Time_head.back_track );
	utility_process_time_init ( &Time_head.output );
	utility_process_time_init ( &Time_head.reflesh );
	utility_process_time_init ( &Time_head.implication );

}

/********************************************************************************
 * << Function >>
 *		実行パラメータの設定
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
 * Time_head     I/O    TIME_HEAD   
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/08
 *
 ********************************************************************************/

static void topgun_env_init
(
 void
 ){

	/* Atpg_headの数を変更する */
	gene_env_init();

	/* Comp_headの数を変更する */
	comp_env_init();
	
}


 /********************************************************************************
 * << Function >>
 *		外部入力ヘッダPi_headの初期化
 *
 * << Function ID >>
 *	   	2-2
 *
 * << Return >>
 *      None
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Pi_head       I/O    LINE ** PrimayInput Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/
#ifdef TOPGUN_NOUSE
void init_pi_head(
){
    Ulong i;
    Ulong cnt;
    LINE  *lp;

    char  *func_name = "init_pi_head";	/* 関数名  */

    Pi_head = ( LINE ** )topgun_malloc( FMT_LINE_P, sizeof(LINE *),
					Line_info.n_pi, func_name );

    cnt = 0;

	/* Line_headの最初は外部入力 */
    for ( i = 0; i < Line_info.n_line; i++ ) {
	lp = &( Line_head[ i ] );
	if ( lp->type == TOPGUN_PI ) {
	    Pi_head[ cnt++ ] = lp;
	}
    }
}
#endif /* TOPGUN_NOUSE */

/********************************************************************************
 * << Function >>
 *		外部出力ヘッダPo_headの初期化
 *
 * << Function ID >>
 *	   	2-3
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Po_head       I/O    LINE ** PrimaryOutput header
 *
 *
 ********************************************************************************/
#ifdef TOPGUN_NOUSE
void init_po_head
(
 void
){

    Ulong i;   /* 信号線数 */
    Ulong cnt; /* 外部出力(PO)数 */
    LINE            *lp;

	void  *topgun_malloc();

    char  *func_name = "init_po_head";	/* 関数名 */


	Po_head = ( LINE ** )topgun_malloc( FMT_LINE_P, sizeof( LINE * ),
										Line_info.n_po, func_name );

    for ( cnt = 0, i = 0; i < Line_info.n_line; i++ ) {
		lp = &( Line_head[ i ] );
		if ( lp->type == TOPGUN_PO ) {
			Po_head[ cnt++ ] = lp;
		}
    }

	/* 外部出力数とPOの数があわない場合 */
}
#endif /* TOPGUN_NOUSE */

