/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ATPGをコントロールする関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h>

#include "topgun.h"
#include "topgunState.h"   /* STATE_3, STATE_9 */
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunFlist.h"
#include "topgunCompact.h"
#include "topgunFsim.h"
#include "topgunInfo.h"
#include "topgunTime.h"

extern FSIM_HEAD Fsim_head;
extern LINE_INFO Line_info;
extern	FLIST	       **Flist_head;

/* original function */
extern int  topgun_atpg (char *fault_id );        //!< atpgを実行を制御する
static void atpg_info_initialize ( ATPG_INFO * ); //!< atpg()用情報(gene, comp, fsimも含む)の初期化
static void gene_info_reflesh( ATPG_INFO * );     //!< gene_infoをパターンごとに初期化する

/* other function list */

void  generate( GENE_INFO *, char * );

/* topgun_result.c */
void  fault_classify_with_generate( GENE_INFO * );

/* topgun_compact.c */
void  compact ( COMP_INFO * );
void  compact_mini_x ( COMP_INFO * );
void  compact_cc_pat_initialize ( CC_PATTERN * );
void  compact_info_initialize ( COMP_INFO * );
COMP_ON_CHECK comp_on_check ( void );
void  compact_get_parallel_pattern( FSIM_INFO *, COMP_INFO * );

/* topgun_fsim.c */
void  fsim( FSIM_INFO *, COMP_INFO * );
void topgun_fsim_random( Ulong , Ulong );

/* topgun_stock.c */
void stock_compact( FSIM_INFO *, COMP_INFO *);

void utility_renew_parameter_with_env( char *, Ulong *, Ulong );

/* topgun_read.c */
extern void read_detectable_fault ( char * );

/* topgun_test.c */
void test_all_state_xx( void );
void test_all_line_flag( void );

/* topgun_print.c */
void topgun_print_mes_next_fault ( void );

//int   check_end();

/*!
  @param topgun_atpg
  @brief ATPGマネージャ

  @retunr ATPGが完了した ( ATPG_FINISH )、打ち切り ( ATPG_?? )
*/

