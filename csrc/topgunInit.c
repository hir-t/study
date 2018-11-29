/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ������˴ؤ���ؿ���
	
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
void atpg_justified_init ( void ); /* ��������Ϣ�δؿ��Υݥ��󥿤ν�����򤹤� */

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
 *		topgun�ν�����ޥ͡�����ؿ�
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
 char *bench_name  /* ATPG�оݲ�ϩ��file̾ */
){

	/* char *func_name = "topgun_init"; *//* �ؿ�̾ */
	
	topgun_1_utility_enter_start_time ( &Time_head.main_initialize );
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 1-1 �ѥ�᡼��������                                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	topgun_env_init();

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 1-1 ���֤ν����                                                 %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	time_head_init();
	
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 1-1 Scan Chain��ǧ��                                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	
	/* ̤��(2005/8/24) */
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-2 �ȹ礻��ϩ�ؤ��Ѵ�                                           $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	/* ̤��(2005/8/24) */
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-3 �ξ������                                                   $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	
	/* read circuit data */
	/* for temp function behind KOEI */
	/* test_read_circuit(); */
	/* pre koei */
	/* $$$ [1]�ϲ� $$$ */

	read_circuit( bench_name );

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-4 ��٥�η׻�                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	level_init ();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-5 �����ξ����                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	flist_equivalence();

	flist_make();
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-5 �ƥ����ӥ�ƥ��᥸��η׻�                                   $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	tm_init_scoap();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-6 uniq�ν����                                                 $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	gene_uniq_init();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? ATPG�Υޥ͡���������                                       $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* implication �ؿ��Υݥ��󥿤ν���� */
	implication_init();

	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	// $$$ 1-? line�ι�¤�������                                           $$$
	// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//info_line_structure();
	//info_line_reconvergence_structure();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-7 ��Ū�ؽ�                                                     $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	//ffrg();

	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-6 ��Ū�ؽ�                                                     $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	static_learn();
	//exit(0);
	
	/* make flist */
	/*
	  if(flist_init() == ERROR){
	  return(ERROR);
	  }
	*/


	/* justified �ؿ��Υݥ��󥿤ν���� */
	atpg_justified_init();

	/* select �ؿ��Υե饰�ν���� */
	select_init();
	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? �ξ㥷�ߥ�졼���ν����                                     $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

	fsim_initialize();

	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? ���̥Хåե��ν����                                         $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* Fsim_head.bit_size��ɬ�� */
	compact_buffer_initialize();

	
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* $$$ 1-? stock�Хåե��ν����                                        $$$ */
	/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
	/* Fsim_head.bit_size��ɬ�� */
	stock_initialize();

	topgun_1_utility_enter_end_time ( &Time_head.main_initialize );
	
}

/********************************************************************************
 * << Function >>
 *		���ַ�¬�إå�Time_head�ν����
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
 *		�¹ԥѥ�᡼��������
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

	/* Atpg_head�ο����ѹ����� */
	gene_env_init();

	/* Comp_head�ο����ѹ����� */
	comp_env_init();
	
}


 /********************************************************************************
 * << Function >>
 *		�������ϥإå�Pi_head�ν����
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

    char  *func_name = "init_pi_head";	/* �ؿ�̾  */

    Pi_head = ( LINE ** )topgun_malloc( FMT_LINE_P, sizeof(LINE *),
					Line_info.n_pi, func_name );

    cnt = 0;

	/* Line_head�κǽ�ϳ������� */
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
 *		�������ϥإå�Po_head�ν����
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

    Ulong i;   /* �������� */
    Ulong cnt; /* ��������(PO)�� */
    LINE            *lp;

	void  *topgun_malloc();

    char  *func_name = "init_po_head";	/* �ؿ�̾ */


	Po_head = ( LINE ** )topgun_malloc( FMT_LINE_P, sizeof( LINE * ),
										Line_info.n_po, func_name );

    for ( cnt = 0, i = 0; i < Line_info.n_line; i++ ) {
		lp = &( Line_head[ i ] );
		if ( lp->type == TOPGUN_PO ) {
			Po_head[ cnt++ ] = lp;
		}
    }

	/* �������Ͽ���PO�ο�������ʤ���� */
}
#endif /* TOPGUN_NOUSE */

