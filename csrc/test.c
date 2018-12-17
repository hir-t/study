#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
LINE *dfs2(int entry,int M ,int loops,int length,LINE *start);

void test(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int entry = 0;
	int length = 2; //作成するループの長さ
	int loops = 1; //作成するループの数
	int M = 0;    //長さ(深さ)を数える
	int num = 0; //ループの初期ノードid
	int gate = 0;
	int count = 0;

	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start[loops];
	LINE *end[loops];
	//LINE *mux = NULL;

	/*** 出力ファイル設定 ***/
	char out_fn[128]; //出力ファイル名を入れるchar型配列
	char filename[128];
	FILE *out_fp; //出力ファイル・ポインタ
	printf("Input the name of Obfuscated output file name 'filename'_obf_cnf.txt: ");
	scanf("%s",out_fn);

	sprintf(filename,"%s_obf_cnf.txt",out_fn);
	out_fp = fopen(filename,"w");
	if(out_fp == NULL){
		printf("ERROR\n");
		exit(1);
	}


	printf("ENTRY1:%d\n",entries);
	/*	ID順にレベルやタイプを取得 */
	//printf("----------ソート前-----------\n");
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		if(2<line->type && line->type<11){		//回路内のゲートの数を数える
			gate++;
		}
		data[i] = line;
		data[i]->level  = line->lv_pi; //ここをポインタに
		data[i]->line_id  = line->line_id;
		data[i]->type = line->type;
		//printf("ID:%lu,入力レベル:%lu,TYPE:%u\n", data[i]->line_id,data[i]->level,data[i]->type);
	}
	printf("The number of gate is %d\n",gate);

	/**** ゲートだけをまとめた配列を作成する ****/
	int gateId[gate];
	int n = 0;		//ゲート数をカウント
	for (int i = 0; i < Line_info.n_line; i++)
	{
		line = &(Line_head[i]);
		if(2<line->type && line->type<11)
		{
			gateId[n] = line->line_id;
			n++;
		}
		if(gate == n){
			break;
		}
	}
	/*
	//ゲートの確認
	for(int i = 0;i<gate;i++){
		printf("GATE:%d\n",gateId[i]);
	}*/


	/***** ループの開始地点と折り返し地点を決める *****/
	//int  N = 0;		//作成するループ数を数える
	/*shuffle(gateId,gate);
	 for(int i=0;i<gate;i++){
        printf("%d,",gateId[i]);
    }*/
	//for (int i = 0; i < loops; i++)
	//while((N != loops && entries < 1) || (N != loops && discovery !=1))
	while(discovery != loops && entries < 1)
	{
		srand( (int)time(NULL) );	//乱数SEED設定
		printf("--------------TAKE%d--------------\n",N);
		M = 0;
		//num = 4;
		num = rand() % gate; //ランダムに選択するゲート番号を選ぶ(この値は信号線なども含む)
		//printf("num:%d\n",gateId[num]);
		//num[i] = 10;
		start[N] = data[gateId[num]];

		printf("START_ID%d:%lu\n",N,start[N]->line_id);

		end[N] = dfs2(entry,M,loops,length,start[N]);
		printf("END_ID:%lu\n",end[N]->line_id);

		//printf("endID:%lu,endTYPE:%u\n",end[i]->line_id,end[i]->type);
/*		if(entries < 1 || discovery != 1){ //複数のエントリポイントが見つからなかった場合、または経路が短かった場合
			i = i-1;	 //もう一度探索し直すために、カウント変数を減らす
			count++;
		}*/

		if(count == 2){
			printf("!!!! ERROR !!!!\n");
			break;
		}
		N++;
	}



	N = 0; //Nを初期化する
	Ulong c1 = 100;
	Ulong c2 = 200;
	/*	ID順にレベルやタイプを取得 */
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		//printf("check\n");
				//入力数が1のとき
		//input:a output:z
		if(line->line_id == end[N]->line_id){
			if(line->n_in == 1){
				if(line->type==3){																	//INVゲートの時
					fprintf(out_fp,"%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//a + z
					fprintf(out_fp,"-%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a+ -z
				}
				if(line->type==4){																	//BUFゲートの時
					fprintf(out_fp,"-%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a + z
					fprintf(out_fp,"%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//a+ -z
				}
			}
			if(line->n_in == 2){
				if(line->type==5){																								//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id); 								//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);		//z + -a + -b
				}

				if(line->type==6){																								//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);		//-z + -a + -b
				}

				if(line->type==7){																								//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);		//-z + a + b
				}

				if(line->type==8){																								//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);								//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);			//z + a + b
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//z + -a + b
					fprintf(out_fp,"%lu %lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//z + a + -b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id); 		//-z + -a + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//-z + a + b
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   	//-z + -a + b
					fprintf(out_fp,"-%lu %lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   	//z + a + -b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id); 		//z + -a + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//z + a + b
				}
			}
				printf("aa\n");
				fprintf(out_fp,"mux\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", c2,end[N]->out[0]->line_id,start[N]->in[0]->line_id);			//-z + a + b
				fprintf(out_fp,"-%lu %lu %lu 0\n", c2,end[N]->out[0]->line_id,start[N]->in[0]->line_id);			//-z + a + s
				fprintf(out_fp,"-%lu %lu %lu 0\n", c2,start[N]->in[0]->line_id,c1);			//-z + b + -s
				fprintf(out_fp,"%lu %lu %lu 0\n", c2,start[N]->in[0]->line_id,c1);			//z + -b + -s
				fprintf(out_fp,"%lu %lu %lu 0\n", c2,end[N]->out[0]->line_id,c1);			//z + -a + s
				fprintf(out_fp,"%lu %lu %lu 0\n", c2,end[N]->out[0]->line_id,start[N]->in[0]->line_id);			//z + -a + -b
				/*mux->n_in = 3;
				mux->n_out = 1;
				mux->in[0]->line_id = end[N]->out[0]->line_id;		//a
				mux->in[1]->line_id = start[N]->in[0]->line_id;		//b
				mux->in[2]->line_id = Line_info.n_line + 1;			//s
				mux->out[0]->line_id = Line_info.n_line + 2;		//z
				//MUXをはさむ
				fprintf(out_fp,"aaaaaaaaa\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", mux->out[0]->line_id,mux->in[0]->line_id,mux->in[1]->line_id);			//-z + a + b
				fprintf(out_fp,"-%lu %lu %lu 0\n", mux->out[0]->line_id,mux->in[0]->line_id,mux->in[2]->line_id);			//-z + a + s
				fprintf(out_fp,"-%lu %lu %lu 0\n", mux->out[0]->line_id,mux->in[1]->line_id,mux->in[2]->line_id);			//-z + b + -s
				fprintf(out_fp,"-%lu %lu %lu 0\n", mux->out[0]->line_id,mux->in[1]->line_id,mux->in[2]->line_id);			//z + -b + -s
				fprintf(out_fp,"-%lu %lu %lu 0\n", mux->out[0]->line_id,mux->in[0]->line_id,mux->in[2]->line_id);			//z + -a + s
				fprintf(out_fp,"-%lu %lu %lu 0\n", mux->out[0]->line_id,mux->in[0]->line_id,mux->in[1]->line_id);			//z + -a + -b
*/
		}
		else{
			if(line->n_in == 1){
				if(line->type==3){																	//INVゲートの時
					fprintf(out_fp,"%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//a + z
					fprintf(out_fp,"-%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a+ -z
				}
				if(line->type==4){																	//BUFゲートの時
					fprintf(out_fp,"-%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a + z
					fprintf(out_fp,"%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//a+ -z
				}
			}
			if(line->n_in == 2){
				if(line->type==5){																								//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id); 								//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);		//z + -a + -b
				}

				if(line->type==6){																								//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);		//-z + -a + -b
				}

				if(line->type==7){																								//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);		//-z + a + b
				}

				if(line->type==8){																								//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);								//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);			//z + a + b
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//z + -a + b
					fprintf(out_fp,"%lu %lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//z + a + -b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id); 		//-z + -a + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//-z + a + b
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   	//-z + -a + b
					fprintf(out_fp,"-%lu %lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   	//z + a + -b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id); 		//z + -a + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id);   		//z + a + b
				}
			}
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