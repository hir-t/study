/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief CELL formatに関する関数群
	
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

static CELL_HEAD Cell_head; /* セル登録用データ */

extern LINE_INFO Line_info;  /* ATPG用データヘッダ */
extern LINE      *Line_head;  /* ATPG用データヘッダ */
extern LINE      **Pi_head;  /* ATPG用データヘッダ */
extern LINE      **Po_head;  /* ATPG用データヘッダ */
extern LINE      **BBPi_head;  /* ATPG用データヘッダ */
extern LINE      **BBPo_head;  /* ATPG用データヘッダ */
extern FLIST     **Flist_head; /* 故障データヘッダ */


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
	
	char *func_name = "cell_head_initiaize"; /* 関数名 */
	
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
 *		名前からハッシュ値を求め、ハッシュテーブルからCellを求める
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	cell_p   : ハッシュ値と名前で探索したcellのポインタ
 *	NULL     : 登録されていなかった場合
 *
 * << Argument >>
 * args			(I/O)	type	description
 * position      I      int     ハッシュ値
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
 char *name     /* ゲート名 */
 ){
	CELL	*cell_p;
	Ulong   hash_value;

	/* ハッシュ値を求める */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH );

	cell_p = (CELL *)Cell_head.cell[ hash_value ];

	while( cell_p != NULL ){
		if ( ! ( strcmp( cell_p->name, name ) ) ) {
			return( cell_p );
		}
		cell_p = cell_p->next;
	}
	/* 登録されてなかった場合 */
	return( NULL );
}

/********************************************************************************
 * << Function >>
 *		cellにゲートのタイプを設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      登録するcellのポインタ
 * type          I      CELL_TYPE   cellの型
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
 *		cellにゲートのタイプを再度設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      登録するcellのポインタ
 * type          I      CELL_TYPE   cellの型
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
 *		cellにidを設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      設定するcellのポインタ
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
 *		cellに名前を設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      登録するcellのポインタ
 * name          I      char *      登録するcellの名前
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
 char *gate_name /* ゲート名 */
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
 *		cellをtableにハッシュ値を用いて登録する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_p        I      CELL *  登録するcellのポインタ
 * hash_value    I      Ulong   cell_pのハッシュ値
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
 *		cellに入力数を設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      登録するcellのポインタ
 * n_in          I      Ulong       cellの入力数
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
 *		cellに出力数を設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      登録するcellのポインタ
 * n_out         I      Ulong       cellの出力数
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
 *		cellに故障を設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_p        I      cell *      登録するcellのポインタ
 * fault         I      CELL_FLAG   cellの故障フラグ
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
 *		cell_ioを新たに作って、cellを登録し、cell_io_headへ登録する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_io_head  I      Ulong   cell_pのハッシュ値
 * cell          I      CELL *  登録するcellのポインタ
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
 CELL_IO **cell_io_head, /* 接続元のcellのIO */
 CELL *cell /* 接続先のcell */
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
 *		回路展開によりcellの入力を新たなcellの入力へ渡す
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * old           I      CELL *      渡し元のcellのポインタ  
 * new           I      CELL *      渡す先のcellのポインタ  
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
 CELL *old, /* 旧の接続先 */
 CELL *new  /* 新な接続先 */
 ){
	CELL_IO *old_io = old->in;
	CELL_IO *new_io = new->in;
	CELL_IO *cell_tmp;
	CELL_IO *in_cell;

	char *func_name = "cell_io_hand_over_input";

	test_cell_io_null ( old_io, func_name );
	
	cell_tmp = old_io;  /* cell_tmpに渡す接続を入力 */
	old->in  = old_io->next; /* old->inを更新 */

	cell_tmp->next = new_io; /* 渡す接続を先頭に入れる */
	new->in = cell_tmp;      /* new->inを更新 */

	/* 入力のCELLの出力信号線も変更*/
	in_cell = cell_tmp->cell->out;
	while( 1 ) {
		if( in_cell->cell == old ) {
			break;
		}
		in_cell = in_cell->next;

		/* 見つからないわけがない */
		test_cell_io_null ( in_cell, func_name );
	}
	in_cell->cell = new;
}

