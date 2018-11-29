/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief Topgunの結果を出力する関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h> //exit()用
#include <string.h>

#include "topgun.h"
#include "topgunState.h" //! STATE_3
#include "topgunLine.h" 
#include "topgunStruct.h"
#include "topgunAtpg.h"
#include "topgunFlist.h"  /* FLIST & FLIST->info */
#include "topgunOut.h"
#include "topgunTime.h"
#include "topgunError.h"  /* FEC_XX */
#include "topgunMemory.h" /* FMT_XX */

extern LINE_INFO Line_info;
extern FLIST     **Flist_head;
extern LINE	     **Pi_head;
extern LINE	     *Line_head;
extern LINE      ***Lv_pi_head;
extern LINE      **Po_head;  /* ATPG用データヘッダ */

static Ulong p_cnt = 0;

static Ulong Bit_code[32] = { 0x00000001,
							  0x00000002,
							  0x00000004,
							  0x00000008,
							  0x00000010,
							  0x00000020,
							  0x00000040,
							  0x00000080,
							  0x00000100,
							  0x00000200,
							  0x00000400,
							  0x00000800,
							  0x00001000,
							  0x00002000,
							  0x00004000,
							  0x00008000,
							  0x00010000,
							  0x00020000,
							  0x00040000,
							  0x00080000,
							  0x00100000,
							  0x00200000,
							  0x00400000,
							  0x00800000,
							  0x01000000,
							  0x02000000,
							  0x04000000,
							  0x08000000,
							  0x10000000,
							  0x20000000,
							  0x40000000,
							  0x80000000,
};
/* function list */

/* original function */
void all_fault_output ( void );
void one_pat_output ( void );
char *add_name ( char *, char * );
FILE *topgun_open ( char *, char *, char * );
void topgun_close( FILE *, char * );
FILE *topgun_open_add_name( char *, char *, char *, char * );

STR_TYPE struct_reconv_check( Ulong *, Ulong *, Ulong );
Ulong *struct_get_outinfo( LINE *, Ulong );
void struct_reconv_flag_on( LINE *, Ulong , Ulong );

/* other function */
/* topgun_state.c */
STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 );
void    atpg_get_chara_stat ( STATE_3, char * );
LEARN_LIST *gene_uniq( LINE * );


/* print function */
void    topgun_print_mes_pat_cnt( Ulong );
void    topgun_print_mes_pat( char * );
void    topgun_print_mes_n( void );

 
/********************************************************************************
 * << Function >>
 *		output of atpg result 
 *
 * << Function ID >>
 *	   	11-1
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/02/07
 *
 ********************************************************************************/

void output
(
 void
){
	topgun_1_utility_enter_start_time( &Time_head.output );

	/* 故障の分類結果出力 */
	all_fault_output();
	
	topgun_1_utility_enter_end_time( &Time_head.output );
}

/********************************************************************************
 * << Function >>
 *		output identified fault
 *
 * << Function ID >>
 *	   	11-1
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	      description
 * Line_info   I      HEADER  Topgun main header
 * Flist_head    I      **Flist_head  defined Fault list
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/5/11
 *
 ********************************************************************************/

