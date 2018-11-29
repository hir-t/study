/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief .bench形式ネットリスト読込関数群
	
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

extern void      read_circuit_bench ( char * ); /* bench形式のファイルを読み込み、ATPGのデータを作成する */

static int       bench_analyze_header ( FILE *, char * ); /* 行の先頭から行の内容を判断する */
static CELL_TYPE bench_analyze_gate_type ( char * ); /* benchのゲートタイプをTopgunのゲートに変換する */
static void      bench_make_gateinfo ( FILE *, char *); /* 行の後半を読んでゲートのCellを作成する */
static void      bench_make_piinfo ( FILE * ); /* 行の後半を読んでPIのCellを作成する */
static void      bench_make_poinfo ( FILE * ); /* 行の後半を読んでPOのCellを作成する */
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
void cell_entry_name ( CELL *, char * ); /* セル名を登録する */

/* topgun_output.c */
FILE *topgun_open ( char *, char *, char * ); /* fileを開く */

/* topgun_uty.c */
int   utility_skip_sentence ( FILE * ); /* 1行を読み飛ばす */
int   utility_skip_char ( FILE *, char * ); /* 区切り文字が来るまで読み飛ばし、行頭の文字を得る */
void  utility_token ( FILE *, char *, char *, Ulong ); /* 区切り文字が来るまでの文字を得る */
void  utility_jump_to_char ( FILE *, char ); /* 区切り文字が来るまで読み飛ばす */
Ulong utility_calc_string_hash_value ( char *, Ulong ); /* 文字列とハッシュサイズからハッシュ値を計算する */

/* common function */
void  topgun_error( TOPGUN_ERROR_CODE, char * ); /* エラーメッセージを出力する */

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
    
    /* ファイルを開く */
    fp = topgun_open( net_list_file_name, "r", func_name );
    
    /* ファイルをすべて読み終わるまで繰り返す */
    topgun_print_mes_bench( 0, 0 );
    
    while(1){
	
	head = bench_analyze_header( fp, string ); /* gateの場合別stringを使って処理が必要 */
	
	/* type別に処理を行う */
	switch ( head ) {
	case BENCH_GATE :
	    /*	ゲート入出力線を登録	*/
	    bench_make_gateinfo ( fp, string );
	    break;
	case BENCH_INPUT : 
	    /*	外部入力線を登録	*/
	    bench_make_piinfo( fp );
	    break;
	case BENCH_OUTPUT :
	    /*	外部出力線を登録	*/
	    bench_make_poinfo( fp );
	    break;
	case BENCH_COMMENT :
	    /*	1行読み飛ばし	*/
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
 FILE *fp,   /* ヘッダを解析するファイルのポインタ */
 char *string /* 読み込んだ文字列 */
 )
{
    int  c;
    
    /* コメント対応 */
    /* 改行(DELIMITER_LF)まで進んで行頭の文字をcに代入する */
    c = utility_skip_char( fp, DELIMITER_LF );
    
    /* #のみ */
    if( c == BENCH_STR_COMMENT ){
	
	topgun_print_mes_bench ( 1, 0 );
	return( BENCH_COMMENT );
    }
    else if( c == EOF ) {

	topgun_print_mes_bench ( 1, 1 );
	return( BENCH_END );
    }


    /* 行頭の文字列を取ってくる */
    utility_token( fp, string, DELIMITER_EQ, BENCH_NAME_SIZE );
    
    /* BENCHは行頭の文字列でinput or output or gateの判定が可能 */
    /* input or output or gate の判定 */
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
	CELL  *cell_p; /* 登録するゲートのポインタ */
	char  gate_name[ BENCH_NAME_SIZE ];

	/* ゲートのタイプを判別する */
	utility_jump_to_char( fp, BENCH_STR_GATE_CONECT );	
	c = utility_skip_char( fp, DELIMITER_LF );
	utility_token( fp, gate_name, DELIMITER_LA, BENCH_NAME_SIZE );

	topgun_print_mes_bench_char( 2, 0 , gate_name );

	/* ゲートタイプ解析 */
	type = bench_analyze_gate_type( gate_name );

	/* 登録の確認とcellの作成 */
	cell_p = cell_check_and_make_gate ( out_name, type );

	/* 入力信号線読込み */
	bench_entry_gate_in_net( fp, cell_p );

	/* 行末まで移動 */
	utility_skip_sentence( fp );

}


/********************************************************************************
 * << Function >>
 *		外部出力信号線のCellを作成する
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
  テーブルに登録する名前(go_name)の読込みんで、
  ゲート出力線(p_go)と外部出力線(p_po)を登録する
  外部出力線名はp_goの名前＋"_PO"				
         ______									
  p_go  |      |  p_po						
    ----| port |--------○					
        |      |								
         ------
*/

void bench_make_poinfo
(
 FILE *fp
 ){
	CELL  *input;   /* ゲート出力線	*/
	CELL  *output;	/* 外部出力線   */
	char  input_name[ BENCH_NAME_SIZE ];
	char  output_name[ BENCH_NAME_SIZE ];
	Ulong hash_value;

	/*
	  char *func_name = "bench_make_poinfo";
	*/

	/* Cell_headのn_poを一つふやす */
	cell_head_po_count_up();

	/* 外部出力線名(へ接続しているゲート名)を読込み */
	bench_get_name_io( fp, input_name, BENCH_NAME_SIZE );

	topgun_print_mes_bench_char( 4, 0, input_name );

	input = cell_check_and_make_gate ( input_name, CELL_UNK );
	
	output = cell_make_new_cell();
	
	/* POの前段のゲートと接続 */
	cell_out_count_up( input );
	cell_make_new_iolist( &input->out, output );


	/* 外部出力線情報をセルに設定 */
	bench_define_input_output( BENCH_CELL_OUTPUT, output_name, input_name, output );
	cell_make_new_iolist( &output->in, input );


	/* テーブルに登録 */
	hash_value = utility_calc_string_hash_value( output_name, CELL_HASH );
	cell_entry_table( output, hash_value );

	topgun_print_mes_bench_char( 4, 3, output->name );

}

/********************************************************************************
 * << Function >>
 *		PI/POの信号線名を読み込む
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
 Ulong name_size /* 名前の最大長 */
 ){
	/* 左括弧まで移動 */
	utility_jump_to_char( fp, TOPGUN_LA );

	/* スペース、タブ読み飛ばし */
	utility_skip_char( fp, DELIMITER_LF );

	/* 外部信号線名読込み */
	utility_token( fp, name, DELIMITER_RA, name_size );

	/* 右括弧まで移動 */
	utility_jump_to_char( fp, TOPGUN_RA );

	/* 行末まで移動 */
	utility_skip_sentence( fp );
}






