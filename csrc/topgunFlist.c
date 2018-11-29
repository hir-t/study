/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ξ�ꥹ�Ȥ˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include<stdio.h>
#include "topgun.h"
#include "topgunState.h"
#include "topgunLine.h"
#include "topgunFlist.h"
#include "topgunMemory.h" /* FMT_XXX */
#include "topgunError.h" /* FEC_XXX */
#include "topgunOut.h" /* FEC_XXX */

extern LINE_INFO Line_info;
extern LINE      *Line_head;
extern LINE      ***Lv_pi_head;

extern FLIST     **Flist_head;

/* function list */
extern void flist_equivalence ( void );
static void flist_hand_over ( LINE  *, FSIM_FLT , FSIM_FLT );


/* other function list */

/* print function */
void topgun_print_mes_fault_list( void );

/* test function */
void test_line_out_null ( LINE *, char * );

/********************************************************************************
 * << Function >>
 *		�����ξ����
 *
 * << Function ID >>
 *	   	?-1
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
 * Pi_head       I      LINE**  PrimaryInput header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/14
 *
 ********************************************************************************/

void flist_equivalence
(
 void
){
	Ulong i; /* level�ο� */
	Ulong n_lv_pi; /* level�ο������� */
	Ulong j; /* level�ο������� */
	LINE *line; /* LINE�ݥ��� */
	Ulong all_fault_cnt; /* ���ξ�������� */

	
	char *func_name = "flist_equivalence";

	all_fault_cnt = 0;

	/* �����ξ�������θξ�ꥹ�� */
	topgun_print_mes_fault_list();
	
    for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		n_lv_pi = Line_info.n_lv_pi[ i ];
		for ( j = 0 ; j < n_lv_pi ; j++ ) {
			line =  Lv_pi_head[ i ][ j ];

			if ( line->n_out == 1 ) {
				/* 1���Ϥξ��������ξ���Ϥ��� */

				test_line_out_null ( line , func_name );

				switch ( line->out[ 0 ]->type ) {
				case TOPGUN_PO:
				case TOPGUN_BLKO:
				case TOPGUN_BUF:
					/* 0�Ͻ��Ϥ�0���Ϥ� */
					flist_hand_over ( line , FSIM_SA0, FSIM_SA0 );
					/* 1�Ͻ��Ϥ�1���Ϥ� */
					flist_hand_over ( line , FSIM_SA1, FSIM_SA1 );
					break;
				
				case TOPGUN_INV:
					/* 0�Ͻ��Ϥ�1���Ϥ� */
					flist_hand_over ( line , FSIM_SA0, FSIM_SA1 );
					/* 1�Ͻ��Ϥ�0���Ϥ� */
					flist_hand_over ( line , FSIM_SA1, FSIM_SA0 );					
					break;

				case TOPGUN_AND:
					/* 0�Ͻ��Ϥ�0���Ϥ� */
					flist_hand_over ( line , FSIM_SA0, FSIM_SA0 );
					/* 1�Ϥ��Τޤ� */				
					break;
				
				case TOPGUN_NAND:
					/* 0�Ͻ��Ϥ�1���Ϥ� */
					flist_hand_over ( line , FSIM_SA0, FSIM_SA1 );
					/* 1�Ϥ��Τޤ� */
					break;
				
				case TOPGUN_OR:
					/* 1�Ͻ��Ϥ�1���Ϥ� */
					flist_hand_over ( line , FSIM_SA1, FSIM_SA1 );
					/* 0�Ϥ��Τޤ� */				
					break;				
				
				case TOPGUN_NOR:
					/* 1�Ͻ��Ϥ�0���Ϥ� */
					flist_hand_over ( line , FSIM_SA1, FSIM_SA0 );
					/* 0�Ϥ��Τޤ� */
					break;				
				
				case TOPGUN_PI:   /* ���ϥ����Ȥ�PI��NG */
				case TOPGUN_BLKI: /* ���ϥ����Ȥ�BLKI��NG */				
				case TOPGUN_BR:	  /* 1���ϥ����Ȥν��Ϥ�branch��NG */
				case TOPGUN_XOR:  
				case TOPGUN_XNOR: /* XOR/XNOR�ϻȤ�ʤ� */
				case TOPGUN_UNK:  				
				default:
					topgun_error ( FEC_PRG_LINE_TYPE, func_name );
					break;
				}
			}
			else {
				/* ¿���Ϥξ��������ξ���ϤǤ��ʤ� */
				;
			}

			if ( line->flist[0] != NULL ) {
				all_fault_cnt++;
			}
			if ( line->flist[1] != NULL ) {
				all_fault_cnt++;
			}			
		}
	}

	Line_info.n_fault_atpg = all_fault_cnt;

}

/********************************************************************************
 * << Function >>
 *		�ξ��錄��
 *
 * << Function ID >>
 *	   	?-1
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * moto          I      flist * 
 * ukeru         I      flist * 
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/14
 *
 ********************************************************************************/