/********************************************************************************
 * << Function >>
 *		回路展開によりcellの出力を新たなcellの入力へ渡す
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * old           I      CELL *      渡し元のcellのポインタ  
 * new           I      CELL *      渡す先のcellのポインタ  
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
 CELL *old, /* 旧の接続先 */
 CELL *new  /* 新な接続先 */
 ){
	CELL_IO *old_io = old->out;
	CELL_IO *new_io = new->out;
	CELL_IO *cell_tmp;
	CELL_IO *out_cell;

	char *func_name = "cell_io_hand_over_output";

	test_cell_io_null ( old_io, func_name );
	
	cell_tmp = old_io;  /* cell_tmpに渡す接続を入力 */
	old->out = old_io->next; /* old->outを更新 */

	cell_tmp->next = new_io; /* 渡す接続を先頭に入れる */
	new->out = cell_tmp;      /* new->outを更新 */


	/* 出力のCELLの入力信号線も変更*/
	out_cell = cell_tmp->cell->in;
	while( 1 ) {
		if( out_cell->cell == old ) {
			break;
		}
		out_cell = out_cell->next;

		/* 見つからないわけがない */
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
				/* ファンナウト挿入	*/
				cell_insert_fanout( cell );
			}
			cell = cell->next;
		}
	}
}
/********************************************************************************
 * << Function >>
 *		実際にfanoutを挿入する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stem          I      CELL *  ブランチを挿入するステム
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
          _______|  |			信号線A_0とA_1を新たに作成する
                 |  |___ X      ・ゲートタイプ : CELL_BR
       A_0 ______|  |           ・n_in  : 1,  in  : A
          |      |__|           ・n_out : 1,  out : X
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

		/* ブランチ信号線名の決定 */
		cell_make_add_name( stem, name, &branch_count );

		topgun_print_mes_bench_char_char( 6, 1, name, stem->name );

		/* 新規セル作成(ブランチ用)	*/
		branch = cell_make_new_cell( );

		cell_entry_type (branch, CELL_BR );
		cell_entry_name (branch, name );

		/* branchは一入力一出力 */
		branch->n_in  = 1;
		branch->n_out = 1;

		topgun_print_mes_bench_char_char( 6, 2, stem->name, o_net->name );
		
		cell_make_new_iolist( &branch->in,  stem  );
		cell_make_new_iolist( &branch->out, o_net );

		/* テーブルに登録 */
		hash_value =  utility_calc_string_hash_value( name, CELL_HASH );
		cell_entry_table( branch, hash_value );

		/* stemの出力信号線を変更 */
		out_cell->cell = branch;

		/* o_netの入力信号線を変更*/
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
 *		fanout branchの名前をつくる(branch_name+数字:branch_count)
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * stem          I      CELL *  ブランチを挿入するステム
 * name          I/O    char *  作る名前
 * branch_count  I      Ulong   名前の添数字
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
	/* branchの名前をつくる */
	sprintf( name, "%s_%ld", stem->name, *count);

	/* 同じ名前があるかしらべる */
	while( cell_search_with_hash_table( name ) != NULL ) {
		/* 同じ名前があるのでbranchcountを一つ殖やして次を調べる */
		sprintf( name, "%s_%ld", stem->name, ++(*count));
	}
}

/********************************************************************************
 * << Function >>
 *		cellのデータ構造からTopgunのデータ構造を作る
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
    
    /* メモリアロケーション */
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
    
    /* TOPGUNデータ作成 */
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
	    
	    /* 名前とidを一致するためのデータ */
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

    

    /* 外部入力信号線をline_idでソート */
    qsort((void *)Pi_head, Line_info.n_pi, sizeof(LINE *), cmp_id);

    /* 外部出力信号線をline_idでソート */
    qsort((void *)Po_head, Line_info.n_po, sizeof(LINE *), cmp_id);


}

/********************************************************************/
/*  関数名      :  cmp_id					    */
/*  概要        :  line_idの比較(qsort関数用)			    */
/*  戻り値      :  比較結果					    */
/********************************************************************/
int cmp_id(const void *d0, const void *d1)
{
	return( (*((LINE **)d0))->line_id - (*((LINE **)d1))->line_id);
}

