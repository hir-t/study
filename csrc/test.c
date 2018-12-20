#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
LINE *dfs2(int entry,int N,int M ,int loops,int length,LINE *start,LINE *route[loops][length]);		//ループ作成の経路を探す
int shuffle(int[],int[],int);										//乱数生成用関数
//void makeCnf(FILE*,LINE *start,LINE *end,Ulong,int);				//cnf記述用関数

void test(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int length = 5;		//作成するループの長さ
	int loops = 2;		//作成するループの数
	int M = 0;			//長さ(深さ)を数える
	int num = 0;		//ループの初期ノードid
	int gate = 0;		//回路内のゲート数
	int entry = 0;		//エントリポイントをのチェックに使う

	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start[loops];				//ループの視点
	LINE *end[loops];				//ループの折り返し地点
	LINE *route[loops][length];			//ループの経路

	/*** 出力ファイル設定 ***/
	char out_fn[128]; //出力ファイル名を入れるchar型配列
	char filename[128];
	FILE *out_fp; //出力ファイル・ポインタ
	printf("Input the name of Obfuscated output file name 'filename'_obf.cnf: ");
	scanf("%s",out_fn);

	sprintf(filename,"%s_obf.cnf",out_fn);
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

	Ulong size = sizeof data / sizeof data[0] ;		//信号線の数を取得
	printf("size:%lu\n",size);

	/**** ゲートだけをまとめた配列を作成する ****/
	int gateId[gate];		//この配列を使ってループの開始、折り返しゲートを決める
	int randId[gate];				//muxで繋ぐゲートID(ランダムに選択する)
	int n = 0;				//ゲート数をカウント
	for (int i = 0; i < Line_info.n_line; i++)
	{
		line = &(Line_head[i]);
		if(2<line->type && line->type<11)
		{
			gateId[n] = line->line_id;
			randId[n] = line->line_id;
			n++;
		}
		if(gate == n){
			break;
		}
	}

	/***** ループの開始地点と折り返し地点、muxをで接続するゲートを決める *****/
	int  N = 0;		//作成するループ数を数える
	shuffle(gateId,randId,gate);
	 for(int i=0;i<gate;i++){
        printf("gateId:%d\n",gateId[i]);
        printf("randId:%d\n",randId[i]);
    }
	int g = 0;		//変数名変えたい
	while(discovery < loops && entries < 1)
	//while(discovery < loops)
	{
		srand( (int)time(NULL) );	//乱数SEED設定
		printf("--------------TAKE%d--------------\n",N);
		//printf("%d\n", discovery);
		M = 0;
		num = gateId[g];
		//printf("num:%d\n",gateId[num]);
		//start[N] = data[gateId[num]];
		start[N] = data[num];

		end[N] = dfs2(entry,N,M,loops,length,start[N],route);
		printf("START_ID%d:%lu\n",N,start[N]->line_id);
		printf("END_ID:%lu\n",end[N]->line_id);
		printf("********************************\n");
		printf("--------------TAKE%d--------------\n\n",N);

		//訪問済みチェック(flag)を初期化して次のdfsに備える
		for(int i = 0;i<Line_info.n_line;i++){
			line = &(Line_head[i]);
			data[i] = line;
			data[i]->flag = 0;
		}

		if(flag2 ==1){			//経路が見つかった時
			N++;				//次のループ作成のためにNをインクリメント
			entries = 0;		//次のループのために初期化
		}
		//printf("N:%d\n",N);

		//N++;
		g++;
	}
	for (int i = 0; i < loops; i++)
	{
		for (int j = 0; j < length; j++)
		{
			printf("route[%d][%d]-ID:%lu,n_out%lu\n",i,j,route[i][j]->line_id,route[i][j]->n_out);
		}
	}

	int m_node = 0;			//MUXの追加によって増えるノードの数
	Ulong m_edge = 0;		//MUXの追加によって増えるエッジの数
	int loop_m[loops];		//1ループに追加するMUXの総数(折り返し地点以外)
	int m_num = 0;			//1ループに追加するMUXの総数(折り返し地点以外)を数える

	//追加するmuxの数と、それに伴って増加する信号線の数を数える
	for (int i = 0; i < loops; i++)
	{
		for (int j = 0; j < length; j++)
		{
			if(route[i][j]->n_out != end[i]->line_id)	//経路の折り返し地点以外の時
			{
				if(route[i][j]->n_out == 1){						//経路のファンアウトが1のとき
					m_node = m_node + 2;							//MUXを2つ増やすため
					m_num  = m_num  + 2;
					m_edge = m_edge + 1;							//入力sが1つ増えるため(2つのMUXの入力sは同じ)
				}
				else{												//経路のファンアウトが1以外(2で考えるがそれ以上の場合もある)のとき
					m_node = m_node + 1;							//MUXが1つ
					m_num  = m_num  + 1;
					m_edge = m_edge + 1;							//sが1つ
				}
			}
			else{													//折り返し地点の時
					m_node = m_node + 1;							//MUXが1つ
					m_edge = m_edge + 1;							//sが1つ
			}
		}
		loop_m[i] = m_num;
		m_num	  = 0;												//次のループのファンアウト数を数えるため初期化
	}

	for (int i = 0; i < loops; i++)
	{
		printf("loop_m[%d]:%d\n",i,loop_m[i]);
	}

	LINE *r[m_node];												//追加するMUXとランダムに接続するゲートを格納する
	for (int i = 0; i < m_node; i++)
	{
		for (int j = 0; i < gate; j++)
		{
			num = randId[j];										//numにランダムなゲートのIDを代入
			r[i] = data[num];										//ランダムなゲートr[i]はdata[num]とする
			//ランダムゲートがループの視点、折り返し地点、でなく、フラグが1でない(ランダムゲートとして選択されていない)とき
			if( (r[i]->line_id != start[N]->line_id)&&(r[i]->line_id !=end[N]->line_id) && data[num]->flag !=1){
				data[num]->flag = 1;								//フラグを1に(ランダムゲートとして選択済み)にする
				break;												//ループを抜ける
			}
		}
		//if(i / end[N]->n_out == 1 && i % end[N]->n_out == 0) N++;	//追加する
	}

	for(int i=0;i<m_node;i++){
		printf("r%d:%lu\n",i,r[i]->line_id);
	}

	int node = gate + m_node;										//MUXを追加した後の回路全体のゲート数
	Ulong edge  = Line_info.n_line + m_edge + 1;					//MUXを追加した後の回路全体の信号線数

	//N = M = 0;
	g = 0;
	//makeCnf(out_fp,*start[],*end[],size,N);
	Ulong add = 1;			//muxの信号線に使用
	Ulong c1 = size + add;		//muxの入力s
	//int count = 0;			//追加したMUXの数を数える

	/****	ID順にレベルやタイプを取得し、cnfに変換していく ****/
	fprintf(out_fp,"p cnf %lu %d\n",edge,node);
	//for (N = 0; N < loops; N++)
	//{
		M=0;
		int count = 0;
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		//printf("check%lu\n",line->line_id);

		if(line->line_id == start[N]->line_id || line->line_id == end[N]->line_id || line->line_id == route[N][M]->line_id){	//取得したIDがループ経路のとき
		//if(line->line_id == route[N][M]->line_id){//取得したIDがループ経路のとき
			//入力数が1のとき
			//input:a output:z
			printf("ID:%lu\n",line->line_id);
			if(line->n_in == 1){
				if(line->type==3){																		//INVゲートの時
					fprintf(out_fp,"%lu %lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//a + z
					fprintf(out_fp,"-%lu -%lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//-a+ -z
				}
				if(line->type==4){																		//BUFゲートの時
					fprintf(out_fp,"-%lu %lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//-a + z
					fprintf(out_fp,"%lu -%lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//a+ -z
				}
			}

			//入力数が2のとき
			//intput1:a,input2:b,output:z
			if(line->n_in == 2){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//z + -a + -b
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);	//-z + -a + -b
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//-z + a + b
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//z + a + b
				}

				if(line->type==9){																									//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + -a + b
					fprintf(out_fp,"%lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1); 	//-z + -a + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//-z + a + b
				}

				if(line->type==10){																									//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//-z + -a + b
					fprintf(out_fp,"-%lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1); 	//z + -a + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + b
				}
			}

			//入力数が3のとき
			//intput1:a,input2:b,input3:c,output:z
			if(line->n_in == 3){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//z + -a + -b + -c
														line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//-z + -a + -b + -c
															line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,							//-z + a + b + +c
															line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,							//z + a + b + c
															line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==9){																									//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + -a + b + c
					fprintf(out_fp,"%lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + -b + c
					fprintf(out_fp,"%lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + b + -c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1); 						//z + -a + -b + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + b + c
				}

				if(line->type==10){																									//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + -a + b + c
					fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + -b + c
					fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + b + -c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1); 						//-z + -a + -b + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + b + c
				}
			}

			//入力数が4のとき
			//intput:a,b,c,d
			//output:z
			if(line->n_in == 4){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//z + -a + -b + -c + -d
														line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + -a + -b + -d
														line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1+1,line->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//-z + a + b + c + d
													line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//z + a + b + c + d
													line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==9){																									//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + -a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   	//z + a + -b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + -c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==10){																									//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + -a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   	//-z + a + -b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + -c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//z + -a + -b + -c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}
			}

			//入力数が5のとき
			//intput:a,b,c,d,e
			//output:z
			if(line->n_in == 5){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,				//-z + -a + -b + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + a + b + c + d + e
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + a + b + c + d + e
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==9){																										//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + -a + b + c + d + e
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + -b + c + d + e
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + -c + d + e
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + -d + e
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + d + -e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1); 		//z + -a + -b + -c + -d + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + d + e
				}

				if(line->type==10){																										//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + -a + b + c + d + e
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + -b + c + d + e
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + -c + d + e
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + -d + e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + d + -e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id); 		//-z + -a + -b + -c + -d + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + d + e
				}
			}

			//入力数が6のとき
			//intput:a,b,c,d,e,f
			//output:z
			if(line->n_in == 6){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,				//z + -a + -b + -c + -d + -e + -f
									line->in[1]->line_id,line->in[2]->line_id,line->in[3]->line_id,line->in[4]->line_id,
									line->in[5]->line_id);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,			//-z + -a + -b + -d + -e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//-z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==9){																												//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + -a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   		//z + a + -b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + -c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + -d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + -e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d + -e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==10){																												//XNORゲートの時
					fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + -a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   		//-z + a + -b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + -c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + -d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + -e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					 	//z + -a + -b + -c + -d + -e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}
			}

			//入力数が7のとき
			//intput:a,b,c,d,e,f,g
			//output:z
			if(line->n_in == 7){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//-z + g
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//z + -a + -b + -c + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 									//z + g
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//-z + -a + -b + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);									//z + -g
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//-z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//-z + -g
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//z + -a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//z + a + -b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//z + a + b + -c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//z + a + b + c + -d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//z + a + b + c + d + -e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//z + a + b + c + d + e + -f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//z + a + b + c + d + e + f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//z + -a + -b + -c + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//-z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//-z + -a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//-z + a + -b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//-z + a + b + -c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//-z + a + b + c + -d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//-z + a + b + c + d + -e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//-z + a + b + c + d + e + -f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//-z + a + b + c + d + e + f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//-z + -a + -b + -c + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}
			}

			//入力数が8のとき
			//intput:a,b,c,d,e,f,g,h
			//output:z
			if(line->n_in == 8){
				if(line->type==5){																								//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==6){																								//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",											//-z + -a + -b + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==7){																								//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//-z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==8){																								//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//z + -a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//z + a + -b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 													//z + a + b + -c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//z + a + b + c + -d + e + f + g + h
									line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id,
									line->in[2]->line_id,line->in[3]->line_id,line->in[4]->line_id,
									line->in[5]->line_id,line->in[6]->line_id,line->in[7]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//z + a + b + c + d + -e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  						//z + a + b + c + d + e + -f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  						//z + a + b + c + d + e + f + -g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//z + a + b + c + d + e + f + g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//-z + -a + -b + -c + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//-z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//-z + -a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//-z + a + -b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + -c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//-z + a + b + c + -d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//-z + a + b + c + d + -e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  					//-z + a + b + c + d + e + -f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  					//-z + a + b + c + d + e + f + -g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//-z + a + b + c + d + e + f + g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//z + -a + -b + -c + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}
			}

			//入力数が9のとき
			//intput:a,b,c,d,e,f,g,h,i
			//output:z
			if(line->n_in == 9){
				if(line->type==5){																								//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1); 							//-z + i
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==6){																								//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1); 								//z + i
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										//-z + -a + -b + -d + -e + -f + -g + -h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==7){																								//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1);								//z + -i
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==8){																								//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1);							//-z + -i
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",											   	//z + -a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + -b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + -c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + -d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + -e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + -f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + -g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//z + a + b + c + d + e + f + g + -h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//z + a + b + c + d + e + f + g + h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + -a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + a + -b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + -c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + -d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + -e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//-z + a + b + c + d + e + -f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + -g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + -h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//-z + a + b + c + d + e + f + g + h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 									//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}
			}

			/*** 折り返し地点から開始地点へフェードバック(muxを挟む) ***/
			//Ulong c1 = size + add + 1;		//muxの入力s
			//Ulong c2 = size + add + 1;	//muxの出力z c1 + 1
			//経路の折り返し地点の時
			//フィードバックのためのMUXを追加
			if(line->line_id == end[N]->line_id){
				//fprintf(out_fp,"mux\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", start[N]->in[0]->line_id+1,end[N]->out[0]->line_id+1,start[N]->in[0]->line_id+1+1);				//-z + a + b
				fprintf(out_fp,"-%lu %lu %lu 0\n", start[N]->in[0]->line_id+1,end[N]->out[0]->line_id+1,c1);									//-z + a + s
				fprintf(out_fp,"-%lu %lu -%lu 0\n", start[N]->in[0]->line_id+1,start[N]->in[0]->line_id+1,c1);									//-z + b + -s
				fprintf(out_fp,"%lu -%lu -%lu 0\n", start[N]->in[0]->line_id+1,start[N]->in[0]->line_id+1,c1);									//z + -b + -s
				fprintf(out_fp,"%lu -%lu %lu 0\n", start[N]->in[0]->line_id+1,end[N]->out[0]->line_id+1,c1);									//z + -a + s
				fprintf(out_fp,"%lu -%lu -%lu 0\n", start[N]->in[0]->line_id+1,end[N]->out[0]->line_id+1,start[N]->in[0]->line_id+1);				//z + -a + -b
				count++;
				add++;
				printf("1\n");
			}
			//経路内の折り返し地点以外の時
			//経路を開くためのMUXを追加する
			else
			{//bはr[]
				//ファンアウト数が1のときMUXを2つ追加
				if(route[N][M]->n_out == 1){
					for (int j = 0; j < 2; j++)
					{
						//fprintf(out_fp,"mux\n");
						fprintf(out_fp,"-%lu %lu %lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//-z + a + b
						fprintf(out_fp,"-%lu %lu %lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,c1);						//-z + a + s
						fprintf(out_fp,"-%lu %lu -%lu 0\n", route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1,c1);								//-z + b + -s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1,c1);								//z + -b + -s
						fprintf(out_fp,"%lu -%lu %lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,c1);						//z + -a + s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//z + -a + -b
						g++;
						count++;
						printf("2\n");
					}
					//count++;
					M++;
					add++;
				}
				//ファンアウトが1以外の時MUXを1つ追加+1
				else{
					//fprintf(out_fp,"mux\n");
					fprintf(out_fp,"-%lu %lu %lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//-z + a + b
					fprintf(out_fp,"-%lu %lu %lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,c1);						//-z + a + s
					fprintf(out_fp,"-%lu %lu -%lu 0\n", route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1,c1);								//-z + b + -s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1,c1);								//z + -b + -s
					fprintf(out_fp,"%lu -%lu %lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,c1);						//z + -a + s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", route[N][M]->out[0]->line_id+1,route[N][M]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//z + -a + -b
					g++;
					M++;
					count++;
					add++;
					printf("3\n");
				}
			}
			printf("count:%d\n",count);
			if(count == loop_m[N]){	//1つのループのMUXを追加し終えたら
				N++;
				break;
				count = 0;
				printf("%dloop\n",N);
			}
			//add++;
				//add = add + 2;
				/*
				* end[N]->out[0]->line_id	:	muxの入力a
				* start[N]->in[0]->line_id	:	muxの入力b
				* c1						:	muxの入力s
				* start[N]->in[0]->line_id	:	muzの出力z(c2にするかも？)
				*/

		}
		else{																													//折り返し地点でない時
			//入力数が1のとき
			//input:a output:z
			if(line->n_in == 1){
				if(line->type==3){																								//INVゲートの時
					fprintf(out_fp,"%lu %lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);									//a + z
					fprintf(out_fp,"-%lu -%lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);									//-a+ -z
				}
				if(line->type==4){																								//BUFゲートの時
					fprintf(out_fp,"-%lu %lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);									//-a + z
					fprintf(out_fp,"%lu -%lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);									//a+ -z
				}
			}

			//入力数が2のとき
			//intput1:a,input2:b,output:z
			if(line->n_in == 2){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//z + -a + -b
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);	//-z + -a + -b
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//-z + a + b
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//z + a + b
				}

				if(line->type==9){																									//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + -a + b
					fprintf(out_fp,"%lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1); 	//-z + -a + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//-z + a + b
				}

				if(line->type==10){																									//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//-z + -a + b
					fprintf(out_fp,"-%lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1); 	//z + -a + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + b
				}
			}

			//入力数が3のとき
			//intput1:a,input2:b,input3:c,output:z
			if(line->n_in == 3){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//z + -a + -b + -c
														line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//-z + -a + -b + -c
															line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,							//-z + a + b + +c
															line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,							//z + a + b + c
															line->in[1]->line_id+1,line->in[2]->line_id+1);
				}

				if(line->type==9){																									//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + -a + b + c
					fprintf(out_fp,"%lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + -b + c
					fprintf(out_fp,"%lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + b + -c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1); 						//z + -a + -b + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + b + c
				}

				if(line->type==10){																									//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + -a + b + c
					fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + -b + c
					fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + b + -c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1); 						//-z + -a + -b + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
															line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + b + c
				}
			}

			//入力数が4のとき
			//intput:a,b,c,d
			//output:z
			if(line->n_in == 4){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//z + -a + -b + -c + -d
														line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + -a + -b + -d
														line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1+1,line->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//-z + a + b + c + d
													line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//z + a + b + c + d
													line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==9){																									//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + -a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   	//z + a + -b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + -c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}

				if(line->type==10){																									//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + -a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   	//-z + a + -b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + -c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//z + -a + -b + -c + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + c + d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				}
			}

			//入力数が5のとき
			//intput:a,b,c,d,e
			//output:z
			if(line->n_in == 5){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,				//-z + -a + -b + -d
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + a + b + c + d + e
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + a + b + c + d + e
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
				}

				if(line->type==9){																										//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + -a + b + c + d + e
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + -b + c + d + e
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + -c + d + e
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + -d + e
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + d + -e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1); 		//z + -a + -b + -c + -d + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + d + e
				}

				if(line->type==10){																										//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + -a + b + c + d + e
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + -b + c + d + e
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + -c + d + e
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + -d + e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + d + -e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id); 		//-z + -a + -b + -c + -d + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + d + e
				}
			}

			//入力数が6のとき
			//intput:a,b,c,d,e,f
			//output:z
			if(line->n_in == 6){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,				//z + -a + -b + -c + -d + -e + -f
									line->in[1]->line_id,line->in[2]->line_id,line->in[3]->line_id,line->in[4]->line_id,
									line->in[5]->line_id);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,			//-z + -a + -b + -d + -e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//-z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==9){																												//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + -a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   		//z + a + -b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + -c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + -d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + -e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d + -e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}

				if(line->type==10){																												//XNORゲートの時
					fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + -a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   		//-z + a + -b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + -c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + -d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + -e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					 	//z + -a + -b + -c + -d + -e + -f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + e + f
									line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1);
				}
			}

			//入力数が7のとき
			//intput:a,b,c,d,e,f,g
			//output:z
			if(line->n_in == 7){
				if(line->type==5){																									//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//-z + g
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//z + -a + -b + -c + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==6){																									//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 									//z + g
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//-z + -a + -b + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==7){																									//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);									//z + -g
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//-z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==8){																									//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//-z + -g
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//z + -a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//z + a + -b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//z + a + b + -c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//z + a + b + c + -d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//z + a + b + c + d + -e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//z + a + b + c + d + e + -f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//z + a + b + c + d + e + f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//z + -a + -b + -c + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//-z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//-z + -a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//-z + a + -b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//-z + a + b + -c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//-z + a + b + c + -d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//-z + a + b + c + d + -e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//-z + a + b + c + d + e + -f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//-z + a + b + c + d + e + f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//-z + -a + -b + -c + -d + -e + -f + -g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//z + a + b + c + d + e + f + g
									line->out[0]->line_id+1,line->in[0]->line_id+1,
									line->in[1]->line_id+1,line->in[2]->line_id+1,
									line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1);
				}
			}

			//入力数が8のとき
			//intput:a,b,c,d,e,f,g,h
			//output:z
			if(line->n_in == 8){
				if(line->type==5){																								//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==6){																								//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",											//-z + -a + -b + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==7){																								//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//-z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==8){																								//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//z + -a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//z + a + -b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 													//z + a + b + -c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//z + a + b + c + -d + e + f + g + h
									line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id,
									line->in[2]->line_id,line->in[3]->line_id,line->in[4]->line_id,
									line->in[5]->line_id,line->in[6]->line_id,line->in[7]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//z + a + b + c + d + -e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  						//z + a + b + c + d + e + -f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  						//z + a + b + c + d + e + f + -g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//z + a + b + c + d + e + f + g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//-z + -a + -b + -c + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//-z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//-z + -a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//-z + a + -b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + -c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//-z + a + b + c + -d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//-z + a + b + c + d + -e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  					//-z + a + b + c + d + e + -f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  					//-z + a + b + c + d + e + f + -g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//-z + a + b + c + d + e + f + g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//z + -a + -b + -c + -d + -e + -f + -g + -h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//z + a + b + c + d + e + f + g + h
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				}
			}

			//入力数が9のとき
			//intput:a,b,c,d,e,f,g,h,i
			//output:z
			if(line->n_in == 9){
				if(line->type==5){																								//ANDゲートの時
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1); 							//-z + i
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==6){																								//NANDゲートの時
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1); 								//z + i
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										//-z + -a + -b + -d + -e + -f + -g + -h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==7){																								//ORゲートの時
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1);								//z + -i
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==8){																								//NORゲートの時
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1);							//-z + -i
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==9){																								//XORゲートの時
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",											   	//z + -a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + -b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + -c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + -d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + -e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + -f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + -g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//z + a + b + c + d + e + f + g + -h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//z + a + b + c + d + e + f + g + h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}

				if(line->type==10){																								//XNORゲートの時
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + -a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + a + -b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + -c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + -d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + -e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//-z + a + b + c + d + e + -f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + -g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + -h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//-z + a + b + c + d + e + f + g + h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 									//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + g + h + i
									line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
									line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
									line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
									line->in[8]->line_id+1);
				}
			}
		}
	}
//}
}

/*
*************************************************
* *****************TYPE一覧********************* *
*												*
*    TOPGUN_PI   = 0,	//!< 外部入力				*
*    TOPGUN_PO   = 1,	//!< 外部出力				*
*    TOPGUN_BR   = 2,	//!< ファンアウトブランチ	*
*    TOPGUN_INV  = 3,	//!< インバータ			*
*    TOPGUN_BUF  = 4,	//!< バッファ				*
*    TOPGUN_AND  = 5,	//!< アンド				*
*    TOPGUN_NAND = 6,	//!< ナンド				*
*    TOPGUN_OR   = 7,	//!< オア				*
*    TOPGUN_NOR  = 8,	//!< ノア				*
*    TOPGUN_XOR  = 9,	//!< イクルーシブオア		*
*    TOPGUN_XNOR = 10,	//!< イクルーシブノア		*
*    TOPGUN_BLKI = 11,	//!< 不定入力				*
*    TOPGUN_BLKO = 12,	//!< 不定出力				*
*    TOPGUN_UNK  = 13,	//!< 未確定型				*
*    TOPGUN_NUM_PRIM,    //!< 型の数				*
*												*
* ********************************************* *
*************************************************
*/