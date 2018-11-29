/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ƥ����ӥ�ƥ��᥸��˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h>

#include "topgun.h"
#include "topgunState.h" /* STATE_3, STATE_9 */
#include "topgunLine.h"  /* LINE, LINE_TYPE, LINE_INFO */
#include "topgunTm.h"    /* TM_XXX */
#include "topgunError.h" /* FEC_XX */

extern LINE_INFO   Line_info;
extern LINE     ***Lv_pi_head;

/* function list */

/* orginal function */

extern void tm_init_scoap ( void );  /* scoap�Υƥ����ӥ�ƥ������ */
static void tm_scoap_con ( LINE * ); /* scoap������ƥ����ӥ�ƥ������ */
static void tm_scoap_obs ( LINE * ); /* scoap�δ�¬�ƥ����ӥ�ƥ������ */
static Ulong tm_add ( Ulong , Ulong );



/* print function */
void topgun_print_mes_tm_con_calc ( Ulong, Ulong );
void topgun_print_mes_tm_obs_calc ( LINE * );
void topgun_print_mes_tm_calc_result ( void );
void topgun_print_mes_tm_con_pi ( LINE * );
void topgun_print_mes_tm_con_inv ( LINE * );
void topgun_print_mes_tm_con_in1 ( LINE * );
void topgun_print_mes_tm_con_and ( LINE * );
void topgun_print_mes_tm_con_nand ( LINE * );
void topgun_print_mes_tm_con_or ( LINE * );
void topgun_print_mes_tm_con_nor ( LINE * );
void topgun_print_mes_tm_con_xor ( LINE * );
void topgun_print_mes_tm_con_xnor ( LINE * );



/* test function */

void test_line_null ( LINE *, char * );
void test_line_one_type ( LINE *, LINE_TYPE, char * );
void test_line_out_null ( LINE *, char * );
void test_line_in_null ( LINE *, char * );


/********************************************************************************
 * << Function >>
 *		Initialize testability measure for SCOAP
 *
 * << Function ID >>
 *	   	4-1
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Lv_pi_head    I      LINE***
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/6
 *
 ********************************************************************************/

void tm_init_scoap
(
 void
){
    Ulong i; /* ���祲�����ʿ�(max_lv_pi) */
    Ulong j; /* Ʊ����٥�κ��翮������ */
    Ulong n_lv_pi; /* Ʊ����٥�κ��翮������ */
    LINE  *lp;
    
    for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
	n_lv_pi = Line_info.n_lv_pi[ i ];
	for ( j = 0 ; j < n_lv_pi ; j++ ) {
	    lp =  Lv_pi_head[ i ][ j ];
	    
	    /* �Ȥꤢ����SCOAP�١��� */
	    tm_scoap_con( lp );
	}
    }
    
    for ( i = Line_info.max_lv_pi ; i > 0; ){
	--i;
	n_lv_pi = Line_info.n_lv_pi[ i ];
	for ( j = 0 ; j < n_lv_pi ; j++){
	    lp =  Lv_pi_head[ i ][ j ];
	    
	    /* �Ȥꤢ����SCOAP�١��� */
	    tm_scoap_obs ( lp );
	    topgun_print_mes_tm_obs_calc ( lp );
	}
    }
    
    topgun_print_mes_tm_calc_result ( );
}


/********************************************************************************
 * << Function >>
 *		Initialize testability measure for SCOAP
 *
 * << Function ID >>
 *	   	4-2
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line_tm       I      LINE*   calculate controlablilty of line_tm
 *
 * << extern >>
 * name			(I/O)	type	description
 * Lv_pi_head    I      LINE***
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/6
 *
 ********************************************************************************/

