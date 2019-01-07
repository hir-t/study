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
void TopologicalSort(int,int,LINE *[],LINE *[]);
int countClauses(void);

void obfuscation(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int length = 6;		//作成するループの長さ
	int loops = 6;		//作成するループの数
	int M = 0;			//長さ(深さ)を数える
	int num = 0;		//ループの初期ノードid
	int gate = 0;		//回路内のゲート数
	int entry = 0;		//エントリポイントをのチェックに使う
	//int discovery = 0;				//経路発見に使用する

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


	//printf("ENTRY1:%d\n",entries);
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
		//printf("ID:%lu\n",data[i]->line_id);
		//printf("ID:%lu,入力レベル:%lu,TYPE:%u\n", data[i]->line_id,data[i]->level,data[i]->type);
	}
	printf("The number of gate is %d\n",gate);

	//Ulong size = sizeof data / sizeof data[0] ;		//信号線の数を取得
	Ulong size = Line_info.n_line;
	printf("size:%lu\n",size);

	/**** ゲートだけをまとめた配列を作成する ****/
	int gateId[gate];		//この配列を使ってループの開始、折り返しゲートを決める
	int randId[gate];		//muxで繋ぐゲートID(ランダムに選択する)
	int n = 0;				//ゲート数をカウント
	for (int i = 0; i < Line_info.n_line; i++)
	{
		line = &(Line_head[i]);
		if(2<line->type && line->type<11)
		{
			gateId[n] = line->line_id;
			randId[n] = line->line_id;
			if (line->type==1)
			{
				printf("ID:%lu,type:%u\n", line->line_id,line->type);
			}
			n++;
		}
		if(gate == n){
			break;
		}
	}

	/***** ループの開始地点と折り返し地点、muxをで接続するゲートを決める *****/
	int  N = 0;		//作成するループ数を数える
	shuffle(gateId,randId,gate);
/*	 for(int i=0;i<gate;i++){
        printf("gateId:%d\n",gateId[i]);
        printf("randId:%d\n",randId[i]);
    }*/
	int g = 0;
	while(N < loops && entries == 0)
	//while(discovery < loops)
	{
		srand( (int)time(NULL) );	//乱数SEED設定
		//printf("--------------ROUTE%d--------------\n",N);
		//printf("%d\n", discovery);
		M = 0;
		num = gateId[g];
		//printf("gateNum:%d,gateID[%d]:%d\n",g,num,gateId[num]);
		//start[N] = data[gateId[num]];
		start[N] = data[num];

		end[N] = dfs2(entry,N,M,loops,length,start[N],route);
		end[N]->endflg = 1;
		end[N]->st = start[N];
		//printf("START_ID%d:%lu\n",N,start[N]->line_id);
		//printf("END_ID:%lu\n",end[N]->line_id);
		//printf("ST_ID:%lu\n",end[N]->st->line_id);
		//printf("********************************\n");

		if(flag2 == 1){			//経路が見つかった時
			for (int m = 0; m < length; m++)
			{
				//route[N][m]->selected = 1;
				route[N][m]->rtflg = 1;
			}
			N++;				//次のループ作成のためにNをインクリメント
			entries = 0;		//次のループのために初期化
//			flag2 = 0;
			//printf("発見\n");
		}
		//printf("--------------ROUTE%d--------------\n\n",N);

		//訪問済みチェック(flag)を初期化して次のdfsに備える
		for(int i = 0;i<Line_info.n_line;i++){
			data[i]->flag = 0;
			//printf("init%d\n",i);
		}
		//N++;
		g++;	//次の始点を決める
	}

	//取得した経路を出力
/*	for (int i = 0; i < loops; i++)
	{
		for (int j = 0; j < length; j++)
		{
			printf("route[%d][%d]-ID:%lu,n_out%lu\n",i,j,route[i][j]->line_id,route[i][j]->n_out);
		}
	}*/

	//取得した経路に含まれないゲートを集める

	LINE *routeNode[loops*length];			//レベル順に経路内のゲートをまとめる
	int k = 0;
	N = M = 0;
	while(k < loops*length){

		routeNode[k] = route[N][M];
		//printf("route[%d][%d]:%lu\n",N,M,route[N][M]->line_id);
		int id = routeNode[k]->line_id;
		//data[id]->rtflg = 1;
		data[id]->rtcnt += 1;
		if(M < length){
			data[id]->next = route[N][M+1];
			M++;
		}
		if(M == length)
		{
			data[id]->next = route[N][M];
			N++;
			M = 0;
		}
		k++;
	}

