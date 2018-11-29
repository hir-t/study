/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief CELL format�˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <string.h> /* strcmp */
#include <stdlib.h> /* qsort */

#include "topgun.h"
#include "topgunLine.h"
#include "topgunTm.h"
#include "topgunCell.h"
#include "topgunFlist.h" /* FLIST & FLIST->info */
#include "topgunOut.h" /* TOPGUN_OUTFILE_CIRCUIT */
#include "topgunMemory.h" /* FMT_XXX */
#include "topgunError.h" /* FEC_XXX */

static CELL_HEAD Cell_head; /* ������Ͽ�ѥǡ��� */

extern LINE_INFO Line_info;  /* ATPG�ѥǡ����إå� */
extern LINE      *Line_head;  /* ATPG�ѥǡ����إå� */
extern LINE      **Pi_head;  /* ATPG�ѥǡ����إå� */
extern LINE      **Po_head;  /* ATPG�ѥǡ����إå� */
extern LINE      **BBPi_head;  /* ATPG�ѥǡ����إå� */
extern LINE      **BBPo_head;  /* ATPG�ѥǡ����إå� */
extern FLIST     **Flist_head; /* �ξ�ǡ����إå� */


static void cell_make_add_name ( CELL *, char *, Ulong * );
static void cell_insert_fanout( CELL * );
int         cmp_id(const void *d0, const void *d1);
static LINE *cell_2_line_initialize ( CELL *, Ulong );
static void cell_remake_primitive_xor ( CELL * );
static void cell_remake_primitive_xnor ( CELL * );
static void cell_remake_primitive_blk( CELL * );
static void cell_add_black_box( CELL * );
static LINE_TYPE cell_type_2_line_type ( CELL_TYPE );
static void cell_entry_fault ( CELL *, CELL_FLAG );
static void cell_make_fault ( CELL *, LINE * );
static void cell_make_fault_in  ( CELL *, LINE * );
static void cell_make_fault_out ( CELL *, LINE * );


/* topgun_bench.c */
extern CELL *cell_make_new_cell ( void );
extern void cell_make_new_iolist ( CELL_IO **, CELL * );
extern void cell_entry_name ( CELL *, char * );
extern void cell_entry_n_in ( CELL *, Ulong );
extern void cell_entry_n_out ( CELL *, Ulong );

/* topgun_uty.c */
Ulong utility_calc_string_hash_value( char *, Ulong );

/* topgun_output.c */
FILE *topgun_open_add_name( char *, char *, char *, char * );
void topgun_close( FILE *, char * );

/* print function */
void topgun_print_mes_bench_char( int, int, char * );
void topgun_print_mes_bench_char_char( int, int, char *, char * );
void topgun_print_mes_bench_fanout_info( CELL * ) ;
void topgun_print_mes_bench_fanout_result( CELL * );
void topgun_print_mes_n();
void topgun_print_mes_cell_fault ( CELL *, LINE *, Ulong );

/* test_function */
void test_cell_null ( CELL *, char * );
void test_cell_io_null ( CELL_IO *, char * );
void test_number_equal_ulong ( Ulong, Ulong, char * );

/********************************************************************************
 * << Function >>
 *		Cell_head initaize
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      Void
 *	
 *
 * << Argument >>
 * args			(I/O)	type	    description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Cell_head     I      CELL_HEAD   Cell structure infomation
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_head_initialize
(
 void
 )
{
	Ulong i;
	
	char *func_name = "cell_head_initiaize"; /* �ؿ�̾ */
	
	/* initial */
	Cell_head.n_id   = 0;
	Cell_head.n_pi   = 0;
	Cell_head.n_po   = 0;
	Cell_head.n_bbpi = 0;
	Cell_head.n_bbpo = 0;	

	Cell_head.cell = (CELL **)topgunMalloc( FMT_CELL_P, sizeof(CELL *),
											 CELL_HASH, func_name );

	for ( i = 0; i < CELL_HASH ; i++ ) {
		Cell_head.cell[ i ] = NULL;
	}

}

/********************************************************************************
 * << Function >>
 *		̾������ϥå����ͤ��ᡢ�ϥå���ơ��֥뤫��Cell�����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	cell_p   : �ϥå����ͤ�̾����õ������cell�Υݥ���
 *	NULL     : ��Ͽ����Ƥ��ʤ��ä����
 *
 * << Argument >>
 * args			(I/O)	type	description
 * position      I      int     �ϥå�����
 * name          I
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

CELL *cell_search_with_hash_table
(
 char *name     /* ������̾ */
 ){
	CELL	*cell_p;
	Ulong   hash_value;

	/* �ϥå����ͤ���� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH );

	cell_p = (CELL *)Cell_head.cell[ hash_value ];

	while( cell_p != NULL ){
		if ( ! ( strcmp( cell_p->name, name ) ) ) {
			return( cell_p );
		}
		cell_p = cell_p->next;
	}
	/* ��Ͽ����Ƥʤ��ä���� */
	return( NULL );
}

/********************************************************************************
 * << Function >>
 *		cell�˥����ȤΥ����פ����ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      ��Ͽ����cell�Υݥ���
 * type          I      CELL_TYPE   cell�η�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_entry_type
(
 CELL *cell_p, 
 CELL_TYPE type
 ){
	/*
	  char *func_name = "cell_entry_type";
	*/
	if ( cell_p->type == CELL_UNK ) {
		cell_p->type  = type;
	}
}

/********************************************************************************
 * << Function >>
 *		cell�˥����ȤΥ����פ�������ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      ��Ͽ����cell�Υݥ���
 * type          I      CELL_TYPE   cell�η�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void cell_reentry_type
(
 CELL *cell_p, 
 CELL_TYPE type
 ){
	/*
	  char *func_name = "cell_reentry_type";
	*/
	cell_p->type  = type;
}

/********************************************************************************
 * << Function >>
 *		cell��id�����ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      ���ꤹ��cell�Υݥ���
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_entry_id
(
 CELL *cell_p
 ){
	/*
	  char *func_name = "cell_entry_id";
	*/
	/*
	cell_p->id = Cell_head.n_id++;
	*/
}


/********************************************************************************
 * << Function >>
 *		cell��̾�������ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      ��Ͽ����cell�Υݥ���
 * name          I      char *      ��Ͽ����cell��̾��
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_entry_name
(
 CELL *cell_p,
 char *gate_name /* ������̾ */
 ){
	char *func_name = "cell_entry_name";

	topgun_print_mes_bench_char( 3, 0, gate_name );

	if ( cell_p->name != NULL ) {
		topgun_error( FEC_PRG_CELL, func_name ) ;
	}

	cell_p->name  = ( char * )topgunMalloc
		( FMT_Char, ( strlen( gate_name ) + 1)  * sizeof( char ), 1, func_name );
	
	strcpy( cell_p->name, gate_name );

}

/********************************************************************************
 * << Function >>
 *		cell��table�˥ϥå����ͤ��Ѥ�����Ͽ����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_p        I      CELL *  ��Ͽ����cell�Υݥ���
 * hash_value    I      Ulong   cell_p�Υϥå�����
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

void cell_entry_table
(
 CELL  *cell_p,
 Ulong hash_value
 ){
	cell_p->next = Cell_head.cell[ hash_value ];
	Cell_head.cell[ hash_value ] = cell_p;
}

/********************************************************************************
 * << Function >>
 *		cell�����Ͽ������ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      ��Ͽ����cell�Υݥ���
 * n_in          I      Ulong       cell�����Ͽ�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/9
 *
 ********************************************************************************/

void cell_entry_n_in
(
 CELL *cell_p, 
 Ulong n_in
 ){
	/*
	  char *func_name = "cell_entry_n_in";
	*/
	cell_p->n_in  = n_in;
}

/********************************************************************************
 * << Function >>
 *		cell�˽��Ͽ������ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      ��Ͽ����cell�Υݥ���
 * n_out         I      Ulong       cell�ν��Ͽ�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/9
 *
 ********************************************************************************/

void cell_entry_n_out
(
 CELL *cell_p, 
 Ulong n_out
 ){
	/*
	  char *func_name = "cell_entry_n_in";
	*/
	cell_p->n_out  = n_out;
}

