/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ͥåȥꥹ�Ȥʤɥե������ɹ��˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "topgun.h"
#include "topgunState.h"  
#include "topgunLine.h"
#include "topgunIo.h"  
#include "topgunFlist.h"  /* FLIST */
#include "topgunMemory.h" /* FMT_xx */
#include "topgunError.h" /* FEC_xx */


extern FLIST   **Flist_head; /* �ξ�ǡ����إå� */
extern LINE    *Line_head;  /* ATPG�ѥǡ����إå� */

#define FSIM_SA0 0
#define FSIM_SA1 1

/* function list */

/* original function */
extern void read_circuit ( char * );

extern void make_flist ( void );

void flist_line_check( FILE *fp, FLIST *flist );
FLIST_TYPE flist_analyze_header( FILE *fp ) ;
void read_detectable_fault ( char * );


/* topgun_cell.c */
void cell_head_initialize ( void ); /* Cell_head���������� */
void cell_remake_primitive_for_line ( void ); /* primitiveŸ������ */
void cell_print_table( void ); /* Cell_head����Ϥ��� */
void cell_make_fanout( void ); /* fanout���ɲä��� */
void cell_2_line ( void ); /* cell�ǡ�����ATPG��(LINE)�Υǡ�����¤���Ѵ����� */
void cell_head_free( void ); /* Cell_head��������� */

/* topgun_bench.c */
void read_circuit_bench ( char * ); /* bench�ե�������ɤ߹��� */

/* topgun_flist.c */
void flist_detectable( FLIST * );

/* topgun_output.c */
FILE *topgun_open ( char *, char *, char * ); /* file�򳫤� */

/* topgun_print.c */
void topgun_print_mes_cell_head ( void );

/* topgun_uty.c */
int   utility_skip_sentence ( FILE * ); /* 1�Ԥ��ɤ����Ф� */
int   utility_skip_char ( FILE *, char * ); /* ���ڤ�ʸ�������ޤ��ɤ����Ф�����Ƭ��ʸ�������� */
void  utility_token ( FILE *, char *, char *, Ulong ); /* ���ڤ�ʸ�������ޤǤ�ʸ�������� */

/* test function */
void test_line_structure_all ( char * );

/********************************************************************************
 * << Function >>
 *		read circuit for edif
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  0           : Normal Terminate
 *	ERROR		: System Error
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 *
 ********************************************************************************/

void read_circuit
(
 char *net_list_file_name
 )
{
	char  *func_name = "read_circuit_bench"; /* �ؿ�̾ */

	cell_head_initialize();
	
	/* ��ϩ���ɹ� */
	read_circuit_bench( net_list_file_name );
	
	/* ��ϩ�Υ����(for debug) */
	topgun_print_mes_cell_head();

	/* cell��primitiveŸ�� */
	cell_remake_primitive_for_line();
	
	/* �ե��󥢥��Ȥ��ɲ� */
	cell_make_fanout();

	/* ��ϩ�Υ����(for debug) */
	topgun_print_mes_cell_head();

	/* cell��line�ǡ�����¤���Ѵ� */
	cell_2_line();

	/* �ơ��֥���ΰ���� */
	cell_head_free();

	test_line_structure_all( func_name );
}

/********************************************************************************
 * << Function >>
 *		read circuit for edif
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  0           : Normal Terminate
 *	ERROR		: System Error
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
 *		2005/7/7
 *
 ********************************************************************************/

void read_detectable_fault 
(
 char *fault_list_file_name
 )
{
	FLIST flist;
	FLIST_TYPE head;

	FILE *fp;
	
	char  *func_name = "read_detectable_fault"; /* �ؿ�̾ */

	//char string[ BENCH_NAME_SIZE ];
	
	/* �ξ���ɹ��ꥹ�Ȥκ��� */
	fp = topgun_open( fault_list_file_name, "r", func_name );

	while( 1 ) {

		head = flist_analyze_header( fp ); /* gate�ξ����string��Ȥäƽ�����ɬ�� */

		/* type�̤˽�����Ԥ� */
		switch ( head ) {
			case FLIST_ORG :
				/*	�ξ����Ͽ	*/
				flist_line_check ( fp, &flist );
				break;
			case FLIST_CON :
			case FLIST_COMMENT :
				/*	1���ɤ����Ф�	*/
				utility_skip_sentence( fp );
				break;
			case FLIST_END :
				return ;
			default :
				utility_skip_sentence( fp );
				break;
		}
		/* �����Ǥϴ��˸��Фˤ��� */
		flist_detectable (  &flist );
	}
}