/*	for (int i = 0; i < Line_info.n_line; i++)
	{
		if (data[i]->rtflg==1)
		{
			printf("ID:%lu,nextID:%lu\n", data[i]->line_id,data[i]->next->line_id);
		}
	}*/

	//ソート前出力
/*	k = 0;
	while(k < loops*length){
		printf("前ID:%lu,LV.%lu\n",routeNode[k]->line_id,routeNode[k]->level);
		k++;
	}*/

	TopologicalSort(loops,length,routeNode,data);	//経路内のノードをレベル順にソート

	//ソート後出力
	k = 0;
	//printf("--ソート後--\n");
/*	while(k < loops*length){
	printf("ID:%lu,LV.%lu,flg:%lu,rtcnt:%lu,nextID%lu\n",routeNode[k]->line_id,routeNode[k]->level,routeNode[k]->endflg,routeNode[k]->rtcnt,routeNode[k]->next->line_id);
		k++;
	}*/
/*
	for (int i = 0; i < Line_info.n_line; i++)
	{
		if (data[i]->rtflg ==1)
		{
			printf("a.ID:%lu,入力レベル:%lu,TYPE:%u,rtflg:%lu\n", data[i]->line_id,data[i]->level,data[i]->type,data[i]->rtflg);
		}

	}*/

	//追加するmuxの数と、それに伴って増加する信号線の数を数える
	int m_node 	 =  loops;			//MUXの追加によって増えるノードの数
	int m_num 	 =  0;				//1ループに追加するMUXの総数(折り返し地点以外)を数える
	Ulong m_edge =  loops*2;		//MUXの追加によって増えるエッジの数
	//int loop_m[loops];			//1ループに追加するMUXの総数(折り返し地点以外)

	for (int i = 0; i < loops*length; i++)
	{
		if (routeNode[i]->endflg == 0)
		{
			if (routeNode[i]->n_out==1)					//経路のファンアウトが1のとき
			{
				m_node = m_node + 2;					//MUXを2つ増やすため
				m_num = m_num + 2;						//MUXを2つ増やすため
				m_edge = m_edge + 4;					//入力sが2つ,出力zが2つ増えるため(2つのMUXの入力s,は同じ)
				//m_edge = m_edge + 3;					//入力sが1つ,出力zが2つ増えるため(2つのMUXの入力s,は同じ)
			}
			else{										//経路のファンアウトが1以外(2で考えるがそれ以上の場合もある)のとき
				m_node = m_node + 1;					//MUXが1つ
				m_num = m_num + 1;						//MUXを1つ増やすため
				m_edge = m_edge + 2;					//s,zが1つ
			}
		}
	}

/*	for (int i = 0; i < loops; i++)
	{
		printf("ROUTE%d's num of MUXes:%d\n",i,loop_m[i]);
	}*/

	//フラグの初期化
	for (int i = 0; i < Line_info.n_line; i++)
	{
		data[i]->flag = 0;
	}

	LINE *r[m_num];										//追加するMUXとランダムに接続するゲートを格納する
	printf("r_size = %lu\n", sizeof(r));
	printf("r_element = %lu\n", sizeof(r)/sizeof(Ulong));

	//ルートに含まれるゲートのフラグを1にしてランダムゲートとして選択されないようにする
	for (int i = 0; i < Line_info.n_line; i++)
	{
		for (int k = 0; k < loops*length; k++)
		{
			if(routeNode[k]->line_id==data[i]->line_id){
				data[i]->flag = 1;
			}
		}
	}
	for (int i = 0; i < m_node; i++)
	{
		for (int j = 0; i < gate; j++)
		{
			num = randId[j];								//numにランダムなゲートのIDを代入

			if (2<data[num]->type && data[num]->type<11)	//c7552のとき、ランダムなゲートとして外部出力の信号線が選択されることがあるためそれを無理やつ回避するためのif分
			{												//randG[]作成の際にも回避しているが、なぜか含まれる場合がある
				r[i] = data[num];							//ランダムなゲートr[i]はdata[num]とする

			//ランダムゲートがループの視点、折り返し地点、でなく、フラグが1でない(ランダムゲートとして選択されていない)とき
				if( data[num]->flag !=1){								//ランダムに選んだゲートが経路に含まれていなければ
					data[num]->flag = 1;								//フラグを1に(ランダムゲートとして選択済み)にする
					break;												//ループを抜ける
				}
			}
		}
	}


	//ランダムなゲートの確認
