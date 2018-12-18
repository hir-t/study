#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
LINE *dfs2(int entry,int M ,int loops,int length,LINE *start);
int shuffle(int[],int);

void test(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int entry = 0;
	int length = 2; //作成するループの長さ
	int loops = 2; //作成するループの数
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

	Ulong size = sizeof data / sizeof data[0] ;
	printf("size:%lu\n",size);

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

	/***** ループの開始地点と折り返し地点を決める *****/
	int  N = 0;		//作成するループ数を数える
	shuffle(gateId,gate);
	 /*for(int i=0;i<gate;i++){
        printf("%d,",gateId[i]);
    }*/
	//while((N != loops && entries < 1) || (N != loops && discovery !=1))
	int ran = 0;
	while(discovery < loops && entries < 1)
	//while(discovery < loops)
	{
		srand( (int)time(NULL) );	//乱数SEED設定
		printf("--------------TAKE%d--------------\n",N);
		printf("%d\n", discovery);
		M = 0;
		num = gateId[ran];
		//num = rand() % gate; //ランダムに選択するゲート番号を選ぶ(この値は信号線なども含む)
		//printf("num:%d\n",gateId[num]);
		//num[i] = 10;
		//start[N] = data[gateId[num]];
		start[N] = data[num];

		printf("START_ID%d:%lu\n",N,start[N]->line_id);

		end[N] = dfs2(entry,M,loops,length,start[N]);
		printf("END_ID:%lu\n",end[N]->line_id);
		if(flag2 ==1){
			N++;
			entries = 0;
		}
		printf("N:%d\n",N);
		//printf("endID:%lu,endTYPE:%u\n",end[i]->line_id,end[i]->type);
/*		if(entries < 1 || discovery != 1){ //複数のエントリポイントが見つからなかった場合、または経路が短かった場合
			i = i-1;	 //もう一度探索し直すために、カウント変数を減らす
			count++;
		}*/

		if(count == 2){
			printf("!!!! ERROR !!!!\n");
			break;
		}
		//N++;
		ran++;
	}



	int N2 = 0; //N2を初期化,ループ数用変数2
	for(int i = 0; i<loops; i++){
		printf("START%d:%lu\n",N2,start[i]->line_id);
		printf("END%d:%lu\n",N2,end[i]->line_id);
		N2++;
	}
	N = 0;
	Ulong add = 1;			//muxの信号線に使用
	//Ulong c1 = size + add;
	//Ulong c2 = size + add + 1; //c1 + 1
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
				Ulong c1 = size + add;		//muxの入力s
				//Ulong c2 = size + add + 1;	//muxの出力z c1 + 1
				printf("aa\n");
				fprintf(out_fp,"mux\n");
				printf("ii\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", start[N]->in[0]->line_id,end[N]->out[0]->line_id,start[N]->in[0]->line_id);			//-z + a + b
				printf("uu\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", start[N]->in[0]->line_id,end[N]->out[0]->line_id,start[N]->in[0]->line_id);			//-z + a + s
				printf("ee\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", start[N]->in[0]->line_id,start[N]->in[0]->line_id,c1);								//-z + b + -s
				printf("oo\n");
				fprintf(out_fp,"%lu %lu %lu 0\n", start[N]->in[0]->line_id,start[N]->in[0]->line_id,c1);								//z + -b + -s
				printf("ka\n");
				fprintf(out_fp,"%lu %lu %lu 0\n", start[N]->in[0]->line_id,end[N]->out[0]->line_id,c1);									//z + -a + s
				printf("ki\n");
				fprintf(out_fp,"%lu %lu %lu 0\n", start[N]->in[0]->line_id,end[N]->out[0]->line_id,start[N]->in[0]->line_id);			//z + -a + -s
				printf("ku\n");
				N++;
				add++;
				//add = add + 2;
				/*
				* end[N]->out[0]->line_id	:	muxの入力a
				* start[N]->in[0]->line_id	:	muxの入力b
				* c1						:	muxの入力s
				* start[N]->in[0]->line_id	:	muzの出力z(c2にするかも？)
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