/* *******************************************************************************
 @func topgun_atpg
 @brief ATPGマネージャ
 *
 * << Function ID >>
 *	   	4-1
 *
 * << Return >>
 *  ATPG_FINISH : ATPGが完了
 *  ???         : ATPGを打ち切り $$$ 機能追加時に追加 $$$
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
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

int topgun_atpg
(
 char *fault_id
 ){

    static Ulong gene_detect_count = 0;   // generateで, detした回数
    static Ulong gene_undetect_count = 0; // generatで, detしなかった回数
    static Ulong fsim_pat_count = 0;      // fsimで, 実行したパタン数
    double rest;
    
    Ulong  num_x_sim_pattern = 0;
    
    ATPG_INFO atpg_info; /* atpg_info */	//COMP_ON_CHECK on_check = COMP_ON_STOP;
    COMP_ON_CHECK on_check = COMP_ON_START;
    
    /* char  *func_name = "topgun_atpg"; *//* 関数名 */
    
    topgun_1_utility_enter_start_time ( &Time_head.atpg );
    
    /* ATPG各機能の情報インターフェースの初期化 */
    atpg_info_initialize( &atpg_info );

    /*
      {
      Ulong i;
      for ( i = 0 ; i < Line_info.n_fault_atpg ; i++ ) {
      if ( Flist_head[i]->info & TOPGUN_SA0 ) {
      printf("C-O   : %8ld sa0\n", Flist_head[i]->line->line_id);
      }
      else {
      printf("C-O   : %8ld sa1\n", Flist_head[i]->line->line_id);
      }
      }
	}
    */
    
    if ( fault_id != NULL ) {
	read_detectable_fault( fault_id );
	fault_id = NULL;
    }
	

    rest = ( ( Line_info.n_fault_atpg_count * 100 )/ (double)( Line_info.n_fault_atpg ));	
    printf("START               Det %8ld Und %8ld ", gene_detect_count, gene_undetect_count );
    printf("Sim  %4ld Rest %6.2f ( %8ld / %8ld )\n",
	   fsim_pat_count, rest, Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );
			
    topgun_fsim_random( atpg_info.fsim_info.num_rand_pat, 0 );
	
    //rest = ( ( Line_info.n_fault_atpg_count * 100 )/ (double)( Line_info.n_fault_atpg ));	
    //printf("RANDOM     %6ld   Det %8ld Und %8ld ", atpg_info.fsim_info.num_rand_pat, gene_detect_count, gene_undetect_count );
    //printf("Sim  %4ld Rest %6.2f ( %8ld / %8ld )\n",
    //fsim_pat_count, rest, Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );
    
    while ( 1 ) {
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-1 ATPG終了条件の確認                                       $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/*
	  result = check_end();
	  if ( result != ATPG_CONT ) { */
	/* ATPG終了 */
	/* $$$ どの条件に引掛ったかは?? $$$ */
			/* $$$ check_endと連携する必要あり $$$ */
	/*
	  break;
	  }
	*/

	/* パターン生成機能の情報インターフェースの再初期化 */ 
	gene_info_reflesh( &( atpg_info ) );
	

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-3 パターン生成 & 故障分類                                  $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	generate( &( atpg_info.gene_info ), fault_id );
	
	if ( atpg_info.gene_info.f_class == GENE_NO_FAULT ) {
	    /* 故障が選択できない */
	    break;
	}
	
	test_all_state_xx();
	test_all_line_flag();
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-4 故障分類の登録                                           $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	fault_classify_with_generate( &( atpg_info.gene_info ) );
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 2-5 パターン生成の可否                                       $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	if ( atpg_info.gene_info.f_class == GENE_DET_A ) {

	    /*
	      {
	      GENE_INFO *gene_info = &( atpg_info.gene_info );
	      
	      printf("G : line %8ld", gene_info->fault->line->line_id);
	      if ( gene_info->fault->info & TOPGUN_SA0 ) {
	      printf(" sa0\n");
	      }
	      else {
	      printf(" sa1\n");
	      }
	      }
	    */
	    
	    gene_detect_count++;
	    
	    if ( on_check == COMP_ON_START ) {
		
		/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		/* $$$ 2-5 パターン圧縮                                         $$$ */
		/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		
		//compact( &( atpg_info.comp_info ) );
		compact_mini_x( &( atpg_info.comp_info ) );
		
		/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		/* $$$ 2-6 故障シミュレーションの実施判断                       $$$ */
		/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		
		if ( atpg_info.comp_info.result == COMPACT_NG ) {

		    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		    /* $$$ 2-7 故障シミュレーションの実施                       $$$ */
		    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		    
		    /* パタン並列で実施 */
		    /* 圧縮バッファから並列用のパタンを取得する */
		    atpg_info.fsim_info.mode = FSIM_PPSFP;
		    atpg_info.fsim_info.n_cc_pat = 32;
		    compact_get_parallel_pattern( & ( atpg_info.fsim_info ),
												  & ( atpg_info.comp_info ) );
		    
		    fsim( & ( atpg_info.fsim_info ), & ( atpg_info.comp_info ) );
		    rest = ( ( Line_info.n_fault_atpg_count * 100 )/ (double)( Line_info.n_fault_atpg ));
		    printf("Pat %5ld ( %5ld ) Det %8ld Und %8ld ", atpg_info.fsim_info.num_pat, atpg_info.fsim_info.num_waste_pat, gene_detect_count, gene_undetect_count );
		    printf("Sim  %4ld Rest %6.2f ( %8ld / %8ld )\n",
			   ++fsim_pat_count, rest, Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );

		    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		    /* $$$ 2-8 検出故障分類の登録                               $$$ */
		    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
		    
		    /* 未完(2005/8/24) */
		}
		else if ( atpg_info.comp_info.result == COMPACT_OK ) {

		    if ( atpg_info.fsim_info.fsim3 == FSIM_VAL3 ){
			
			/* Fsim3起動するか */
			atpg_info.fsim_info.mode = FSIM_PPSFP_VAL3;
			atpg_info.fsim_info.n_cc_pat = 1;
			fsim( & ( atpg_info.fsim_info ), NULL );
						
			rest = ( ( Line_info.n_fault_atpg_count * 100 )/ (double)( Line_info.n_fault_atpg ));
			printf("X-Sim      %6ld   Det %8ld Und %8ld ", ++num_x_sim_pattern, gene_detect_count, gene_undetect_count );
			printf("Sim  %4ld Rest %6.2f ( %8ld / %8ld )\n",
			       ++fsim_pat_count, rest, Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );
		    }
		}
	    }
	    else {
		atpg_info.fsim_info.n_cc_pat = 1;
		atpg_info.fsim_info.mode = FSIM_PPSFP;
		
		fsim( & ( atpg_info.fsim_info ), NULL );
		
		rest = ( ( Line_info.n_fault_atpg_count * 100 )/ (double)( Line_info.n_fault_atpg ));
		printf("Pat %5ld ( %5ld ) Det %8ld Und %8ld ", atpg_info.fsim_info.num_pat, atpg_info.fsim_info.num_waste_pat, gene_detect_count, gene_undetect_count );
		printf("Sim  %4ld, Rest %6.2f ( %8ld / %8ld )\n",
		       ++fsim_pat_count,  rest, 
		       Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );

		/* 圧縮を開始するかチェック */
		on_check = comp_on_check();
	    }
	}
	else {
	    gene_undetect_count++;
	}
	
	topgun_print_mes_next_fault ();
    }

    /* generate終了後残りパターンをシミュレーションする */
    atpg_info.fsim_info.mode = FSIM_OUTPUT;
    atpg_info.fsim_info.n_cc_pat = 1;
    fsim( & ( atpg_info.fsim_info ),  & (atpg_info.comp_info ) );
	


    rest = ( ( Line_info.n_fault_atpg_count * 100 ) / (double)( Line_info.n_fault_atpg ));
    printf("Pat %5ld ( %5ld ) Det %8ld Und %8ld ", atpg_info.fsim_info.num_pat, atpg_info.fsim_info.num_waste_pat, gene_detect_count, gene_undetect_count );
    printf("Sim  %4ld Rest %6.2f ( %8ld / %8ld )\n",
	   ++fsim_pat_count, rest, Line_info.n_fault_atpg_count, Line_info.n_fault_atpg );

    if ( atpg_info.fsim_info.static_comp == FSIM_S_COMP ){
	/* 順々にパタンシミュレーションしてみる */
	atpg_info.fsim_info.mode = FSIM_STOCK;
	stock_compact( & ( atpg_info.fsim_info ),  & ( atpg_info.comp_info ) );
    }
	
	
    topgun_1_utility_enter_end_time( &Time_head.atpg );


    return( ATPG_FINISH );
}