/********************************************************************************
 * << Function >>
 *		cell�˸ξ�����ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      ��Ͽ����cell�Υݥ���
 * fault         I      CELL_FLAG   cell�θξ�ե饰
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/9
 *
 ********************************************************************************/

void cell_entry_fault
(
 CELL *cell_p, 
 CELL_FLAG fault
 ){
	/*
	  char *func_name = "cell_entry_n_in";
	*/
	cell_p->flag  = fault;

}



/********************************************************************************
 * << Function >>
 *		cell_io�򿷤��˺�äơ�cell����Ͽ����cell_io_head����Ͽ����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_io_head  I      Ulong   cell_p�Υϥå�����
 * cell          I      CELL *  ��Ͽ����cell�Υݥ���
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
void cell_make_new_iolist
(
 CELL_IO **cell_io_head, /* ��³����cell��IO */
 CELL *cell /* ��³���cell */
 ){
	CELL_IO *cell_io;

	char *func_name = "cell_make_new_iolist";

	cell_io = ( CELL_IO * )topgunMalloc( FMT_CELL_IO, sizeof( CELL_IO ),
										  1, func_name);
	cell_io->cell = cell;
	cell_io->next = *cell_io_head;
	*cell_io_head = cell_io;
}

/********************************************************************************
 * << Function >>
 *		��ϩŸ���ˤ��cell�����Ϥ򿷤���cell�����Ϥ��Ϥ�
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * old           I      CELL *      �Ϥ�����cell�Υݥ���  
 * new           I      CELL *      �Ϥ����cell�Υݥ���  
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/7
 *
 ********************************************************************************/

void cell_io_hand_over_input
(
 CELL *old, /* �����³�� */
 CELL *new  /* ������³�� */
 ){
	CELL_IO *old_io = old->in;
	CELL_IO *new_io = new->in;
	CELL_IO *cell_tmp;
	CELL_IO *in_cell;

	char *func_name = "cell_io_hand_over_input";

	test_cell_io_null ( old_io, func_name );
	
	cell_tmp = old_io;  /* cell_tmp���Ϥ���³������ */
	old->in  = old_io->next; /* old->in�򹹿� */

	cell_tmp->next = new_io; /* �Ϥ���³����Ƭ������� */
	new->in = cell_tmp;      /* new->in�򹹿� */

	/* ���Ϥ�CELL�ν��Ͽ��������ѹ�*/
	in_cell = cell_tmp->cell->out;
	while( 1 ) {
		if( in_cell->cell == old ) {
			break;
		}
		in_cell = in_cell->next;

		/* ���Ĥ���ʤ��櫓���ʤ� */
		test_cell_io_null ( in_cell, func_name );
	}
	in_cell->cell = new;
}

/********************************************************************************
 * << Function >>
 *		��ϩŸ���ˤ��cell�ν��Ϥ򿷤���cell�����Ϥ��Ϥ�
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * old           I      CELL *      �Ϥ�����cell�Υݥ���  
 * new           I      CELL *      �Ϥ����cell�Υݥ���  
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/8
 *
 ********************************************************************************/

void cell_io_hand_over_output
(
 CELL *old, /* �����³�� */
 CELL *new  /* ������³�� */
 ){
	CELL_IO *old_io = old->out;
	CELL_IO *new_io = new->out;
	CELL_IO *cell_tmp;
	CELL_IO *out_cell;

	char *func_name = "cell_io_hand_over_output";

	test_cell_io_null ( old_io, func_name );
	
	cell_tmp = old_io;  /* cell_tmp���Ϥ���³������ */
	old->out = old_io->next; /* old->out�򹹿� */

	cell_tmp->next = new_io; /* �Ϥ���³����Ƭ������� */
	new->out = cell_tmp;      /* new->out�򹹿� */


	/* ���Ϥ�CELL�����Ͽ��������ѹ�*/
	out_cell = cell_tmp->cell->in;
	while( 1 ) {
		if( out_cell->cell == old ) {
			break;
		}
		out_cell = out_cell->next;

		/* ���Ĥ���ʤ��櫓���ʤ� */
		test_cell_io_null ( out_cell, func_name );
	}
	out_cell->cell = new;

}

/********************************************************************************
 * << Function >>
 *		make fan-out branch
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

void  cell_make_fanout
(
 void
 ){
	CELL  *cell;
	Ulong i;

	for ( i = 0; i < CELL_HASH; i++ ) {

		cell = Cell_head.cell[ i ];

		while ( cell != NULL ) {
			if( cell->n_out > 1){
				/* �ե���ʥ�������	*/
				cell_insert_fanout( cell );
			}
			cell = cell->next;
		}
	}
}
/********************************************************************************
 * << Function >>
 *		�ºݤ�fanout����������
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stem          I      CELL *  �֥������������륹�ƥ�
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
 /*
                  __										        
          _______|  |			������A_0��A_1�򿷤��˺�������
                 |  |___ X      �������ȥ����� : CELL_BR
       A_0 ______|  |           ��n_in  : 1,  in  : A
          |      |__|           ��n_out : 1,  out : X
   A -----+       __											    
          |______|  |	
       A_1       |  |___ Y
           ______|  |		
                 |__|		
*/
void cell_insert_fanout
(
 CELL *stem
 )
{
	CELL	*o_net;
	CELL	*branch;

	CELL_IO	*out_cell;
	CELL_IO	*in_cell;

	char	name[ CELL_NAME_SIZE ];

	Ulong	branch_count;
	Ulong   hash_value;

	out_cell = stem->out;
	branch_count = 0;

	topgun_print_mes_bench_fanout_info( stem ) ;

	while ( out_cell != NULL ) {
		o_net = out_cell->cell;

		/* �֥���������̾�η��� */
		cell_make_add_name( stem, name, &branch_count );

		topgun_print_mes_bench_char_char( 6, 1, name, stem->name );

		/* �����������(�֥�����)	*/
		branch = cell_make_new_cell( );

		cell_entry_type (branch, CELL_BR );
		cell_entry_name (branch, name );

		/* branch�ϰ����ϰ���� */
		branch->n_in  = 1;
		branch->n_out = 1;

		topgun_print_mes_bench_char_char( 6, 2, stem->name, o_net->name );
		
		cell_make_new_iolist( &branch->in,  stem  );
		cell_make_new_iolist( &branch->out, o_net );

		/* �ơ��֥����Ͽ */
		hash_value =  utility_calc_string_hash_value( name, CELL_HASH );
		cell_entry_table( branch, hash_value );

		/* stem�ν��Ͽ��������ѹ� */
		out_cell->cell = branch;

		/* o_net�����Ͽ��������ѹ�*/
		in_cell = o_net->in;
		while( in_cell != NULL ) {
			if( in_cell->cell == stem ) {
				break;
			}
			in_cell = in_cell->next;
		}
		in_cell->cell = branch;

		out_cell = out_cell->next;
		branch_count++;
	}

	topgun_print_mes_bench_fanout_result( stem );
}


/********************************************************************************
 * << Function >>
 *		fanout branch��̾����Ĥ���(branch_name+����:branch_count)
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stem          I      CELL *  �֥������������륹�ƥ�
 * name          I/O    char *  ���̾��
 * branch_count  I      Ulong   ̾����ź����
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

void cell_make_add_name
(
 CELL  *stem,
 char  *name,
 Ulong *count
 ){
	/* branch��̾����Ĥ��� */
	sprintf( name, "%s_%ld", stem->name, *count);

	/* Ʊ��̾�������뤫����٤� */
	while( cell_search_with_hash_table( name ) != NULL ) {
		/* Ʊ��̾��������Τ�branchcount���Ŀ��䤷�Ƽ���Ĵ�٤� */
		sprintf( name, "%s_%ld", stem->name, ++(*count));
	}
}

