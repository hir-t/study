/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �������˴ؤ���ؿ���
	
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
#include "topgunTime.h"
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h"  /* FEC_XX */

extern GENE_HEAD	Gene_head;
extern JUST_INFO    **Lv_head;

JUST_METHOD just_method  = 1;


JUST_RESULT (* Switch_cand_justified[ TOPGUN_NUM_PRIM] )( JUST_INFO * );
ASS_LIST* (* Switch_line_justified[ TOPGUN_NUM_PRIM ])( JUST_INFO * );

JUST_INFO* (* Switch_justified_method[ NUM_JUST_METHOD ])( JUST_INFO * );

Ulong (* Switch_get_justified_stat )( Ulong );

/* ����������ݤ˳������� */
static STATE_3 Justified_line_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  ̤���� */
		STATE3_C, /* PO  ̤���� */
		STATE3_C, /* BR  ̤���� */
		STATE3_C, /* INV ̤���� */
		STATE3_C, /* BUF ̤���� */
		STATE3_0,  /* AND        */
		STATE3_0,  /* NAND       */
		STATE3_1,  /* OR         */
		STATE3_1,  /* NOR        */
		STATE3_C, /* XOR  00 or 11�Τ���*/
		STATE3_C, /* XNOR 00 or 11�Τ���*/
		STATE3_C, /* BLKI ̤���� */
		STATE3_C, /* BLKO ̤���� */
		STATE3_C, /* UNK  ̤���� */
};


/* ���������뿮�������� */
static STATE_3 Justify_line_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  ̤���� */
		STATE3_C, /* PO  ̤���� */
		STATE3_C, /* BR  ̤���� */
		STATE3_C, /* INV ̤���� */
		STATE3_C, /* BUF ̤���� */
		STATE3_0,  /* AND        */
		STATE3_1,  /* NAND       */
		STATE3_1,  /* OR         */
		STATE3_0,  /* NOR        */
		STATE3_C, /* XOR  00 or 11�Τ���*/
		STATE3_C, /* XNOR 00 or 11�Τ���*/
		STATE3_C, /* BLK ̤���� */
		STATE3_C, /* UNK ̤���� */
};

/* ��������õ���κݤ����Ƥ����Ϥ������������� */
static STATE_3 Justified_allin_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  ̤���� */
		STATE3_C, /* PO  ̤���� */
		STATE3_C, /* BR  ̤���� */
		STATE3_C, /* INV ̤���� */
		STATE3_C, /* BUF ̤���� */
		STATE3_1,  /* AND        */
		STATE3_0,  /* NAND       */
		STATE3_0,  /* OR         */
		STATE3_1,  /* NOR        */
		STATE3_C, /* XOR  00 or 11�Τ���*/
		STATE3_C, /* XNOR 00 or 11�Τ���*/
		STATE3_C, /* BLK ̤���� */
		STATE3_C, /* UNK ̤���� */
};

/* ��������õ���κݤ������Ϥ������� */
static STATE_3 Justified_single_state[ TOPGUN_NUM_PRIM ] =
	{	STATE3_C, /* PI  ̤���� */
		STATE3_C, /* PO  ̤���� */
		STATE3_C, /* BR  ̤���� */
		STATE3_C, /* INV ̤���� */
		STATE3_C, /* BUF ̤���� */
		STATE3_0,  /* AND        */
		STATE3_1,  /* NAND       */
		STATE3_1,  /* OR         */
		STATE3_0,  /* NOR        */
		STATE3_C, /* XOR  00 or 11�Τ���*/
		STATE3_C, /* XNOR 00 or 11�Τ���*/
		STATE3_C, /* BLK ̤���� */
		STATE3_C, /* UNK ̤���� */
};


/* function list */
GENE_RESULT atpg_justified ( Ulong ); /* ���������뿮�����������ӡ���������Ԥ� */
static void atpg_enter_justified ( JUST_INFO * ); /* ���������뿮������ꥹ�Ȥ���Ͽ���� */
GENE_RESULT atpg_line_justified ( JUST_INFO *, Ulong ); /* ���������뿮�������ͤ����ꤷ���ް����� */
Ulong     atpg_startlist_justified ( void ); /* �ǽ����������ɬ�פʿ�����(���¤���PO)����Ͽ���� */
void      atpg_search_justified ( JUST_INFO * ); /* ��������ɬ�פʿ�������õ������ */
static void atpg_enter_line_cand_justified ( LINE *, Uint condition ); /* ��������ɬ�פʿ������θ������Ͽ���� */
int       atpg_check_finish_justified ( void ); /* ����������λ������������å����� */
JUST_INFO *atpg_select_line_justified ( void ); /* ���������뿮������ꥹ�Ȥ������򤹤� */
extern void atpg_justified_init ( void ); /* ��������Ϣ�δؿ��Υݥ��󥿤ν�����򤹤� */
STATE_3   atpg_check_reverse_state_btree ( BTREE *btree ); /* back trace���˵դ��ͤ���뤫Ƚ��򤹤� */
void      atpg_enter_reverse_state ( STATE_3 r_state3, BTREE *btree, ASS_LIST *ass_list ); /* �դ��ͤ����ꤹ�� */
int       atpg_justified_cand_update ( LINE *line, Ulong cand_in, Ulong ass_in );  /* �ɤ����Ϥ����������뤫����ꤹ�� */
Ulong     atpg_justified_get_cand_value ( LINE  *line ); /* ���������뿮������ɾ���ͤ���� */
int       atpg_justified_judge ( Ulong cand_value, Ulong ass_value ); /* ���������뿮������ɾ���ͤ����ݤ���� */
void      atpg_btree_just_info_reflesh ( JUST_INFO * ); /* just_info��õ�� */
void 	  atpg_line_justified_single_path_trace( ASS_LIST * );
void 	  atpg_line_justified_single_path_trace_with_implying_node( ASS_LIST * );

GENE_BACK gene_back_track_justified ( Ulong ); /* ����������ȯ������back track�ν����򤹤� */

static Ulong atpg_get_just_info ( void );
JUST_INFO *atpg_make_just_info ( LINE *line, Uint condition );
void      atpg_enter_lv_head_just_info ( JUST_INFO *just_info );


/* switch function */
/* ��gate�����������뿮��������Ӹ������� */
static JUST_RESULT atpg_check_cand_justified_in1( JUST_INFO * ); /* po, br, inv */
static JUST_RESULT atpg_check_cand_justified_in0( JUST_INFO * ); /* pi, blk     */
static JUST_RESULT atpg_check_cand_justified_and_nand_or_nor( JUST_INFO * );

#ifdef TOPGUN_XOR_ON
int      atpg_check_cand_justified_xor( JUST_INFO * );
int      atpg_check_cand_justified_xnor( JUST_INFO * );
#endif /* TOPGUN_XOR_ON */

/* ��gate�ο������������������ͤ���� */
ASS_LIST *atpg_line_justified_and_nand_or_nor( JUST_INFO * ); /* and, nand, or, nor */
ASS_LIST *atpg_line_justified_in1( JUST_INFO * ); /* po, br, inv */
ASS_LIST *atpg_line_justified_in0( JUST_INFO * ); /* pi, blk */

#ifdef TOPGUN_XOR_ON
ASS_LIST *atpg_line_justified_xor( JUST_INFO * );  /* xor  */
ASS_LIST *atpg_line_justified_xnor( JUST_INFO * ); /* xnor */
#endif /* TOPGUN_XOR_ON */


/* other file function list */
/* topgun_state.c */
IMP_RESULT gene_enter_state( ASS_LIST *, Ulong );
STATE_3  atpg_get_state9_2_normal_state3 ( STATE_9 );
STATE_3  atpg_get_state9_2_failure_state3 ( STATE_9 );
Uint     atpg_reverse_condition ( Uint );

/* topgun_state.h�Υޥ��� */
//STATE_3  atpg_get_state9_with_condition_2_state3 ( STATE_9, Uint );

/* topgun_btree.c */
void     atpg_ent_just_btree( JUST_INFO * );
void     gene_ent_btree( ASS_LIST *);
void     atpg_make_btree( void );
void	 gene_imp_trace_2_btree( Ulong );
extern void atpg_btree_enter_just_flag ( JUST_INFO *);

/* topgun_reflesh.c */
void     atpg_btree_state_reflesh( BTREE *btree );

/* tachyon_implication.c */
IMP_RESULT implication ( void );

/* test function list */
void  test_line_null ( LINE *, char * );
void  test_line_in_null ( LINE *, char * );
void  test_line_flag_justify ( char  * );
void  test_state_normal ( Ulong, char * );
void  test_condition ( Uint, char * );
void  test_ass_list ( ASS_LIST *, char * );
void  test_just_info ( JUST_INFO *, char * );
void  test_just_info_null ( JUST_INFO *, char * );
void  test_btree_null ( BTREE *, char * );
void  test_lv_head_null ( char * );

LINE_STACK *atpg_justified_packjust_info ( JUST_INFO *, LINE_STACK * );

/* print function */
void  topgun_print_mes( int, int );
void  topgun_print_mes_id_type_state9( LINE * );
void  topgun_print_mes_just( LINE * );
void  topgun_print_mes_lv_head( void );
void  topgun_print_mes_can_just( LINE * );
void  topgun_print_mes_j_ass_list ( LINE *, ASS_LIST * );
void  topgun_print_mes_justified_search ( LINE *, Uint );
void  topgun_print_mes_gate_at_condition( LINE *, Ulong, Uint );
void  topgun_print_mes_gate_next_condition( LINE *, Ulong, Uint );
void  topgun_print_mes_gate_end_condition( LINE *, Ulong, Uint );
void  topgun_print_mes_just_e ( void );
void  topgun_print_mes_ent_just_flag ( void );
void  topgun_print_mes_btree_ass_only ( void );