void all_fault_output
(
 void
){

	Ulong i;   /* 故障数 */
	Ulong j;   /* 等価故障数 */
	FILE  *fp; /* 故障ファイル書き出し用ファイルポインタ */
	FLIST *fl; /* 故障リスト用ポインタ */


	Ulong detect = 0;
	Ulong r_detect = 0;
	Ulong redun = 0;
	Ulong abort = 0;

	Ulong a_detect = 0;
	Ulong detectable = 0;
	double cover = 0.0;

	/*
	Ulong detect_eqf = 0;
	Ulong r_detect_eqf = 0;
	Ulong redun_eqf = 0;
	Ulong abort_eqf = 0;
	*/


	char mes[16] = "FFF COVERAGE(%)";

	char  *func_name = "all_fault_output"; /* 関数名 */

	 fp = topgun_open_add_name
		( TOPGUN_OUTFILE_CIRCUIT, TOPGUN_OUTFILE_FAULT, "w", func_name );

	 for ( i = 0; i < Line_info.n_fault_atpg; i++ ) {
		 fl = Flist_head[ i ];

		 /* fault ID */
		 /* for 10,000,000 fault */
		 fprintf( fp, "o %9ld ", fl->f_id );

		 fprintf( fp, "%9ld ", fl->line->line_id );

		 /* fault type */
		 if ( fl->info & TOPGUN_SA0 ) {
			 fprintf( fp, "%s ", TOPGUN_OUT_SA0 );
		 }
		 else if ( fl->info & TOPGUN_SA1 ) {
			 fprintf( fp, "%s ", TOPGUN_OUT_SA1 );
		 }
		 else{
			 topgun_error( FEC_PRG_FLIST, func_name );
		 }

		 /* identified */
		 /* $$$ decode 要検討 $$$ */
		 if ( ( fl->info & TOPGUN_DTA ) || 
			  ( fl->info & TOPGUN_DTS ) ) {
			 fprintf( fp, "%s ", TOPGUN_OUT_DET );
			 detect++;
		 }
		 else if ( ( fl->info & TOPGUN_DTX ) ||
				   ( fl->info & TOPGUN_DTR ) ){
			 fprintf( fp, "%s ", TOPGUN_OUT_DTX );
			 r_detect++;
		 }
		 else if( fl->info & TOPGUN_ABT ) {
			 fprintf( fp, "%s ", TOPGUN_OUT_ABORT );
			 abort++;
		 }
		 else if( fl->info & TOPGUN_RED ) {
			 fprintf( fp, "%s ",TOPGUN_OUT_REDUN );
			 redun++;
		 }
		 else if( fl->info & TOPGUN_IUT ) {
			 fprintf( fp, "%s ", TOPGUN_OUT_INITUNT );
		 }
		 else{
			 //fprintf( fp, "%lu ",fl->info );
			 fprintf( fp, "%s ",TOPGUN_OUT_UNTEST);
		 }
		 fprintf( fp, "\n" );


		 for ( j = 0 ; j < ( fl->n_eqf - 1 ); j++ ) {
			 fprintf( fp, "e %9ld\n", fl->eqf[ j ] );
		 }
	 }
	 printf("FFF FAULT CLASSIFICATION\n");
	 printf("FFF TOTAL      %8lu\n",detect+r_detect+abort+redun);
	 printf("FFF DETECT     %8lu\n",detect);
	 printf("FFF RANDOM DET %8lu\n",r_detect);
	 printf("FFF REDUNDANT  %8lu\n",redun);
	 printf("FFF ABORT      %8lu\n",abort);

	 a_detect   = (detect + r_detect) * 100;
	 detectable = (detect + r_detect + abort );
	 cover = (double)a_detect / detectable;
	 
	 printf("%15s  %6.2f\n", mes, cover );
	 fclose( fp );
}


/********************************************************************************
 * << Function >>
 *		output identified fault
 *
 * << Function ID >>
 *	   	11-2
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	      description
 * Line_info   I      HEADER  Topgun main header
 * Pi_head       I      LINE**  PrimaryInput header
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/5/12
 *
 ********************************************************************************/

void one_pat_output
(
 void
){

	Ulong i; /* 外部入力数 */
	FILE  *fp;

	STATE_3 state3;

	char  normal[2]; /* パターンのキャラクタ */

	char  *func_name = "one_pat_output"; /* 関数名 */

	topgun_1_utility_enter_start_time( &Time_head.output );

	/* fileをひらく */
	fp = topgun_open_add_name
		( TOPGUN_OUTFILE_CIRCUIT, TOPGUN_OUTFILE_ONEPAT, "aw", func_name );

	fprintf( fp, "%8ld: ", ++p_cnt );
	topgun_print_mes_pat_cnt ( p_cnt );

    for ( i = 0; i < Line_info.n_pi; i++ ) {

		state3 = atpg_get_state9_2_normal_state3 ( Pi_head[ i ]->state9 );
		atpg_get_chara_stat( state3, normal );

		fprintf(fp, "%s", normal );
		topgun_print_mes_pat( normal );
    }

	fprintf( fp,"\n" );
	topgun_print_mes_n();

	/* fileを閉じる */
	/* $$$ staticで開いとくのも一つの手 */
	topgun_close( fp, func_name );

	topgun_1_utility_enter_end_time( &Time_head.output );
}

/********************************************************************************
 * << Function >>
 *		make new added charcter pointer
 *
 * << Function ID >>
 *	   	11-3
 *
 * << Return >>
 *	name		: added charcter pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      char *  based character
 * add           I      char *  added character
 *
 * << extern >>
 * name			(I/O)	type	      description
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/5/11
 *
 ********************************************************************************/