/********************************************************************************
 * << Function >>
 *		cell�Υǡ�����¤����Topgun�Υǡ�����¤����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
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

void cell_2_line
(
 void
 ){
    CELL       *cell;
    
    LINE	*line;
    Ulong	pi_pos;
    Ulong	po_pos;
    Ulong	bbpi_pos;
    Ulong	bbpo_pos;
    Ulong	i;      /* CELL_HASH */
    
    Ulong   line_id = 0;
    
    FILE    *fp;
    
    char    *func_name = "cell_2_line";
    
    Line_info.n_line = Cell_head.n_id;
    Line_info.n_pi   = Cell_head.n_pi;
    Line_info.n_po   = Cell_head.n_po;
    Line_info.n_bbpi = Cell_head.n_bbpi;
    Line_info.n_bbpo = Cell_head.n_bbpo;
    
    /* ���ꥢ��������� */
    Line_head = (LINE *)topgunMalloc( FMT_LINE, sizeof( LINE ),
				      Cell_head.n_id, func_name );
    Pi_head   = (LINE **)topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
				       Cell_head.n_pi, func_name );
    Po_head   = (LINE **)topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
				       Cell_head.n_po, func_name );
    
    BBPi_head   = (LINE **)topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
					 Cell_head.n_bbpi, func_name );
    BBPo_head   = (LINE **)topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
					 Cell_head.n_bbpo, func_name );
    
    /* TOPGUN�ǡ������� */
    pi_pos = 0;
    po_pos = 0;
    bbpi_pos = 0;
    bbpo_pos = 0;
    
    fp = topgun_open_add_name
	( TOPGUN_OUTFILE_CIRCUIT, TOPGUN_OUTFILE_NAMEID, "w", func_name );
    
    for( i = 0; i < CELL_HASH ; i++ ) {
	
	cell = Cell_head.cell[i];
	
	while( cell != NULL ) {

	    line = cell_2_line_initialize( cell, line_id++ );
	    
	    /* ̾����id����פ��뤿��Υǡ��� */
	    fprintf( fp, "%ld %2d %s\n", line->line_id, line->type, cell->name );
	    
	    switch ( cell->type ) {
	    case CELL_PI:
		Pi_head[pi_pos++] = line;
		break;
	    case CELL_PO:
		Po_head[po_pos++] = line;
		break;
	    case CELL_BLKI:
		BBPi_head[bbpi_pos++] = line;
		break;
	    case CELL_BLKO:				
		BBPo_head[bbpo_pos++] = line;
		break;
	    default:
		break;
	    }
	    cell = cell->next;
	}
    }
    
    for( i = 0; i < CELL_HASH ; i++ ) {
	
	cell = Cell_head.cell[i];
	
	while( cell != NULL ) {
	    
	    cell_make_fault ( cell, &(Line_head[ cell->id ]) );
	    
	    cell = cell->next;
	}
    }


    topgun_close ( fp, func_name );

    

    /* �������Ͽ�������line_id�ǥ����� */
    qsort((void *)Pi_head, Line_info.n_pi, sizeof(LINE *), cmp_id);

    /* �������Ͽ�������line_id�ǥ����� */
    qsort((void *)Po_head, Line_info.n_po, sizeof(LINE *), cmp_id);


}

/********************************************************************/
/*  �ؿ�̾      :  cmp_id					    */
/*  ����        :  line_id�����(qsort�ؿ���)			    */
/*  �����      :  ��ӷ��					    */
/********************************************************************/
int cmp_id(const void *d0, const void *d1)
{
	return( (*((LINE **)d0))->line_id - (*((LINE **)d1))->line_id);
}

/********************************************************************************
 * << Function >>
 *		cell�Υǡ�����¤���������
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
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

void cell_head_free
(
 void
 ){
	CELL	*cell;
	CELL	*del_cell;
	CELL_IO	*cell_io;
	CELL_IO	*del_cell_io;
	Ulong   i;

	char    *func_name = "cell_head_free";

	for ( i = 0; i < CELL_HASH; i++ ) {

		cell = Cell_head.cell[ i ];

		while( cell != NULL ) {
			cell_io = cell->in;
			/* ���Ϥ������free */
			while( cell_io != NULL ) {
				del_cell_io = cell_io;
				cell_io     = cell_io->next;
				topgunFree( del_cell_io, FMT_CELL_IO, 1, func_name);
			}
			/* ���Ϥ������free */
			cell_io = cell->out;
			while(cell_io != NULL ) {
				del_cell_io = cell_io;
				cell_io     = cell_io->next;
				topgunFree( del_cell_io, FMT_CELL_IO, 1, func_name);
			}
			del_cell = cell;
			cell     = cell->next;
			topgunFree( del_cell, FMT_CELL, 1, func_name );
		}
	}
}


/********************************************************************************
 * << Function >>
 *		make primitive gate
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
 *		2005/9/26
 *
 ********************************************************************************/

void cell_remake_primitive_for_line
(
 void
 )
{
	CELL  *cell;
	Ulong i;

	char *func_name = "cell_make_primitive";
	
	for ( i = 0; i < CELL_HASH; i++ ) {

		cell = Cell_head.cell[ i ];

		while ( cell != NULL ) {
			/* ����Υ����פ��Ȥ˾��ʬ�� */
			/* $$$ �ؿ��Υݥ��󥿲�������!! $$$ */
			switch ( cell->type ) {
			case CELL_PI:
				/* ���⤷�ʤ� */
			case CELL_PO:
				/* ���⤷�ʤ� */
				break;
			case CELL_BR:
				/* �ޤ�̵���Ϥ� */
				topgun_error( FEC_PRG_LINE_TYPE, func_name );
				break;
			case CELL_INV:
				/* ���⤷�ʤ� */
				/* ̵�������⤢�� */
				break;
			case CELL_BUF:
				/* ���⤷�ʤ� */
				/* ̵�������⤢�� */
				break;
			case CELL_AND:
				/* ���Ͽ��˱����ơ�Ÿ������ */
				/*
				cell_make_primitive_and( cell );
				*/
				break;
			case CELL_NAND:
				/* ���Ͽ��˱����ơ�Ÿ������ */
				/*
				cell_make_primitive_nand( cell );
				*/
				break;
			case CELL_OR:
				/* ���Ͽ��˱����ơ�Ÿ������ */
				/*
				cell_make_primitive_or( cell );
				*/
				break;
			case CELL_NOR:
				/* ���Ͽ��˱����ơ�Ÿ������ */
				/*
				cell_make_primitive_nor( cell );
				*/
				break;
			case CELL_XOR:
				/* XOR�����Ȥ��Τ�Τȡ����Ͽ��˱����ơ�Ÿ������ */
				cell_remake_primitive_xor( cell );
				break;
			case CELL_XNOR:
				/* XNOR�����Ȥ��Τ�Τȡ����Ͽ��˱����ơ�Ÿ������ */
				cell_remake_primitive_xnor( cell );
				break;
			case CELL_BLK:
			case CELL_BLKI:
			case CELL_BLKO:
				cell_remake_primitive_blk( cell ); 
				break;
			case CELL_UNK:
				break;
			default:
				break;
			}

			/* �����Ϥ��ʤ������б�(=BlackBOX���ɲ�)�򤹤� */
			cell_add_black_box( cell );
			
			cell = cell->next;
		}
	}
}

/********************************************************************************
 * << Function >>
 *		XOR�����Ȥ�ATPG�Ѥ�AND/OR/INV��ʬ�򤹤�
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
 *		2005/12/7
 *
 ********************************************************************************/