void flist_hand_over
(
 LINE  *line,
 FSIM_FLT from, /* �Ϥ����θξ��� */
 FSIM_FLT to    /* �Ϥ���θξ��� */
){

	FLIST *tmp_flist;

	char *func_name = "flist_hand_over";

	test_line_out_null ( line , func_name );

	if ( line->flist[ from ] != NULL ) {
		/* �ξ�ꥹ�Ȥ����� */
		
		if ( line->out[ 0 ]->flist[ to ] == NULL ) {
			/* �ξ�ꥹ�Ȥ�Ǹ�����­�� */			
			line->out[ 0 ]->flist[ to ] = line->flist[ from ];

			line->out[ 0 ]->flist[ to ]->line = line->out[ 0 ];

/* 			printf("#move %4ld[%d] -> %4ld[%d]\n", line->line_id, from, line->out[0]->line_id,to); */
		}
		else {
			/* �Ǹ�����õ�� */
			tmp_flist = line->out[ 0 ]->flist[ to ];

			while ( tmp_flist->next ) {
				tmp_flist = tmp_flist->next;
			}
			/* �ξ�ꥹ�Ȥ�Ǹ�����­�� */
			tmp_flist->next = line->flist[ from ];
/* 			printf("#delete %4ld[%d] -> %4ld[%d]\n",line->line_id, from, line->out[0]->line_id,to); */
		}
		/* ���ꥸ�ʥ��õ�� */
		line->flist [ from ] = NULL;
	}
}

/********************************************************************************
 * << Function >>
 *		�����ξ��eqf����Ͽ����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/12
 *
 ********************************************************************************/

void flist_make_eqf
(
 FLIST *flist
 ){

	FLIST *tmp_flist = flist->next;
	FLIST *free_flist;
	Ulong cnt = 0;

	char *func_name = "flist_make_eqf";
	
	while ( tmp_flist ) {
		cnt++;
		tmp_flist = tmp_flist->next;
	}

	if ( cnt == 0 ) {
		flist->n_eqf = 1;
	}
	else {
		/* equivalence fault */
		flist->eqf = ( Ulong * )topgunMalloc( FMT_Ulong, sizeof( Ulong ),
											   cnt, func_name );

		flist->n_eqf = ( cnt + 1 ); /* flist��NULL�Ǥʤ��ΤǺ��������ξ����1���� */

		tmp_flist = flist->next;
		cnt       = 0;
	
		while ( tmp_flist ) {

			flist->eqf[ cnt++ ] = tmp_flist->f_id;
		
			free_flist = tmp_flist;
			tmp_flist = tmp_flist->next;

			topgunFree ( free_flist, FMT_FLIST, 1, func_name );
		}

		flist->next = NULL;
	}
}

/********************************************************************************
 * << Function >>
 *		�ξ�ꥹ�Ȥ��������(�ʰ���)
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/12
 *
 ********************************************************************************/

void flist_make
(
    void
)
{
    char *func_name = "flist_make";

    FLIST *flist;
	LINE  *line;
    Ulong i;
	Ulong fault_count = 0;

    /* ������� */
    Flist_head = ( FLIST ** )topgunMalloc( FMT_FLIST_P, sizeof( FLIST * ),
											Line_info.n_fault_atpg, func_name );
	
	Line_info.n_fault_atpg_count = Line_info.n_fault_atpg;

    for ( i = 0; i < Line_info.n_line; i++ ) {

		line  = &( Line_head[ i ] );
		
		flist = line->flist[ FSIM_SA0 ];  

		if ( flist != NULL ) {

			Flist_head [ fault_count++ ] = flist;

			if ( flist->line->line_id != line->line_id ) {
				printf(" flist id %6ld  line id %6ld\n", flist->line->line_id, line->line_id );
			}

			flist_make_eqf ( flist );
			
		}

		flist = line->flist[ FSIM_SA1 ];  

		if ( flist != NULL ) {

			Flist_head [ fault_count++ ] = flist;

			if ( flist->line->line_id != line->line_id ) {
				printf(" flist id %6ld  line id %6ld\n", flist->line->line_id, line->line_id );
			}

			flist_make_eqf ( flist );
			
		}
	}
}

/********************************************************************************
 * << Function >>
 *		�ξ�򸡽кѤߤˤ���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/12
 *
 ********************************************************************************/

void flist_detectable
(
 FLIST *flist
)
{
	LINE  *line;

	line = flist->line;

	if ( flist->info & TOPGUN_SA0 ) {
		// $$$ �Ȥꤢ����randomdet���� 
		line->flist[ FSIM_SA0 ]->info |= TOPGUN_DTR;
		line->flist[ FSIM_SA0 ]->info |= TOPGUN_DCD;		
	}
	else if ( flist->info & TOPGUN_SA1 ) {
		line->flist[ FSIM_SA1 ]->info |= TOPGUN_DTR;
		line->flist[ FSIM_SA1 ]->info |= TOPGUN_DCD;		
	}
}


/* End of File */