/********************************************************************************
 * << Function >>
 *		cellのデータ構造を解放する
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
			/* 入力があればfree */
			while( cell_io != NULL ) {
				del_cell_io = cell_io;
				cell_io     = cell_io->next;
				topgunFree( del_cell_io, FMT_CELL_IO, 1, func_name);
			}
			/* 出力があればfree */
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
			/* セルのタイプごとに場合分け */
			/* $$$ 関数のポインタ化も視野に!! $$$ */
			switch ( cell->type ) {
			case CELL_PI:
				/* 何もしない */
			case CELL_PO:
				/* 何もしない */
				break;
			case CELL_BR:
				/* まだ無いはず */
				topgun_error( FEC_PRG_LINE_TYPE, func_name );
				break;
			case CELL_INV:
				/* 何もしない */
				/* 無くす場合もあり */
				break;
			case CELL_BUF:
				/* 何もしない */
				/* 無くす場合もあり */
				break;
			case CELL_AND:
				/* 入力数に応じて、展開する */
				/*
				cell_make_primitive_and( cell );
				*/
				break;
			case CELL_NAND:
				/* 入力数に応じて、展開する */
				/*
				cell_make_primitive_nand( cell );
				*/
				break;
			case CELL_OR:
				/* 入力数に応じて、展開する */
				/*
				cell_make_primitive_or( cell );
				*/
				break;
			case CELL_NOR:
				/* 入力数に応じて、展開する */
				/*
				cell_make_primitive_nor( cell );
				*/
				break;
			case CELL_XOR:
				/* XORゲートそのものと、入力数に応じて、展開する */
				cell_remake_primitive_xor( cell );
				break;
			case CELL_XNOR:
				/* XNORゲートそのものと、入力数に応じて、展開する */
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

			/* 入出力がない場合の対応(=BlackBOXを追加)をする */
			cell_add_black_box( cell );
			
			cell = cell->next;
		}
	}
}

/********************************************************************************
 * << Function >>
 *		XORゲートをATPG用にAND/OR/INVに分解する
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

/* XORゲートを展開する 2入力限定
       ■ branch追加前
	   
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

    ■ branch追加後(この関数では追加しない)										 
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
	
	Ulong   name_count = 0; /* 新たに作成したCellの名前に付随する数字 */

	char	name[ CELL_NAME_SIZE ];

	/* セル置き換え (1) OR */
	cell_reentry_type ( cell, CELL_OR );             /* typeを設定 */
	
	/* 新規セル作成  (2) AND_A  */
	and_a = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( and_a, CELL_AND );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( and_a, name );               /* 名前を設定 */
	cell_entry_n_in ( and_a, 2 );                  /* 入力信号線数 */
	cell_entry_n_out ( and_a, 1 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_a, hash_value );         /* テーブルに登録 */

	/* 新規セル作成  (3) AND_B  */
	and_b = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( and_b, CELL_AND );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( and_b, name );               /* 名前を設定 */
	cell_entry_n_in ( and_b, 2 );                  /* 入力信号線数 */
	cell_entry_n_out (and_b, 1 );                  /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_b, hash_value );         /* テーブルに登録 */
	

	/* 新規セル作成  (4) INV_A  */
	inv_a = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( inv_a, CELL_INV );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( inv_a, name );               /* 名前を設定 */
	cell_entry_n_in ( inv_a, 1 );                  /* 入力信号線数 */
	cell_entry_n_out ( inv_a, 1 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_a, hash_value );         /* テーブルに登録 */

	/* 新規セル作成  (5) INV_B  */
	inv_b = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( inv_b, CELL_INV );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( inv_b, name );               /* 名前を設定 */
	cell_entry_n_in ( inv_b, 1 );                  /* 入力信号線数 */
	cell_entry_n_out ( inv_b, 1 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_b, hash_value );         /* テーブルに登録 */

	/* 新規セル作成  (6) BUF_A  */
	buf_a = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( buf_a, CELL_BUF );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( buf_a, name );               /* 名前を設定 */
	cell_entry_n_in ( buf_a, 1 );                  /* 入力信号線数 */
	cell_entry_n_out ( buf_a, 2 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_a, hash_value );         /* テーブルに登録 */
	
	/* 新規セル作成  (7) BUF_B  */
	buf_b = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type( buf_b, CELL_BUF );            /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name( buf_b, name );                /* 名前を設定 */
	cell_entry_n_in( buf_b, 1 );                   /* 入力信号線数 */
	cell_entry_n_out( buf_b, 2 );                  /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_b, hash_value );         /* テーブルに登録 */


	/* 故障定義 */
	cell_entry_fault ( cell,  Fault_out_on ); /* 出力の故障のみ */
	cell_entry_fault ( buf_a,  Fault_in_on ); /* 入力の故障のみ */
	cell_entry_fault ( buf_b,  Fault_in_on ); /* 入力の故障のみ */
	
	/* 接続 */
	
	/* 元のcellの接続の置き換え */

	/* (1)->out */
	/* そのまま使うので置き換えなし */


	/* (6)->in */
	cell_io_hand_over_input ( cell,  buf_a );
	
	/* (7)->in */
	cell_io_hand_over_input ( cell,  buf_b );

	/* 新たに足す */
	/* (2) -> (1) */
	cell_make_new_iolist( &cell->in,   and_a );
	cell_make_new_iolist( &and_a->out, cell );

	/* (3) -> (1) */
	cell_make_new_iolist( &cell->in,   and_b );
	cell_make_new_iolist( &and_b->out, cell );
	
	/* 新規cell同士 */
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
 *		XNORゲートをATPG用にAND/NOR/INVに分解する
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

