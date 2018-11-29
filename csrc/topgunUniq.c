/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ��ճ������˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunState.h"
#include "topgunAtpg.h"
#include "topgunLine.h"
#include "topgunMemory.h"
#include "topgunError.h"


extern LEARN_LIST *gene_uniq ( LINE * );
extern void gene_uniq_init ( void  );
static LINE *uniq_get_line_from_level_stack ( Ulong * );
static LINE *uniq_get_and_delete_line_from_level_stack ( Ulong * );
static void uniq_enter_level_stack ( LINE * );
static FL_BOOL uniq_drive_list ( Ulong *, Ulong * );
static LEARN_LIST *uniq_make_ass_list ( LINE * );
static void uniq_level_stack_reflesh ( Ulong , Ulong );

static LINE_STACK **Level_stack;
extern LINE_INFO  Line_info;

/* topgun_state.c */
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9�ͤ��������ͤ���Ф� */

/* topgun_uty.c */
LEARN_LIST *utility_combine_learn_list ( LEARN_LIST *, LEARN_LIST * );

/* topgun_print.c */
void topgun_print_mes_id ( LINE * );
void topgun_print_mes_n( void );
void topgun_print_mes_uniq_level_stack ( void );
void topgun_print_mes_uniq_learn_list ( LEARN_LIST * );
void topgun_print_mes_check_number_level_stack( Ulong );
void topgun_print_mes_level_stack_level ( Ulong );

/* topgun_test.c */
void test_line_null ( LINE *, char * );
void test_line_out_null ( LINE *, char * );
void test_ulong_small_big ( Ulong , Ulong , char * );

/*!
  @brief ��ճ������򤪤��ʤ��ؿ�

  @param [in] *line
  @param [in] *ass_list
  @return void

*/
LEARN_LIST *gene_uniq
(
 LINE     *drive_line
 ){

	Ulong     i; /* count of number of output line */
	Ulong     current_level;
	Ulong     n_list_line;
	LINE      *uniq_line;
	LEARN_LIST *result_uniq_list = NULL;
	LEARN_LIST *add_uniq_list = NULL;
	FL_BOOL   result;

	char    *func_name = "gene_uniq"; /* �ؿ�̾ */

	/* ������� */
	current_level = drive_line->lv_pi;
	n_list_line   = 0;

	if ( ( drive_line->type == TOPGUN_PO ) ||
		 ( drive_line->type == TOPGUN_BLKO ) ) {
		return ( NULL );
	}

	test_line_out_null ( drive_line, func_name );

	
	for ( i = 0 ; i < drive_line->n_out ; i++ ) {

		uniq_enter_level_stack ( drive_line->out[ i ] );

		n_list_line++;

		topgun_print_mes_check_number_level_stack ( n_list_line );

	}


	while ( 1 ) {


		if ( n_list_line == 1 ) {
			
			/* ɬ���ξ㤬���¤��뿮���� */
			uniq_line = uniq_get_line_from_level_stack( &current_level );

			test_line_null ( uniq_line, func_name );

			/* �����ɥ���ץåȤ�õ�� */
			add_uniq_list = uniq_make_ass_list ( uniq_line );


			if ( add_uniq_list != NULL ){
				/* �ꥹ�Ȥ򤯤äĤ��� */
				result_uniq_list = utility_combine_learn_list ( result_uniq_list, add_uniq_list );
			}
		}

		topgun_print_mes_level_stack_level ( current_level );
		topgun_print_mes_check_number_level_stack ( n_list_line );
		
		result =  uniq_drive_list ( &current_level, &n_list_line );

		if ( TOPGUN_NG == result ){

			uniq_level_stack_reflesh ( current_level, n_list_line );
			break;

		}
	}

	topgun_print_mes_uniq_learn_list( result_uniq_list );

	return ( result_uniq_list );

	/* return is ass_list of argument */
}




/*!
  @brief enter_level_stack list

  @param [in] *line
  @param [in] *ass_list
  @return void

*/
void uniq_enter_level_stack
(
 LINE     *line
 ){

	LINE_STACK *tmp_stack;

	char    *func_name = "uniq_enter_level_stack"; /* �ؿ�̾ */


	test_line_null ( line, func_name );


	if ( ! ( line->flag & LF_UNIQ ) ) {
		
		line->flag |= LF_UNIQ;
	
		/* imp_info�Υ������ */
		tmp_stack = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK, sizeof( LINE_STACK ),
												   1, func_name );

		tmp_stack->line = line;
		tmp_stack->next = Level_stack[ line->lv_pi ];
		Level_stack[ line->lv_pi ] = tmp_stack;

	}

}

