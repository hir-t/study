/*
	* a:入力1
	* b:入力2
	* c:出力1
*/

#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void eval(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	//LINE *Logic_level[Line_info.n_line];
	LINE *line;

	char out_fn[128]; //出力ファイル名を入れるchar型配列
	char filename[128];
	FILE *out_fp; //出力ファイル・ポインタ
	printf("Input the name of OutputFile_name 'filename'_cnf.txt: ");
	scanf("%s",out_fn);

	sprintf(filename,"%s_cnf.txt",out_fn);
	  out_fp = fopen(filename,"w");
	  if(out_fp == NULL){
	    printf("ERROR\n");
	    exit(1);
	  }

	/*	ID順にレベルやタイプを取得 */
	//printf("----------ソート前-----------\n");
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		//入力数が1のとき
		//input:a output:b
		if(line->n_in == 1){
			if(line->type==3){															// INVゲートの時
				fprintf(out_fp,"%lu %lu 0",line->in[0]->line_id,line->out[0]->line_id);			//a + c
				fprintf(out_fp,"-%lu -%lu 0",line->in[0]->line_id,line->out[0]->line_id);		//-a+ -c
			}
			if(line->type==4){															//BUFゲートの時
				fprintf(out_fp,"-%lu %lu 0",line->in[0]->line_id,line->out[0]->line_id);		//-a + c
				fprintf(out_fp,"%lu -%lu 0",line->in[0]->line_id,line->out[0]->line_id);		//a+ -c
			}
		}
		//入力数が2のとき
		//intput1:a,input2:b,output:c
		if(line->n_in == 2){
			if(line->type==5){																							//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id); 								//-c + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id); 								//-c + b
					fprintf(out_fp,"-%lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//-a + -b + c
				}

				if(line->type==6){																						//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//c + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//c + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//-a + -b + -c
				}

				if(line->type==7){																						//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//c + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//c + -b
					fprintf(out_fp,"%lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//a + b + -c
				}

				if(line->type==8){																						//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);								//-c + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);								//-c + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);			//a + b + c
				}

				if(line->type==9){																						//XORゲートの時
					fprintf(out_fp,"%-lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//-a + b + c
					fprintf(out_fp,"%lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + -b + c
					fprintf(out_fp,"%-lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id); 		//-a + -b + -c
					fprintf(out_fp,"%lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + b + -c
				}

				if(line->type==10){																						//XNORゲートの時
					fprintf(out_fp,"%-lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//-a + b + -c
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//a + -b + -c
					fprintf(out_fp,"%-lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//-a + -b + c
					fprintf(out_fp,"%lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + b + c
				}
				printf("\n");
			}
		}

	}


/**********************************************
**************TYPE一覧*************************
*
*    TOPGUN_PI   = 0,	//!< 外部入力
*    TOPGUN_PO   = 1,	//!< 外部出力
*    TOPGUN_BR   = 2,	//!< ファンアウトブランチ
*    TOPGUN_INV  = 3,	//!< インバータ
*    TOPGUN_BUF  = 4,	//!< バッファ
*    TOPGUN_AND  = 5,	//!< アンド
*    TOPGUN_NAND = 6,	//!< ナンド
*    TOPGUN_OR   = 7,	//!< オア
*    TOPGUN_NOR  = 8,	//!< ノア
*    TOPGUN_XOR  = 9,	//!< イクルーシブオア
*    TOPGUN_XNOR = 10,	//!< イクルーシブノア
*    TOPGUN_BLKI = 11,	//!< 不定入力
*    TOPGUN_BLKO = 12,	//!< 不定出力
*    TOPGUN_UNK  = 13,	//!< 未確定型
*    TOPGUN_NUM_PRIM,    //!< 型の数
*
************************************************/