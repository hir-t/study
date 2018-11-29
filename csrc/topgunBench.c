/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief .bench�����ͥåȥꥹ���ɹ��ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h> /* toupper */

#include "topgun.h"
#include "topgunIo.h" /* DELIMITER_XXX */
#include "topgunBench.h" /* BENCH_XXX */
#include "topgunMemory.h" /* FMT_XXX */
#include "topgunCell.h" /* CELL */
#include "topgunError.h" /* FEC_XXX */

/* function list */

extern void      read_circuit_bench ( char * ); /* bench�����Υե�������ɤ߹��ߡ�ATPG�Υǡ������������ */

static int       bench_analyze_header ( FILE *, char * ); /* �Ԥ���Ƭ����Ԥ����Ƥ�Ƚ�Ǥ��� */
static CELL_TYPE bench_analyze_gate_type ( char * ); /* bench�Υ����ȥ����פ�Topgun�Υ����Ȥ��Ѵ����� */
static void      bench_make_gateinfo ( FILE *, char *); /* �Ԥθ�Ⱦ���ɤ�ǥ����Ȥ�Cell��������� */
static void      bench_make_piinfo ( FILE * ); /* �Ԥθ�Ⱦ���ɤ��PI��Cell��������� */
static void      bench_make_poinfo ( FILE * ); /* �Ԥθ�Ⱦ���ɤ��PO��Cell��������� */
static void      bench_entry_gate_in_net ( FILE *, CELL * );
static void      bench_get_name_io ( FILE *, char *, Ulong );
static int       bench_check_continue_input_line ( FILE * );
static void      bench_define_input_output ( BENCH_CELL_DEFINE, char *, char *, CELL *);

/* other file function */

/* topgun_cell.c */
void cell_head_pi_count_up( void );
void cell_head_po_count_up( void );
void cell_in_count_up ( CELL * );
void cell_out_count_up ( CELL * );
CELL *cell_check_and_make_gate( char *, CELL_TYPE );
CELL *cell_make_new_cell ( void );
void cell_make_new_iolist ( CELL_IO **, CELL * );
void cell_entry_table ( CELL *, Ulong );
void cell_entry_name ( CELL *, char * ); /* ����̾����Ͽ���� */

/* topgun_output.c */
FILE *topgun_open ( char *, char *, char * ); /* file�򳫤� */

/* topgun_uty.c */
int   utility_skip_sentence ( FILE * ); /* 1�Ԥ��ɤ����Ф� */
int   utility_skip_char ( FILE *, char * ); /* ���ڤ�ʸ�������ޤ��ɤ����Ф�����Ƭ��ʸ�������� */
void  utility_token ( FILE *, char *, char *, Ulong ); /* ���ڤ�ʸ�������ޤǤ�ʸ�������� */
void  utility_jump_to_char ( FILE *, char ); /* ���ڤ�ʸ�������ޤ��ɤ����Ф� */
Ulong utility_calc_string_hash_value ( char *, Ulong ); /* ʸ����ȥϥå��奵��������ϥå����ͤ�׻����� */

/* common function */
void  topgun_error( TOPGUN_ERROR_CODE, char * ); /* ���顼��å���������Ϥ��� */

/* print function */
void  topgun_print_mes_bench( int, int );
void  topgun_print_mes_bench_char( int, int, char * );
void  topgun_print_mes_bench_char_char( int, int, char *, char *);
void  topgun_print_mes_bench_fanout_info( CELL * ) ;
void  topgun_print_mes_bench_fanout_result( CELL * );
void  topgun_print_mes_n();


/********************************************************************************
 * << Function >>
 *		read for bench file
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
 ********************************************************************************/