/********************************************************************************
 * << Function >>
 *		PIのcellの作成およびtableへの登録
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

	/* 外部入力線名を読込み */
	bench_get_name_io( fp, name, BENCH_NAME_SIZE );

	topgun_print_mes_bench_char ( 5, 0, name );

	/* Cell_headのn_piを一つ増やす */
	cell_head_pi_count_up();

	/* 外部入力線名を読み込み、外部入力のCELLを作成する */
	cell_p = cell_check_and_make_gate ( name, CELL_PI );

}



/********************************************************************************
 * << Function >>
 *		benchのgate typeをTopgunに変換する
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
		/* 文字を大文字に統一する */
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
	return ( CELL_UNK ); /* こない */
}

/********************************************************************************
 * << Function >>
 *		ゲートの入力セルを読み込んで、新規ならばセルを作成し、接続を行う
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
 * cell_p          I    cell *      入力側を探すcellのポインタ
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
	int   c; /* ただの受け皿 */

	topgun_print_mes_bench ( 7, 0 );
	
	/* 左括弧まで移動 */
	utility_jump_to_char( fp, TOPGUN_LA );

	/* ループ(終了条件：右括弧)	*/
	do{
		/* 空白、タブ読み飛ばし	*/
		/* $$$ エラーチェック必要 */
		c = utility_skip_char( fp, DELIMITER_SP );

		/* 入力信号線名読込み(","まで読み込み) */
		utility_token( fp, string, DELIMITER_COM, BENCH_NAME_SIZE );


		cell_p = cell_check_and_make_gate ( string, CELL_UNK );


		/* 入力側のcellの出力接続する */
		cell_out_count_up ( cell_p );
		cell_make_new_iolist( &cell_p->out, out_net );
		
		/* 逆向き(入力方向)も接続する */
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
	return ( 0 ); /* こない */
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
 * type          I      BENCH_CELL_DEFINE benchのセルの型
 * output_name   I      char *           cell上のPI/POの名前
 * input_name    I      char *           PI/POの名前
 * cell_p        I      cell *           作成するcellのポインタ
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