/*!
  @brief get lowest level level_stack list

  @param [in] *line
  @param [in] *ass_list
  @return void

*/
LINE *uniq_get_line_from_level_stack
(
 Ulong *current_level
 ){

	LINE       *uniq_line;
	LINE_STACK *line_stack;
	char       *func_name = "uniq_get_line_from_level_stack"; /* �ؿ�̾ */

	while ( 1 ) {

		if ( Level_stack [ ( *current_level ) ] != NULL ) {
			
			
			line_stack = Level_stack [ ( *current_level ) ];
			uniq_line = line_stack->line;
			
			break;
		}

		( *current_level )++;
					
		test_ulong_small_big ( *current_level, Line_info.max_lv_pi, func_name ) ;
	}

	return ( uniq_line );
}

/*!
  @brief get and delete lowest level level_stack list

  @param [in] *line
  @param [in] *ass_list
  @return void

*/
LINE *uniq_get_and_delete_line_from_level_stack
(
 Ulong *current_level
 ){

	LINE       *uniq_line;
	LINE_STACK *line_stack;
	char       *func_name = "uniq_get_and_delete_line_from_level_stack"; /* �ؿ�̾ */

	while ( 1 ) {

		if ( Level_stack [ ( *current_level ) ] != NULL ) {
			
			
			line_stack = Level_stack [ ( *current_level ) ];
			uniq_line = line_stack->line;
			Level_stack [ ( *current_level) ] = line_stack->next;

			break;
		}

		( *current_level )++;
					
		test_ulong_small_big ( *current_level, Line_info.max_lv_pi, func_name ) ;
	}


	line_stack->line->flag &= ~LF_UNIQ;
	
	topgunFree ( line_stack, FMT_LINE_STACK, 1, func_name );
	
	return ( uniq_line );
}

/*!
  @brief 

  @param [in] *uniq_info
  @return result

*/
FL_BOOL uniq_drive_list
(
 Ulong *current_level,
 Ulong *n_list_line
 ){

	Ulong      i;
	LINE       *drive_line = NULL;

	drive_line = uniq_get_and_delete_line_from_level_stack ( current_level );

	( *n_list_line )--;
	topgun_print_mes_check_number_level_stack ( *n_list_line );

	if ( ( drive_line->type == TOPGUN_PO ) ||
		 ( drive_line->type == TOPGUN_BLKO ) ) {

		if ( drive_line->type == TOPGUN_PO ) {
		
			/* ����¾�ˤ�⤦�����Ϥ䤯ʬ�뽪λ�㤬���뤬�����ʤΤǥ����å����ʤ� */
			return ( TOPGUN_NG );
		}
		else {
			if ( ( *n_list_line ) == 0 ) {
				/* BB_PO�����ʤ���� */
				return ( TOPGUN_NG );
			}
		}
	}
	
	for ( i = 0 ; i < drive_line->n_out ; i++ ) {

		uniq_enter_level_stack ( drive_line->out[ i ] );

		( *n_list_line)++;
		topgun_print_mes_check_number_level_stack ( *n_list_line );

	}

	return ( TOPGUN_OK );
}

/*!
  @brief  Level_stack�ν����

  @return Void
*/

void gene_uniq_init
(
 void 
 ){

	Ulong      i;
	char       *func_name = "gene_uniq_init"; /* �ؿ�̾ */


	Level_stack = ( LINE_STACK ** )topgunMalloc
		( FMT_LINE_STACK_P, sizeof( LINE_STACK * ), Line_info.max_lv_pi, func_name );


	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {

		Level_stack[ i ] = NULL;

	}
}

/*!
  @brief ���¤�ɬ�פʿ������γ�����Ƥ����

  @param [in] *uniq_line
  @return *learn_list

*/