char *add_name
(
 char *org,
 char *add
 ){
	char *name;

	char *func_name = "add_name";	/* 関数名 */

	name = (char *)topgunMalloc( FMT_Char, ( strlen( org ) + strlen( add ) + 1),
								  1, func_name );

	strcpy( name, org );
	strcat( name, add );

	return( name );
}

/********************************************************************************
 * << Function >>
 *		topgun flie open
 *
 * << Function ID >>
 *	   	11-4
 *
 * << Return >>
 *	fp          : file pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * f_name        I      char *  open file name
 * mode          I      char *  file open mode
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

FILE *topgun_open
(
 char *f_name,
 char *mode,
 char *org_func_name
){
	FILE *fp;

	char *func_name = "topgun_open"; /* 関数名 */

	/* fileをひらく */
	fp = fopen( f_name, mode );

	if ( fp == NULL ) {
		/* fileを開けなかった場合、エラー終了 */
		topgun_error_o( FEC_FILE_OPEN_ERROR, org_func_name, func_name );
	}
	return( fp );
}

/********************************************************************************
 * << Function >>
 *		topgun flie close
 *
 * << Function ID >>
 *	   	11-5
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * f_name        I      char *  open file name
 * f_name        I      char *  open file name
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

void topgun_close
(
 FILE *fp,
 char *org_func_name
){
	int result;

	char *func_name = "topgun_close"; /* 関数名 */

	/* fileをひらく */
	result = fclose( fp );

	if ( result == EOF ) {
		/* fileを閉じれなかった場合、エラー終了 */
		topgun_error_o( FEC_FILE_CLOSE_ERROR, org_func_name, func_name );
	}
}


/********************************************************************************
 * << Function >>
 *		topgun flie open with circuit name
 *
 * << Function ID >>
 *	   	11-6
 *
 * << Return >>
 *	fp          : file pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      char *  based character
 * add           I      char *  added character
 * mode          I      char *  file open mode
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

FILE *topgun_open_add_name
(
 char *org,
 char *add,
 char *mode,
 char *org_func_name
){
	FILE *fp;
	char *f_name;

	size_t add_size = 0;

	char *func_name = "topgun_open_add_name"; /* 関数名 */

	/* 名前を作る */
	f_name = add_name( org, add );
	fp= topgun_open( f_name, mode, func_name );

#ifdef TOPGUN_MEMORY_LOG
	/* add_nameと同じカウント方 */
	add_size = strlen( org ) + strlen( add ) + 1;
#endif /* TOPGUN_MEMORY_LOG */

	topgunFree( f_name, FMT_Char, add_size, func_name );

	return( fp );
}


/********************************************************************************
 * << Function >>
 *		topgun flie open with circuit name
 *
 * << Function ID >>
 *	   	11-6
 *
 * << Return >>
 *	fp          : file pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      char *  based character
 * add           I      char *  added character
 * mode          I      char *  file open mode
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

void info_line_structure
(
 void 
 ){
	Ulong i,j;
	LINE *line;

	Ulong dual_connect = 0;
	Ulong multi_connect = 0;

	Ulong type[TOPGUN_NUM_PRIM];

	char print_mes_type[TOPGUN_NUM_PRIM][5] = {
		"PI  ",						/*  0 */
		"PO  ",						/*  1 */
		"BR  ",						/*  2 */
		"INV ",						/*  3 */
		"BUF ",						/*  4 */	
		"AND ",						/*  5 */
		"NAND",						/*  6 */
		"OR  ",						/*  7 */
		"NOR ",						/*  8 */	
		"XOR ",						/*  9 */
		"XNOR",						/* 10 */	
		"BLKI",						/* 11 */
		"BLKO",						/* 11 */
		"UNK ",						/* 12 */
	};

	for ( i = 0 ; i < TOPGUN_NUM_PRIM ; i++ ) {
		type[ i ] = 0;
	}
	
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		line = &( Line_head[ i ] );

		printf("ID %2lu ",line->line_id);

		printf("TYPE %s ",print_mes_type[line->type]);

		type[line->type]++;
		
		printf("lv %2lu in %3lu out %3lu "
			   ,line->lv_pi, line->n_in, line->n_out);

		printf("F ");
		for( j = 0 ; j < line->n_in ; j++ ) {
			printf("%2lu ",line->in[j]->line_id);
		}
		printf("B ");
		for( j = 0 ; j < line->n_out ; j++ ) {
			printf("%2lu ",line->out[j]->line_id);
		}
		printf("\n");
		
		if ( ( line->n_out > 1 ) &&
			 ( line->n_in > 1 ) ){
			dual_connect++;
		}
		if ( line->n_out > 1 ) {
			multi_connect++;
		}
	}

	printf("n_line      %lu\n",Line_info.n_line);
	printf("PI          %lu\n",type[0]);
	printf("PO          %lu\n",type[1]);
	printf("BR          %lu\n",type[2]);
	printf("INV         %lu\n",type[3]);
	printf("BUF         %lu\n",type[4]);
	printf("AND         %lu\n",type[5]);
	printf("NAND        %lu\n",type[6]);
	printf("OR          %lu\n",type[7]);
	printf("NOR         %lu\n",type[8]);
	printf("BLKI        %lu\n",type[11]);
	printf("BLKO        %lu\n",type[12]);
	printf("multi       %lu\n",multi_connect);
	printf("dual_multi  %lu\n",dual_connect);
	//exit(0);
}

