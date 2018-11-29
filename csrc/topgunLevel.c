/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ��٥�˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h>
#include "topgun.h"
#include "topgunState.h"  /* STATE_3, STATE_9 */
#include "topgunLine.h"   /* LINE, LINE_TYPE, LINE_INFO */
#include "topgunAtpg.h"   /* JUST_INFO */
#include "topgunMemory.h" /* FMT_XX */
#include "topgunError.h"  /* FEC_XX */

extern LINE_INFO  Line_info;
extern LINE       **Pi_head;
extern LINE       **Po_head;
extern LINE       **BBPi_head;
extern LINE       **BBPo_head;
extern LINE       *Line_head;
extern LINE       ***Lv_pi_head;
extern JUST_INFO  **Lv_head;

/* function list */
extern void level_init ( void );
static void level_pi_calc ( void );
static void level_po_calc ( void );
static void lv_head ( void );
static void level_pi_head ( void );


/* print function */
void  topgun_print_mes_lv_calc_result ( LINE * );
void  topgun_print_mes_n_lv_calc_result ( Ulong );

/* test function */
void  test_line_in_null ( LINE *, char * );
void  test_line_out_null ( LINE *, char * );
void  test_lv_pi_head ( Ulong *, char * );



/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-1
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Line_info   I/O    HEADER  main infomation
 * Pi_head       I/O    LINE**  PrimaryInput header
 * Line_head     I      LINE*   Line Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_init ( void ) {
    /* line.level�η׻� */
    level_pi_calc();

	/* lv_po */
	level_po_calc();

	/* $$$ �ɤä��������줹�٤� $$$ */
	/* level_pi_head�κ��� */
    level_pi_head();

	/* lv_head�κ��� */
    lv_head();
}

/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-1
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Line_info   I/O    HEADER  main infomation
 * Pi_head       I/O    LINE**  PrimaryInput header
 * Line_head     I      LINE*   Line Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_pi_calc