/* XOR�����Ȥ�Ÿ������ 2���ϸ���
       �� branch�ɲ���
	   
                                         +---+
                       -|>(6)-+----------+ A |
                              |          | N(2)-+
        +---+                 | +-|>o(4)-+ D |  |  +---+
     ---+ X |                 | |        +---+  +--+ O | 
        | O(1) ---  ->        | |        +---+     |  (1)---
     ---+ R |                 +---|>o(5)-+ A |  +--+ R |
        +---+                   |        | N(3)-+  +---+
                       -|>(7)---+--------+ D |
                                         +---+

	1: XOR.Y, XOR.A, XOR.B
								  
   (1) OR    ( OR.Y -> XOR.Y, OR.A = AND_A.Y, OR.B = AND_B.Y ) 
   (2) AND_A ( AND_A.Y = OR.A, AND_A.A = BUF_A.Y, AND_A.B = INV_B.Y )
   (3) AND_B ( AND_B.Y = OR.B, AND_B.A = BUF_B.Y, AND_B.B = INV_A.Y )
   (4) INV_A ( INV_A.Y = AND_B.B, INV_A.A = BUF_A.Y )
   (5) INV_B ( INV_B.Y = AND_A.B, INV_B.A = BUF_B.Y )
   (6) BUF_A ( BUF_A.Y = AND_A.A INV_A.A, BUF_A.A -> XOR_A )
   (7) BUF_B ( BUF_B.Y = AND_B.A INV_B.A, BUF_B.A -> XOR_B )										 

    �� branch�ɲø�(���δؿ��Ǥ��ɲä��ʤ�)										 
                                         +---+
                      -|>(10)-+------(6)-+ A |
                              |          | N(2)-+
        +---+                 | +-|>o(4)-+ D |  |  +---+
     ---+ X |                 | (9)      +---+  +--+ O | 
        | O +---  ->        (8) |        +---+     |  (1)---
     ---+ R |                 +---|>o(5)-+ A |  +--+ R |
        +---+                   |        | N(3)-+  +---+
                       -|>(11)--+----(7)-+ D |
                                         +---+										 

	1: XOR.Y, XOR.A, XOR.B
								  
   (1) OR    ( OR.Y -> XOR.Y, OR.A = AND_A.Y, OR.B = AND_B.Y ) 
   (2) AND_A ( AND_A.Y = OR.A, AND_A.A = BR_AA.Y, AND_A.B = INV_B.Y )
   (3) AND_B ( AND_B.Y = OR.B, AND_B.A = BR_BB.Y, AND_B.B = INV_A.Y )
   (4) INV_A ( INV_A.Y = AND_B.B, INV_A.A = BR_AB.Y )
   (5) INV_B ( INV_B.Y = AND_A.B, INV_B.A = BR_BB.Y )
   (6) BR_AA ( BR_AA.Y = AND_A.A, BR_AA.A = BUF_A.Y )
   (7) BR_AB ( BR_AB.Y = INV_A.A, BR_AB.A = BUF_A.Y )
   (8) BR_BA ( BR_BB.Y = AND_B.A, BR_BB.A = BUF_B.Y )
   (9) BR_BB ( BR_BB.Y = INV_B.A, BR_BB.A = BUF_B.Y )
  (10) BUF_A ( BUF_A.Y = BR_AA.A BR_AB.A, BUF_A.A -> XOR_A )
  (11) BUF_B ( BUF_B.Y = BR_BA.A BR_BB.A, BUF_B.A -> XOR_B )

*/

void cell_remake_primitive_xor
(
 CELL *cell
 )
{
	CELL    *and_a;
	CELL    *and_b;	
	CELL    *inv_a;	
	CELL    *inv_b;	
	CELL    *buf_a;	
	CELL    *buf_b;

	Ulong   hash_value = 0;
	
	Ulong   name_count = 0; /* �����˺�������Cell��̾�����տ魯����� */

	char	name[ CELL_NAME_SIZE ];

	/* �����֤����� (1) OR */
	cell_reentry_type ( cell, CELL_OR );             /* type������ */
	
	/* �����������  (2) AND_A  */
	and_a = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( and_a, CELL_AND );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( and_a, name );               /* ̾�������� */
	cell_entry_n_in ( and_a, 2 );                  /* ���Ͽ������� */
	cell_entry_n_out ( and_a, 1 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_a, hash_value );         /* �ơ��֥����Ͽ */

	/* �����������  (3) AND_B  */
	and_b = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( and_b, CELL_AND );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( and_b, name );               /* ̾�������� */
	cell_entry_n_in ( and_b, 2 );                  /* ���Ͽ������� */
	cell_entry_n_out (and_b, 1 );                  /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_b, hash_value );         /* �ơ��֥����Ͽ */
	

	/* �����������  (4) INV_A  */
	inv_a = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( inv_a, CELL_INV );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( inv_a, name );               /* ̾�������� */
	cell_entry_n_in ( inv_a, 1 );                  /* ���Ͽ������� */
	cell_entry_n_out ( inv_a, 1 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_a, hash_value );         /* �ơ��֥����Ͽ */

	/* �����������  (5) INV_B  */
	inv_b = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( inv_b, CELL_INV );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( inv_b, name );               /* ̾�������� */
	cell_entry_n_in ( inv_b, 1 );                  /* ���Ͽ������� */
	cell_entry_n_out ( inv_b, 1 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_b, hash_value );         /* �ơ��֥����Ͽ */

	/* �����������  (6) BUF_A  */
	buf_a = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( buf_a, CELL_BUF );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( buf_a, name );               /* ̾�������� */
	cell_entry_n_in ( buf_a, 1 );                  /* ���Ͽ������� */
	cell_entry_n_out ( buf_a, 2 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_a, hash_value );         /* �ơ��֥����Ͽ */
	
	/* �����������  (7) BUF_B  */
	buf_b = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type( buf_b, CELL_BUF );            /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name( buf_b, name );                /* ̾�������� */
	cell_entry_n_in( buf_b, 1 );                   /* ���Ͽ������� */
	cell_entry_n_out( buf_b, 2 );                  /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_b, hash_value );         /* �ơ��֥����Ͽ */


	/* �ξ���� */
	cell_entry_fault ( cell,  Fault_out_on ); /* ���Ϥθξ�Τ� */
	cell_entry_fault ( buf_a,  Fault_in_on ); /* ���Ϥθξ�Τ� */
	cell_entry_fault ( buf_b,  Fault_in_on ); /* ���Ϥθξ�Τ� */
	
	/* ��³ */
	
	/* ����cell����³���֤����� */

	/* (1)->out */
	/* ���Τޤ޻Ȥ��Τ��֤������ʤ� */


	/* (6)->in */
	cell_io_hand_over_input ( cell,  buf_a );
	
	/* (7)->in */
	cell_io_hand_over_input ( cell,  buf_b );

	/* ������­�� */
	/* (2) -> (1) */
	cell_make_new_iolist( &cell->in,   and_a );
	cell_make_new_iolist( &and_a->out, cell );

	/* (3) -> (1) */
	cell_make_new_iolist( &cell->in,   and_b );
	cell_make_new_iolist( &and_b->out, cell );
	
	/* ����cellƱ�� */
	/* (6) -> (2) */
	cell_make_new_iolist( &and_a->in,  buf_a );
	cell_make_new_iolist( &buf_a->out, and_a );
	
	/* (4) -> (2) */
	cell_make_new_iolist( &and_a->in,  inv_b );
	cell_make_new_iolist( &inv_b->out, and_a );
	
	/* (7) -> (3) */
	cell_make_new_iolist( &and_b->in,  buf_b );
	cell_make_new_iolist( &buf_b->out, and_b );
	
	/* (5) -> (3) */
	cell_make_new_iolist( &and_b->in,  inv_a );
	cell_make_new_iolist( &inv_a->out, and_b );

	/* (6) -> (5) */
	cell_make_new_iolist( &inv_a->in,  buf_a );
	cell_make_new_iolist( &buf_a->out, inv_a );
	
	/* (7) -> (4) */
	cell_make_new_iolist( &inv_b->in,  buf_b );
	cell_make_new_iolist( &buf_b->out, inv_b );
	
}

/********************************************************************************
 * << Function >>
 *		XNOR�����Ȥ�ATPG�Ѥ�AND/NOR/INV��ʬ�򤹤�
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
 *		2005/12/7
 *
 ********************************************************************************/