/********************************************************************************
 * << Function >>
 *		all justified line check
 *
 * << Function ID >>
 *    	16-1
 *
 * << Return >>
 *	JUSTI_KEEP	: justified continue
 *	JUSTI_CONF	: justified conflict
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

GENE_RESULT atpg_justified
(
 Ulong imp_id
){

	JUST_INFO  *just_info;
	JUST_INFO  *tmp_just;

	GENE_RESULT	result;
	Ulong       start_lv; // �������򳫻Ϥ���lv_pi
	Ulong       i;        // ������

	// char       *func_name = "atpg_justified"; //�ؿ�̾

	topgun_3_utility_enter_start_time ( &Time_head.justified );

	/* btree�Υ������ */
	atpg_make_btree();

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-5-1 ̤�������������Υꥹ�ȥ��å�                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* �ξ㤬��ã�����������Ϥ�������¦����������������õ���򳫻Ϥ��� */

	/* start_lv����������ɬ�פʿ�������Lv_head����Ͽ���� */
	start_lv = atpg_get_just_info();

	topgun_print_mes ( 25, 0 );
	topgun_print_mes ( 25, 1 );


	for ( i = start_lv+1 ; i >  0 ; ){

		i--;

		just_info = Lv_head[ i ];


		Lv_head[ i ] = NULL;

		while ( just_info ) {

			/* ������������line��õ��������Ͽ���� */
			atpg_search_justified( just_info );


			/* õ�������ä�just_info��JUSTIY_NO, _FL��flag�ε����ˤĤ��� */
			tmp_just  = just_info;
			just_info = just_info->next;

			atpg_btree_enter_just_flag ( tmp_just );

		}
	}

	topgun_print_mes_ent_just_flag ();

	/* Gene_head.btree->just_e�˲�����Ͽ����ʤ����Ͻ�λ */
	if(Gene_head.btree->just_e == NULL){

		/*
		tmp_btree = Gene_head.btree;
		Gene_head.btree = Gene_head.btree->prev;
		Gene_head.btree->next = NULL;

		topgunFree ( tmp_btree, FMT_BTREE, 1, func_name );
		*/

		topgun_print_mes( 25, 5 );

		topgun_3_utility_enter_end_time ( &Time_head.justified );
		return( JUSTI_CONT );
	}
	topgun_print_mes_just_e ();

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-5-2 ̤�������������������򤹤�                             %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Gene_head.btree->just_e����Ͽ����Ƥ���line���������򤷡�
	   �Τ���ϸ��䤽�Τޤ� */
	just_info = atpg_select_line_justified( );

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* %%% 2-3-5-3 ̤���������������ͤ�����������                           %%% */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	result = atpg_line_justified( just_info, imp_id ); /* JUSTI_CONT or JUSTI_CONT */
	
	topgun_3_utility_enter_end_time ( &Time_head.justified );

	return( result );
}

/********************************************************************************
 * << Function >>
 *		enter justified lists
 *
 * << Function ID >>
 *    	16-15
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	 I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/19
 *
 ********************************************************************************/

void atpg_enter_justified
(
 JUST_INFO *just_info
 ){

	JUST_INFO *enter_just_info;

	char      *func_name = "atpg_enter_justified"; /* �ؿ�̾ */

	/* �ξ���嵯�ȸξ�����¤�ȯ�� */
	/* tri-state��1hot�Ȥ������ATPG�Ȥ��Ǥ�ȯ�������� */

	test_just_info ( just_info, func_name );

	enter_just_info = atpg_make_just_info ( just_info->line, just_info->condition );

	/* back trace tree����Ͽ */
	atpg_ent_just_btree( enter_just_info );
}

/********************************************************************************
 * << Function >>
 *		justified by line
 *
 * << Function ID >>
 *    	16-7
 *
 * << Return >>
 *	JUSTI_KEEP	: justified continue
 *	JUSTI_CONF	: justified conflict
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

GENE_RESULT atpg_line_justified
(
 JUST_INFO *just_info,
 Ulong imp_id
 ){
	/* ������ ��� ������ */
	/* �����ͤ��ߤ����Τ��ξ��ͤ��ߤ����Τ� */
	/* �������ξ�郎�ߤ��ʤ� */
	/* �Ȥꤢ���������ͤ��� */

	/* ������ ���� ������ 2005/09/05 */
	/* �����ͤ��������ȸξ��ͤ��������򤽤줾��ͤ��� */

	IMP_RESULT imp_result; /* �»ܷ�� */
	ASS_LIST *ass_list; /* Switch_line_justified �η�� */
	ASS_LIST *tmp_ass_list;
	ASS_LIST *top_ass_list; 

	char     *func_name = "atpg_line_justified"; /* �ؿ�̾ */


	topgun_print_mes( 25, 3 );
	topgun_print_mes_id_type_state9( just_info->line );

	// initail imp_result
	imp_result.i_state = IMP_KEEP;
	//imp_result.imp_id  = 0;

	/* ���������뿮�����Ȥ����ͤ���� */
	/* �ؿ��Υݥ���version */
	ass_list = Switch_line_justified[ just_info->line->type ]( just_info );

	// D���르�ꥺ��Τޤ�(ľ��ο��������ͤ��꿶��) 
	top_ass_list = ass_list;

	if ( ass_list->next == NULL ) {

		if ( ass_list->condition == COND_NORMAL ) {
			// case1 : single path trace�ǳ������Ϥ��ͤ����ꤹ��
			// ����ɬ�פʤ�Τ��񤷤����ɤ�Ǥ��ɤ���Τϴ�ñ�ʤ�Τ�
			//atpg_line_justified_single_path_trace( ass_list );
			
			// case2 : ���ζ�˵���ͤ��꿶��
			
			// case3 : ���ִްտ����礭���������ˡ��ͤ����ꤹ�� ( with IG )
			//atpg_line_justified_extend_mini_level_node( ass_list );

			// case4 : implying node
			atpg_line_justified_single_path_trace_with_implying_node( ass_list );
		}
			//! ��Ȥ�ȸξ��ͤ����äƤ������ɡ�����������������ͤ��ͤ��ѹ��ˤʤä����
		//if ( ( ass_list->condition == COND_FAILURE ) &&
		//	 ( ! ( ass_list->line->flag & FV_ASS ) ) ) {
		//		 ass_list->condition = COND_NORMAL;
		//}
	}


	
	test_ass_list ( ass_list, func_name );

	topgun_print_mes_j_ass_list ( just_info->line, ass_list);

	/* assign������������������Ͽ */
    // �ͳ�����Ƥ��ʳ��ʤΤǡ�0�����ꤹ��
	imp_result = gene_enter_state( ass_list, 0 );


    if ( imp_result.i_state == IMP_CONF ) {
		/* ���ͤ��������ͤξ�� ASS_CONF->JUSTI_FAIL_ENTER���Ѵ�  */
	
		ass_list = top_ass_list;
		while ( ass_list ) {
			tmp_ass_list = ass_list;
			ass_list = ass_list->next;
			topgunFree ( tmp_ass_list , FMT_ASS_LIST, 1, func_name );
		}

		return( JUSTI_FAIL_ENTER );
    }

	/* back trace tree �ο�����Ͽ */
	gene_ent_btree( ass_list );
	
	ass_list = top_ass_list;
	while ( ass_list ) {
		tmp_ass_list = ass_list;
		ass_list = ass_list->next;
		topgunFree ( tmp_ass_list , FMT_ASS_LIST, 1, func_name );
	}

	/* ������btree���ͳ��������ɽ�� */
	topgun_print_mes_btree_ass_only ();

	/* �ް���� */
    imp_result = implication();

	/* imp_trace�ΰ��Ϥ� */
	gene_imp_trace_2_btree( imp_id );

    if ( IMP_CONF == imp_result.i_state ) {
		return( JUSTI_FAIL_IMP );
    }
	return( JUSTI_CONT );
}

ASS_LIST  *atpg_line_justified_in1( JUST_INFO *just_info ){

	char *func_name = "atpg_line_justified_in1"; /* �ؿ�̾ */
	topgun_error( FEC_PRG_JUSTIFIED, func_name );

	return( NULL ); /* ��ʤ����� */
}

ASS_LIST  *atpg_line_justified_in0( JUST_INFO *just_info ){

	char *func_name = "atpg_line_justified_in0"; /* �ؿ�̾ */

	topgun_error( FEC_PRG_JUSTIFIED, func_name );

	return( NULL ); /* ��ʤ����� */
}

/********************************************************************************
 * << Function >>
 *		entry candidate justified line list
 *
 * << Function ID >>
 *    	16-3
 *
 * << Return >>
 *     lv_pi    : �ξ㤬���¤����������Υ�٥�
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/9
 *		2005/11/9 << M.Yoshimura FLEETS >> JUST_INFOƳ���ˤ�����̲���
 *
 ********************************************************************************/

Ulong atpg_startlist_justified
(
 void
){
	/* 1st�ϸξ㤬���¤���PO���� */
	/* �ξ�����ϤϤɤ������ɤ��Ϥ� */

	JUST_INFO  *just_info = NULL;

	char       *func_name = "atpg_startlist_justified"; /* �ؿ�̾ */

	/* $$$ dfront_c�����¤���PO�ȤϤ�����ʤ� $$$ */
	test_line_null ( Gene_head.propa_line, func_name );


	Gene_head.propa_line->flag |= JUSTIFY_NO;

	/* �����ͤξ������� */
	just_info = atpg_make_just_info ( Gene_head.propa_line, COND_NORMAL );

	/* �ǽ��Lv_head��ľ����Ͽ���� */
	atpg_enter_lv_head_just_info ( just_info );


	Gene_head.propa_line->flag |= JUSTIFY_FL;
	/* �ξ��ͤξ������� */
	just_info = atpg_make_just_info ( Gene_head.propa_line, COND_FAILURE );

	/* �ǽ��Lv_head��ľ����Ͽ���� */
	atpg_enter_lv_head_just_info ( just_info );

	return ( Gene_head.propa_line->lv_pi );
}

/********************************************************************************
 * << Function >>
 *		���ߤο��������餵���õ�����뿮���������������뿮���������
 *
 * << Function ID >>
 *    	16-5
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description

 *	Switch_cand_justified				select function
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/9
 *
 ********************************************************************************/