/* XNORゲートを展開する 2入力限定
       ■ branch追加前
	   
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

    ■ branch追加後(この関数では追加しない)										 
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
	Ulong   name_count = 0; /* 新たに作成したCellの名前に付随する数字 */

	char	name[ CELL_NAME_SIZE ];

	/* セル置き換え (1) NOR */
	cell_reentry_type ( cell, CELL_NOR );          /* typeを設定 */
	
	/* 新規セル作成  (2) AND_A  */
	and_a = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( and_a, CELL_AND );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( and_a, name );               /* 名前を設定 */
	cell_entry_n_in ( and_a, 2 );                  /* 入力信号線数 */
	cell_entry_n_out ( and_a, 1 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_a, hash_value );         /* テーブルに登録 */

	/* 新規セル作成  (3) AND_B  */
	and_b = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( and_b, CELL_AND );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( and_b, name );               /* 名前を設定 */
	cell_entry_n_in ( and_b, 2 );                  /* 入力信号線数 */
	cell_entry_n_out (and_b, 1 );                  /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( and_b, hash_value );         /* テーブルに登録 */
	

	/* 新規セル作成  (4) INV_A  */
	inv_a = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( inv_a, CELL_INV );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( inv_a, name );               /* 名前を設定 */
	cell_entry_n_in ( inv_a, 1 );                  /* 入力信号線数 */
	cell_entry_n_out ( inv_a, 1 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_a, hash_value );         /* テーブルに登録 */

	/* 新規セル作成  (5) INV_B  */
	inv_b = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( inv_b, CELL_INV );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( inv_b, name );               /* 名前を設定 */
	cell_entry_n_in ( inv_b, 1 );                  /* 入力信号線数 */
	cell_entry_n_out ( inv_b, 1 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( inv_b, hash_value );         /* テーブルに登録 */

	/* 新規セル作成  (6) BUF_A  */
	buf_a = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type ( buf_a, CELL_BUF );           /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name ( buf_a, name );               /* 名前を設定 */
	cell_entry_n_in ( buf_a, 1 );                  /* 入力信号線数 */
	cell_entry_n_out ( buf_a, 2 );                 /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_a, hash_value );         /* テーブルに登録 */
	
	/* 新規セル作成  (7) BUF_B  */
	buf_b = cell_make_new_cell( );                 /* cellを確保 */
	cell_entry_type( buf_b, CELL_BUF );            /* typeを設定 */
	cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
	cell_entry_name( buf_b, name );                /* 名前を設定 */
	cell_entry_n_in( buf_b, 1 );                   /* 入力信号線数 */
	cell_entry_n_out( buf_b, 2 );                  /* 出力信号線数 */
	hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
	cell_entry_table( buf_b, hash_value );         /* テーブルに登録 */


	/* 故障定義 */
	cell_entry_fault ( cell,  Fault_out_on ); /* 出力の故障のみ */
	cell_entry_fault ( buf_a,  Fault_in_on ); /* 入力の故障のみ */
	cell_entry_fault ( buf_b,  Fault_in_on ); /* 入力の故障のみ */


	/* 接続 */
	
	/* 元のcellの接続の置き換え */

	/* (1)->out */
	/* そのまま使うので置き換えなし */

	/* (6)->in */
	cell_io_hand_over_input ( cell,  buf_a );
	
	/* (7)->in */
	cell_io_hand_over_input ( cell,  buf_b );

	/* 新たに足す */
	/* (2) -> (1) */
	cell_make_new_iolist( &cell->in,   and_a );
	cell_make_new_iolist( &and_a->out, cell );

	/* (3) -> (1) */
	cell_make_new_iolist( &cell->in,   and_b );
	cell_make_new_iolist( &and_b->out, cell );
	
	/* 新規cell同士 */
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
 *		XNORゲートをATPG用にAND/NOR/INVに分解する
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
  BLAKBOXゲートを展開する
  すべてのBlackBoxを1入力1出力にする