/********************************************************************************
 * << Function >>
 *		topgun flie open with circuit name
 *
 * << Function ID >>
 *	   	11-6
 *
 * << Return >>
 *	fp          : file pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      char *  based character
 * add           I      char *  added character
 * mode          I      char *  file open mode
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

void info_line_reconvergence_structure
(
 void 
 ){
	Ulong i, j, k; //カウンタ
	Ulong n_bit, n_array; //bitデコード用
	LINE  *line;

	Ulong n_po_flag = 0;
	Ulong n_reconv_flag = 0;

	Ulong *out_info_a = NULL;
	Ulong *out_info_b = NULL;
	
	LINE **lf_head = NULL;
	Ulong lf_entry_cnt = 0;
	Ulong lf_get_cnt = 0;

	STR_TYPE str_type;

	char *func_name = "info_line_reconvergence_structure"; // 関数名

	// ここら辺の一次情報はテスト生成には不要
	// 最終的にはlineに影響しないデータ構造が望ましい
	// 取得した結果だけlineに反映させることが望ましい

	// PO flag用のデータを確保する
	n_po_flag = ( Line_info.n_po / 32 ) + 1 ;

	
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {
		line = &( Line_head[ i ] );

		// fanout stemとPOを探す
		if ( line->n_out != 1 ) {

			// 到達可能外部出力の情報
			line->out_info = ( Ulong * )topgunMalloc( FMT_Ulong, sizeof( Ulong ),
													   n_po_flag, func_name );
			for ( j = 0 ; j < n_po_flag ; j++ ) {
				line->out_info[ j ] = 0; //初期化
			}

			n_reconv_flag = ( line->n_out / 32 ) + 1 ;
			//各ブランチが再収斂するかの情報
			line->reconv_info = ( Ulong * )topgunMalloc( FMT_Ulong, sizeof( Ulong ),
													   n_reconv_flag, func_name );
			for ( j = 0 ; j < n_reconv_flag ; j++ ) {
				line->reconv_info[ j ] = 0; //初期化
			}
		}
	}

	// フラグをたてるためのFIFO形式メモリを確保
	// ほんとは最大Fan-inコーン数で良い
	lf_head = ( LINE ** )topgunMalloc( FMT_LINE_P, sizeof( LINE * ),
										Line_info.n_line, func_name );
	for ( i = 0 ; i < Line_info.n_line; i++ ) {
		lf_head[ i ]  = NULL;
	}

	
	// PO flagをたてる
	for ( i = 0, n_bit = 0, n_array = 0 ; i < Line_info.n_po ; i++, n_bit++ ) {
		
		// POのIDをエンコードする(32bit限定)
		// PO ID = i = k * 32 + j

		
		if ( n_bit == 32 ) {
			n_bit = 0;
			n_array++;
		}
		
		lf_entry_cnt = 0;
		lf_get_cnt = 0; 
		
		line = Po_head[ i ]->in[ 0 ];

		Po_head[ i ]->out_info[ n_array ] |= Bit_code[ n_bit ];

		//printf("PO %2lu ID %4lu",i, line->line_id);

		line->flag |= PO_FLAG;
		lf_head[ lf_entry_cnt++ ] = line;

		while ( lf_entry_cnt != lf_get_cnt ) {

			line = 	lf_head[ lf_get_cnt++ ];

			if ( line->n_out > 1 ) {
				line->out_info[ n_array ] |= Bit_code[ n_bit ];
				//printf(" S%4lu",line->line_id);
			}
			for ( j = 0 ; j < line->n_in ; j++ ) {

				if ( ! ( line->in[ j ]->flag & PO_FLAG ) ){
					line->in[ j ]->flag |= PO_FLAG;
					lf_head[ lf_entry_cnt++ ] = line->in[ j ];
				}
			}
		}
		for ( j = 0 ; j < lf_entry_cnt ; j++ ) {
			lf_head[ j ]->flag &= ~PO_FLAG;
		}
		//printf("\n");
	}


	// bit演算を用いて、再収斂があるかを判断する


	// info中のflagと到達可能フラグの取り扱いをどうするのか?
	// 到達可能フラグはFSで持てば良い
	// フラグはブランチに属するため、同一では問題が発生する
	// 上書きされても困るし
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		if ( Line_head[ i ].n_out > 1 ) {
			
			// fanout stemを抽出
			line = &( Line_head[ i ] );

			//printf("STEM %4lu check\n",line->line_id);
			for ( j = 0 ; j < line->n_out - 1 ; j++ ) {
			
 				out_info_a = struct_get_outinfo ( line, j );
				//printf("                BR %2lu %lu",j,out_info_a[0]);

				for ( k = j + 1 ; k < line->n_out ; k++ ) {

					// jとkが両方ともONならばする必要はない
					out_info_b = struct_get_outinfo ( line, k );
					//printf(" vs %2lu %lu",k,out_info_b[0]);
					
					str_type = struct_reconv_check( out_info_a, out_info_b, n_reconv_flag );

					if ( STR_RECONV == str_type ) {
						struct_reconv_flag_on ( line, j, k );
						//printf(" reconv ");
					}
				}
				//printf("\n");
			}

			printf("STEM %4lu lvpo %3lu out %3lu ",line->line_id, line->lv_po, line->n_out);
			n_bit = 0;
			n_array = 0;
			for ( j = 0 ; j < line->n_out ; j++, n_bit++ ) {
				if ( n_bit == 32 ) {
					n_bit = 0;
					n_array++;
				}
				if ( line->reconv_info[ n_array ] & Bit_code[ n_bit ] ) {
					printf("R");
				}
				else {
					printf("o");
				}
			}
			printf("\n");
		}
	}
	exit(0);
}


/********************************************************************************
 * << Function >>
 *		topgun flie open with circuit name
 *
 * << Function ID >>
 *	   	11-6
 *
 * << Return >>
 *	fp          : file pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      char *  based character
 * add           I      char *  added character
 * mode          I      char *  file open mode
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

STR_TYPE struct_reconv_check
(
 Ulong *out_info_a,
 Ulong *out_info_b,
 Ulong  n_reconv_flag
 ){
	Ulong judge;
	Ulong i;

	for ( i = 0 ; i < n_reconv_flag ; i++ ) {
		judge = out_info_a[ i ] & out_info_b[ i ];

		if ( judge != 0 ) {
			return ( STR_RECONV ) ;
		}
	}
	return ( STR_UNDECIDE );
}


/********************************************************************************
 * << Function >>
 *		topgun flie open with circuit name
 *
 * << Function ID >>
 *	   	11-6
 *
 * << Return >>
 *	fp          : file pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      char *  based character
 * add           I      char *  added character
 * mode          I      char *  file open mode
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

Ulong *struct_get_outinfo
(
 LINE *org_line,
 Ulong br_id
 ){
	LINE *line = org_line->out[ br_id ]->out[ 0 ];


	while ( 1 ) {

		if ( line->n_out == 1 ) {
			line = line->out[ 0 ];
		}
		else {
			return ( line->out_info );
		}
	}
}

/********************************************************************************
 * << Function >>
 *		topgun flie open with circuit name
 *
 * << Function ID >>
 *	   	11-6
 *
 * << Return >>
 *	fp          : file pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org           I      char *  based character
 * add           I      char *  added character
 * mode          I      char *  file open mode
 * org_func_name I      char *  open file name
 *
 * << extern >>
 * name			(I/O)	type	      description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/29
 *
 ********************************************************************************/

void struct_reconv_flag_on
(
 LINE *line,
 Ulong br_a,
 Ulong br_b
 ){

	Ulong sho;
	Ulong amari;
	
	sho   = br_a / 32 ;
	amari = br_a % 32 ;
	
	line->reconv_info[ sho ] |= Bit_code[ amari ];

	sho   = br_b / 32 ;
	amari = br_b % 32 ;
	
	line->reconv_info[ sho ] |= Bit_code[ amari ];

}

/* End of File */