void atpg_search_justified
(
 JUST_INFO *just_info
 ){
	LINE  *line;
	Uint  condition;
	JUST_RESULT just_result;
	Ulong i;

	char  *func_name = "atpg_search_justified"; /* �ؿ�̾ */

	test_just_info ( just_info, func_name );

	line      = just_info->line;
	condition = just_info->condition;

	/* JUSTIFY�ե饰��Ω�Ƥ�Ľ��just����Ͽ����Ľ����� */

	topgun_print_mes_justified_search ( line, condition );

	just_result = Switch_cand_justified[ line->type ]( just_info );

#ifndef TOPGUN_TEST
	if ( just_result == JUST_ERROR ) {
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
#endif /* TOPGUN_TEST */

	/* ��̤γ�ǧ�ȼ��ؤν��� */
	if ( just_result & JUST_OWN ) {
		/* �������ꥹ�Ȥ���Ͽ���� */
		atpg_enter_justified( just_info );

		topgun_print_mes_gate_at_condition( line, OUT_PIN_NUM, condition );
	}
	else if ( just_result & JUST_ALL ) {

		for ( i = 0 ; i < line->n_in ; i++ ) {

			topgun_print_mes_gate_next_condition( line, i+1, condition );

			/* ���٤Ƥ�in��JUSTIFY flag�򤿤Ƥ��� */
			atpg_enter_line_cand_justified( line->in[ i ], condition );

		}
	}
	else if ( just_result & JUST_1IN ) {

		/* in?��JUSTIFY flag�򤿤Ƥ��� */
		i = (int)( ( just_result - JUST_1IN ) / JUST_1IN_N );

		topgun_print_mes_gate_next_condition( line, i+1, condition );

		atpg_enter_line_cand_justified( line->in[ i ], condition );

	}
	else if ( just_result & JUST_NONE ) {
		topgun_print_mes_gate_end_condition( line, OUT_PIN_NUM, condition );
	}
	else {
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
}


/********************************************************************************
 * << Function >>
 *		entry candidate justified line list
 *
 * << Function ID >>
 *    	16-16
 *
 * << Return >>
 *	   void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/10
 *
 ********************************************************************************/

void atpg_enter_line_cand_justified
(
 LINE *line,
 Uint condition
 ){

	JUST_INFO *just_info = NULL;
	char      *func_name = "atpg_enter_line_cand_justified"; /* �ؿ�̾ */

	test_line_null ( line, func_name );
	test_condition ( condition, func_name );

	if ( condition == COND_FAILURE ) {
		if ( ( ! ( line->flag & FP_IAP ) ) ||
			 ( line->flag & LINE_FLT ) ) {

			/* �ξ����¤β�ǽ�����ʤ���� */
			condition = COND_NORMAL;
		}
	}

	/* �����å��ؿ� */
	/* �Ť�... flag�Ȥä������� */
	/* ���flag���б� */
	if ( condition == COND_NORMAL ) {
		if ( line->flag & JUSTIFY_NO ) {
			/* ��Ͽ�ѤΤ���ʤˤ⤷�ʤ� */
			;
		}
		else {
			line->flag |= JUSTIFY_NO;
			/* just_info ���� */
			just_info = atpg_make_just_info ( line, condition );

			/* Lv_head����Ͽ */
			atpg_enter_lv_head_just_info ( just_info );
		}
	}
	else {
		if ( line->flag & JUSTIFY_FL ) {
			/* ��Ͽ�ѤΤ���ʤˤ⤷�ʤ� */
			;
		}
		else {
			line->flag |= JUSTIFY_FL;
			/* just_info ���� */
			just_info = atpg_make_just_info ( line, condition );

			/* Lv_head����Ͽ */
			atpg_enter_lv_head_just_info ( just_info );
		}
	}
}


/********************************************************************************
 * << Function >>
 *		judge xor gate for justified
 *
 * << Function ID >>
 *    	16-14
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *      2005/09/29 multi gate 
 *
 ********************************************************************************/

#ifdef TOPGUN_XOR_ON
int atpg_check_cand_justified_xor
(
 JUST_INFO *just_info
 ){
	LINE *line  = just_info->line;

	int  result = 0; /* ��������ѿ��ν���� */

	STATE_3 out; /* state of justified line  */
	STATE_3 in0; /* state of line of input No.0 of justified line */
	STATE_3 in1; /* state of line of input No.1 of justified line */

	char *func_name = "atpg_check_cand_justified_xor"; /* �ؿ�̾ */

	test_just_info ( just_info, func_name );
	
	if ( just_info->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		out = atpg_get_state9_2_normal_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_normal_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_normal_state3( line->in[ 1 ]->state9 ) ;
	}
	else {
		/* COND_FAILURE */
		out = atpg_get_state9_2_failure_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_failure_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_failure_state3( line->in[ 1 ]->state9 ) ;
	}
	
	/* �����ͤˤ�ä�ʬ�� */
	if ( STATE3_X == out ) {
		;
	}
	else{
		/* in0 X ���� in1 X�ʤ鼫�Ȥ������� */
		if ( ( STATE3_X == in0 ) &&
			 ( STATE3_X == in0 ) ) {
			result |= JUST_OWN;
			return( result );
		}
		/* in0 X ���� in1 X�Ǥʤ��ʤ�������Ĥ������� */
		result |= JUST_ALL;

	}
	return ( result );
}

#endif /* TOPGUN_XOR_ON */

/********************************************************************************
 * << Function >>
 *		judge "XNOR" gate for justified
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *
 ********************************************************************************/

#ifdef TOPGUN_XOR_ON
int atpg_check_cand_justified_xnor
(
 JUST_INFO *just_info
 ){

	LINE *line  = just_info->line;
	int  result = 0; /* ��������ѿ��ν���� */

	STATE_3 out; /* state of justified line  */
	STATE_3 in0; /* state of line of input No.0 of justified line */
	STATE_3 in1; /* state of line of input No.1 of justified line */

	char *func_name = "atpg_check_cand_justified_xnor"; /* �ؿ�̾ */

	test_just_info ( just_info, func_name );
	
	if ( just_info->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		out = atpg_get_state9_2_normal_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_normal_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_normal_state3( line->in[ 1 ]->state9 ) ;
	}
	else {
		/* COND_FAILURE */
		out = atpg_get_state9_2_failure_state3( line->state9 ) ;
		in0 = atpg_get_state9_2_failure_state3( line->in[ 0 ]->state9 ) ;
		in1 = atpg_get_state9_2_failure_state3( line->in[ 1 ]->state9 ) ;
	}
	
	/* �����ͤˤ�ä�ʬ�� */
	/* $$$ �̤ˤ��ʤ��Ƥ��ɤ�? $$$ */
	if ( STATE3_X == out) {
		;
	}
	else{
		/* in0 X ���� in1 X�ʤ鼫�Ȥ������� */
		if ( ( STATE3_X == in0 ) &&
			 ( STATE3_X == in1 ) ) {
			result |= JUST_OWN;
			return( result );
		}
		/* in0 X ���� in1 X�Ǥʤ��ʤ�������Ĥ������� */
		result |= JUST_ALL;

#ifndef TOPGUN_CIRCUIT
		/* �ް����⤷����������������Ω */
		/* �����ͤȸξ��ͤ�ξ�������������櫓�ǤϤʤ� */
		/*
		if ( ( in0 == STATE3_X ) ||
			 ( in1 == STATE3_X ) ) {

			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		*/
#endif /* TOPGUN_CIRCUIT */
	}
	return( result );
}
#endif /* TOPGUN_XOR_ON */
/********************************************************************************
 * << Function >>
 *		judge gate with only one input for justified
 *
 * << Function ID >>
 *    	16-11
 *
 * << Return >>
 *	JUST_ALL    : �������ե饰�����Ϥ�����
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/10
 *
 ********************************************************************************/


JUST_RESULT atpg_check_cand_justified_in1
(
 JUST_INFO *just_info
 ){

#ifndef TOPGUN_TEST
	STATE_3 normal3;
	STATE_3 failure3;

	char *func_name = "atpg_check_cand_justified_in1"; /* �ؿ�̾ */

	/* �ް����Υߥ��Τ� */
	normal3  = atpg_get_state9_2_normal_state3( just_info->line->state9 );
	failure3 = atpg_get_state9_2_failure_state3( just_info->line->state9 );
	if ( ( normal3 == STATE3_X ) && ( failure3 == STATE3_X ) ) {
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
#endif /* TOPGUN_TEST */

	return( JUST_ALL );
}

/********************************************************************************
 * << Function >>
 *		judge gate with NO input for justified
 *
 * << Function ID >>
 *    	16-12
 *
 * << Return >>
 *      int (Void)
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified candidate
 *
 * << extern >>
 * name			(I/O)	type			description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/28
 *
 ********************************************************************************/


JUST_RESULT atpg_check_cand_justified_in0
(
 JUST_INFO *just_info
 ){
	/*	char *func_name = "atpg_check_cand_justified_in0"; *//* �ؿ�̾ */
	/* 	void       topgun_error(); */
	/* 	topgun_error( FEC_PRG_JUSTIFIED, func_name ); */

	return(JUST_NONE);
}


/********************************************************************************
 * << Function >>
 *		check finished to justified
 *
 * << Function ID >>
 *    	16-2
 *
 * << Return >>
 *  JUST_END    : finish to propagate fault
 *  JUST_CONT   : don't finish to propagate fault
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/2/18
 *
 ********************************************************************************/

int atpg_check_finish_justified
(
 void
){

	/* char       *func_name = "atpg_check_finish_justified"; *//* �ؿ�̾ */

	if ( Gene_head.flag & ATPG_JUSTI_S )  {  /* ATPG_JUSTI_S��Ω�äƤ� */
		/* atpg_get_just_info�򻲾� */

		if ( Gene_head.btree == NULL ) {
			/* start�� */
			return ( JUSTI_CONT );
		}
		else if ( Gene_head.btree->just_e != NULL ) {
				return( JUSTI_CONT );			
		}
		/* Lv_head[]��NULL�Ǥʤ���� */
		/*
		start_lv = Gene_head.dfront_c->line->lv_pi;
	
		for ( i = start_lv+1 ; i > 0 ; ) {
			i--;
			if ( Lv_head[ i ] != NULL ) {        
				return( JUSTI_KEEP );
			}
		}
		*/
	}
	/* justified��ɬ�׸��䤬�ʤ��ʤ齪λ */
	/* �ξ����¤Ȥ��������⽪λ = ATPG��λ */
	/* 2005/11/8 ��� */
	/*
	Gene_head.btree->imp_trace = Gene_head.imp_trace;
	Gene_head.imp_trace        = NULL;
	*/
	/* 2005/11/8 �����ޤ� */
	
	/* ��λ����btree�϶��ʤΤǲ���/�õ� */

	/* $$$ �ʤ����ʤ����� ?? $$$ */
	/* just_flag�δط����ᤷ������� 
	tmp_btree = Gene_head.btree;
	Gene_head.btree = Gene_head.btree->prev;
	Gene_head.btree = Gene_head.btree->prev;

	topgunFree ( tmp_btree, FMT_BTREE, 1, func_name );
	*/

	topgun_print_mes( 25, 4 );
	
	return( JUSTI_PASS );
}



/********************************************************************************
 * << Function >>
 *		select line for justified
 *
 * << Function ID >>
 *    	16-6
 *
 * << Return >>
 *	line		: line poitner for justified
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

JUST_INFO  *atpg_select_line_justified
(
 void
){

	JUST_INFO  *select_just = NULL;
	//JUST_INFO  *just_info;

	char       *func_name = "atpg_select_line_justified"; /*  �ؿ�̾ */

	test_btree_null ( Gene_head.btree, func_name );
	test_just_info ( Gene_head.btree->just_e, func_name );

	select_just = Switch_justified_method[ just_method ] ( Gene_head.btree->just_e ) ;

	return( select_just );
}

/********************************************************************************
 * << Function >>
 *		select line for justified
 *
 * << Function ID >>
 *    	16-6
 *
 * << Return >>
 *	line		: line poitner for justified
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

JUST_INFO  *atpg_select_line_justified_lv_pi_high
(
 JUST_INFO *just_info
){

	JUST_INFO  *select_just;

	char       *func_name = "atpg_select_line_justified_lv_pi_high"; /*  �ؿ�̾ */

	test_just_info ( just_info, func_name );

	select_just = just_info;

	while ( just_info ) {
		/* lv_pi�κǤ��礭��just_info�����򤹤� */
		if ( select_just->line->lv_pi < just_info->line->lv_pi ) {
			select_just = just_info;
		}
		just_info = just_info->next ;
	}

	return( select_just );
}

/********************************************************************************
 * << Function >>
 *		select line for justified
 *
 * << Function ID >>
 *    	16-6
 *
 * << Return >>
 *	line		: line poitner for justified
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/31
 *
 ********************************************************************************/

JUST_INFO  *atpg_select_line_justified_lv_pi_low
(
 JUST_INFO *just_info
){

	JUST_INFO  *select_just;

	char       *func_name = "atpg_select_line_justified_lv_pi_low"; /*  �ؿ�̾ */

	test_just_info ( just_info, func_name );

	select_just = just_info;

	while ( just_info ) {
		/* lv_pi�κǤ��礭��just_info�����򤹤� */
		if ( select_just->line->lv_pi > just_info->line->lv_pi ) {
			select_just = just_info;
		}
		just_info = just_info->next ;
	}

	return( select_just );
}

/********************************************************************************
 * << Function >>
 *		back track on justified
 *
 * << Function ID >>
 *    	16-4
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 * 		Masayoshi Yoshimura (Kyushu University)
 *
 * << Date >>
 *		2005/04/26 initailize
 *		2007/06/05 add imp_id
 *
 ********************************************************************************/


GENE_BACK gene_back_track_justified
(
 Ulong imp_id
){
	IMP_RESULT imp_result;
	STATE_3  r_state3;
	BTREE    *btree;
	BTREE    *tmp_btree; /* free�ѥƥ�ݥ�� */
	ASS_LIST ass_list;
	ASS_LIST *top_ass_list;
	ASS_LIST *tmp_ass_list;
		
	char     *func_name = "gene_back_track_justified"; /* �ؿ�̾ */

	//inital imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id = 0;
	
	btree = Gene_head.btree;

	while ( btree != NULL ) {

		/* �Ƕ������������ͤȤ����ȼ���ް������ͤ򸵤��᤹ */
		atpg_btree_state_reflesh( btree );


		/* �դ��ͤ�����Ǥ��뤫��Ĵ�٤� */
		r_state3 = atpg_check_reverse_state_btree( btree );

		if ( r_state3 != STATE3_C ) {

			/* �դ��ͤ����ꤹ��(btree) */
			atpg_enter_reverse_state ( r_state3, btree, &ass_list );

			test_ass_list ( &ass_list, func_name );
			
			/* assign������������������Ͽ */
            // ����ްդ˶ᤤ�Τǡ�0�Ȥ��Ƥ���$$$$
            // imp_id��reverse_state���ֹ�-1������
			imp_result = gene_enter_state( &ass_list, 0 );

			top_ass_list = ass_list.next;
			while( top_ass_list ) {
				tmp_ass_list = top_ass_list;
				top_ass_list = top_ass_list->next;
				topgunFree ( tmp_ass_list , FMT_ASS_LIST, 1, func_name );
			}
			
			if ( IMP_CONF != imp_result.i_state ) {
				/* �ް�����»� */
				imp_result = implication();
				
				/* imp_trace�ΰ��Ϥ� */
				gene_imp_trace_2_btree( imp_id );
				
				if ( IMP_CONF == imp_result.i_state ) {
					/* ���ꤷ�����ɼ��Ԥ������ */
					topgun_print_mes ( 18, 4 );
					
					/* �ޤ�����btree�����ꤷ���ͤ򸵤ˤ�ɤ� */
					/* ���ꤷ���դ��ͤ򸵤ˤ�ɤ� */
					topgun_print_mes ( 18, 6 );
					atpg_btree_state_reflesh( btree );
				}
				else {
					/* �ް������������Ȥ���back track��λ */
					topgun_print_mes ( 18, 5 );
					return ( GENE_BACK_CONT );
				}
			}
			else{
				topgun_print_mes ( 18, 7 );
			}
		}

		/* �դ��ͤ�����Ǥ��ʤ� */

		/* btree��JUSTFY_NO, FL���ᤷ��just_e/d��free���� */
		atpg_btree_just_info_reflesh ( btree->just_e );
		atpg_btree_just_info_reflesh ( btree->just_d );
		atpg_btree_just_info_reflesh ( btree->just_flag );

		btree->just_e = NULL;
		btree->just_d = NULL;
		
		/* ������� */
		tmp_btree = btree;
		btree = btree->prev;
		if ( btree == NULL ) {
			/* �ξ�����²ս��JUSTIFY_NO, _FL���᤹ */
			test_line_flag_justify( func_name );
			
		}
		else {
			btree->next = NULL;
		}

		Gene_head.btree = btree;

		topgunFree ( tmp_btree, FMT_BTREE, 1, func_name );
		
		topgun_print_mes ( 18, 6 );
	}
	/* justified �����ꤷ���ͤ����٤����� */
	/* propagate����� */
	return ( GENE_BACK_END );
}

/********************************************************************************
 * << Function >>
 *		initailized justified function
 *
 * << Function ID >>
 *    	6-8
 *
 * << Return >>
 *	    void
 *
 * << Argument >>
 * 		void
 *
 * << extern >>
 * name			(I/O)	type		description
 *
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/6
 *
 ********************************************************************************/

void atpg_justified_init
(
 void
){
	Switch_cand_justified[ TOPGUN_PI   ] = &atpg_check_cand_justified_in0; /* pi  */
	Switch_cand_justified[ TOPGUN_PO   ] = &atpg_check_cand_justified_in1; /* po  */
	Switch_cand_justified[ TOPGUN_BR   ] = &atpg_check_cand_justified_in1; /* br  */
	Switch_cand_justified[ TOPGUN_INV  ] = &atpg_check_cand_justified_in1; /* inv */
	Switch_cand_justified[ TOPGUN_BUF  ] = &atpg_check_cand_justified_in1; /* buf */
	Switch_cand_justified[ TOPGUN_AND  ] = &atpg_check_cand_justified_and_nand_or_nor; /* and */
	Switch_cand_justified[ TOPGUN_NAND ] = &atpg_check_cand_justified_and_nand_or_nor; /* nand */
	Switch_cand_justified[ TOPGUN_OR   ] = &atpg_check_cand_justified_and_nand_or_nor; /* or */
	Switch_cand_justified[ TOPGUN_NOR  ] = &atpg_check_cand_justified_and_nand_or_nor; /* nor */
	Switch_cand_justified[ TOPGUN_BLKI ] = &atpg_check_cand_justified_in0; /* blk in */
	Switch_cand_justified[ TOPGUN_BLKO ] = &atpg_check_cand_justified_in1; /* blk out */	
	
#ifdef TOPGUN_XOR_ON
	Switch_cand_justified[ TOPGUN_XOR  ] = &atpg_check_cand_justified_xor; /* xor */
	Switch_cand_justified[ TOPGUN_XNOR ] = &atpg_check_cand_justified_xnor;/* xnor */
#endif /* TOPGUN_XOR_ON */


	Switch_line_justified[ TOPGUN_PI   ] = &atpg_line_justified_in0; /* pi  */
	Switch_line_justified[ TOPGUN_PO   ] = &atpg_line_justified_in1; /* po  */
	Switch_line_justified[ TOPGUN_BR   ] = &atpg_line_justified_in1; /* br  */
	Switch_line_justified[ TOPGUN_INV  ] = &atpg_line_justified_in1; /* inv */
	Switch_line_justified[ TOPGUN_BUF  ] = &atpg_line_justified_in1; /* buf */
	Switch_line_justified[ TOPGUN_AND  ] = &atpg_line_justified_and_nand_or_nor; /* and */
	Switch_line_justified[ TOPGUN_NAND ] = &atpg_line_justified_and_nand_or_nor; /* nand */
	Switch_line_justified[ TOPGUN_OR   ] = &atpg_line_justified_and_nand_or_nor; /* or */
	Switch_line_justified[ TOPGUN_NOR  ] = &atpg_line_justified_and_nand_or_nor; /* nor */
	Switch_line_justified[ TOPGUN_BLKI ] = &atpg_line_justified_in0; /* blk */
	Switch_line_justified[ TOPGUN_BLKO ] = &atpg_line_justified_in1; /* blk */	

#ifdef TOPGUN_XOR_ON	
	Switch_line_justified[ TOPGUN_XOR  ] = &atpg_line_justified_xor; /* xor */
	Switch_line_justified[ TOPGUN_XNOR ] = &atpg_line_justified_xnor;/* xnor */
#endif /* TOPGUN_XOR_ON */	

	Switch_justified_method[ JUST_METHOD_LV_PI_HIGH ] = &atpg_select_line_justified_lv_pi_high;
	Switch_justified_method[ JUST_METHOD_LV_PI_LOW  ] = &atpg_select_line_justified_lv_pi_low; 
	
}


/********************************************************************************
 * << Function >>
 *		back track on justified
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  ASS_R_CONF  : conflict reverse state for back trace
 *  STAT_??     : reverse state for back trace
 *
 * << Argument >>
 * name			(I/O)	type			description
 * btree         I      BTREE *         current back trace tree
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/8/26
 *
 ********************************************************************************/


STATE_3 atpg_check_reverse_state_btree
(
 BTREE *btree
){
	STATE_3 state3;
	STATE_3 r_state3;
	Ulong flag;
	Uint  reverse_condition = 0;
	
	char *func_name = "atpg_check_reverse_state_btree"; /* �ؿ�̾ */

	flag = btree->flag;

	state3 = STATE3_C; /* initialize */

	topgun_print_mes ( 18, 0 );
	
	/* �̾�������Ǥ�ξ����Ƥ��� */
	if ( ( ( flag & ASS_N_0 ) && ( flag & ASS_N_1 ) )|| 
		 ( ( flag & ASS_F_0 ) && ( flag & ASS_F_1 ) ) ) {

		topgun_print_mes ( 18, 2 );
		return ( STATE3_C ); /* ȿž�ͤ�����Ǥ��ʤ� */
	}

	/* �դ�condition(�� or ��)����� */
	reverse_condition = atpg_reverse_condition ( btree->condition );

	/* �դ�condition��state����� */
	r_state3 = atpg_get_state9_with_condition_2_state3
		( btree->line->state9, reverse_condition );
		
	if ( ( btree->line->flag & FP_IAP ) ||
		 ( r_state3 == STATE3_X ) ) {
	
		/* ������/�ξ��ͤ�ξ��������Ǥ��� */
	
		/* ass_stat�������ꤷ���ͤ��Ȥ�� */
		switch ( btree->ass_state3 ) {
		case STATE3_0:
			state3 = STATE3_1;
			break;
		case STATE3_1:
			state3 = STATE3_0;
			break;
		case STATE3_X:
		case STATE3_U:
		default:
			topgun_error( FEC_PRG_BACK_TRACK, func_name );		
		}
	}
	else {
		topgun_print_mes ( 18, 3 );
	}

	return ( state3 );
}

/********************************************************************************
 * << Function >>
 *		back track on justified
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  ASS_R_CONF  : conflict reverse state for back trace
 *  STAT_??     : reverse state for back trace
 *
 * << Argument >>
 * name			(I/O)	type			description
 * btree         I      BTREE *         current back trace tree
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/8/26
 *
 ********************************************************************************/


void atpg_enter_reverse_state
(
 STATE_3  r_state3,
 BTREE    *btree,
 ASS_LIST *ass_list
){
	
	char *func_name = "atpg_enter_reverse_state"; /* �ؿ�̾ */
	
	/* �դ��ͤ���Ͽ */
	btree->ass_state3 = r_state3;

	/* btree->flag���ͤ���Ͽ��Ͽ���� */

	test_condition ( btree->condition, func_name );

	if ( btree->condition == COND_NORMAL ) {
		ass_list->condition = COND_NORMAL;
		if ( r_state3 & STATE3_0 ) {
			btree->flag |= ASS_N_0;
		}
		if ( r_state3 & STATE3_1 ) {
			btree->flag |= ASS_N_1;
		}
	}
	else {
		ass_list->condition = COND_FAILURE;
		if ( r_state3 & STATE3_0 ) {
			btree->flag |= ASS_F_0;
		}
		if ( r_state3 & STATE3_1 ) {
			btree->flag |= ASS_F_1;
		}
	}

	ass_list->ass_state3  = r_state3;

	/* *** �ѹ���ɬ�� *** */
	ass_list->line     = btree->line;
	ass_list->next     = NULL;

	test_ass_list ( ass_list, func_name );
	
}


/********************************************************************************
 * << Function >>
 *		and gate justisfied 
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	ass			: justified list
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/4
 *		2005/9/28 for multi input  and multi gate
 *
 ********************************************************************************/

ASS_LIST  *atpg_line_justified_and_nand_or_nor
(
 JUST_INFO *just_info
 ){

	LINE     *line;
	Uint     condition;
	Ulong    i;         // ���Ͽ�������������
	Ulong    ass_in;    // �ͤ���������Ͽ����������ֹ�

	STATE_3  line_state3;
	STATE_3  in_state3;

	ASS_LIST *ass_list = NULL;   // �ͳ�����Ƥο���������

	char    *func_name = "atpg_line_justified_and_nand_or_nor"; /* �ؿ�̾ */

	// �Ȥꤢ���������ͤ����ͤ���
	/* $$$ ����!! �ξ��ͤ�ɬ�� 2005/6/2 sub��
	   -> ���䤳��ϴް����Ǥ�?? $$$ */

	test_just_info ( just_info, func_name );

	line      = just_info->line;
	condition = just_info->condition;

	line_state3 = atpg_get_state9_with_condition_2_state3
		( line->state9, condition );

	if ( line_state3 == Justify_line_state[ line->type ] ) {

		/* �ɤ�����0(and/nand) or 1(or/nor) �����ꤹ�� */
		/* ��state��X�Ǥ��ľ������������������ */
		/* 2005/09/29�Ǥξ���tm0�������㤤������ */
		/* $$$ ¿���ϥ������ʿ��ʤɤ���θ�θ��� $$$ */

		ass_list = ( ASS_LIST * )topgunMalloc
			( FMT_ASS_LIST, sizeof( ASS_LIST ), 1, func_name );

		ass_list->next = NULL;


		/* �ͤ�����뿮��������� */
		ass_in = line->n_in;

		for ( i = 0 ; i < line->n_in ; i++ ) {
			/* �ޤ�in[i]���ͤ�X��Ĵ�٤� */

			in_state3 = atpg_get_state9_with_condition_2_state3
				(  line->in[ i ]->state9, condition );

			if ( STATE3_X == in_state3 ) {
				/* �ɤ����Ͽ����������򤹤�Τ� */
				if ( atpg_justified_cand_update( line, i, ass_in ) ) {
					/* �������� */
					ass_in = i;
				}
			}
		}

#ifndef TOPGUN_TEST
		if ( ass_in == line->n_in ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
#endif /* TOPGUN_TEST */

		/* line������ */
		ass_list->line = line->in [ ass_in ];

		/* pre_stat������ */
		/* **** �׳�ǧ *** */
		/*
		ass->pre_stat = ass->line->stat;
		*/

		/* ass_stat������ */
		ass_list->ass_state3 = Justified_line_state[line->type];

		/* condition ������ */

		ass_list->condition = COND_NORMAL ;
		if ( condition == COND_FAILURE ) {
			if ( line->flag & FV_ASS ) {
				ass_list->condition = condition ;
			}
		}
	}
	else {
		/* �����������ͤ����������ʤ��ξ�� */
		topgunFree( ass_list, FMT_ASS_LIST, 1, func_name );
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	test_ass_list ( ass_list, func_name );

	return( ass_list );
}




/********************************************************************************
 * << Function >>
 *		�ɤ����Ͽ����������򤹤�Τ����Ӥ��ؿ�
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  0           : stay
 *  1           : update
 *  if()ʸ��Ƚ�ꤹ�뤿��
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line of start point for justified
 * cand_in       I      Ulong           the number of new candidate input line
 * ass_in        I      Ulong           the number of original candidate input line 
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/9/28
 *
 ********************************************************************************/


int atpg_justified_cand_update
(
 LINE  *line,
 Ulong cand_in,
 Ulong ass_in
){

	Ulong 	cand_value; /* ���ߤθ����ɾ���� */
	Ulong 	ass_value;  /* �����θ����ɾ���� */
	
	if ( ass_in == line->n_in ) {
		/* ���䤬�ޤ������äƤ��ʤ� */
		/* -> �������� */
		return ( 1 ) ;
	}

	/* ���ߤθ�����ͤ���� */
	cand_value = atpg_justified_get_cand_value ( line->in[ cand_in ] );
	
	/* �����θ�����ͤ���� */
	/* $$$ ��֤�����ʤ�����ˤ���꤬���� $$$ */
	ass_value  = atpg_justified_get_cand_value ( line->in[ ass_in  ] );
		
	return ( atpg_justified_judge ( cand_value , ass_value ) );
}

/********************************************************************************
 * << Function >>
 *		�ɤ����Ͽ����������򤹤���Ȥʤ��ͤ����ؿ�
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  value       : �����������򤹤�����
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line for calclurate judgement value
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/9/28
 *
 ********************************************************************************/


Ulong atpg_justified_get_cand_value 
(
 LINE  *line
){
	Ulong value = 0;

	char  *func_name = "atpg_justified_get_cand_value"; /* �ؿ�̾ */

	/* 2005/09/29�����Ǥ�tm */
	switch ( line->out[0]->type ) {
	case TOPGUN_AND:
	case TOPGUN_NAND:
		value = line->tm_c0;
		break;
	case TOPGUN_OR:
	case TOPGUN_NOR:
		value = line->tm_c1;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
			
	return ( value );
}

/********************************************************************************
 * << Function >>
 *		�ɤ����Ͽ����������򤹤���Ȥʤ��ͤ������򤹤�ؿ�
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  0           : stay
 *  1           : update
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line for calclurate judgement value
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/9/28
 *
 ********************************************************************************/


int atpg_justified_judge
(
 Ulong cand_value ,
 Ulong ass_value
 ){

	/* 2005/09/28�Ǥ�min */
	/* $$$ ���ޥ�ɲ���ɬ�� $$$ */
	if ( cand_value < ass_value ) {
		return ( 1 );
	}
	else {
		return ( 0 );
	}
}

/********************************************************************************
 * << Function >>
 *		judge "AND/NAND/OR/NOR" gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/7
 *		2005/9/29 multi gate and multi input
 *
 ********************************************************************************/

JUST_RESULT atpg_check_cand_justified_and_nand_or_nor
(
 JUST_INFO *just_info
 ){
	STATE_3 in_state3;
	STATE_3 out_state3;

	LINE        *line  = just_info->line;
	JUST_RESULT just_result = JUST_ERROR; /* ��������ѿ��ν���� */

	Ulong   input_n;  /* the number of justified line */
	Ulong   i;        /* counter */

	char *func_name = "atpg_check_cand_justified_and_nand_or_nor"; /* �ؿ�̾ */

	test_just_info ( just_info, func_name );

	in_state3 = STATE3_C ; /* initialize */

	if ( just_info->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		out_state3 = atpg_get_state9_2_normal_state3 ( line->state9 );
	}
	else {
		/* COND_FAILURE */
		out_state3 = atpg_get_state9_2_failure_state3 ( line->state9 );
	}

	/* �����ͤ˱����ơ��ɤο����������������뤫�ͤ��� */

	if ( out_state3 == Justified_single_state[ line->type ] ) {

		input_n = FL_ULMAX; /* ����� */

		/* �ɤ����Ͽ������������ͤʤΤ�Ĵ�٤� */
		for ( i = 0 ;  i < line->n_in ; i++ ) {

			if ( just_info->condition == COND_NORMAL ) {
				in_state3 = atpg_get_state9_2_normal_state3
					( line->in[ i ]->state9 );
			}
			else {
				in_state3 = atpg_get_state9_2_failure_state3
					( line->in[ i ]->state9 );
			}


			if ( in_state3 == Justified_line_state[ line->type ] ) {
				/* �����ͤξ�� */
				/* $$$ �ʰ��� $$$ */
				input_n = i;
				break;
			}
			else {
				/* �����ͤǤʤ���� */
				;
			}
		}

		/* �����ͤ����Ϥ����뤫 */
		if( input_n != FL_ULMAX ) {
			/* �����ͤ����Ϥ����ä���� */
			just_result = JUST_1IN;
			just_result += ( JUST_1IN_N * input_n );
		}
		else {
			/* �����ͤ����Ϥ��ʤ��ä���� = ALL X */
			just_result = JUST_OWN;
		}
	}
	else {
		if ( out_state3 == Justified_allin_state[ line->type ] ) {
			/* ���٤��������ͤξ��(AND/NOR = 1 , OR/NAND == 0) */
			just_result = JUST_ALL;
		}
		else {
			/* X or U�ξ�� */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}

	return ( just_result );
}

/********************************************************************************
 * << Function >>
 *		xor gate justisfied
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	ass			: justified list
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/8/2
 *		2005/10/21 for condition
 *
 ********************************************************************************/

/* �ͳ������1�Ľ����version */

ASS_LIST  *atpg_line_justified_xor
(
 JUST_INFO *just_info
 ){
	LINE	 *in0;
	LINE     *in1;
	
	ASS_LIST *ass_list;
	
	STATE_3  line_state3;
	STATE_3  in0_state3;
	STATE_3  in1_state3;

	char     *func_name = "atpg_line_justified_xor"; /* �ؿ�̾ */

	test_just_info ( just_info, func_name );
	test_line_in_null ( just_info->line, func_name );
	
	ass_list = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ),
										1, func_name );
	/* inialized */
	ass_list->next  = NULL;
	ass_list->ass_state3 = STATE3_X;
	ass_list->condition = just_info->condition;

	in0      = just_info->line->in[ 0 ];
	in1      = just_info->line->in[ 1 ];
	
	/* �Ȥꤢ����������ͥ��Ǥ�äƤߤ� */
	if ( ass_list->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		line_state3 = atpg_get_state9_2_normal_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_normal_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_normal_state3 ( in1->state9 );
	}
	else {
		/* COND_FAILURE */
		line_state3 = atpg_get_state9_2_failure_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_failure_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_failure_state3 ( in1->state9 );
	}


	/* �Ȥꤢ���������ͤ����ͤ��� */
	switch ( line_state3 ) {
	case STATE3_0:
		/* 11 or 00 �ˤ��� */

		/* ������X, ������0 */
		/* case 1: in0 0 / in1 X -> in1 �� 0��                */
		/* case 2: in0 X / in1 0 -> in1  0 ������justified    */

		/* ������X, ������1 -> X��1��assign */ 
		/* case 3: in0 1 / in1 X -> in1  0 ������justified    */
		/* case 4: in0 X / in1 1 -> high 0 ������justified    */

		/* ξ����X��11��00�ΰ¤��� */
		/* case 5: in0 X / in1 X -> in1  0 ������justified    */


		/* NG(�ͤ����˷�äƤ��뤿��) */
		/* case 6: in0 0 / in1 0 -> just flag ����            */
		/* case 7: in0 0 / in1 1 -> conflict */
		/* case 8: in0 1 / in1 0 -> in1  0 ������justified    */
		/* case 9: in0 1 / in1 1 -> �ް����ߥ�(ERROR)       */

		/* case��ʬ�������׸�Ƥ */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:
				/* case 5 */

				/* 00��11�Τɤ��餬�ưפ�? */
				if ( ( in0->tm_c0 + in1->tm_c0 ) >
					 ( in0->tm_c1 + in1->tm_c1 ) ) {

					/* 11�ˤ��� */
					ass_list->ass_state3 = STATE3_1;
					
					/* �ɤä���1����������񤤤�
					   (�¤Ϥɤä��Ǥ⤤����®�٤�����) */
					if ( in0->tm_c1 > in1->tm_c1 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				else{
					/* 00�ˤ��� */
					ass_list->ass_state3 = STATE3_0;
					/* �ɤä���0����������񤤤�
					   (�¤Ϥɤä��Ǥ⤤����®�٤�����) */
					if ( in0->tm_c0 > in1->tm_c0 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				break;
			case STATE3_0:
				/* case 2 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 4 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if ( STATE3_X == in1_state3 ) {
			switch( in0_state3 ) {
			case STATE3_0:
				/* case 1 */
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 3 */
				ass_list->line              = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_X:
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_1:
		/* 01 or 10 �ˤ��� */

		/* ������X, ������0 */ 
		/* case 1: in0 0 / in1 X -> in1 �� 1�� */
		/* case 2: in0 X / in1 0 -> in0 �� 1�� */

		/* ������X, ������1 -> X��0��assign */ 
		/* case 3: in0 1 / in1 X -> in1 �� 0�� */
		/* case 4: in0 X / in1 1 -> in0 �� 0�� */

		/* ξ����X��01��10�ΰ¤��� */
		/* case 5: in0 X / in1 X -> tm������å� */

		/* NG(�ͤ����˷�äƤ��뤿��) */
		/* case 6: in0 0 / in1 0 */
		/* case 7: in0 0 / in1 1 */
		/* case 8: in0 1 / in1 0 */
		/* case 9: in0 1 / in1 1 */

		/* case��ʬ�������׸�Ƥ */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:

				/* 01��10�Τɤ��餬�ưפ�? */
				if ( (in0->tm_c0 + in1->tm_c1 ) >
				     (in0->tm_c1 + in1->tm_c0 ) ) {
					/* 01�ˤ��� */
					if( in0->tm_c0 > in1->tm_c1 ) {
						/* in0��0�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line              = in0;
					}
					else {
						/* in1��1�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line              = in1;
					}
				}
				else {
					/* 10�ˤ��� */
					if( in0->tm_c1 > in1->tm_c0 ) {
						/* in0��1�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line              = in0;
					}
					else {
						/* in1��0�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line              = in1;
					}
				}
				break;
			case STATE3_0:
				ass_list->line              = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if( STATE3_X == in1_state3 ) {
			switch ( in0_state3 ) {
			case STATE3_0:
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_X:
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_X:
	default:
		/* ��������� */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	test_ass_list ( ass_list, func_name );
	
	return( ass_list );
}

/********************************************************************************
 * << Function >>
 *		xnor gate justisfied
 *
 * << Function ID >>
 *    	???
 *
 * << Return >>
 *	ass			: justified list
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line			 I		LINE *			line pointer of justified
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/4
 *
 ********************************************************************************/

ASS_LIST  *atpg_line_justified_xnor
(
 JUST_INFO *just_info
 ){

	LINE	 *in0;
	LINE     *in1;

	ASS_LIST *ass_list;

	STATE_3  line_state3;
	STATE_3  in0_state3;
	STATE_3  in1_state3;

	char     *func_name = "atpg_line_justified_xnor"; /* �ؿ�̾ */

	test_just_info ( just_info, func_name );
	test_line_in_null ( just_info->line, func_name );

	ass_list = ( ASS_LIST * )topgunMalloc( FMT_ASS_LIST, sizeof( ASS_LIST ),
										1, func_name );

	/* inialized */
	ass_list->next  = NULL;
	ass_list->ass_state3 = STATE3_X;
	ass_list->condition = just_info->condition;
	
	in0      = just_info->line->in[ 0 ];
	in1      = just_info->line->in[ 1 ];

	/* �Ȥꤢ����������ͥ��Ǥ�äƤߤ� */
	if ( ass_list->condition == COND_NORMAL ) {
		/* COND_NORMAL */
		line_state3 = atpg_get_state9_2_normal_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_normal_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_normal_state3 ( in1->state9 );
	}
	else {
		/* COND_FAILURE */
		line_state3 = atpg_get_state9_2_failure_state3 ( just_info->line->state9 );
		in0_state3  = atpg_get_state9_2_failure_state3 ( in0->state9 );
		in1_state3  = atpg_get_state9_2_failure_state3 ( in1->state9 );
	}
	
	/* �Ȥꤢ���������ͤ����ͤ��� */
	switch ( line_state3 ) {
	case STATE3_1:   /* <<-- xor�Ȥΰ㤤 (1/2) */
		/* 11 or 00 �ˤ��� */

		/* ������X, ������0 */
		/* case 1: in0 0 / in1 X -> in1 �� 0��                */
		/* case 2: in0 X / in1 0 -> in1  0 ������justified    */

		/* ������X, ������1 -> X��1��assign */ 
		/* case 3: in0 1 / in1 X -> in1  0 ������justified    */
		/* case 4: in0 X / in1 1 -> high 0 ������justified    */

		/* ξ����X��11��00�ΰ¤��� */
		/* case 5: in0 X / in1 X -> in1  0 ������justified    */


		/* NG(�ͤ����˷�äƤ��뤿��) */
		/* case 6: in0 0 / in1 0 -> just flag ����            */
		/* case 7: in0 0 / in1 1 -> conflict */
		/* case 8: in0 1 / in1 0 -> in1  0 ������justified    */
		/* case 9: in0 1 / in1 1 -> �ް����ߥ�(ERROR)       */

		/* case��ʬ�������׸�Ƥ */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:
				/* case 5 */

				/* ¿ʬ¿���Ȼפ� */
				ass_list->line = in0;

				/* 00��11�Τɤ��餬�ưפ�? */
				if ( ( in0->tm_c0 + in1->tm_c0 ) >
					 ( in0->tm_c1 + in1->tm_c1 ) ) {
					/* 11�ˤ��� */
					ass_list->ass_state3 = STATE3_1;
					/* �ɤä���1����������񤤤�
					   (�¤Ϥɤä��Ǥ⤤����®�٤�����) */
					if ( in0->tm_c1 > in1->tm_c1 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				else{
					/* 00�ˤ��� */
					ass_list->ass_state3 = STATE3_0;
					/* �ɤä���0����������񤤤�
					   (�¤Ϥɤä��Ǥ⤤����®�٤�����) */
					if ( in0->tm_c0 > in1->tm_c0 ) {
						ass_list->line = in0;
					}
					else {
						ass_list->line = in1;
					}
				}
				break;
			case STATE3_0:
				/* case 2 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 4 */
				ass_list->line = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if ( STATE3_X == in1_state3 ) {
			switch( in0_state3 ) {
			case STATE3_0:
				/* case 1 */
				ass_list->line              = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_1:
				/* case 3 */
				ass_list->line              = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_X:
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_0: /* <<-- xor�Ȥΰ㤤 (1/2) */
		/* 01 or 10 �ˤ��� */

		/* ������X, ������0 */ 
		/* case 1: in0 0 / in1 X -> in1 �� 1�� */
		/* case 2: in0 X / in1 0 -> in0 �� 1�� */

		/* ������X, ������1 -> X��0��assign */ 
		/* case 3: in0 1 / in1 X -> in1 �� 0�� */
		/* case 4: in0 X / in1 1 -> in0 �� 0�� */

		/* ξ����X��01��10�ΰ¤��� */
		/* case 5: in0 X / in1 X -> tm������å� */

		/* NG(�ͤ����˷�äƤ��뤿��) */
		/* case 6: in0 0 / in1 0 */
		/* case 7: in0 0 / in1 1 */
		/* case 8: in0 1 / in1 0 */
		/* case 9: in0 1 / in1 1 */

		/* case��ʬ�������׸�Ƥ */
		if ( STATE3_X == in0_state3 ) {
			switch ( in1_state3 ) {
			case STATE3_X:

				/* 01��10�Τɤ��餬�ưפ�? */
				if ( (in0->tm_c0 + in1->tm_c1 ) >
				     (in0->tm_c1 + in1->tm_c0 ) ) {
					/* 01�ˤ��� */
					if( in0->tm_c0 > in1->tm_c1 ) {
						/* in0��0�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line = in0;
					}
					else {
						/* in1��1�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line = in1;
					}
				}
				else {
					/* 10�ˤ��� */
					if( in0->tm_c1 > in1->tm_c0 ) {
						/* in0��1�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_1;
						ass_list->line = in0;
					}
					else {
						/* in1��0�ˤ���ۤ����񤷤� */
						ass_list->ass_state3 = STATE3_0;
						ass_list->line = in1;
					}
				}
				break;
			case STATE3_0:
				ass_list->line       = in0;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line       = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else if( STATE3_X == in1_state3 ) {
			switch ( in0_state3 ) {
			case STATE3_0:
				ass_list->line       = in1;
				ass_list->ass_state3 = STATE3_1;
				break;
			case STATE3_1:
				ass_list->line       = in1;
				ass_list->ass_state3 = STATE3_0;
				break;
			case STATE3_X:
			default:
				/* ��������� */
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else{
			/* case 6, case 7, case 8, case 9 */
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
		break;
	case STATE3_X:
	default:
		/* ��������� */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	test_ass_list ( ass_list, func_name );
	
	return( ass_list );
}

/********************************************************************************
 * << Function >>
 *		����������ɬ�פ����뿮�����ξ�����������
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  JUST_INFO    : ���������뿮�����ξ���
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      LINE *          line for calclurate judgement value
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/9
 *
 ********************************************************************************/


JUST_INFO *atpg_make_just_info
(
 LINE    *line,
 Uint    condition
 ){
	JUST_INFO   *just_info;

	char *func_name = "atpg_make_just_info"; /* �ؿ�̾ */

	test_line_null ( line, func_name );

	/* just_info�Υ������ */
	just_info = ( JUST_INFO * )topgunMalloc( FMT_JUST_INFO, sizeof( JUST_INFO ),
											  1, func_name );

	just_info->line       = line;
	just_info->condition  = condition;
	just_info->next       = NULL;

	return ( just_info );
}

/********************************************************************************
 * << Function >>
 *		����������ɬ�פ����뿮�����ξ����btree����Ͽ����
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *     void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justified infomation
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/9
 *
 ********************************************************************************/


void atpg_enter_btree_just_info
(
 JUST_INFO *just_info
 ){
	BTREE *btree = Gene_head.btree;
	
	char  *func_name = "atpg_enter_just_info"; /* �ؿ�̾ */

	test_just_info_null ( just_info, func_name );
	test_btree_null ( btree, func_name );

	/* btree�ؤ���Ͽ */
	just_info->next = btree->just_e;
	btree->just_e   = just_info;

}

/********************************************************************************
 * << Function >>
 *		����������ɬ�פ����뿮�����ξ����Lv_head����Ͽ����
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *     void
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justified infomation
 *
 * << extern >>
 * name			(I/O)	type			description
 *	Lv_head      I		JUST_INFO **	levelize header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/9
 *
 ********************************************************************************/


void atpg_enter_lv_head_just_info
(
 JUST_INFO *just_info
 ){

	char  *func_name = "atpg_enter_lv_head_just_info"; /* �ؿ�̾ */

	test_just_info_null ( just_info, func_name );

	/* btree�ؤ���Ͽ */

	/* Lv_head����Ͽ */
	just_info->next = Lv_head[ just_info->line->lv_pi ];
	Lv_head[ just_info->line->lv_pi ] = just_info;

}


/********************************************************************************
 * << Function >>
 *		����������ɬ�פ����뿮�����ξ�����������
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *  start_lv    : ���������뿮�����κ����٥�
 *
 * << Argument >>
 * name			(I/O)	type			description
 *     void
 *
 * << extern >>
 * name			(I/O)	type			description
 *	ATPG_HEAD	 I		Gene_head		infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/09
 *
 ********************************************************************************/


Ulong atpg_get_just_info
(
 void
 ){
	JUST_INFO *just_e    = NULL;
	JUST_INFO *just_info = NULL;
	Ulong     start_lv   = 0;

	char  *func_name = "atpg_get_just_info"; /* �ؿ�̾ */

	test_btree_null ( Gene_head.btree, func_name );

	/* ���٤�NULL����ǧ���� */
	test_lv_head_null ( func_name );

	if ( Gene_head.btree->prev == NULL ) {
		/* �����������򳫻Ϥ����� */
		start_lv = atpg_startlist_justified ();
	}
	else {
		/* ���Ǥ��������� */
		just_e = Gene_head.btree->prev->just_e;

		while ( just_e ) {

			/* ñ�ʤ륳�ԡ��������
			   $$$ ��ä��ɤ�����������뤫�� $$$ */
			just_info = atpg_make_just_info ( just_e->line, just_e->condition );

			/* Lv_head����Ͽ */
			atpg_enter_lv_head_just_info ( just_info );

			/* start_lv�򹹿����� */
			if ( start_lv < just_e->line->lv_pi ) {
				start_lv = just_e->line->lv_pi;
			}

			/* �Ĥ�����Ͽ�� */
			just_e = just_e->next;
		}
	}

	return ( start_lv );
}

/********************************************************************************
 * << Function >>
 *		���������뿮�����ξ����õ��
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/


void atpg_btree_just_info_reflesh
(
 JUST_INFO *just_info
 ){
	JUST_INFO *tmp_just_info; /* ���곫���� */

	char  *func_name = "atpg_btree_just_info_reflesh"; /* �ؿ�̾ */

	while ( just_info ) {

		if ( just_info->condition == COND_NORMAL ) {
			/* COND_NORMAL */
			just_info->line->flag &= ~JUSTIFY_NO;
		}
		else {
			/* COND_FAILURE */
			just_info->line->flag &= ~JUSTIFY_FL;
		}
		
		/* �����᤹����ν��� */
		tmp_just_info = just_info;
		just_info     = just_info->next;



		topgunFree( tmp_just_info, FMT_JUST_INFO, 1, func_name );
	}
}


/********************************************************************************
 * << Function >>
 *		0���ͤ������Ƥ�����񤷤������������
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_difficult_0
(
 LINE *line
 ){
	LINE  *in   = NULL; // ���Ͽ������Υݥ���
	Ulong score = 0;    // �񤷤��������
	Ulong i     = 0;    // ���������Ͽ��Υ�����

	STATE_3 state3 = STATE3_X;
	
	char  *func_name = "atpg_line_justified_trace_difficult_0"; // �ؿ�̾

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score < line->in[ i ]->tm_c0 ) {

			//$$$ �ʤ󤫥����å��ؿ��ˤ��������ɤ��� $$$
			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); // 9�ͤ��������ͤ���Ф�

			if ( state3 == STATE3_X ){
				score = line->in[ i ]->tm_c0;
				in    = line->in[ i ];
			}
		}
	}

	{
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		0���ͤ������Ƥ���ְפ��������������
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_easy_0
(
 LINE *line
 ){
	LINE  *in   = NULL;     // ���Ͽ������Υݥ���
	Ulong score = FL_ULMAX; // �񤷤��������
	Ulong i     = 0;        // ���������Ͽ��Υ�����
	STATE_3 state3 = STATE3_X;

	char  *func_name = "atpg_line_justified_trace_easy_0"; // �ؿ�̾

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score > line->in[ i ]->tm_c0 ) {

			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); // 9�ͤ��������ͤ���Ф�

			if ( state3 == STATE3_X ){
				score = line->in[ i ]->tm_c0;
				in    = line->in[ i ];
			}
		}
	}
	{
		// �����å���
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		1���ͤ������Ƥ�����񤷤������������
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_difficult_1
(
 LINE *line
 ){
	LINE  *in   = NULL; // ���Ͽ������Υݥ���
	Ulong score = 0;    // �񤷤��������
	Ulong i     = 0;    // ���������Ͽ��Υ�����

	STATE_3 state3 = STATE3_X;
	
	char  *func_name = "atpg_line_justified_trace_difficult_0"; // �ؿ�̾

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score < line->in[ i ]->tm_c1 ) {
			//$$$ �ʤ󤫥����å��ؿ��ˤ��������ɤ��� $$$
			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); // 9�ͤ��������ͤ���Ф�

			if ( STATE3_X == state3 ) {
				score = line->in[ i ]->tm_c1;
				in    = line->in[ i ];
			}
		}
	}

	{
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		1���ͤ������Ƥ���ְפ��������������
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      in    : line pointer
 *
 * << Argument >>
 * name			(I/O)	type			description
 * line          I      line *          line pointer
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/12/07
 *
 ********************************************************************************/

extern inline LINE *atpg_line_justified_trace_easy_1
(
 LINE *line
 ){
	LINE  *in      = NULL;     // ���Ͽ������Υݥ���
	Ulong score    = FL_ULMAX; // �񤷤��������
	Ulong i        = 0;        // ���������Ͽ��Υ�����
	STATE_3 state3 = STATE3_X;
		
	char  *func_name = "atpg_line_justified_trace_easy_1"; // �ؿ�̾

	for ( i = 0 ; i < line->n_in; i++ ) {

		if ( score > line->in[ i ]->tm_c1 ) {
			state3 = atpg_get_state9_2_normal_state3 ( line->in[ i ]->state9 ); /* 9�ͤ��������ͤ���Ф� */
			if ( state3 == STATE3_X ) {
				score = line->in[ i ]->tm_c1;
				in    = line->in[ i ];
			}
		}
	}
	{
		// �����å���
		if ( in == NULL ) {
			topgun_error( FEC_PRG_JUSTIFIED, func_name );
		}
	}
	
	return ( in );
}

/********************************************************************************
 * << Function >>
 *		���������뿮������������椷�䤹���������Ϥ��ѹ�����
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/


void atpg_line_justified_single_path_trace
(
 ASS_LIST *org_ass_list
 ){
	LINE  *line = org_ass_list->line;
	STATE_3 state3 = org_ass_list->ass_state3;
	
	char  *func_name = "atpg_line_justified_single_path_trace_input"; // �ؿ�̾
	
	topgun_3_utility_enter_start_time ( &Time_head.justified_spt );
	
	// ���٤Ƥ������ͤ�ɬ�פʤȤ�(AND�ν���1�ʤ�)�ϰ����񤷤����(̵����Ͻ���)
	// 1�Ĥ������ͤ�ɬ�פʤȤ�(AND�ν���0�ʤ�)�ϰ���ͥ�������

	while ( 1 ) {

		// ���Ϥ��ڤ�Ƥ���Ȥ����б���˺�줺��
		if ( line->type == TOPGUN_PI ) {
			break;
		}

		if ( state3 == STATE3_0 ) {

			// ���ߤο������γ�������ͤ�0�ξ��

			// �ؿ��Υݥ��󥿲���switch��ʤ���
			switch ( line->type ) {
			
			case TOPGUN_OR:
				// 0�ǰ����񤷤�����
				line = atpg_line_justified_trace_difficult_0 ( line );
				break;
			case TOPGUN_NOR:
				// 1�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_1 ( line );
				state3 = STATE3_1;
				break;
			case TOPGUN_AND:
				// 0�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_0 ( line );
				break;
			case TOPGUN_NAND:
				// 1�ǰ����񤷤�����
				line = atpg_line_justified_trace_difficult_1 ( line );
				state3 = STATE3_1;
				break;
			case TOPGUN_INV:
				state3 = STATE3_1;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else {
			// ���ߤο������γ�������ͤ�1�ξ��
			switch ( line->type ) {
			
			case TOPGUN_OR:
				// 1�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_1 ( line );
				break;
			case TOPGUN_NOR:
				// 0�ǰ����񤷤�����
				line = atpg_line_justified_trace_difficult_0 ( line );
				state3 = STATE3_0;
				break;
			case TOPGUN_AND:
				// 1�ǰ����񤷤�����
				line = atpg_line_justified_trace_difficult_1 ( line );
				break;
			case TOPGUN_NAND:
				// 0�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_0 ( line );
				state3 = STATE3_0;
				break;
			case TOPGUN_INV:
				state3 = STATE3_0;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
	}
	org_ass_list->line       = line;
	org_ass_list->ass_state3 = state3;
	
	topgun_3_utility_enter_end_time ( &Time_head.justified_spt );
}

/********************************************************************************
 * << Function >>
 *		���������뿮������������椷�䤹���������Ϥ��ѹ�����
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2007/02/20
 *
 ********************************************************************************/


void atpg_line_justified_single_path_trace_with_implying_node
(
 ASS_LIST *org_ass_list
 ){
	LINE  *line = org_ass_list->line;
	STATE_3 state3 = org_ass_list->ass_state3;
	
	char  *func_name = "atpg_line_justified_single_path_trace_input"; // �ؿ�̾
	
	topgun_3_utility_enter_start_time ( &Time_head.justified_spt );
	
	// 1�Ĥ������ͤ�ɬ�פʤȤ�(AND�ν���0�ʤ�)�ϰ���ͥ�������
	// ���٤Ƥ������ͤ�ɬ�פʤȤ�(AND�ν���1�ʤ�)�Ϥ�����

	while ( 1 ) {

		// ���Ϥ��ڤ�Ƥ���Ȥ����б���˺�줺��
		if ( line->type == TOPGUN_PI ) {
			break;
		}

		if ( state3 == STATE3_0 ) {

			// ���ߤο������γ�������ͤ�0�ξ��
			// �ؿ��Υݥ��󥿲���switch��ʤ���
			switch ( line->type ) {
			case TOPGUN_OR:
				// ���٤�0
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_0;
				return;
			case TOPGUN_NOR:
				// 1�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_1 ( line );
				state3 = STATE3_1;
				break;
			case TOPGUN_AND:
				// 0�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_0 ( line );
				break;
			case TOPGUN_NAND:
				// ���٤�1
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_0;
				return;
			case TOPGUN_INV:
				state3 = STATE3_1;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
		else {
			// ���ߤο������γ�������ͤ�1�ξ��
			switch ( line->type ) {
			
			case TOPGUN_OR:
				// 1�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_1 ( line );
				break;
			case TOPGUN_NOR:
				// ���Ͽ��������٤Ƥ�0�ˤ���
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_1;
				return;
			case TOPGUN_AND:
				// ���Ͽ��������٤Ƥ�1�ˤ���
				org_ass_list->line       = line;
				org_ass_list->ass_state3 = STATE3_1;
				return;
			case TOPGUN_NAND:
				// 0�Ǥɤ�Ǥ�褤�ɤ�
				line = atpg_line_justified_trace_easy_0 ( line );
				state3 = STATE3_0;
				break;
			case TOPGUN_INV:
				state3 = STATE3_0;
				line = line->in[0];
				break;
			case TOPGUN_BUF:
			case TOPGUN_BR:
				line = line->in[0];
				break;
			case TOPGUN_PI:
			case TOPGUN_PO:
			case TOPGUN_XOR:
			case TOPGUN_XNOR:
			case TOPGUN_BLKI:
			case TOPGUN_BLKO:
			case TOPGUN_UNK:
			default:
				topgun_error( FEC_PRG_JUSTIFIED, func_name );
			}
		}
	}
	org_ass_list->line       = line;
	org_ass_list->ass_state3 = state3;
	
	topgun_3_utility_enter_end_time ( &Time_head.justified_spt );
}

/********************************************************************************
 * << Function >>
 *		���������뿮������������椷�䤹���������Ϥ��ѹ�����
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/


void atpg_line_justified_extend_large_line 
(
 ASS_LIST *org_ass_list
 ){
}


/********************************************************************************
 * << Function >>
 *		���������뿮������õ���򵭲�����(for debug)
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *      void   : 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * just_info     I      JUST_INFO *     justify infomation for free
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/17
 *
 ********************************************************************************/
#ifdef TOPGUN_NOUSE
void atpg_justified_packjust_info
(
 JUST_INFO  *just_info
 ){

	char  *func_name = "atpg_justify_pack_just_info"; /* �ؿ�̾ */


	new_stack = ( LINE_STACK * )topgunMalloc(FMT_LINE_STACK, sizeof( LINE_STACK ),
											  1, func_name );

	new_stack->line = just_info->line;
	new_stack->next = flag_stack;
	flag_stack      = new_stack;

	return ( flag_stack );

}




/********************************************************************************
 * << Function >>
 *		judge and gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-3
 *
 * << Return >>
 *	result		: justified line result
 *                JUST_OWN (0x04)      �оݿ���������
 *                JUST_ALL (0x08)      ���Ϥ�����
 *                JUST_1IN (0x10)      ���ϤΤҤȤĤɤ��� 
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *      2005/09/29 multi gate
 *
 ********************************************************************************/

int atpg_check_cand_justified_and
(
 LINE *line,
 Ulong flag
 ){

	int  result = 0; /* ��������ѿ��ν���� */
	void topgun_error();

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */

	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();

	
	char  *func_name = "atpg_check_cand_justified_and"; /* �ؿ�̾ */

	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}


	/* �����ͤ˱����ơ��ɤο����������������뤫�ͤ��� */
	switch ( out ) {
	case STATE3_1:
		/* ���Ϥ�1�ΤȤ���ξ���������ͤ�ɬ�� */
		result |= JUST_ALL;
		break;

	case STATE3_0:
		/* ���Ϥ�0�ΤȤ��Ϥɤä��������������ͤ�ɬ�� */
		if ( (in0 == STATE3_0) &&
			 (in1 == STATE3_0) ) {
			/* in[0] = 0, in[1] = 0 */
			/* $$$ �Ȥꤢ���� $$$ */
			
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */
			
		}
		else if ( in0 == STATE3_0 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */
			
		}
		else if ( in1 == STATE3_0 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"1"�Τ��� */
			
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;
		}
		break;
	case STATE3_X:
		break;
	default:
		/* ����������/���ʤ���Ƚ�ǰ��������� */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}
	return( result );
}

/********************************************************************************
 * << Function >>
 *		judge nand gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *
 ********************************************************************************/

int atpg_check_cand_justified_nand
(
 LINE *line,
 int   flag
 ){

	int  result = 0; /* ��������ѿ��ν���� */
	void topgun_error();

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */

	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();

	char  *func_name = "atpg_check_cand_justified_nand"; /* �ؿ�̾ */


	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
	
	/* �����ͤ˱����ơ��ɤο����������������뤫�ͤ��� */
	switch ( out ) {
	case STATE3_0:
		/* ���Ϥ�1�ΤȤ���ξ���������ͤ�ɬ�� */
		result |= JUST_ALL;
		return( result );

	case STATE3_1:
		/* ���Ϥ�0�ΤȤ��Ϥɤä��������������ͤ�ɬ�� */
		if ( (in0 == STATE3_0) &&
			 (in1 == STATE3_0) ) {
			/* in[0] = 0, in[1] = 0 */
			/* $$$ �Ȥꤢ���� $$$ */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */

			return( result );
		}
		else if ( in0 == STATE3_0 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */
			return( result );
		}
		else if ( in1 == STATE3_0 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"1"�Τ��� */
			return( result );
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;

			return( result );
		}
	case STATE3_X:
		return ( result );
		
	default:
		/* ����������/���ʤ���Ƚ�ǰ��������� */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}

	return ( 0 ); /* ��ʤ����� */
}

/********************************************************************************
 * << Function >>
 *		judge "OR" gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/1
 *
 ********************************************************************************/

int atpg_check_cand_justified_or
(
 LINE *line,
 int  flag
 ){

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */
	
	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();
	
	int  result = 0; /* ��������ѿ��ν���� */
	void topgun_error();
	char *func_name = "atpg_check_cand_justified_or"; /* �ؿ�̾ */


	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}

	
	/* �����ͤ˱����ơ��ɤο����������������뤫�ͤ��� */
	switch ( out ) {
	case STATE3_0:
		/* ���Ϥ�0�ΤȤ���ξ���������ͤ�ɬ�� */
		result |= JUST_ALL;
		return( result );

	case STATE3_1:
		/* ���Ϥ�1�ΤȤ��Ϥɤä��������������ͤ�ɬ�� */
		if ( (in0 == STATE3_1) &&
			 (in1 == STATE3_1) ) {

			/* $$$ �Ȥꤢ���� $$$ */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */

			return( result );
		}
		else if ( in0 == STATE3_1 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */
			return( result );
		}
		else if ( in1 == STATE3_1 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"0"�Τ��� */
			return( result );
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;

			return( result );
		}
	case STATE3_X:
		return( result );
		
	default:
		/* ����������/���ʤ���Ƚ�ǰ��������� */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}
	return(0);
}

/********************************************************************************
 * << Function >>
 *		judge "NOR" gate for justified for normal or failure state
 *
 * << Function ID >>
 *    	16-??
 *
 * << Return >>
 *	result		: justified line result
 *
 * << Argument >>
 * name			(I/O)	type			description
 * out           I      Ulong           out line state
 * in0           I      Ulong           in[0] line state
 * in1           I      Ulong           in[1] line state
 *
 * << extern >>
 * name			(I/O)	type			description
 *	   None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/7
 *
 ********************************************************************************/

int atpg_check_cand_justified_nor
(
 LINE *line,
 int   flag
 ){

	int  result = 0; /* ��������ѿ��ν���� */

	Ulong out = 0; /* state of justified line  */
	Ulong in0 = 0; /* state of line of input No.0 of justified line */
	Ulong in1 = 0; /* state of line of input No.1 of justified line */

	Ulong atpg_get_normal_stat();
	Ulong atpg_get_failure_stat();
	
	void topgun_error();
	char *func_name = "atpg_check_cand_justified_nor"; /* �ؿ�̾ */

	switch ( flag ){
	case JUSTIFY_NO:
		out = atpg_get_normal_stat( line->stat ) ;
		in0 = atpg_get_normal_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_normal_stat( line->in[ 1 ]->stat ) ;
		break;
	case JUSTIFY_FL:
		out = atpg_get_failure_stat( line->stat ) ;
		in0 = atpg_get_failure_stat( line->in[ 0 ]->stat ) ;
		in1 = atpg_get_failure_stat( line->in[ 1 ]->stat ) ;
		break;
	default:
		topgun_error( FEC_PRG_JUSTIFIED, func_name );
	}
	
	/* �����ͤ˱����ơ��ɤο����������������뤫�ͤ��� */
	switch ( out ) {
	case STATE3_1:
		/* ���Ϥ�1�ΤȤ���ξ���������ͤ�ɬ�� */
		result |= JUST_ALL;
		return( result );

	case STATE3_0:
		/* ���Ϥ�0�ΤȤ��Ϥɤä��������������ͤ�ɬ�� */
		if ( (in0 == STATE3_1) &&
			 (in1 == STATE3_1) ) {
			/* in[0] = 0, in[1] = 0 */
			/* $$$ �Ȥꤢ���� $$$ */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */
			return( result );
		}
		else if ( in0 == STATE3_1 ) {
			/* in[0] = 0, in[1] = X or 1 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 0 ); /* in"0"�Τ��� */
			return( result );
		}
		else if ( in1 == STATE3_1 ) {
			/* in[0] = X or 1, in[1] = 0 */
			result |= JUST_1IN;
			result += ( JUST_1IN_N * 1 ); /* in"1"�Τ��� */
			return( result );
		}
		else{
			/* in[0] = X, in[1] = X */
			result |= JUST_OWN;

			return( result );
		}
	case STATE3_X:
		return( result );
		
	default:
		/* ����������/���ʤ���Ƚ�ǰ��������� */
		/* ERROR */
		topgun_error( FEC_PRG_JUSTIFIED, func_name );

		/* old version 2005/6/28
		  topgun_error_message_just();
		*/
	}
	return( 0 );
}
#endif /* TOPGUN_NOUSE */
/* End of File */
