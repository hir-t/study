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

	/*	ID順にレベルやタイプを取得 */
	//printf("----------ソート前-----------\n");
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);

		if(line->n_in == 1){			//入力数が1のとき
			if(line->type==3){//inv
				printf("%lu %lu",line->in[0]->line_id,line->out[0]->line_id);				//a + c
				printf("-%lu -%lu",line->in[0]->line_id,line->out[0]->line_id);			//-a+ -c
			}
			if(line->type==4){		//buf
				printf("-%lu %lu",line->in[0]->line_id,line->out[0]->line_id);			//-a + c
				printf("%lu -%lu",line->in[0]->line_id,line->out[0]->line_id);			//a+ -c
			}
		}

		if(line->n_in == 2){			//入力数が2のとき
			if(line->type==5){			//and
					printf("-%lu %lu\n", line->out[0]->line_id,line->in[0]->line_id); 								//-c + a
					printf("-%lu %lu\n", line->out[0]->line_id,line->in[1]->line_id); 								//-c + b
					printf("-%lu -%lu %lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //-a + -b + c
				}

				if(line->type==6){		//nand
					printf("%lu %lu\n", line->out[0]->line_id,line->in[0]->line_id);								//
					printf("%lu %lu\n", line->out[0]->line_id,line->in[1]->line_id);
					printf("-%lu -%lu -%lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);
				}

				if(line->type==7){		//or
					printf("%lu -%lu\n", line->out[0]->line_id,line->in[0]->line_id);
					printf("%lu -%lu\n", line->out[0]->line_id,line->in[1]->line_id);
					printf("%lu %lu -%lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);
				}

				if(line->type==8){		//nor
					printf("-%lu -%lu\n", line->out[0]->line_id,line->in[0]->line_id);
					printf("-%lu -%lu\n", line->out[0]->line_id,line->in[1]->line_id);
					printf("%lu %lu %lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);
				}

				if(line->type==9){		//xor
					printf("%-lu %lu %lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //-a + b + c
					printf("%lu -%lu %lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //a + -b + c
					printf("%-lu -%lu -%lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //-a + -b + -c
					printf("%lu %lu -%lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //a + b + -c
				}

				if(line->type==10){	//xnor
					printf("%-lu %lu -%lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //-a + b + -c
					printf("%lu -%lu -%lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //a + -b + -c
					printf("%-lu -%lu %lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //-a + -b + c
					printf("%lu %lu %lu\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   //a + b + c
				}
				printf("\n");
			}
		}
		//printf("ID:%lu,入力レベル:%lu,TYPE:%u\n", Logic_level[i]->line_id,Logic_level[i]->level,Logic_level[i]->type);

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