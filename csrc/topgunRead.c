/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ネットリストなどファイル読込に関する関数群
	
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


extern FLIST   **Flist_head; /* 故障データヘッダ */
extern LINE    *Line_head;  /* ATPG用データヘッダ */

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
void cell_head_initialize ( void ); /* Cell_headを初期化する */
void cell_remake_primitive_for_line ( void ); /* primitive展開する */
void cell_print_table( void ); /* Cell_headを出力する */
void cell_make_fanout( void ); /* fanoutを追加する */
void cell_2_line ( void ); /* cellデータをATPG用(LINE)のデータ構造に変換する */
void cell_head_free( void ); /* Cell_headを解放する */

/* topgun_bench.c */
void read_circuit_bench ( char * ); /* benchファイルの読み込む */

/* topgun_flist.c */
void flist_detectable( FLIST * );

/* topgun_output.c */
FILE *topgun_open ( char *, char *, char * ); /* fileを開く */

/* topgun_print.c */
void topgun_print_mes_cell_head ( void );

/* topgun_uty.c */
int   utility_skip_sentence ( FILE * ); /* 1行を読み飛ばす */
int   utility_skip_char ( FILE *, char * ); /* 区切り文字が来るまで読み飛ばし、行頭の文字を得る */
void  utility_token ( FILE *, char *, char *, Ulong ); /* 区切り文字が来るまでの文字を得る */

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
	char  *func_name = "read_circuit_bench"; /* 関数名 */

	cell_head_initialize();
	
	/* 回路の読込 */
	read_circuit_bench( net_list_file_name );
	
	/* 回路のダンプ(for debug) */
	topgun_print_mes_cell_head();

	/* cellのprimitive展開 */
	cell_remake_primitive_for_line();
	
	/* ファンアウトの追加 */
	cell_make_fanout();

	/* 回路のダンプ(for debug) */
	topgun_print_mes_cell_head();

	/* cellをlineデータ構造に変換 */
	cell_2_line();

	/* テーブルの領域を開放 */
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
	
	char  *func_name = "read_detectable_fault"; /* 関数名 */

	//char string[ BENCH_NAME_SIZE ];
	
	/* 故障の読込リストの作成 */
	fp = topgun_open( fault_list_file_name, "r", func_name );

	while( 1 ) {

		head = flist_analyze_header( fp ); /* gateの場合別stringを使って処理が必要 */

		/* type別に処理を行う */
		switch ( head ) {
			case FLIST_ORG :
				/*	故障を登録	*/
				flist_line_check ( fp, &flist );
				break;
			case FLIST_CON :
			case FLIST_COMMENT :
				/*	1行読み飛ばし	*/
				utility_skip_sentence( fp );
				break;
			case FLIST_END :
				return ;
			default :
				utility_skip_sentence( fp );
				break;
		}
		/* ここでは既に検出にする */
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
 FILE *fp   /* ヘッダを解析するファイルのポインタ */
 )
{
	int  c;
	char  *func_name = "flist_analyze_header"; /* 関数名 */
	
	/* コメント対応 */
	/* 改行(DELIMITER_LF)まで進んで行頭の文字をcに代入する */
	c = utility_skip_char( fp, DELIMITER_LF );

	/* #のみ */
 	if( c == '#' ){

		return( FLIST_COMMENT );
	}
	/* oのみ */
 	else if( c == 'o' ){
		return( FLIST_ORG );
	}
	/* eのみ */
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
	Ulong name_size = 11; /* Ulong = 42億で10桁+1 */
	char  *func_name = "flist_line_check"; /* 関数名 */

	char  name[ name_size ];
	
	/* スペース、タブ読み飛ばし */
	utility_skip_char( fp, DELIMITER_LF );

	/* fault_id読込み */
	utility_token( fp, name, DELIMITER_SP, name_size );
	
	/* スペース、タブ読み飛ばし */
	utility_skip_char( fp, DELIMITER_LF );

	/* fault_id読込み */
	utility_token( fp, name, DELIMITER_SP, name_size );

	//printf("falut id %s\n", name );

	flist->f_id = atol( name );
	
	/* スペース、タブ読み飛ばし */
	utility_skip_char( fp, DELIMITER_LF );
	
	/* line_id読込み */
	utility_token( fp, name, DELIMITER_SP, name_size );

	//printf("line id %s\n", name );

	line_id = atol( name );
	flist->line = &Line_head[ line_id ];
	
	/* スペース、タブ読み飛ばし */
	utility_skip_char( fp, DELIMITER_LF );
	
	/* sa0, sa1読込み */
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

	/* スペース、タブ読み飛ばし */
	utility_skip_char( fp, DELIMITER_LF );
	
	/* DET読込み */
	utility_token( fp, name, DELIMITER_SP, name_size );

	//printf("DET %s\n", name );

	if  (  strcmp ( name, "ABT" ) == 0 ) {
		topgun_error( FEC_FAULT_LIST, func_name);
	}

	/* 行末まで移動 */
	utility_skip_sentence( fp );
}

/* end of file */