/********************************************************************************
 * << Function >>
 *		ATPG情報初期化
 *
 * << Function ID >>
 *	   	4-2
 *
 * << Return >>
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/20
 *
 ********************************************************************************/

void atpg_info_initialize
(
 ATPG_INFO *atpg_info /* atpg()用情報 */
 ){

    GENE_INFO *gene_info = &( atpg_info->gene_info ); /* generate()用情報 */
    COMP_INFO *comp_info = &( atpg_info->comp_info ); /* compress()用情報 */
    FSIM_INFO *fsim_info = &( atpg_info->fsim_info ); /* fsim()用情報 */

    compact_cc_pat_initialize ( &( atpg_info->cc_pat ) );
    
    /* gene_info */
    gene_info->f_class = 0;
    gene_info->result  = NO_RESULT;
    gene_info->cc_pat  = &( atpg_info->cc_pat );
    gene_info->fault   = NULL;
    
    compact_info_initialize ( comp_info );
    
    /* 各情報間のリンク */
    comp_info->cc_pat  = & ( atpg_info->cc_pat );
    
    /* fsim_info */
    fsim_info->mode           = FSIM_NO_INFO;
    fsim_info->cc_pat        = & ( atpg_info->cc_pat );
    fsim_info->n_detect      = 0;
    fsim_info->num_pat       = 0; 	/* pat count */
    fsim_info->num_waste_pat = 0; 	/* waste pat count */
    
    utility_renew_parameter_with_env ( "TOPGUN_N_RAND_PAT", &( fsim_info->num_rand_pat ), 0 );
    utility_renew_parameter_with_env ( "TOPGUN_FSIM3", &( fsim_info->fsim3 ), 0 );
    utility_renew_parameter_with_env ( "TOPGUN_STATIC_COMP", &( fsim_info->static_comp ), 0 );
}

/********************************************************************************
 * << Function >>
 *		パターン生成情報初期化
 *
 * << Function ID >>
 *	   	4-3
 *
 * << Return >>
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/01/31
 *
 ********************************************************************************/

/*!
  atpg_info.gene_infoの中身を初期化する
  @param[in,out] gene_info 初期化する変数
*/

void gene_info_reflesh
(
 ATPG_INFO *atpg_info
 ){
    
    GENE_INFO *gene_info = &(atpg_info->gene_info); /* generate()用情報 */
    
    /* gene_info */
    gene_info->f_class = 0;
    gene_info->result  = NO_RESULT;
    
    gene_info->fault   = NULL;
    
    /* 毎度は不要 */
    
    gene_info->cc_pat  = &( atpg_info->cc_pat );

}


/* End of File */