/* XNOR�����Ȥ�Ÿ������ 2���ϸ���
       �� branch�ɲ���
	   
                                         +---+
                       -|>(6)-+----------+ A |
        +---+                 |          | N(2)-+
        | X |                 | +-|>o(4)-+ D |  |  +---+
     ---+ N |                 | |        +---+  +--+ N | 
        | O(1) ---  ->        | |        +---+     | O(1)---
     ---+ R |                 +---|>o(5)-+ A |  +--+ R |
        +---+                   |        | N(3)-+  +---+
                       -|>(7)---+--------+ D |
                                         +---+

	1: XNOR.Y, XNOR.A, XNOR.B
								  
   (1) NOR   ( NOR.Y -> XNOR.Y, NOR.A = AND_A.Y, NOR.B = AND_B.Y ) 
   (2) AND_A ( AND_A.Y = NOR.A, AND_A.A = BUF_A.Y, AND_A.B = INV_B.Y )
   (3) AND_B ( AND_B.Y = NOR.B, AND_B.A = BUF_B.Y, AND_B.B = INV_A.Y )
   (4) INV_A ( INV_A.Y = AND_B.B, INV_A.A = BUF_A.Y )
   (5) INV_B ( INV_B.Y = AND_A.B, INV_B.A = BUF_B.Y )
   (6) BUF_A ( BUF_A.Y = AND_A.A INV_A.A, BUF_A.A -> XNOR_A )
   (7) BUF_B ( BUF_B.Y = AND_B.A INV_B.A, BUF_B.A -> XNOR_B )										 

    �� branch�ɲø�(���δؿ��Ǥ��ɲä��ʤ�)										 
                                         +---+
                      -|>(10)-+------(6)-+ A |
        +---+                 |          | N(2)-+
        |   |                 | +-|>o(4)-+ D |  |  +---+
     ---+ X |                 | (9)      +---+  +--+ N | 
        | O +---  ->        (8) |        +---+     | O(1)---
     ---+ R |                 +---|>o(5)-+ A |  +--+ R |
        +---+                   |        | N(3)-+  +---+
                       -|>(11)--+----(7)-+ D |
                                         +---+										 

	1: XNOR.Y, XNOR.A, XNOR.B
								  
   (1) NOR   ( NOR.Y -> XNOR.Y, NOR.A = AND_A.Y, NOR.B = AND_B.Y ) 
   (2) AND_A ( AND_A.Y = NOR.A, AND_A.A = BR_AA.Y, AND_A.B = INV_B.Y )
   (3) AND_B ( AND_B.Y = NOR.B, AND_B.A = BR_BB.Y, AND_B.B = INV_A.Y )
   (4) INV_A ( INV_A.Y = AND_B.B, INV_A.A = BR_AB.Y )
   (5) INV_B ( INV_B.Y = AND_A.B, INV_B.A = BR_BB.Y )
   (6) BR_AA ( BR_AA.Y = AND_A.A, BR_AA.A = BUF_A.Y )
   (7) BR_AB ( BR_AB.Y = INV_A.A, BR_AB.A = BUF_A.Y )
   (8) BR_BA ( BR_BB.Y = AND_B.A, BR_BB.A = BUF_B.Y )
   (9) BR_BB ( BR_BB.Y = INV_B.A, BR_BB.A = BUF_B.Y )
  (10) BUF_A ( BUF_A.Y = BR_AA.A BR_AB.A, BUF_A.A -> XNOR_A )
  (11) BUF_B ( BUF_B.Y = BR_BA.A BR_BB.A, BUF_B.A -> XNOR_B )

*/

void cell_remake_primitive_xnor
(
 CELL *cell
 )
{
	CELL    *and_a;
	CELL    *and_b;	
	CELL    *inv_a;	
	CELL    *inv_b;	
	CELL    *buf_a;	
	CELL    *buf_b;

	Ulong   hash_value = 0;
	Ulong   name_count = 0; /* �����˺�������Cell��̾�����տ魯����� */

	char	name[ CELL_NAME_SIZE ];

	/* �����֤����� (1) NOR */
	cell_reentry_type ( cell, CELL_NOR );          /* type������ */
	
	/* �����������  (2) AND_A  */
	and_a = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( and_a, CELL_AND );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( and_a, name );               /* ̾�������� */
	cell_entry_n_in ( and_a, 2 );                  /* ���Ͽ������� */
	cell_entry_n_out ( and_a, 1 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_a, hash_value );         /* �ơ��֥����Ͽ */

	/* �����������  (3) AND_B  */
	and_b = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( and_b, CELL_AND );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( and_b, name );               /* ̾�������� */
	cell_entry_n_in ( and_b, 2 );                  /* ���Ͽ������� */
	cell_entry_n_out (and_b, 1 );                  /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_b, hash_value );         /* �ơ��֥����Ͽ */
	

	/* �����������  (4) INV_A  */
	inv_a = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( inv_a, CELL_INV );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( inv_a, name );               /* ̾�������� */
	cell_entry_n_in ( inv_a, 1 );                  /* ���Ͽ������� */
	cell_entry_n_out ( inv_a, 1 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_a, hash_value );         /* �ơ��֥����Ͽ */

	/* �����������  (5) INV_B  */
	inv_b = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( inv_b, CELL_INV );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( inv_b, name );               /* ̾�������� */
	cell_entry_n_in ( inv_b, 1 );                  /* ���Ͽ������� */
	cell_entry_n_out ( inv_b, 1 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_b, hash_value );         /* �ơ��֥����Ͽ */

	/* �����������  (6) BUF_A  */
	buf_a = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type ( buf_a, CELL_BUF );           /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name ( buf_a, name );               /* ̾�������� */
	cell_entry_n_in ( buf_a, 1 );                  /* ���Ͽ������� */
	cell_entry_n_out ( buf_a, 2 );                 /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_a, hash_value );         /* �ơ��֥����Ͽ */
	
	/* �����������  (7) BUF_B  */
	buf_b = cell_make_new_cell( );                 /* cell����� */
	cell_entry_type( buf_b, CELL_BUF );            /* type������ */
	cell_make_add_name( cell, name, &name_count ); /* ̾������� */
	cell_entry_name( buf_b, name );                /* ̾�������� */
	cell_entry_n_in( buf_b, 1 );                   /* ���Ͽ������� */
	cell_entry_n_out( buf_b, 2 );                  /* ���Ͽ������� */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_b, hash_value );         /* �ơ��֥����Ͽ */


	/* �ξ���� */
	cell_entry_fault ( cell,  Fault_out_on ); /* ���Ϥθξ�Τ� */
	cell_entry_fault ( buf_a,  Fault_in_on ); /* ���Ϥθξ�Τ� */
	cell_entry_fault ( buf_b,  Fault_in_on ); /* ���Ϥθξ�Τ� */


	/* ��³ */
	
	/* ����cell����³���֤����� */

	/* (1)->out */
	/* ���Τޤ޻Ȥ��Τ��֤������ʤ� */

	/* (6)->in */
	cell_io_hand_over_input ( cell,  buf_a );
	
	/* (7)->in */
	cell_io_hand_over_input ( cell,  buf_b );

	/* ������­�� */
	/* (2) -> (1) */
	cell_make_new_iolist( &cell->in,   and_a );
	cell_make_new_iolist( &and_a->out, cell );

	/* (3) -> (1) */
	cell_make_new_iolist( &cell->in,   and_b );
	cell_make_new_iolist( &and_b->out, cell );
	
	/* ����cellƱ�� */
	/* (6) -> (2) */
	cell_make_new_iolist( &and_a->in,  buf_a );
	cell_make_new_iolist( &buf_a->out, and_a );
	
	/* (4) -> (2) */
	cell_make_new_iolist( &and_a->in,  inv_b );
	cell_make_new_iolist( &inv_b->out, and_a );
	
	/* (7) -> (3) */
	cell_make_new_iolist( &and_b->in,  buf_b );
	cell_make_new_iolist( &buf_b->out, and_b );
	
	/* (5) -> (3) */
	cell_make_new_iolist( &and_b->in,  inv_a );
	cell_make_new_iolist( &inv_a->out, and_b );

	/* (6) -> (5) */
	cell_make_new_iolist( &inv_a->in,  buf_a );
	cell_make_new_iolist( &buf_a->out, inv_a );
	
	/* (7) -> (4) */
	cell_make_new_iolist( &inv_b->in,  buf_b );
	cell_make_new_iolist( &buf_b->out, inv_b );
	
}

/********************************************************************************
 * << Function >>
 *		XNOR�����Ȥ�ATPG�Ѥ�AND/NOR/INV��ʬ�򤹤�
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
 *		2005/12/7
 *
 ********************************************************************************/

/*
  BLAKBOX�����Ȥ�Ÿ������
  ���٤Ƥ�BlackBox��1����1���Ϥˤ���
*/