/********************************************************************************
 * << Function >>
 *		analyze line header on file
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *  BENCH_COMMENT : comment line
 *  BENCH_END     : end of file
 *  BENCH_INPUT   : input
 *	BENCH_OUTPUT  : output
 *	BENCH_GATE    : gate
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
 *		2005/7/8
 *
 ********************************************************************************/

FLIST_TYPE flist_analyze_header
(
 FILE *fp   /* �إå�����Ϥ���ե�����Υݥ��� */
 )
{
	int  c;
	char  *func_name = "flist_analyze_header"; /* �ؿ�̾ */
	
	/* �������б� */
	/* ����(DELIMITER_LF)�ޤǿʤ�ǹ�Ƭ��ʸ����c���������� */
	c = utility_skip_char( fp, DELIMITER_LF );

	/* #�Τ� */
 	if( c == '#' ){

		return( FLIST_COMMENT );
	}
	/* o�Τ� */
 	else if( c == 'o' ){
		return( FLIST_ORG );
	}
	/* e�Τ� */
 	else if( c == 'e' ){
		return( FLIST_CON );
	}
	else if( c == EOF ) {
		return( FLIST_END );
	}
	topgun_error( FEC_FAULT_LIST, func_name);
	return ( FLIST_END );
}

/********************************************************************************
 * << Function >>
 *		make gate infomation for bench file
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	BENCH_GATE    : gate
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
 *		2005/7/8
 *
 ********************************************************************************/

void flist_line_check
(
 FILE *fp,
 FLIST *flist
 )
{
	long line_id = 0;
	Ulong name_size = 11; /* Ulong = 42����10��+1 */
	char  *func_name = "flist_line_check"; /* �ؿ�̾ */

	char  name[ name_size ];
	
	/* ���ڡ����������ɤ����Ф� */
	utility_skip_char( fp, DELIMITER_LF );

	/* fault_id�ɹ��� */
	utility_token( fp, name, DELIMITER_SP, name_size );
	
	/* ���ڡ����������ɤ����Ф� */
	utility_skip_char( fp, DELIMITER_LF );

	/* fault_id�ɹ��� */
	utility_token( fp, name, DELIMITER_SP, name_size );

	//printf("falut id %s\n", name );

	flist->f_id = atol( name );
	
	/* ���ڡ����������ɤ����Ф� */
	utility_skip_char( fp, DELIMITER_LF );
	
	/* line_id�ɹ��� */
	utility_token( fp, name, DELIMITER_SP, name_size );

	//printf("line id %s\n", name );

	line_id = atol( name );
	flist->line = &Line_head[ line_id ];
	
	/* ���ڡ����������ɤ����Ф� */
	utility_skip_char( fp, DELIMITER_LF );
	
	/* sa0, sa1�ɹ��� */
	utility_token( fp, name, DELIMITER_SP, name_size );

	//printf("sa? %s\n", name );

	if ( strcmp ( name, "sa0" ) == 0 ) {
		flist->info = TOPGUN_SA0;
	}
	else if ( strcmp ( name, "sa1" ) == 0 ) {
		flist->info = TOPGUN_SA1;
	}
	else {
		topgun_error( FEC_FAULT_LIST, func_name);
	}

	/* ���ڡ����������ɤ����Ф� */
	utility_skip_char( fp, DELIMITER_LF );
	
	/* DET�ɹ��� */
	utility_token( fp, name, DELIMITER_SP, name_size );

	//printf("DET %s\n", name );

	if  (  strcmp ( name, "ABT" ) == 0 ) {
		topgun_error( FEC_FAULT_LIST, func_name);
	}

	/* �����ޤǰ�ư */
	utility_skip_sentence( fp );
}

/* end of file */