LEARN_LIST *uniq_make_ass_list
(
 LINE *uniq_line
 ){

	LINE *out;
	LINE *out_in = NULL;
	STATE_3 ass_state3 = STATE3_C;
	Ulong uniq_id = uniq_line->line_id;
	Ulong i;

	LEARN_LIST *learn_list = NULL;
	LEARN_LIST *tmp_learn_list;
	
	char *func_name = "uniq_make_ass_list"; /* �ؿ�̾ */



	if ( uniq_line->n_out == 0 ) {
		return ( NULL ) ;
	}

	out = uniq_line->out[ 0 ];
	test_line_null ( out , func_name );
	
	if ( out->n_in == 1 ) {
		return ( NULL );
	}
	

	switch ( out->type ) {

	case TOPGUN_AND:
	case TOPGUN_NAND:
		ass_state3 = STATE3_1; //!< AND/NAND����������
		break;

	case TOPGUN_OR:
	case TOPGUN_NOR:
		ass_state3 = STATE3_0; //!< OR/NOR����������
		break;

	default:
		topgun_error( FEC_PRG_LINE_TYPE, func_name );
	}


	for ( i = 0 ; i < out->n_in ; i++ ) {

		out_in = out->in[ i ];

		if ( out_in->line_id != uniq_id ) {

			tmp_learn_list = ( LEARN_LIST * )topgunMalloc
				( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );

			tmp_learn_list->line = out_in;
			tmp_learn_list->ass_state3 = ass_state3;

			tmp_learn_list->next = learn_list;

			learn_list = tmp_learn_list;

		}
	}

	return ( learn_list ); 
}

/*!
  @brief  Level_stack�Υ��ꥢ

  @return Void
*/

void uniq_level_stack_reflesh
(
 Ulong current_level,
 Ulong n_list_line
 ){

	Ulong      i;
	LINE_STACK *line_stack;
	LINE_STACK *tmp_line_stack;
	char       *func_name = "uniq_level_stack_reflesh"; /* �ؿ�̾ */


	for ( i = current_level ; i < Line_info.max_lv_pi ; i++ ) {

		line_stack = Level_stack[ i ];
		

		if ( line_stack != NULL ) {

			Level_stack[ i ] = NULL;


			while ( line_stack ) {
			
				n_list_line--;
			
				tmp_line_stack = line_stack;

				line_stack = line_stack->next;


				tmp_line_stack->line->flag &= ~LF_UNIQ;
				topgunFree ( tmp_line_stack, FMT_LINE_STACK, 1, func_name );
			}
			topgun_print_mes_check_number_level_stack ( n_list_line );

		}

		if ( n_list_line == 0 ) {
			break;
		}
	}
}


/*!
  @brief  Level_stack����Ͽ���γ�ǧ
  @param [in] void  
  @return Void
*/


void topgun_print_mes_check_number_level_stack
(
 Ulong n_list_line
){
#if _DEBUG_PRT_ATPG_ || _DEBUG_PRT_ATPG_UNIQ_
	Ulong i;
	Ulong list = 0;
	LINE_STACK *line_stack;
	
	for ( i = 0; i < Line_info.max_lv_pi ; i++ ) {

		line_stack = Level_stack[ i ];

		while ( line_stack ) {
			
			list++;
			
			line_stack = line_stack->next;
		}
	}
	
	//uniq_display_level_stack();
	
	if ( list == n_list_line ) {
		printf("uniq  EQ list %3ld exe %3ld\n", n_list_line, list );
	}
	else {
		printf("uniq Not list %3ld exe %3ld\n", n_list_line, list );
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

/*!
  @brief  Level_stack����Ͽ��٥�γ�ǧ
  @param [in] void  
  @return Void
*/


void topgun_print_mes_level_stack_level
(
 Ulong current_level
){
#if _DEBUG_PRT_ATPG_ || _DEBUG_PRT_ATPG_UNIQ_
	Ulong i;
	LINE_STACK *line_stack;
	
	for ( i = 0; i < current_level ; i++ ) {

		if ( Level_stack[ i ] != NULL  ) {

			line_stack = Level_stack[ i ];

			while ( line_stack ) {

				printf("uniq  level %3ld ", i );
				topgun_print_mes_id ( line_stack->line );
				topgun_print_mes_n();
				
				line_stack = line_stack->next;
			}

		}
	}
#endif /* _DEBUG_PRT_ATPG_ */	
}
/*!
  @brief  Level_stack��ɽ��
  @param [in] void  
  @return Void
*/


void topgun_print_mes_uniq_level_stack
(
 void
){
#if _DEBUG_PRT_ATPG_ || _DEBUG_PRT_ATPG_UNIQ_  
	Ulong i;
	LINE_STACK *line_stack;
	
	for ( i = 0; i < Line_info.max_lv_pi ; i++ ) {

		line_stack = Level_stack[ i ];

		while ( line_stack ) {

			if ( line_stack != NULL ) {
				printf("UNIQ_RES lv %4ld ", i );
			}
			topgun_print_mes_id ( line_stack->line );
			topgun_print_mes_n();
			
			line_stack = line_stack->next;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

/* End of File */