void cell_remake_primitive_blk
(
 CELL *cell
 )
{
	Ulong    bb_in  = cell->n_in;  /* black box�����Ͽ� */
	Ulong    bb_out = cell->n_out; /* black box�ν��Ͽ� */

	Ulong    hash_value = 0;
	Ulong    name_count = 0;

	Ulong    i = 0; /* cell�����Ͽ�/���Ͽ��Υ����� */
	
	CELL     *new_cell = NULL;

	char	 name[ CELL_NAME_SIZE ];
	
		
	if ( ( bb_out == 0 ) &&
		 ( bb_in  == 0 ) ) {
		; /* �⤭BlackBox�Ϥʤˤ⤷�ʤ� */
	}
	else if ( ( bb_in + bb_out ) == 1 ) {
		; /* �����Ϥ⤷���ϰ���Ϥ�BlackBox�ϲ��⤷�ʤ� */

		if ( bb_in == 1 ) {
			Cell_head.n_bbpo++;
			cell->type = CELL_BLKO;
		}
		else if ( bb_out == 1 ) {
			Cell_head.n_bbpi++;
			cell->type = CELL_BLKI;
		}
	}
	else {
		/* blackbox���ɲä��� */
		if ( bb_in == 0 ) {
			bb_out--; /* ���ꥸ�ʥ�����¦�� */
			cell->type = CELL_BLKI;
			Cell_head.n_bbpi++;
		}
		else {
			bb_in--; /* ���ꥸ�ʥ������¦�� */
			cell->type = CELL_BLKO;
			Cell_head.n_bbpo++;
		}

		/*
		  ���Ϥ�ʣ������blackbox�����Ϥ��Ȥ�ʬ�䤹�� 
                  +---+            +---+
		     -----+ B |   ->   ----+ B |
             -----+ B |            | B |
			      +---+            +---+

				                   +---+
							   ----+ B |
							       | B | ( TOPGUN_BLKO )
							       +---+
		*/
		
		for ( i = 0; i < bb_in ; i++ ) {
				
			new_cell = cell_make_new_cell( );                  /* cell����� */
			cell_entry_type ( new_cell, CELL_BLKO );           /* type������ */
			cell_entry_n_in ( new_cell, 1 );                   /* ���Ͽ������� */
			cell_entry_n_out ( new_cell, 0 );                  /* ���Ͽ������� */
			cell_entry_fault ( new_cell, Fault_in_on );        /* �ξ����� */
			cell_make_add_name( cell, name, &name_count );     /* ̾������� */
			cell_entry_name ( new_cell, name );                /* ̾�������� */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );          /* �ơ��֥����Ͽ */
			cell_io_hand_over_input ( cell,  new_cell );       /* �Ĥʤ����� */

			Cell_head.n_bbpo++;

		}

		/*
		  ���Ϥ�ʣ������blackbox����Ϥ��Ȥ�ʬ�䤹�� 
             +---+            +---+
		     | B +----   ->   | B +----
             | B +----        | B |
			 +---+            +---+

				              +---+
							  | B +----
			( TOPGUN_BLK I)	  | B |
							  +---+
		*/
		
		for ( i = 0; i < bb_out ; i++ ) {
				
			new_cell = cell_make_new_cell( );              /* cell����� */
			cell_entry_type ( new_cell, CELL_BLKI );       /* type������ */
			cell_make_add_name( cell, name, &name_count ); /* ̾������� */
			cell_entry_name ( new_cell, name );            /* ̾�������� */
			cell_entry_n_in ( new_cell, 0 );               /* ���Ͽ������� */
			cell_entry_n_out ( new_cell, 1 );              /* ���Ͽ������� */
			cell_entry_fault ( new_cell, Fault_out_on );   /* �ξ����� */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );      /* �ơ��֥����Ͽ */
			cell_io_hand_over_output ( cell,  new_cell );  /* �Ĥʤ����� */

			Cell_head.n_bbpi++;
		}
	}
}
/********************************************************************************
 * << Function >>
 *		Cell_head�γ������Ͽ��������䤹
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
 * name			(I/O)	type	    description
 * Cell_head     I      CELL_HEAD   cell structure infomation
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_head_pi_count_up
(
 void
 ){
	Cell_head.n_pi++;
}


/********************************************************************************
 * << Function >>
 *		Cell_head�γ������Ͽ��������䤹
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
 * name			(I/O)	type	    description
 * Cell_head     I      CELL_HEAD   cell structure infomation
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_head_po_count_up
(
 void
 ){
	Cell_head.n_po++;
}

/********************************************************************************
 * << Function >>
 *		cell�γ������Ͽ��������䤹
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_p        I      CELL *  cell pointer
 *
 * << extern >>
 * name			(I/O)	type	    description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_in_count_up
(
 CELL *cell_p
 ){
	cell_p->n_in++;
}

/********************************************************************************
 * << Function >>
 *		cell�γ������Ͽ��������䤹
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_p        I      CELL *  cell pointer
 *
 * << extern >>
 * name			(I/O)	type	    description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

void cell_out_count_up
(
 CELL *cell_p
 ){
	cell_p->n_out++;
}


/********************************************************************************
 * << Function >>
 *		Cell����Ͽ��̵ͭ���ǧ�����ʤ���Х����Ȥ��������
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * out_name      I      char *  ��Ͽ����cell��̾��   
 *
 * << extern >>
 * name			(I/O)	type	    description
 * Cell_head     I      CELL_HEAD   cell structure infomation
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

CELL *cell_check_and_make_gate
(
 char *cell_name,
 CELL_TYPE type
  ){
	Ulong hash_value;
	CELL  *cell_p;

	topgun_print_mes_bench_char( 2, 1 , cell_name );
	
	cell_p = cell_search_with_hash_table( cell_name );

	if ( cell_p == NULL ) {
		/* �ơ��֥�̤��Ͽ������	*/
		/* ����κ��� */

		cell_p = cell_make_new_cell();
		cell_entry_name ( cell_p, cell_name );
		cell_entry_fault ( cell_p, Fault_in_out_on ); /* cell�θξ����� */

		hash_value =  utility_calc_string_hash_value( cell_name, CELL_HASH );
		
		cell_entry_table( cell_p, hash_value );
	}
	cell_entry_type ( cell_p, type );

	return ( cell_p );
	
}

/********************************************************************************
 * << Function >>
 *		Cell�򿷵��˳��ݤ���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * cell_p       ���ݤ���Cell�Υݥ���
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	    description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/11/29
 *
 ********************************************************************************/

CELL *cell_make_new_cell
(
 void
  ){
	CELL *cell_p;

	char *func_name = "cell_make_new_cell";
	
	/* cell�Υ������ */
	cell_p = ( CELL * )topgunMalloc( FMT_CELL, sizeof( CELL ), 1, func_name );
	
	/* initialized */
	cell_p->name  = NULL;
	cell_p->id    = Cell_head.n_id++;
	cell_p->flag  = Fault_off;
	cell_p->type  = CELL_UNK;
	cell_p->n_in  = 0;
	cell_p->n_out = 0;
	cell_p->in    = NULL;
	cell_p->out   = NULL;
	cell_p->next  = NULL;

	return ( cell_p );
	
}

/********************************************************************************
 * << Function >>
 *		cell�Υǡ�����¤����Topgun�γƿ��������򥫥���Ȥ���
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/6
 *
 ********************************************************************************/