*/

void cell_remake_primitive_blk
(
 CELL *cell
 )
{
	Ulong    bb_in  = cell->n_in;  /* black boxの入力数 */
	Ulong    bb_out = cell->n_out; /* black boxの出力数 */

	Ulong    hash_value = 0;
	Ulong    name_count = 0;

	Ulong    i = 0; /* cellの入力数/出力数のカウンタ */
	
	CELL     *new_cell = NULL;

	char	 name[ CELL_NAME_SIZE ];
	
		
	if ( ( bb_out == 0 ) &&
		 ( bb_in  == 0 ) ) {
		; /* 浮きBlackBoxはなにもしない */
	}
	else if ( ( bb_in + bb_out ) == 1 ) {
		; /* 一入力もしくは一出力のBlackBoxは何もしない */

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
		/* blackboxを追加する */
		if ( bb_in == 0 ) {
			bb_out--; /* オリジナルを出力側へ */
			cell->type = CELL_BLKI;
			Cell_head.n_bbpi++;
		}
		else {
			bb_in--; /* オリジナルは入力側へ */
			cell->type = CELL_BLKO;
			Cell_head.n_bbpo++;
		}

		/*
		  入力が複数あるblackboxを入力ごとに分割する 
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
				
			new_cell = cell_make_new_cell( );                  /* cellを確保 */
			cell_entry_type ( new_cell, CELL_BLKO );           /* typeを設定 */
			cell_entry_n_in ( new_cell, 1 );                   /* 入力信号線数 */
			cell_entry_n_out ( new_cell, 0 );                  /* 出力信号線数 */
			cell_entry_fault ( new_cell, Fault_in_on );        /* 故障設定 */
			cell_make_add_name( cell, name, &name_count );     /* 名前を作成 */
			cell_entry_name ( new_cell, name );                /* 名前を設定 */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );          /* テーブルに登録 */
			cell_io_hand_over_input ( cell,  new_cell );       /* つなぎ換え */

			Cell_head.n_bbpo++;

		}

		/*
		  出力が複数あるblackboxを出力ごとに分割する 
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
				
			new_cell = cell_make_new_cell( );              /* cellを確保 */
			cell_entry_type ( new_cell, CELL_BLKI );       /* typeを設定 */
			cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
			cell_entry_name ( new_cell, name );            /* 名前を設定 */
			cell_entry_n_in ( new_cell, 0 );               /* 入力信号線数 */
			cell_entry_n_out ( new_cell, 1 );              /* 出力信号線数 */
			cell_entry_fault ( new_cell, Fault_out_on );   /* 故障設定 */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );      /* テーブルに登録 */
			cell_io_hand_over_output ( cell,  new_cell );  /* つなぎ換え */

			Cell_head.n_bbpi++;
		}
	}
}
/********************************************************************************
 * << Function >>
 *		Cell_headの外部入力数を一つ増やす
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
 *		Cell_headの外部出力数を一つ増やす
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
 *		cellの外部入力数を一つ増やす
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
 *		cellの外部出力数を一つ増やす
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
 *		Cellの登録の有無を確認し、なければゲートを作成する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * out_name      I      char *  登録するcellの名前   
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
		/* テーブル未登録信号線	*/
		/* セルの作成 */

		cell_p = cell_make_new_cell();
		cell_entry_name ( cell_p, cell_name );
		cell_entry_fault ( cell_p, Fault_in_out_on ); /* cellの故障設定 */

		hash_value =  utility_calc_string_hash_value( cell_name, CELL_HASH );
		
		cell_entry_table( cell_p, hash_value );
	}
	cell_entry_type ( cell_p, type );

	return ( cell_p );
	
}