(
 void
){

    Ulong      i       = 0; /* �������Ͽ� */
    Ulong      j       = 0; /* ����ʬ���� */
    Ulong      max_lv  = 0; /* �����٥� */
	Ulong      tmp_lv  = 0; /* ��٥륫���� */
    LINE       *lp     = NULL; /* ��٥����뿮���� */
	LINE       *lp_out = NULL; /* �оݿ������ν���¦������ */
    LINE_STACK *head   = NULL; /* stack����Ƭ */
	LINE_STACK *tail   = NULL; /* stack�κǸ��� */
	LINE_STACK *ls_tmp = NULL; /* ��������ѥݥ��� */
	LINE_STACK *ls_ptr = NULL; /* õ���ѥƥ�ݥ�� */

    char       *func_name = "level_pi_calc"; /* �ؿ�̾ */

    head = NULL;
    tail = NULL;

	/* TM�η׻��Ѥ�����¦�κ�¿�ʿ������ MIN = 1, 0��̤���� */
    /* �����ͤ�X����������ڤ�Ƥ������Ϥ�­��ɬ�פ��� */

    for ( i = 0; i < Line_info.n_pi; i++ ) {

		/* PI��1������ */
		lp = Pi_head[ i ];
		lp->lv_pi = 1;


		topgun_print_mes_lv_calc_result ( lp );

		/* �������Ϥν��Ͽ�������stack�ˤĤ� */
		for ( j = 0 ; j < lp->n_out ; j++ ) {
			lp_out = lp->out[ j ];
			if( ( lp_out->flag & LINE_CHK ) == 0 ) {

				lp_out->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
														   sizeof( LINE_STACK ),
														   1, func_name );
				ls_tmp->line = lp_out;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

	/* BBpi */
    for ( i = 0; i < Line_info.n_bbpi; i++ ) {

		/* BBpi��1������ */
		lp = BBPi_head[ i ];
		lp->lv_pi = 1;

		topgun_print_mes_lv_calc_result ( lp );

		/* �������Ϥν��Ͽ�������stack�ˤĤ� */
		for ( j = 0 ; j < lp->n_out ; j++ ) {
			lp_out = lp->out[ j ];
			if( ( lp_out->flag & LINE_CHK ) == 0 ) {
				lp_out->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
													   sizeof( LINE_STACK ),
													   1, func_name );

				ls_tmp->line = lp_out;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

    Line_info.max_lv_pi = 1;
    ls_tmp = head;

    /* initailze head & tail */
    while ( ls_tmp ) {
		ls_ptr = ls_tmp;
		ls_tmp = ls_tmp->next;
    }
    tail       = ls_ptr;

	/* main��ʬ */

	while ( head ) {

		ls_ptr = head;
		head   = head->next;

		if ( head  == NULL ) {
			tail = NULL;
		}

		/* lp��lv_pi���Ӥ�� */
		lp     = ls_ptr->line;
		test_line_in_null ( lp , func_name ) ;

		max_lv = 0;

		for ( i = 0 ; i < lp->n_in ; i++ ) {

			/* lp�����ϥ�٥���ǧ */
			tmp_lv = lp->in[ i ]->lv_pi;

			if ( tmp_lv == 0 ) {
				break; /* ̤��������Ϥ�¸�ߤ��� */
			}

			if ( max_lv < tmp_lv ) {
				/* �����ͤ򹹿� */
				max_lv = tmp_lv;
			}
		}

		/* ls_ptr�Υ����å��ϴ�λ */
		/* -> ls_ptr�ν��Ϥ��Ѥ� or stack�κǲ��ʤ��Ѥ� */

		/* lv_pi��̤��������Ϥ���ĤǤ⤢�� */
		/* stack�κǽ��ʤ˺����� */
		if ( tmp_lv == 0 ){
			ls_ptr->next = NULL;
			if ( tail == NULL ) {
				head         = ls_ptr;
				tail         = ls_ptr;
			}
			else {
				tail->next   = ls_ptr;
				tail         = ls_ptr;
			}
		}
		/* lv_pi�����٤����ꤺ��
		   ls_ptr�ϲ���*/
		else{

			/* ���Ϥ�level�κ����ͤ�1��­�� */
			lp->lv_pi = ( max_lv + 1 );

			/* Line_info.max_lv �ι��� */
			if ( Line_info.max_lv_pi < lp->lv_pi ) {
				Line_info.max_lv_pi = lp->lv_pi;
			}

			topgun_print_mes_lv_calc_result ( lp );

			test_line_out_null ( lp , func_name );

			/* lp�ν��Ϥ�head/tail���Ѥ� */
			for ( j = 0; j < lp->n_out; j++ ) {

				lp_out = lp->out[ j ];

				/* LINE_CHK flag��Ω�äƤ��ʤ뤫 */
				if ( ( lp_out->flag & LINE_CHK ) == 0 ) {
					/* flag��Ω�äƤ��ʤ��ä��� */

					/* flag��Ω�Ƥơ�head/tail���Ѥ� */
					lp_out->flag |= LINE_CHK;

					ls_tmp = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK,
										sizeof( LINE_STACK ),
										1, func_name);

					ls_tmp->line = lp_out;

					ls_tmp->next = NULL;
					if( tail != NULL ){
						tail->next   = ls_tmp;
					}
					else {
						head     = ls_tmp;
					}
					tail         = ls_tmp;
				}

			}
			topgunFree( ls_ptr, FMT_LINE_STACK, 1, func_name );
		}

		/* �ݸ� */
		if( head == (LINE_STACK *)NULL ){

			if(tail != NULL){
				head = tail;
				tail = tail->next;
			}
		}
    }

    /* level 0(PI/PPI)ʬ��û� */
    Line_info.max_lv_pi++;

    for ( i = 0; i < Line_info.n_line; i++ ) {
		Line_head[ i ].flag &= ~LINE_CHK;
	}
}

/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-1
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Line_info   I/O    HEADER  main infomation
 * Pi_head       I/O    LINE**  PrimaryInput header
 * Line_head     I      LINE*   Line Header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_po_calc
(
 void
){

    Ulong      i       = 0; /* �������Ͽ� */
    Ulong      j       = 0; /* ����ʬ���� */
    Ulong      max_lv  = 0; /* �����٥� */
	Ulong      tmp_lv  = 0; /* ��٥륫���� */
    LINE       *lp     = NULL; /* ��٥����뿮���� */
	LINE       *lp_in  = NULL; /* �оݿ������ν���¦������ */
    LINE_STACK *head   = NULL; /* stack����Ƭ */
	LINE_STACK *tail   = NULL; /* stack�κǸ��� */
	LINE_STACK *ls_tmp = NULL; /* ��������ѥݥ��� */
	LINE_STACK *ls_ptr = NULL; /* õ���ѥƥ�ݥ�� */

    char       *func_name = "level_po_calc"; /* �ؿ�̾ */

    head = NULL;
    tail = NULL;

	/* TM�η׻��Ѥ�����¦�κ�¿�ʿ������ MIN = 1, 0��̤���� */
    /* �����ͤ�X����������ڤ�Ƥ������Ϥ�­��ɬ�פ��� */

    for ( i = 0; i < Line_info.n_po; i++ ) {

		/* PI��1������ */
		lp = Po_head[ i ];
		lp->lv_po = 1;


		topgun_print_mes_lv_calc_result ( lp );

		/* �������Ϥν��Ͽ�������stack�ˤĤ� */
		for ( j = 0 ; j < lp->n_in ; j++ ) {
			lp_in = lp->in[ j ];
			if( ( lp_in->flag & LINE_CHK ) == 0 ) {

				lp_in->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
														   sizeof( LINE_STACK ),
														   1, func_name );
				ls_tmp->line = lp_in;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

	/* BBpo */
    for ( i = 0; i < Line_info.n_bbpo; i++ ) {

		/* BBpi��1������ */
		lp = BBPo_head[ i ];
		lp->lv_po = 1;
		
		topgun_print_mes_lv_calc_result ( lp );

		/* �������Ϥν��Ͽ�������stack�ˤĤ� */
		for ( j = 0 ; j < lp->n_in ; j++ ) {
			lp_in = lp->in[ j ];
			if( ( lp_in->flag & LINE_CHK ) == 0 ) {
				lp_in->flag |= LINE_CHK;

				ls_tmp = ( LINE_STACK *)topgunMalloc( FMT_LINE_STACK,
													   sizeof( LINE_STACK ),
													   1, func_name );

				ls_tmp->line = lp_in;
				ls_tmp->next = head;
				head         = ls_tmp;
			}
		}
    }

    Line_info.max_lv_po = 1;
    ls_tmp = head;

    /* initailze head & tail */
    while ( ls_tmp ) {
		ls_ptr = ls_tmp;
		ls_tmp = ls_tmp->next;
    }
    tail       = ls_ptr;

	/* main��ʬ */

	while ( head ) {

		ls_ptr = head;
		head   = head->next;

		if ( head  == NULL ) {
			tail = NULL;
		}

		/* lp��lv_pi���Ӥ�� */
		lp     = ls_ptr->line;
		test_line_in_null ( lp , func_name ) ;

		max_lv = 0;

		for ( i = 0 ; i < lp->n_out ; i++ ) {

			/* lp�����ϥ�٥���ǧ */
			tmp_lv = lp->out[ i ]->lv_po;

			if ( tmp_lv == 0 ) {
				break; /* ̤��������Ϥ�¸�ߤ��� */
			}

			if ( max_lv < tmp_lv ) {
				/* �����ͤ򹹿� */
				max_lv = tmp_lv;
			}
		}

		/* ls_ptr�Υ����å��ϴ�λ */
		/* -> ls_ptr�ν��Ϥ��Ѥ� or stack�κǲ��ʤ��Ѥ� */

		/* lv_pi��̤��������Ϥ���ĤǤ⤢�� */
		/* stack�κǽ��ʤ˺����� */
		if ( tmp_lv == 0 ){
			ls_ptr->next = NULL;
			if ( tail == NULL ) {
				head         = ls_ptr;
				tail         = ls_ptr;
			}
			else {
				tail->next   = ls_ptr;
				tail         = ls_ptr;
			}
		}
		/* lv_pi�����٤����ꤺ��
		   ls_ptr�ϲ���*/
		else{

			/* ���Ϥ�level�κ����ͤ�1��­�� */
			lp->lv_po = ( max_lv + 1 );

			/* Line_info.max_lv �ι��� */
			if ( Line_info.max_lv_po < lp->lv_po ) {
				Line_info.max_lv_po = lp->lv_po;
			}

			topgun_print_mes_lv_calc_result ( lp );

			test_line_out_null ( lp , func_name );

			/* lp�ν��Ϥ�head/tail���Ѥ� */
			for ( j = 0; j < lp->n_in; j++ ) {

				lp_in = lp->in[ j ];

				/* LINE_CHK flag��Ω�äƤ��ʤ뤫 */
				if ( ( lp_in->flag & LINE_CHK ) == 0 ) {
					/* flag��Ω�äƤ��ʤ��ä��� */

					/* flag��Ω�Ƥơ�head/tail���Ѥ� */
					lp_in->flag |= LINE_CHK;

					ls_tmp = ( LINE_STACK * )topgunMalloc( FMT_LINE_STACK,
										sizeof( LINE_STACK ),
										1, func_name);

					ls_tmp->line = lp_in;

					ls_tmp->next = NULL;
					if( tail != NULL ){
						tail->next   = ls_tmp;
					}
					else {
						head     = ls_tmp;
					}
					tail         = ls_tmp;
				}

			}
			topgunFree( ls_ptr, FMT_LINE_STACK, 1, func_name );
		}

		/* �ݸ� */
		if( head == (LINE_STACK *)NULL ){

			if(tail != NULL){
				head = tail;
				tail = tail->next;
			}
		}
    }

    /* level 0(PI/PPI)ʬ��û� */
    Line_info.max_lv_po++;

    for ( i = 0; i < Line_info.n_line; i++ ) {
		Line_head[ i ].flag &= ~LINE_CHK;
	}
}

/********************************************************************************
 * << Function >>
 *		make Lv_head
 *
 * << Function ID >>
 *	   	3-?
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
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/09/07
 *
 ********************************************************************************/

void lv_head
(
 void
){
    Ulong   i;

	char    *func_name = "lv_head"; /* �ؿ�̾ */

    Lv_head = ( JUST_INFO **)topgunMalloc( FMT_JUST_INFO_P, sizeof( JUST_INFO *),
											 Line_info.max_lv_pi, func_name);

	/* n_lv_pi[ i ]�� cnt[ i ]�ν���� */
    for ( i = 0; i < Line_info.max_lv_pi; i++ ) {
		Lv_head[i] = NULL;
    }

}

/********************************************************************************
 * << Function >>
 *		Calculate Level from PI
 *
 * << Function ID >>
 *	   	3-2
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
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2004/12/6
 *
 ********************************************************************************/

void level_pi_head
(
 void
){

    Ulong   i,j,lv_pi, *cnt_lv;

	char    *func_name = "level_pi_head";	/* �ؿ�̾  */

    Line_info.n_lv_pi = (Ulong *)topgunMalloc( FMT_Ulong, sizeof( Ulong ),
												  Line_info.max_lv_pi, func_name);

	cnt_lv = (Ulong *)topgunMalloc( FMT_Ulong, sizeof( Ulong ),
									 Line_info.max_lv_pi, func_name);

	/* n_lv_pi[ i ]�� cnt[ i ]�ν���� */
    for ( i = 0; i < Line_info.max_lv_pi; i++ ) {
		Line_info.n_lv_pi[ i ] = 0;
		cnt_lv[ i ] = 0;
    }

	/* n_lv_pi[ i ]�η׻� */
    for ( i = 0; i < Line_info.n_line; i++ ) {
		if ( ( &( Line_head[ i ] ) ) != NULL ) {
			Line_info.n_lv_pi[ Line_head[ i ].lv_pi ]++;
		}
    }

	/* print */
    for( i = 0; i < Line_info.max_lv_pi; i++ ) {
		topgun_print_mes_n_lv_calc_result ( i );
    }


    Lv_pi_head = ( LINE *** )topgunMalloc( FMT_LINE_PP, sizeof( LINE ** ),
											Line_info.max_lv_pi, func_name);

    for (i = 0; i < Line_info.max_lv_pi; i++){

		Lv_pi_head[ i ] = (LINE **)topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
												  Line_info.n_lv_pi[ i ],
												  func_name);
		for ( j = 0; j < Line_info.n_lv_pi[ i ]; j++) {
			Lv_pi_head[ i ][ j ] = NULL;
		}
    }

    for ( i = 0; i < Line_info.n_line; i++) {
		lv_pi = Line_head[ i ].lv_pi;
		Lv_pi_head[ lv_pi ][ cnt_lv[ lv_pi ] ] = &( Line_head[ i ] );
		cnt_lv[ lv_pi ]++;
    }

	test_lv_pi_head ( cnt_lv, func_name );

	topgunFree( cnt_lv, FMT_Ulong, Line_info.max_lv_pi, func_name);

}

/* End of File */
