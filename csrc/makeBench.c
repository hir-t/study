/* 難読化回路のベンチマークファイルを生成したい */
/* 未完成　*/
#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void makeBench(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start;
	//Ulong startID = 0;
	LINE *end;
	//int loops = 1; //作成するループの数
	int length = 3; //作成するループの長さ

	/*char out_fn[128]; //出力ファイル名を入れるchar型配列
	char filename[128];
	FILE *out_fp; //出力ファイル・ポインタ
	printf("Input the name of OutputFile_name 'filename'_cnf.txt: ");
	scanf("%s",out_fn);*/

	/*	ID順にレベルやタイプを取得 */
	//printf("----------ソート前-----------\n");
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		data[i] = line;
		data[i]->level  = line->lv_pi; //ここをポインタに
		data[i]->line_id  = line->line_id;
		data[i]->type = line->type;
		//printf("ID:%lu,入力レベル:%lu,TYPE:%u\n", data[i]->line_id,data[i]->level,data[i]->type);
	}

	int M = 0;
	int num = 10;
	/* ループの開始地点と折り返し地点を決める　*/
	start = data[num];
	//startID = start->line_id;
	printf("startID:%lu,startTYPE:%u\n",start->line_id,start->type);

	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		if(line->line_id == num){
			while(M<length){
				line = &(Line_head[num]);
				//Logic_level[num] = line;
				for (int j = 0; j < line->n_out; j++){
					//Logic_level[num] = line;
					printf("selectedID:%lu,selectedTYPE:%u,outID:%lu,outTYPE:%u\n",line->line_id,line->type,line->out[j]->line_id,line->out[j]->type);
				}
				num = line->out[0]->line_id; //次の信号線番号へ更新

				if(3<data[num]->out[0]->type && data[num]->out[0]->type<10){ //
					M++;
				}
				if(data[num]->out[0]->type == 1){ //外部出力に到達したら
					break;
				}
			}
			end = data[num];
		}

		//printf("startID:%lu,startTYPE:%u,endID:%lu,endTYPE:%u\n",start->line_id,start->type,end->line_id,end->type);

		/*
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
*/
	}
	printf("endID:%lu,endTYPE:%u\n",end->line_id,end->type);
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