void tm_scoap_con
(
 LINE *line_tm
 ){

    Ulong i;      /* ��������ʬ���� */
    Ulong c0,c1;  /* tm�λ��Хƥ�ݥ�� */
    int   null_line = 0; /* NULL�ΤȤ����б� */
    LINE  *lp; /* ���Ͽ����� */
    
#ifdef TOPGUN_XOR_ON	
    LINE  *lp2; /* XOR/XNOR���Ͽ����� */
#endif 	/* TOPGUN_XOR_ON */

    char *func_name = "tm_scoap_con"; /* �ؿ�̾ */
    
    test_line_in_null ( line_tm, func_name );
    
    /* $$$ key���¤Ӥ��Ŭ������ɬ�פ��� $$$ */
    /* $$$ TM�ξ�¤�Ķ�������б�̤ $$$ */
    switch( line_tm->type ){
	
    case TOPGUN_PI:
	line_tm->tm_c0 = 1;
	line_tm->tm_c1 = 1;
	topgun_print_mes_tm_con_pi ( line_tm );
	break;
	
    case TOPGUN_INV:
	line_tm->tm_c0 = line_tm->in[ 0 ]->tm_c1;
	line_tm->tm_c1 = line_tm->in[ 0 ]->tm_c0;
	topgun_print_mes_tm_con_inv ( line_tm );
	break;
	
    case TOPGUN_PO:
    case TOPGUN_BR:
    case TOPGUN_BUF:
    case TOPGUN_BLKO:		
	line_tm->tm_c0 = line_tm->in[ 0 ]->tm_c0;
	line_tm->tm_c1 = line_tm->in[ 0 ]->tm_c1;
	topgun_print_mes_tm_con_in1 ( line_tm );
	break;

    case TOPGUN_AND:
	
	c0 = TM_INF;
	c1 = 0;
	null_line = 0;
	
	for ( i = 0 ; i < line_tm->n_in ; i++ ) {
	    lp = line_tm->in[ i ];
	    
	    if ( c0 > lp->tm_c0 ) {
		c0 = lp->tm_c0;
	    }
	    c1 = tm_add ( c1, lp->tm_c1 );
	}

	line_tm->tm_c0 = c0;
	line_tm->tm_c1 = c1;
	topgun_print_mes_tm_con_and ( line_tm );
	break;
	
    case TOPGUN_NAND:

	c1 = TM_INF;
	c0 = 0;
	null_line = 0;
	
	for ( i = 0 ; i < line_tm->n_in ; i++ ) {
	    lp = line_tm->in[ i ];
	    
	    if ( c1 > lp->tm_c0 ) {
		/* c1��minimum�ǹ��� */
		c1 = lp->tm_c0;
	    }
	    /* c0��c1�򤹤٤Ʋû� */
	    c0 = tm_add ( c0 , lp->tm_c1 );
	}
	
	line_tm->tm_c0 = c0;
	line_tm->tm_c1 = c1;
	topgun_print_mes_tm_con_nand ( line_tm );
	break;

    case TOPGUN_OR:
	c0 = 0;
	c1 = TM_INF;
	null_line = 0;
	
	for ( i = 0 ; i < line_tm->n_in ; i++ ) {
	    lp = line_tm->in[ i ];
	    
	    if ( c1 > lp->tm_c1 ) {
		c1 = lp->tm_c1;
	    }
	    c0 = tm_add ( c0, lp->tm_c0 );
	}
	
	line_tm->tm_c0 = c0;
	line_tm->tm_c1 = c1;
	topgun_print_mes_tm_con_or ( line_tm );
	break;

    case TOPGUN_NOR:
	
	c1 = 0;
	c0 = TM_INF;
	null_line = 0;

	for ( i = 0 ; i < line_tm->n_in ; i++ ) {
	    lp = line_tm->in[ i ];
	    if ( c0 > lp->tm_c1 ) {
		c0 = lp->tm_c1;
	    }
	    c1 = tm_add ( c1, lp->tm_c0 );
	}

	line_tm->tm_c0 = c0;
	line_tm->tm_c1 = c1;
	topgun_print_mes_tm_con_nor ( line_tm );
	break;
	
#ifdef TOPGUN_XOR_ON
	case TOPGUN_XOR:

	    c0 = TM_INF;
	    c1 = TM_INF;
	    
	    lp = line_tm->in[ 0 ];
	    lp2 = line_tm->in[ 1 ];
	    

	    if ( c0 > ( lp->tm_c0 + lp2->tm_c0 ) ) {
		c0 = lp->tm_c0 + lp2->tm_c0;
	    }
	    if ( c0 > ( lp->tm_c1 + lp2->tm_c1 ) ) {
		c0 = lp->tm_c1 + lp2->tm_c1;
	    }
	    if ( c1 > ( lp->tm_c0 + lp2->tm_c1 ) ) {
		c1 = lp->tm_c0 + lp2->tm_c1;
	    }
	    if ( c1 > ( lp->tm_c1 + lp2->tm_c0 ) ) {
		c1 = lp->tm_c1 + lp2->tm_c0;
	    }
	    line_tm->tm_c0 = c1;
	    line_tm->tm_c1 = c0;
	    topgun_print_mes_tm_con_xor ( line_tm );
	    break;
		
    case TOPGUN_XNOR:
		
	c0 = TM_INF;
	c1 = TM_INF;
	
	lp  = line_tm->in[ 0 ];
	lp2 = line_tm->in[ 1 ];
	
	if ( c1 > ( lp->tm_c0 + lp2->tm_c0 ) ) {
			c1 = lp->tm_c0 + lp2->tm_c0;
	}
		if ( c1 > ( lp->tm_c1 + lp2->tm_c1 ) ) {
		    c1 = lp->tm_c1 + lp2->tm_c1;
		}
		if ( c0 > ( lp->tm_c0 + lp2->tm_c1 ) ) {
		    c0 = lp->tm_c0 + lp2->tm_c1;
		}
		if ( c0 > ( lp->tm_c1 + lp2->tm_c0 ) ) {
			c0 = lp->tm_c1 + lp2->tm_c0;
		}
		line_tm->tm_c0 = c0;
		line_tm->tm_c1 = c1;
		topgun_print_mes_tm_con_xnor ( line_tm );
		break;
#endif /* TOPGUN_XOR_ON */
		
	case TOPGUN_BLKI:
		line_tm->tm_c0 = TM_INF;
		line_tm->tm_c1 = TM_INF;
		topgun_print_mes_tm_con_in1 ( line_tm );
		break;
		
	default:
		topgun_error( FEC_PRG_LINE_TYPE, func_name );
		break;
	}
}