void cell_add_black_box
(
 CELL *cell
 ){

	CELL    *new_cell;
	Ulong   name_count = 0; /* �ɲä���BlackBox��̾����ź�� */
	
	Ulong   in_out;

	Ulong   hash_value;

	Ulong   i; /* ��������Ͽ������Ͽ� */
	
	Ulong   bb_in;
	Ulong   bb_out;

	CELL_IO *cell_io;

	char	name[ CELL_NAME_SIZE ];
	
	char    *func_name = "cell_add_black_box";


	/* BLK���б� */

	/* case 2: ���ϡ����Ϥ������ˤʤ���� */
	/* ���Ϥ�����Ƥ��륲���Ȥؤ��б� */
	if ( ( cell->n_out == 0 ) &&
		 ( cell->type != CELL_PO ) &&
		 ( cell->type != CELL_BLK ) &&
		 ( cell->type != CELL_BLKO ) ) {

		new_cell = cell_make_new_cell( );              /* cell����� */
		cell_entry_type ( new_cell, CELL_BLKO );       /* type������ */
		cell_make_add_name( cell, name, &name_count ); /* ̾������� */
		cell_entry_name ( new_cell, name );            /* ̾�������� */
		cell_entry_n_in ( new_cell, 1 );               /* ���Ͽ������� */
		cell_entry_n_out ( new_cell, 0 );              /* ���Ͽ������� */
		hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
		cell_entry_table( new_cell, hash_value );      /* �ơ��֥����Ͽ */


		cell_entry_n_out ( cell, 1 );                  /* blk�طҤ� */

		/* ��³���ղä��� */
		cell_make_new_iolist( &cell->out,  new_cell  );
		cell_make_new_iolist( &new_cell->in, cell );
		
		return;
	}

	/* ���Ϥ�����Ƥ��륲���Ȥؤ��б� */
	if ( ( cell->n_in == 0 ) &&
		 ( cell->type != CELL_PI ) &&
		 ( cell->type != CELL_BLK ) &&
		 ( cell->type != CELL_BLKI ) ) {

		topgun_error( FEC_PRG_CELL, func_name );
		
		new_cell = cell_make_new_cell( );              /* cell����� */
		cell_entry_type ( new_cell, CELL_BLKI );       /* type������ */
		cell_make_add_name( cell, name, &name_count ); /* ̾������� */
		cell_entry_name ( new_cell, name );            /* ̾�������� */
		cell_entry_n_in ( new_cell, 0 );               /* ���Ͽ������� */
		cell_entry_n_out ( new_cell, 1 );              /* ���Ͽ������� */
		hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
		cell_entry_table( new_cell, hash_value );      /* �ơ��֥����Ͽ */

		cell_entry_n_in ( cell, 1 );                   /* blk�طҤ� */

		/* ��³���ղä��� */
		cell_make_new_iolist( &cell->in,  new_cell  );
		cell_make_new_iolist( &new_cell->out, cell );

		return;
	}


	/* case 3: ���ϡ����Ͽ�����­���Ƥ��� */
	/* bench�Ǥ�ɬ�פʤ����� */
	/* ���Ͽ������Ͽ���������פ����� */
	in_out = 0;
	cell_io = cell->in;

	while ( cell_io != NULL ) {
		if ( cell_io->cell != NULL ) {
			in_out++;
		}
		cell_io = cell_io->next;
	}

	if ( in_out > cell->n_in ) {
		/* ��­���Ƥ��� */
		topgun_error ( FEC_PRG_LINE_IN, func_name );
	}

	if ( in_out < cell->n_in ) {

		bb_in = cell->n_in - in_out;

		for ( i = 0; i < bb_in ; i++ ) {
				
			new_cell = cell_make_new_cell( );                  /* cell����� */
			cell_entry_type ( new_cell, CELL_BLKI );           /* type������ */
			cell_entry_n_in ( new_cell, 0 );                   /* ���Ͽ������� */
			cell_entry_n_out ( new_cell, 1 );                  /* ���Ͽ������� */
			cell_make_add_name( cell, name, &name_count );     /* ̾������� */
			cell_entry_name ( new_cell, name );                /* ̾�������� */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );          /* �ơ��֥����Ͽ */

			/* ��³���ղä��� */
			cell_make_new_iolist( &cell->in,  new_cell  );
			cell_make_new_iolist( &new_cell->out, cell );
			
		}
	}

	/* ���Ͽ��Ƚ��Ͽ���������פ����� */
	in_out = 0;
	cell_io = cell->out;
	
	while ( cell_io != NULL ) {
		if ( cell_io->cell != NULL ) {
			in_out++;
		}
		cell_io = cell_io->next;
	}
	if ( in_out > cell->n_out ) {
		/* ��­���Ƥ��� */
		topgun_error ( FEC_PRG_LINE_OUT, func_name );
	}

	if ( in_out < cell->n_out ) {

		bb_out = cell->n_out - in_out;
		
		for ( i = 0; i < bb_out ; i++ ) {
			new_cell = cell_make_new_cell( );              /* cell����� */
			cell_entry_type ( new_cell, CELL_BLKO );       /* type������ */
			cell_make_add_name( cell, name, &name_count ); /* ̾������� */
			cell_entry_name ( new_cell, name );            /* ̾�������� */
			cell_entry_n_in ( new_cell, 1 );               /* ���Ͽ������� */
			cell_entry_n_out ( new_cell, 0 );              /* ���Ͽ������� */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );      /* �ơ��֥����Ͽ */

			/* ��³���ղä��� */
			cell_make_new_iolist( &cell->out,  new_cell  );
			cell_make_new_iolist( &new_cell->in, cell );

		}
	}
}

/********************************************************************************
 * << Function >>
 *		Cell���Ѵ�����line�ν����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * line         ���������line�Υݥ���
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_p        I      CELL *  line���Ѵ�����Cell�Υݥ���
 * line_id       I      Ulong * line��id
 *
 * << extern >>
 * name			(I/O)	type	    description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/7
 *
 ********************************************************************************/

LINE *cell_2_line_initialize 
(
 CELL *cell_p,
 Ulong line_id
  ){
	LINE    *line;
	CELL_IO *cell_io;
	Ulong	io_pos;

	char *func_name = "cell_2_line_initialize";
	
	//! initailize
	line           = &Line_head[ cell_p->id ];
	line->line_id  = cell_p->id;
	line->type     = cell_type_2_line_type( cell_p->type );
	line->state9   = STATE9_XX;
	line->imp_0    = NULL;
	line->imp_1    = NULL;
	
	line->cnt_propa = 0;
	
	line->tm_c0    = TM_INF;
	line->tm_c1    = TM_INF;
	line->tm_co    = TM_INF;
	line->tm_bb_c0 = TM_INF;
	line->tm_bb_c1 = TM_INF;
	line->tm_bb_co = TM_INF;
	line->flag     = 0;
	line->lv_pi    = 0;
	line->lv_po    = 0;
	
	line->imp_id_n = 0;
	line->imp_id_f = 0;

	
	line->flist[ FSIM_SA0 ] = NULL;
	line->flist[ FSIM_SA1 ] = NULL;

	line->fault_set_flag = 0;
	line->fprop_flag     = 0;
	line->mp_flag        = 0;
	line->n_val_a        = 0;
	line->n_val_b        = 0;
	line->f_val_a        = 0;
	line->f_val_a        = 0;
	line->event_line     = NULL;

	line->reconv_info    = NULL;
	line->out_info       = NULL;

	line->ffrg           = NULL;

	/* line->in[]�κ��� */
	line->n_in     = cell_p->n_in;
	line->in       = (LINE **)topgunMalloc( FMT_LINE_P , sizeof( LINE * ),
											 line->n_in, func_name );
	/* �����󥿤ν���� */
	io_pos = 0;

	cell_io = cell_p->in;
	while ( cell_io != NULL ) {
		/* ���Ϥ���³ */
		line->in[ io_pos++ ] = &Line_head[ cell_io->cell->id ];
		cell_io = cell_io->next;
	}

	/* line->out[]�κ��� */
	line->n_out = cell_p->n_out;
	line->out = (LINE **)topgunMalloc( FMT_LINE_P , sizeof( LINE * ),
										line->n_out, func_name );

	/* �����󥿤ν���� */
	io_pos = 0;
			
	cell_io = cell_p->out;
	while (cell_io != NULL ) {
		/* ���Ϥ���³ */
		line->out[io_pos++] = &Line_head[ cell_io->cell->id ];
		cell_io = cell_io->next;
	}

	return ( line );
}


/********************************************************************************
 * << Function >>
 *		Cell type��line type���Ѵ�����
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * line_type    �Ѵ�����line�Υ�����
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_type     I      CELL_TYPE   line���Ѵ�����Cell�Υݥ���
 *
 * << extern >>
 * name			(I/O)	type	    description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/12
 *
 ********************************************************************************/