/*	for(int i=0;i<m_node;i++){
		printf("r%d:%lu,n_out:%lu\n",i,r[i]->line_id,r[i]->n_out);
	}*/

	/****	ID順にレベルやタイプを取得し、cnfに変換していく ****/
	g = 0;
	Ulong c1	 = Line_info.n_line + 1;		//muxの入力sに与えるID(cnf内で)
	Ulong c2	 = Line_info.n_line + 2;		//muxの入力zに与えるID
	int count = 0;

	int variables  = Line_info.n_line + m_edge;						//MUXを追加した後の回路全体の信号線数(命題変数の数)
	int m_clauses = m_node*6;		//muxの節数が6なため
	printf("m_clauses:%d\n", m_clauses);
	int clauses = countClauses() + m_clauses;
	printf("evClauses:%d\n", countClauses());

	fprintf(out_fp,"p cnf %d %d\n",variables,clauses);
	for(int i = 0;i<Line_info.n_line;i++){
		//printf("%d.ID:%lu,入力レベル:%lu,TYPE:%u,n_in:%lu,n_out:%lu\n", i,data[i]->line_id,data[i]->level,data[i]->type,data[i]->n_in,data[i]->n_out);

		//if(data[i]->line_id == routeNode[NM]->line_id){	//取得したIDがループ経路のとき
		if(data[i]->rtflg == 1){	//取得したIDがループ経路のとき
			//printf("route\n");

			//入力数が1のとき
			//input:a output:z
			if(data[i]->n_in == 1){
				if(data[i]->type==3){																			//INVゲートの時
					//printf("1-inv\n");
					fprintf(out_fp,"%lu %lu 0\n"  ,data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//z + a
					fprintf(out_fp,"-%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//-z+ -a
				}
				else if(data[i]->type==4){																		//BUFゲートの時
					//printf("1-buf\n");
					fprintf(out_fp,"-%lu %lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//-z + a
					fprintf(out_fp,"%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//z+ -a
				}
			}

			//入力数が2のとき
			//intput1:a,input2:b,output:z
			else if(data[i]->n_in == 2){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("2-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,														//z + -a + -b
														data[i]->in[0]->line_id+1 ,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("2-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//z + a
					fprintf(out_fp,"%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,														//-z + -a + -b
														data[i]->in[0]->line_id+1 ,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("2-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,														//-z + a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("2-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", 	data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n",		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,														//z + a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("2-xor\n");
					fprintf(out_fp,"%lu -%lu %lu 0\n", 	data[i]->out[0]->line_id+1,													   	//z + -a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu 0\n", 	data[i]->out[0]->line_id+1,  												 	//z + a + -b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,													 	//-z + -a + -b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,													 	//-z + a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("2-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//-z + -a + b
																				   data[i]->in[1]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						   	//z + a + -b
																				   data[i]->in[1]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						 	//z + -a + -b
																				   data[i]->in[1]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b
																				   data[i]->in[1]->line_id+1);
				}
			}

			//入力数が3のとき
			//intput1:a,input2:b,input3:c,output:z
			else if(data[i]->n_in == 3){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("3-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + -a + -b + -c
														data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("3-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + -a + -b + -c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("3-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + a + b + +c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("3-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//z + a + b + c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("3-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + -a + b + c
					fprintf(out_fp,"%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + a + -b + c
					fprintf(out_fp,"%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + a + b + -c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//z + -a + -b + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + a + b + c
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("3-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + -a + b + c
					fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + a + -b + c
					fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + a + b + -c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//-z + -a + -b + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + a + b + c
				}
			}

			//入力数が4のとき
			//intput:a,b,c,d
			//output:z
			else if(data[i]->n_in == 4){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("4-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//z + -a + -b + -c + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("4-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + -a + -b + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("4-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("4-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("4-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 //-z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("4-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   				//-z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				  	//-z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   				//-z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   				//-z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}
			}

			//入力数が5のとき
			//intput:a,b,c,d,e
			//output:z
			else if(data[i]->n_in == 5){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("5-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,			//z + -a + -b + -c + -d + -e
							data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("5-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + -a + -b + -d
							data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("5-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d + e
							data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("5-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//-z + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + a + b + c + d + e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==9){																											//XORゲートの時
					//printf("5-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + -a + b + c + d + e
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + -b + c + d + e
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + -c + d + e
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + -d + e
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + -e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//z + -a + -b + -c + -d + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + e
				}

				else if(data[i]->type==10){																											//XNORゲートの時
					//printf("5-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + -a + b + c + d + e
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + -b + c + d + e
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + -c + d + e
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + -d + e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + -e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//-z + -a + -b + -c + -d + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + e
				}
			}

			//入力数が6のとき
			//intput:a,b,c,d,e,f
			//output:z
			else if(data[i]->n_in == 6){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("6-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("6-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,			//-z + -a + -b + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("6-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("6-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==9){																												//XORゲートの時
					//printf("6-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==10){																												//XNORゲートの時
					//printf("6-xnor\n");
					fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//-z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					 	//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}
			}

			//入力数が7のとき
			//intput:a,b,c,d,e,f,g
			//output:z
			else if(data[i]->n_in == 7){
				if(data[i]->type==5){																									//ANDゲートの時
				//	printf("7-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//-z + g
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("7-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 									//z + g
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//-z + -a + -b + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("7-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);									//z + -g
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("7-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//-z + -g
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("7-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(line->type==10){																								//XNORゲートの時
					//printf("7-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//-z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//-z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//-z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//-z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//-z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//-z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//-z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//-z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}
			}

			//入力数が8のとき
			//intput:a,b,c,d,e,f,g,h
			//output:z
			else if(data[i]->n_in == 8){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("8-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("8-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",											//-z + -a + -b + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("8-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("8-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("8-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 													//z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id,data[i]->in[0]->line_id,data[i]->in[1]->line_id,
									data[i]->in[2]->line_id,data[i]->in[3]->line_id,data[i]->in[4]->line_id,
									data[i]->in[5]->line_id,data[i]->in[6]->line_id,data[i]->in[7]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  						//z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  						//z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//-z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("8-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//-z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//-z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//-z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//-z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  					//-z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  					//-z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//-z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}
			}

			//入力数が9のとき
			//intput:a,b,c,d,e,f,g,h,i
			//output:z
			else if(data[i]->n_in == 9){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("9-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 							//-z + i
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("9-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 								//z + i
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										//-z + -a + -b + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("9-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);								//z + -i
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("9-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);							//-z + -i
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("9-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",											   	//z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("9-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//-z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//-z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 									//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}
			}

			/*** 折り返し地点から開始地点へフェードバック(muxを挟む) ***/
			//*経路の折り返し地点の時
			//*フィードバックのためのMUXを追加
			if(data[i]->endflg == 1)
			{
				//printf("mux-end\n");
				/*printf("start:%lu\n",start[N]->line_id);
				printf("end:%lu\n",end[N]->line_id);
				printf("in1:%lu",start[N]->in[0]->line_id);
				//printf("in2:%lu",data[i]->in[1]->line_id);
				printf("out1:%lu\n",end[N]->out[0]->line_id);*/
				//fprintf(out_fp,"mux\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,data[i]->st->in[0]->line_id+1);		//-z + a + b
				fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);									//-z + a + s
				fprintf(out_fp,"-%lu %lu -%lu 0\n", c2,data[i]->st->in[0]->line_id+1,c1);								//-z + b + -s
				fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->st->in[0]->line_id+1,c1);								//z + -b + -s
				fprintf(out_fp,"%lu -%lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);									//z + -a + s
				fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->out[0]->line_id+1,data[i]->st->in[0]->line_id+1);		//z + -a + -b
				//printf("c1:%lu,c2:%lu\n", c1,c2);
				c1+=2;
				c2+=2;
			}
			//経路内の折り返し地点以外の時
			//経路を開くためのMUXを追加する
			else
			{//bはr[]
				//ファンアウト数が1のときMUXを2つ追加
				if(data[i]->n_out == 1){
					//printf("2-muxes\n");
					for (int j = 0; j < 2; j++)
					{
						//fprintf(out_fp,"mux\n");
						//printf("St.in1:%lu",start[N]->in[0]->line_id);
						//printf("in2:%lu",data[i]->in[1]->line_id);
						//printf("%d\n", j);
						//printf("rand[%d]ID:%lu,outID%lu\n",g,r[g]->line_id,r[g]->out[0]->line_id);
						//printf("rand[%d]ID:%lu,TYPE:%u\n",g,r[g]->line_id,r[g]->type);
						//printf("outID:%lu\n",r[g]->out[0]->line_id);
						fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//-z + a + b
						fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);						//-z + a + s
						fprintf(out_fp,"-%lu %lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);							//-z + b + -s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);							//z + -b + -s
						fprintf(out_fp,"%lu -%lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);						//z + -a + s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//z + -a + -b
						//printf("c1:%lu,c2:%lu\n", c1,c2);
						g++;
						c2+=2;
					}
					c1+=2;	//追加した2つのMUXはキービットsを共有するため、ここでカウントする
				}
				//ファンアウト数が1以上の時MUXを1つ追加+1
				else if(data[i]->n_out > 1){
					//printf("1-mux\n");
					//fprintf(out_fp,"mux\n");
					//printf("rand[%d]ID:%lu,outID%lu\n",g,r[g]->line_id,r[g]->out[0]->line_id);
					//printf("rand[%d]ID:%lu,TYPE:%u\n",g,r[g]->line_id,r[g]->type);
					//printf("outID:%lu\n",r[g]->out[0]->line_id);
					fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);		//-z + a + b
					fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);							//-z + a + s
					fprintf(out_fp,"-%lu %lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);								//-z + b + -s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);								//z + -b + -s
					fprintf(out_fp,"%lu -%lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);							//z + -a + s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);		//z + -a + -b
					//printf("c1:%lu,c2:%lu\n", c1,c2);
					g++;
					c1+=2;
					c2+=2;
				}
			}
			count++;
				/*
				* end[N]->out[0]->line_id	:	muxの入力a
				* start[N]->in[0]->line_id	:	muxの入力b
				* c1						:	muxの入力s
				* start[N]->out[0]->line_id	:	muzの出力z(c2にするかも？)
				*/

		}
		else{																								//経路でない時
			//入力数が1のとき
			//input:a output:z
			//printf("nomal\n");
			if(data[i]->n_in == 1){
				if(data[i]->type==3){																		//INVゲートの時
					//printf("inv\n");
					fprintf(out_fp,"%lu %lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);		//a + z
					fprintf(out_fp,"-%lu -%lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);	//-a+ -z
				}
				else if(data[i]->type==4){																		//BUFゲートの時
					//printf("buf\n");
					fprintf(out_fp,"-%lu %lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);	//-a + z
					fprintf(out_fp,"%lu -%lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);	//a+ -z
				}
			}

			//入力数が2のとき
			//intput1:a,input2:b,output:z
			else if(data[i]->n_in == 2){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("2-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);		//z + -a + -b
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("2-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);	//-z + -a + -b
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("2-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);		//-z + a + b
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("2-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);		//z + a + b
				}

				else if(data[i]->type==9){																									//XORゲートの時
					//printf("2-xor\n");
					fprintf(out_fp,"%lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + -a + b
					fprintf(out_fp,"%lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1); 	//-z + -a + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//-z + a + b
				}

				else if(data[i]->type==10){																									//XNORゲートの時
					//printf("2-xor\n");
					fprintf(out_fp,"-%lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//-z + -a + b
					fprintf(out_fp,"-%lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1); 	//z + -a + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + a + b
				}
			}

			//入力数が3のとき
			//intput1:a,input2:b,input3:c,output:z
			else if(data[i]->n_in == 3){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("3-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + -a + -b + -c
														data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("3-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + -a + -b + -c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("3-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//-z + a + b + +c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("3-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//z + a + b + c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==9){																									//XORゲートの時
					//printf("3-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + -a + b + c
					fprintf(out_fp,"%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + a + -b + c
					fprintf(out_fp,"%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + a + b + -c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//z + -a + -b + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + a + b + c
				}

				else if(data[i]->type==10){																									//XNORゲートの時
					//printf("3-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + -a + b + c
					fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + a + -b + c
					fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + a + b + -c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//-z + -a + -b + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + a + b + c
				}
			}

			//入力数が4のとき
			//intput:a,b,c,d
			//output:z
			else if(data[i]->n_in == 4){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("4-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//z + -a + -b + -c + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("4-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + -a + -b + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("4-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("4-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==9){																									//XORゲートの時
					//printf("4-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   	//z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==10){																									//XNORゲートの時
					//printf("4-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   	//-z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}
			}

			//入力数が5のとき
			//intput:a,b,c,d,e
			//output:z
			else if(data[i]->n_in == 5){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("5-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("5-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + -a + -b + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("5-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d + e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("5-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d + e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==9){																										//XORゲートの時
					//printf("5-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + -a + b + c + d + e
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + -b + c + d + e
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + -c + d + e
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + -d + e
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + -e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//z + -a + -b + -c + -d + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + e
				}

				else if(data[i]->type==10){																										//XNORゲートの時
					//printf("5-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + -a + b + c + d + e
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + -b + c + d + e
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + -c + d + e
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + -d + e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + -e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//-z + -a + -b + -c + -d + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + e
				}
			}

			//入力数が6のとき
			//intput:a,b,c,d,e,f
			//output:z
			else if(data[i]->n_in == 6){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("6-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("6-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,			//-z + -a + -b + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("6-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("6-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==9){																												//XORゲートの時
					//printf("6-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==10){																												//XNORゲートの時
					//printf("6-xnor\n");
					fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//-z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					 	//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}
			}

			//入力数が7のとき
			//intput:a,b,c,d,e,f,g
			//output:z
			else if(data[i]->n_in == 7){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("7-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//-z + g
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",														//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("7-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 									//z + g
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//-z + -a + -b + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("7-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);									//z + -g
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("7-or\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//-z + -g
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("7-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(line->type==10){																								//XNORゲートの時
					//printf("7-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//-z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//-z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//-z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//-z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//-z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//-z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//-z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//-z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}
			}

			//入力数が8のとき
			//intput:a,b,c,d,e,f,g,h
			//output:z
			else if(data[i]->n_in == 8){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("8-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("8-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",											//-z + -a + -b + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("8-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("8-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("8-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 													//z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id,data[i]->in[0]->line_id,data[i]->in[1]->line_id,
									data[i]->in[2]->line_id,data[i]->in[3]->line_id,data[i]->in[4]->line_id,
									data[i]->in[5]->line_id,data[i]->in[6]->line_id,data[i]->in[7]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  						//z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  						//z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//-z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("8-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//-z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//-z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//-z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//-z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  					//-z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  					//-z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//-z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}
			}

			//入力数が9のとき
			//intput:a,b,c,d,e,f,g,h,i
			//output:z
			else if(data[i]->n_in == 9){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("9-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 							//-z + i
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("9-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 								//z + i
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										//-z + -a + -b + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("9-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);								//z + -i
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("9-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);							//-z + -i
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("9-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",											   	//z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==10){																						//XNORゲートの時
					//printf("9-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//-z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//-z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 									//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}
			}
		}
	}
	printf("count:%d\n",count);
	printf("m_node:%d\n",m_node);
	printf("m_num:%d\n",m_num);
	printf("g:%d\n",g);
	printf("m_edge:%lu\n",m_edge);
	printf("size:%lu\n",size);
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