void read_circuit_bench
(
 char *net_list_file_name
)
{
    char string[ BENCH_NAME_SIZE ];
    int  head;
    FILE *fp;
    
    char *func_name = "read_circuit_bench_file";
    
    /* �ե�����򳫤� */
    fp = topgun_open( net_list_file_name, "r", func_name );
    
    /* �ե�����򤹤٤��ɤ߽����ޤǷ����֤� */
    topgun_print_mes_bench( 0, 0 );
    
    while(1){
	
	head = bench_analyze_header( fp, string ); /* gate�ξ����string��Ȥäƽ�����ɬ�� */
	
	/* type�̤˽�����Ԥ� */
	switch ( head ) {
	case BENCH_GATE :
	    /*	������������������Ͽ	*/
	    bench_make_gateinfo ( fp, string );
	    break;
	case BENCH_INPUT : 
	    /*	��������������Ͽ	*/
	    bench_make_piinfo( fp );
	    break;
	case BENCH_OUTPUT :
	    /*	��������������Ͽ	*/
	    bench_make_poinfo( fp );
	    break;
	case BENCH_COMMENT :
	    /*	1���ɤ����Ф�	*/
	    utility_skip_sentence( fp );
	    break;
	case BENCH_END :
	    return ;
	default :
	    utility_skip_sentence( fp );
	    break;
	}
    }
    topgun_print_mes_bench( 0, 1 );
	
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

int bench_analyze_header
(
 FILE *fp,   /* �إå�����Ϥ���ե�����Υݥ��� */
 char *string /* �ɤ߹����ʸ���� */
 )
{
    int  c;
    
    /* �������б� */
    /* ����(DELIMITER_LF)�ޤǿʤ�ǹ�Ƭ��ʸ����c���������� */
    c = utility_skip_char( fp, DELIMITER_LF );
    
    /* #�Τ� */
    if( c == BENCH_STR_COMMENT ){
	
	topgun_print_mes_bench ( 1, 0 );
	return( BENCH_COMMENT );
    }
    else if( c == EOF ) {

	topgun_print_mes_bench ( 1, 1 );
	return( BENCH_END );
    }


    /* ��Ƭ��ʸ������äƤ��� */
    utility_token( fp, string, DELIMITER_EQ, BENCH_NAME_SIZE );
    
    /* BENCH�Ϲ�Ƭ��ʸ�����input or output or gate��Ƚ�꤬��ǽ */
    /* input or output or gate ��Ƚ�� */
    if ( strcmp( string, BENCH_STR_INPUT ) == 0 ) {
	topgun_print_mes_bench ( 1, 2 );
	return( BENCH_INPUT );
    }
    else if ( strcmp( string, BENCH_STR_OUTPUT )  == 0 ) {
	topgun_print_mes_bench ( 1, 3 );
	return( BENCH_OUTPUT );
    }
    else {
	/* default */
	topgun_print_mes_bench ( 1, 4 );
	return( BENCH_GATE );
    }
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

void bench_make_gateinfo
(
 FILE *fp,
 char *out_name
){
	int   c;
	CELL_TYPE type;
	CELL  *cell_p; /* ��Ͽ���륲���ȤΥݥ��� */
	char  gate_name[ BENCH_NAME_SIZE ];

	/* �����ȤΥ����פ�Ƚ�̤��� */
	utility_jump_to_char( fp, BENCH_STR_GATE_CONECT );	
	c = utility_skip_char( fp, DELIMITER_LF );
	utility_token( fp, gate_name, DELIMITER_LA, BENCH_NAME_SIZE );

	topgun_print_mes_bench_char( 2, 0 , gate_name );

	/* �����ȥ����ײ��� */
	type = bench_analyze_gate_type( gate_name );

	/* ��Ͽ�γ�ǧ��cell�κ��� */
	cell_p = cell_check_and_make_gate ( out_name, type );

	/* ���Ͽ������ɹ��� */
	bench_entry_gate_in_net( fp, cell_p );

	/* �����ޤǰ�ư */
	utility_skip_sentence( fp );

}


/********************************************************************************
 * << Function >>
 *		�������Ͽ�������Cell���������
 *
 * << Function ID >>
 *	   	?-?
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
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/11
 *
 ********************************************************************************/
/*
  �ơ��֥����Ͽ����̾��(go_name)���ɹ��ߤ�ǡ�
  �����Ƚ�����(p_go)�ȳ���������(p_po)����Ͽ����
  ����������̾��p_go��̾����"_PO"				
         ______									
  p_go  |      |  p_po						
    ----| port |--------��					
        |      |								
         ------
*/

void bench_make_poinfo
(
 FILE *fp
 ){
	CELL  *input;   /* �����Ƚ�����	*/
	CELL  *output;	/* ����������   */
	char  input_name[ BENCH_NAME_SIZE ];
	char  output_name[ BENCH_NAME_SIZE ];
	Ulong hash_value;

	/*
	  char *func_name = "bench_make_poinfo";
	*/

	/* Cell_head��n_po���Ĥդ䤹 */
	cell_head_po_count_up();

	/* ����������̾(����³���Ƥ��륲����̾)���ɹ��� */
	bench_get_name_io( fp, input_name, BENCH_NAME_SIZE );

	topgun_print_mes_bench_char( 4, 0, input_name );

	input = cell_check_and_make_gate ( input_name, CELL_UNK );
	
	output = cell_make_new_cell();
	
	/* PO�����ʤΥ����Ȥ���³ */
	cell_out_count_up( input );
	cell_make_new_iolist( &input->out, output );


	/* ��������������򥻥������ */
	bench_define_input_output( BENCH_CELL_OUTPUT, output_name, input_name, output );
	cell_make_new_iolist( &output->in, input );


	/* �ơ��֥����Ͽ */
	hash_value = utility_calc_string_hash_value( output_name, CELL_HASH );
	cell_entry_table( output, hash_value );

	topgun_print_mes_bench_char( 4, 3, output->name );

}

/********************************************************************************
 * << Function >>
 *		PI/PO�ο�����̾���ɤ߹���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fp            I      FILE *  read netlist file pointer
 * name          I/O    char *  name of PI or PO
 * name_size     I      Ulong   maximam name size
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/11
 *
 ********************************************************************************/

void bench_get_name_io
(
 FILE  *fp,
 char  *name, 
 Ulong name_size /* ̾���κ���Ĺ */
 ){
	/* ����̤ޤǰ�ư */
	utility_jump_to_char( fp, TOPGUN_LA );

	/* ���ڡ����������ɤ����Ф� */
	utility_skip_char( fp, DELIMITER_LF );

	/* ����������̾�ɹ��� */
	utility_token( fp, name, DELIMITER_RA, name_size );

	/* ����̤ޤǰ�ư */
	utility_jump_to_char( fp, TOPGUN_RA );

	/* �����ޤǰ�ư */
	utility_skip_sentence( fp );
}






/********************************************************************************
 * << Function >>
 *		PI��cell�κ��������table�ؤ���Ͽ
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fp            I      FILE *  read netlist file pointer
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

void bench_make_piinfo
(
 FILE *fp
 ){
	CELL  *cell_p; 
	char  name[ BENCH_NAME_SIZE ];

	/* ����������̾���ɹ��� */
	bench_get_name_io( fp, name, BENCH_NAME_SIZE );

	topgun_print_mes_bench_char ( 5, 0, name );

	/* Cell_head��n_pi�������䤹 */
	cell_head_pi_count_up();

	/* ����������̾���ɤ߹��ߡ��������Ϥ�CELL��������� */
	cell_p = cell_check_and_make_gate ( name, CELL_PI );

}



/********************************************************************************
 * << Function >>
 *		bench��gate type��Topgun���Ѵ�����
 *
 * << Function ID >>
 *	   	?-?
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
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/12
 *
 ********************************************************************************/

CELL_TYPE bench_analyze_gate_type
(
 char *string
 ){
	int		i;

	char    *func_name = "bench_analyze_gate_type";

	for( i = 0; string[ i ] != TOPGUN_NC ; i++ ) {
		/* ʸ������ʸ�������줹�� */
		string[ i ] = toupper( string[ i ] ); 
	}

	if ( ! (strcmp( string, BENCH_GATE_AND ) ) ) {
		return( CELL_AND );
	}
	else if( ! ( strcmp( string, BENCH_GATE_NAND ) ) ) {
		return( CELL_NAND );
	}
	else if( ! ( strcmp( string, BENCH_GATE_OR ) ) ) {
		return( CELL_OR );
	}
	else if( ! ( strcmp( string, BENCH_GATE_NOR ) ) ) {
		return( CELL_NOR );
	}
	else if ( ! ( strcmp( string, BENCH_GATE_NOT ) ) ) {
		return( CELL_INV );
	}
	else if ( ! ( strcmp( string, BENCH_GATE_BUF ) ) ) {
		return( CELL_BUF );
	}
	else if ( ! ( strcmp( string, BENCH_GATE_INV ) ) ) {
		return( CELL_INV );
	}
	else if ( ! ( strcmp( string, BENCH_GATE_XOR ) ) ) {
		return( CELL_XOR );
	}
	else if ( ! ( strcmp( string, BENCH_GATE_XNOR ) ) ) {
		return( CELL_XNOR );
	}
	else{
		topgun_error( FEC_CIRCUIT_GATE_TYPE, func_name );
	}
	return ( CELL_UNK ); /* ���ʤ� */
}

/********************************************************************************
 * << Function >>
 *		�����Ȥ����ϥ�����ɤ߹���ǡ������ʤ�Х�������������³��Ԥ�
 *
 * << Function ID >>
 *	   	T-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			  (I/O)	type		description
 * fp              I    FILE *      read netlist file pointer
 * cell_p          I    cell *      ����¦��õ��cell�Υݥ���
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/11
 *
 ********************************************************************************/

void bench_entry_gate_in_net
(
 FILE *fp,
 CELL *out_net
 ){
	CELL  *cell_p;
	char  string[ BENCH_NAME_SIZE ];
	int   c; /* �����μ����� */

	topgun_print_mes_bench ( 7, 0 );
	
	/* ����̤ޤǰ�ư */
	utility_jump_to_char( fp, TOPGUN_LA );

	/* �롼��(��λ�������)	*/
	do{
		/* ���򡢥����ɤ����Ф�	*/
		/* $$$ ���顼�����å�ɬ�� */
		c = utility_skip_char( fp, DELIMITER_SP );

		/* ���Ͽ�����̾�ɹ���(","�ޤ��ɤ߹���) */
		utility_token( fp, string, DELIMITER_COM, BENCH_NAME_SIZE );


		cell_p = cell_check_and_make_gate ( string, CELL_UNK );


		/* ����¦��cell�ν�����³���� */
		cell_out_count_up ( cell_p );
		cell_make_new_iolist( &cell_p->out, out_net );
		
		/* �ո���(��������)����³���� */
		cell_in_count_up ( out_net );
		cell_make_new_iolist( &out_net->in, cell_p );

	} while( bench_check_continue_input_line( fp ) == BENCH_LINE_CONTINUE );
	
	topgun_print_mes_n();
}

/********************************************************************************
 * << Function >>
 *		check continue bench input line
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	         description
 * fp            I      FILE *  read netlist file pointer
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/11
 *
 ********************************************************************************/


int bench_check_continue_input_line
(
 FILE *fp
 ){
	int  c;

	char *func_name = "bench_check_continue_input_line";

	utility_skip_char( fp, DELIMITER_SP );

	c = fgetc( fp );
	switch ( c ) {
	case TOPGUN_COM:
		return( BENCH_LINE_CONTINUE );
	case TOPGUN_RA:
		return( BENCH_LINE_END );
	default:
		topgun_error( FEC_FILE_FORMAT_ERROR_NOT_CHAR, func_name );
	}
	return ( 0 ); /* ���ʤ� */
}

/********************************************************************************
 * << Function >>
 *		check hash table for bench gate
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	         description
 * type          I      BENCH_CELL_DEFINE bench�Υ���η�
 * output_name   I      char *           cell���PI/PO��̾��
 * input_name    I      char *           PI/PO��̾��
 * cell_p        I      cell *           ��������cell�Υݥ���
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/11
 *
 ********************************************************************************/

void bench_define_input_output
(
 BENCH_CELL_DEFINE type, /* cell type */
 char *output_name,  /* PO pin name */
 char *input_name,  /* Gate Out pin name */
 CELL *cell_p    /* cell pointer */
 ){
	
	char *func_name = "bench_define_input_output";

	switch( type ) {
	case BENCH_CELL_OUTPUT:
		sprintf( output_name, "%s_PO", input_name);
		cell_p->n_in  = 1;
		cell_p->n_out = 0;
		cell_p->type  = CELL_PO;
		break;
	case BENCH_CELL_INPUT:
		sprintf( output_name, "%s_PI", input_name);
		cell_p->n_in  = 0;
		cell_p->n_out = 1;
		cell_p->type  = CELL_PI;
		break;
	default:
		topgun_error( FEC_CIRCUIT_GATE_TYPE, func_name );
	}

	cell_entry_name ( cell_p, output_name );
	
}

/* end of file */