/********************************************************************************
 * << Function >>
 *		Cellを新規に確保する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * cell_p       確保したCellのポインタ
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
	
	/* cellのメモリ確保 */
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
 *		cellのデータ構造からTopgunの各信号線数をカウントする
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
	Ulong   name_count = 0; /* 追加するBlackBoxの名前の添字 */
	
	Ulong   in_out;

	Ulong   hash_value;

	Ulong   i; /* セルの入力数、出力数 */
	
	Ulong   bb_in;
	Ulong   bb_out;

	CELL_IO *cell_io;

	char	name[ CELL_NAME_SIZE ];
	
	char    *func_name = "cell_add_black_box";


	/* BLKの対応 */

	/* case 2: 入力、出力が完全にない場合 */
	/* 出力がきれているゲートへの対応 */
	if ( ( cell->n_out == 0 ) &&
		 ( cell->type != CELL_PO ) &&
		 ( cell->type != CELL_BLK ) &&
		 ( cell->type != CELL_BLKO ) ) {

		new_cell = cell_make_new_cell( );              /* cellを確保 */
		cell_entry_type ( new_cell, CELL_BLKO );       /* typeを設定 */
		cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
		cell_entry_name ( new_cell, name );            /* 名前を設定 */
		cell_entry_n_in ( new_cell, 1 );               /* 入力信号線数 */
		cell_entry_n_out ( new_cell, 0 );              /* 出力信号線数 */
		hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
		cell_entry_table( new_cell, hash_value );      /* テーブルに登録 */


		cell_entry_n_out ( cell, 1 );                  /* blkへ繋ぐ */

		/* 接続を付加する */
		cell_make_new_iolist( &cell->out,  new_cell  );
		cell_make_new_iolist( &new_cell->in, cell );
		
		return;
	}

	/* 入力がきれているゲートへの対応 */
	if ( ( cell->n_in == 0 ) &&
		 ( cell->type != CELL_PI ) &&
		 ( cell->type != CELL_BLK ) &&
		 ( cell->type != CELL_BLKI ) ) {

		topgun_error( FEC_PRG_CELL, func_name );
		
		new_cell = cell_make_new_cell( );              /* cellを確保 */
		cell_entry_type ( new_cell, CELL_BLKI );       /* typeを設定 */
		cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
		cell_entry_name ( new_cell, name );            /* 名前を設定 */
		cell_entry_n_in ( new_cell, 0 );               /* 入力信号線数 */
		cell_entry_n_out ( new_cell, 1 );              /* 出力信号線数 */
		hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
		cell_entry_table( new_cell, hash_value );      /* テーブルに登録 */

		cell_entry_n_in ( cell, 1 );                   /* blkへ繋ぐ */

		/* 接続を付加する */
		cell_make_new_iolist( &cell->in,  new_cell  );
		cell_make_new_iolist( &new_cell->out, cell );

		return;
	}


	/* case 3: 入力、出力数が不足している */
	/* benchでは必要ないけど */
	/* 入力数と入力信号線を一致させる */
	in_out = 0;
	cell_io = cell->in;

	while ( cell_io != NULL ) {
		if ( cell_io->cell != NULL ) {
			in_out++;
		}
		cell_io = cell_io->next;
	}

	if ( in_out > cell->n_in ) {
		/* 不足している */
		topgun_error ( FEC_PRG_LINE_IN, func_name );
	}

	if ( in_out < cell->n_in ) {

		bb_in = cell->n_in - in_out;

		for ( i = 0; i < bb_in ; i++ ) {
				
			new_cell = cell_make_new_cell( );                  /* cellを確保 */
			cell_entry_type ( new_cell, CELL_BLKI );           /* typeを設定 */
			cell_entry_n_in ( new_cell, 0 );                   /* 入力信号線数 */
			cell_entry_n_out ( new_cell, 1 );                  /* 出力信号線数 */
			cell_make_add_name( cell, name, &name_count );     /* 名前を作成 */
			cell_entry_name ( new_cell, name );                /* 名前を設定 */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );          /* テーブルに登録 */

			/* 接続を付加する */
			cell_make_new_iolist( &cell->in,  new_cell  );
			cell_make_new_iolist( &new_cell->out, cell );
			
		}
	}

	/* 出力数と出力信号線を一致させる */
	in_out = 0;
	cell_io = cell->out;
	
	while ( cell_io != NULL ) {
		if ( cell_io->cell != NULL ) {
			in_out++;
		}
		cell_io = cell_io->next;
	}
	if ( in_out > cell->n_out ) {
		/* 不足している */
		topgun_error ( FEC_PRG_LINE_OUT, func_name );
	}

	if ( in_out < cell->n_out ) {

		bb_out = cell->n_out - in_out;
		
		for ( i = 0; i < bb_out ; i++ ) {
			new_cell = cell_make_new_cell( );              /* cellを確保 */
			cell_entry_type ( new_cell, CELL_BLKO );       /* typeを設定 */
			cell_make_add_name( cell, name, &name_count ); /* 名前を作成 */
			cell_entry_name ( new_cell, name );            /* 名前を設定 */
			cell_entry_n_in ( new_cell, 1 );               /* 入力信号線数 */
			cell_entry_n_out ( new_cell, 0 );              /* 出力信号線数 */
			hash_value =  utility_calc_string_hash_value( name, CELL_HASH ); 
			cell_entry_table( new_cell, hash_value );      /* テーブルに登録 */

			/* 接続を付加する */
			cell_make_new_iolist( &cell->out,  new_cell  );
			cell_make_new_iolist( &new_cell->in, cell );

		}
	}
}