LINE_TYPE cell_type_2_line_type
(
 CELL_TYPE cell_type
  ){

	char *func_name = "cell_type_2_line_type";
	
	switch ( cell_type ) {
	case CELL_PI:
		return ( TOPGUN_PI ) ;
	case CELL_PO:
		return ( TOPGUN_PO );
	case CELL_BUF:
		return ( TOPGUN_BUF );
	case CELL_INV:
		return ( TOPGUN_INV );
	case CELL_AND:
		return ( TOPGUN_AND );
	case CELL_NAND:
		return ( TOPGUN_NAND );
	case CELL_OR:
		return ( TOPGUN_OR );
	case CELL_NOR:
		return ( TOPGUN_NOR );
	case CELL_BLKI:
		return ( TOPGUN_BLKI );
	case CELL_BLKO:
		return ( TOPGUN_BLKO );		
	case CELL_BR:
		return ( TOPGUN_BR );
	case CELL_XOR:
	case CELL_XNOR:
	case CELL_SEL:
	case CELL_FF:
	case CELL_BLK:		
	case CELL_UNK:
	default:
		topgun_error ( FEC_PRG_CELL, func_name );
	}

	return ( TOPGUN_UNK ); /* ��ʤ� */
}

/********************************************************************************
 * << Function >>
 *		Cell��flag�ˤ��ξ�����ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell          I      CELL *      �ξ�����ꤹ��Cell�Υݥ���
 * line          I      LINE *      �ξ�����ꤹ��line�Υݥ���
 *
 * << extern >>
 * name			(I/O)	type	    description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void cell_make_fault
(
 CELL *cell,
 LINE *line
  ){
	char *func_name = "cell_make_fault";
	
	switch ( cell->flag ) {
	case Fault_in_out_on:
		cell_make_fault_in ( cell, line );
		cell_make_fault_out ( cell, line );
		break;
	case Fault_in_on:
		cell_make_fault_in ( cell, line );
		break;
	case Fault_out_on:
		cell_make_fault_out ( cell, line );
		break;
	case Fault_off:
		break;
	default:
		topgun_error ( FEC_PRG_CELL, func_name );
	}
}

/********************************************************************************
 * << Function >>
 *		Cell�����Ϥ˸ξ�����ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell          I      CELL *      �ξ�����ꤹ��Cell�Υݥ���
 * line          I      LINE *      �ξ�����ꤹ��line�Υݥ���
 *
 * << extern >>
 * name			(I/O)	type	    description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void cell_make_fault_in
(
 CELL *cell,
 LINE *line
  ){

	FLIST *flist;
	Ulong i;
	
	char *func_name = "cell_make_fault_in";
	
	test_number_equal_ulong ( cell->n_in, line->n_in, func_name );

	for ( i = 0;  i < cell->n_in ; i++ ) {
	
		flist = topgunMalloc( FMT_FLIST, sizeof( FLIST ), 1, func_name );	
	
		flist->f_id  = Line_info.n_fault++;
		flist->info  = TOPGUN_SA0;
		flist->n_eqf = 1;
		flist->eqf   = NULL;
		flist->line  = line->in[ i ];

		topgun_print_mes_cell_fault ( cell, flist->line, flist->f_id );

		if ( Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ] == NULL ) {
			flist->next  = NULL;
			Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ] = flist;
		}
		else {
			flist->next = Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ];
			Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ] = flist;
		}
			
		flist = topgunMalloc( FMT_FLIST, sizeof( FLIST ), 1, func_name );	
	
		flist->f_id  = Line_info.n_fault++;
		flist->info  = TOPGUN_SA1;
		flist->n_eqf = 1;
		flist->eqf   = NULL;
		flist->line  = line->in[ i ];

		if ( Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ] == NULL ) {
			flist->next  = NULL;
			Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ] = flist;
		}
		else {
			flist->next = Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ];
			Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ] = flist;
		}
	}
}

/********************************************************************************
 * << Function >>
 *		Cell�ν��Ϥ˸ξ�����ꤹ��
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell          I      CELL *      �ξ�����ꤹ��Cell�Υݥ���
 * line          I      LINE *      �ξ�����ꤹ��line�Υݥ���
 *
 * << extern >>
 * name			(I/O)	type	    description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/13
 *
 ********************************************************************************/

void cell_make_fault_out
(
 CELL *cell,
 LINE *line
  ){

	FLIST *flist;
	
	char *func_name = "cell_make_fault_out";
	
	test_number_equal_ulong ( cell->n_out, line->n_out, func_name );

	/* $$$ cell��port�ˤʤ�ޤ����� $$$ */
	/*
	for ( i = 0;  i < cell->n_out ; i++ ) {
	*/
	
		flist = topgunMalloc( FMT_FLIST, sizeof( FLIST ), 1, func_name );	

		
		flist->f_id  = Line_info.n_fault++; 
		flist->info  = TOPGUN_SA0;
		flist->n_eqf = 1;
		flist->eqf   = NULL;
		flist->line  = line;

		topgun_print_mes_cell_fault ( cell, flist->line, flist->f_id );
		
		if ( Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ] == NULL ) {
			flist->next  = NULL;
			Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ] = flist;
		}
		else {
			flist->next = Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ];
			Line_head[ flist->line->line_id ].flist[ FSIM_SA0 ] = flist;
		}
		
		
		flist = topgunMalloc( FMT_FLIST, sizeof( FLIST ), 1, func_name );	
	
		flist->f_id  = Line_info.n_fault++; 
		flist->info  = TOPGUN_SA1;
		flist->n_eqf = 1;
		flist->eqf   = NULL;
		flist->line  = line;

		if ( Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ] == NULL ) {
			flist->next  = NULL;
			Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ] = flist;
		}
		else {
			flist->next = Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ];
			Line_head[ flist->line->line_id ].flist[ FSIM_SA1 ] = flist;
		}
		/*
		  }*/
}



/* print function */
/* Cell_head��internal�Τ��� */

void topgun_print_mes_cell_head ( void ){
#ifdef _DEBUG_INPUT_
	CELL	*net;
	CELL_IO	*io_net;
	Ulong	i;

	printf("**************************************************************\n");
	printf("id_count : %ld\n",Cell_head.n_id);
	printf("pi_count : %ld\n",Cell_head.n_pi);
	printf("po_count : %ld\n",Cell_head.n_po);
	printf("??????????????????????????????????????????????????????????????\n");
	printf("              name   : id   :  type : in(name) : out(name)\n"); 
	printf("**************************************************************\n");	
	for(i = 0; i < CELL_HASH; i++){
		net = Cell_head.cell[i];
		while( net != NULL ){
			printf("%20s : ",net->name);
			printf("%6ld : ",net->id);
			switch( net->type ){
			case CELL_PI:   printf("%5s ","PI"); break;
			case CELL_PO:   printf("%5s ","PO"); break;
			case CELL_AND:  printf("%5s ","AND"); break;
			case CELL_NAND: printf("%5s ","NAND"); break;
			case CELL_OR:   printf("%5s ","OR"); break;
			case CELL_NOR:  printf("%5s ","NOR"); break;
			case CELL_XOR:  printf("%5s ","XOR"); break;
			case CELL_XNOR: printf("%5s ","XNOR"); break;
			case CELL_BR:   printf("%5s ","FOUT"); break;
			case CELL_INV:  printf("%5s ","NOT"); break;
			case CELL_BUF:  printf("%5s ","BUF"); break;
			case CELL_BLK:  printf("%5s ","BLK"); break;
			case CELL_SEL:  printf("%5s ","SEL"); break;
			case CELL_FF:   printf("%5s ","FF"); break;
			default: printf(" type  : UNKNOWN\n"); break;
			}
			printf(": %ld ",net->n_in);
			io_net = net->in;
			while(io_net != NULL){
				printf("%s ",io_net->cell->name);
				io_net = io_net->next;
			}
			printf(": %ld ",net->n_out);
			io_net = net->out;
			while(io_net != NULL){
				printf("%s ",io_net->cell->name);
				io_net = io_net->next;
			}
			topgun_print_mes_n();
			net = net->next;
		}
	}
	printf("**************************************************************\n");
#endif /* _DEBUG_INPUT_ */
}

/* end of file */