/********************************************************************************
 * << Function >>
 *		calculate obserbability of line
 *
 * << Function ID >>
 *	   	4-3
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * line_tm       I      LINE*   calculate obserbability of line_tm
 *
 * << extern >>
 * name			(I/O)	type	description
 * Lv_pi_head    I      LINE***
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/6
 *
 ********************************************************************************/


void tm_scoap_obs
(
 LINE *line_tm
 ) {

    Ulong i; /* ����ʬ���� */
	Ulong co = FL_ULMAX;
    LINE  *lp;

	char  *func_name = "tm_scoap_obs"; /* �ؿ�̾ */

	test_line_null ( line_tm, func_name );
	
	if ( line_tm->n_out == 0 ) {
		if ( line_tm->type == TOPGUN_PO ) {
			/* PO�ξ�� */
			line_tm->tm_co = 0;
			test_line_one_type ( line_tm, TOPGUN_PO, func_name );
		}
		else {
			/* BBpo�ξ�� */
			line_tm->tm_co = TM_INF;
			test_line_one_type ( line_tm, TOPGUN_BLKO, func_name );
		}
	}
	else{
		/* PO�ʳ��ξ�� */

		test_line_null ( line_tm->out[ 0 ], func_name );

		lp = line_tm->out[ 0 ];

		/* $$$ key���¤Ӥ��Ŭ������ɬ�פ��� $$$ */
		/* $$$ tm����¤�Ķ�������б�̤ $$$ */

		switch ( lp->type ) {
		case TOPGUN_BR:
			test_line_out_null ( line_tm, func_name );
			/* �ޤ��Ǿ��ν��ϤβĴ�¬������ */
			for ( i = 0 ; i < line_tm->n_out ; i++ ) {
				if ( line_tm->out[ i ]->tm_co < co ) {
					co = line_tm->out[ i ]->tm_co;
				}
			}
			/* �Ǿ��βĴ�¬���Ĵ�¬��ˤ��� */
			line_tm->tm_co = co;
			break;

		case TOPGUN_INV:
		case TOPGUN_PO:
		case TOPGUN_BUF:
			/* �Ĵ�¬��Τޤ� */

#ifndef TOPGUN_TEST
			/* ʣ��(!=1)�ξ��ϡ�lp->type��branch�ˤʤ�Ϥ� */
			if ( line_tm->n_out != 1 ) {
				topgun_error( FEC_PRG_LINE_TYPE, func_name );
			}
#endif /* TOPGUN_TEST */

			line_tm->tm_co = lp->tm_co;
			break;

		case TOPGUN_AND:
		case TOPGUN_NAND:
			/* �Ĵ�¬��˼��������ʳ���1���������­�� */
			line_tm->tm_co = lp->tm_co;

			test_line_in_null ( lp, func_name );
			
			for ( i = 0 ; i < lp->n_in ; i++ ) {

				if ( lp->in[ i ]->line_id != line_tm->line_id ) {
					line_tm->tm_co = tm_add ( line_tm->tm_co, lp->in[ i ]->tm_c1 );
				}
			}
			break;

		case TOPGUN_OR:
		case TOPGUN_NOR:

			/* �Ĵ�¬��˼��������ʳ���0���������­�� */			
			line_tm->tm_co = lp->tm_co;

			test_line_in_null ( lp, func_name );
			
			for ( i = 0 ; i < lp->n_in ; i++ ) {
				if ( lp->in[ i ]->line_id != line_tm->line_id ) {
					line_tm->tm_co = tm_add ( line_tm->tm_co, lp->in[ i ]->tm_c0 );
				}
			}
			break;

		case TOPGUN_BLKO:
			/* �Ĵ�¬���TM_INF�Ȥ��� */			
			line_tm->tm_co = TM_INF;
			break;

#ifdef TOPGUN_XOR_ON
		case TOPGUN_XOR:
		case TOPGUN_XNOR:
			/* �Ĵ�¬��˼��������ʳ���0�������� or 1��������ΰ¤�����­�� */
			
			/* �ޤ����Ͽ�����ʬ */
			line_tm->tm_co = lp->tm_co;

			test_line_in_null ( lp, func_name );
			/* �����Ϥ�Ĵ�٤� */
			for ( i = 0 ; i < lp->n_in ; i++ ) {

				if ( lp->in[ i ]->line_id != line_tm->line_id ) {
					if ( ( lp->in[ i ]->tm_c1 == TM_INF ) &&
						 ( lp->in[ i ]->tm_c0 == TM_INF ) ) {
						line_tm->tm_co = TM_INF;
						break;
					}
					else{
						/* ����������û� */
						if ( lp->in[ i ]->tm_c1 > lp->in[ i ]->tm_c0 ) {
							line_tm->tm_co
								= tm_add ( line_tm->tm_co, lp->in[ i ]->tm_c0 ) ;
						}
						else{
							line_tm->tm_co
								= tm_add ( line_tm->tm_co, lp->in[ i ]->tm_c1 ) ;
						}
					}
				}
			}
			break;
#endif /* TOPGUN_XOR_ON */

		case TOPGUN_PI:
		case TOPGUN_BLKI:			
		default:
			topgun_error( FEC_CIRCUIT_NO_LINE, func_name );
		}
	}
}

/********************************************************************************
 * << Function >>
 *		adder testability measure
 *
 * << Function ID >>
 *	   	4-4
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      Ulong   original testability measure
 * add           I      Ulong   additional testability measure
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

Ulong tm_add
(
 Ulong org,
 Ulong add
 ){

	Ulong res;

	if ( ( TM_INF == org ) ||
		 ( TM_INF == add ) ) {
		return ( TM_INF );
	}

	res = org + add;
	
	if ( res < org ) {
		return ( TM_MAX );
	}
	else {
		return ( res );
	}
}		

/* File End */