/********************************************************************************
 * << Function >>
 *		Cellを変換するlineの初期化
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * line         初期化したlineのポインタ
 *
 * << Argument >>
 * args			(I/O)	type	description
 * cell_p        I      CELL *  lineに変換するCellのポインタ
 * line_id       I      Ulong * lineのid
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

	/* line->in[]の作成 */
	line->n_in     = cell_p->n_in;
	line->in       = (LINE **)topgunMalloc( FMT_LINE_P , sizeof( LINE * ),
											 line->n_in, func_name );
	/* カウンタの初期化 */
	io_pos = 0;

	cell_io = cell_p->in;
	while ( cell_io != NULL ) {
		/* 入力の接続 */
		line->in[ io_pos++ ] = &Line_head[ cell_io->cell->id ];
		cell_io = cell_io->next;
	}

	/* line->out[]の作成 */
	line->n_out = cell_p->n_out;
	line->out = (LINE **)topgunMalloc( FMT_LINE_P , sizeof( LINE * ),
										line->n_out, func_name );

	/* カウンタの初期化 */
	io_pos = 0;
			
	cell_io = cell_p->out;
	while (cell_io != NULL ) {
		/* 出力の接続 */
		line->out[io_pos++] = &Line_head[ cell_io->cell->id ];
		cell_io = cell_io->next;
	}

	return ( line );
}


/********************************************************************************
 * << Function >>
 *		Cell typeをline typeに変換する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * line_type    変換したlineのタイプ
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell_type     I      CELL_TYPE   lineに変換するCellのポインタ
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

	return ( TOPGUN_UNK ); /* 来ない */
}

/********************************************************************************
 * << Function >>
 *		Cellのflagにより故障を設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell          I      CELL *      故障を設定するCellのポインタ
 * line          I      LINE *      故障を設定するlineのポインタ
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
 *		Cellの入力に故障を設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell          I      CELL *      故障を設定するCellのポインタ
 * line          I      LINE *      故障を設定するlineのポインタ
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
 *		Cellの出力に故障を設定する
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *      Void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * cell          I      CELL *      故障を設定するCellのポインタ
 * line          I      LINE *      故障を設定するlineのポインタ
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

	/* $$$ cellとportになるまで封印 $$$ */
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
/* Cell_headがinternalのため */

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

