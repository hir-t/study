#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>

LINE *dfs(int entry,int N,int M ,int loops,int length,LINE *start,LINE *route[loops][length]);		//ループ作成の経路を探す
LINE *dfs2(int N,int M ,int loops,int length,LINE *start,LINE *end,LINE *route2[loops][length*2]);	//ループ作成の経路で省いたブランチを追加する
int shuffle(int[],int[],int);										//乱数生成用関数
void TopologicalSort(int,int,int,LINE *[],LINE *[],LINE *[]);
Ulong countClauses(void);


/* topgun_output.c */
FILE *topgun_open ( char *, char *, char * ); /* fileを開く */

/* topgunCell.c */
void topgun_close( FILE *, char * );

void obfuscation(char *benchName){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int length = 2;		//作成するループの長さ
	int loops = 1;		//作成するループの数
	int M = 0;			//長さ(深さ)を数える
	int num = 0;		//ループの初期ノードid
	int gate = 0;		//回路内のゲート数
	int pos = 0;		//ランダムに繋ぐ位置
	int entry = 0;		//エントリポイントをのチェックに使う
	//int discovery = 0;				//経路発見に使用する

	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start[loops];				//ループの視点
	LINE *end[loops];				//ループの折り返し地点
	LINE *route[loops][length];		//ループの経路
	LINE *route2[loops][length*2];	//ループの経路+途中のブランチ

	/*** 出力ファイル設定 ***/
	FILE *out_fp; //出力ファイル・ポインタ
	FILE *fpCNF;
    FILE *fpCNFSTART;
    FILE *fpCNFEND;
    FILE *fpKEYINFO;
    FILE *fpPIINFO;
    FILE *fpPOINFO;

    char outputCNFFileName[100];
    char outputCNFStartFileName[100];
    char outputCNFEndFileName[100];
    char outputKEYINFOFileName[100];
   	char outputPIInfoFileName[100];
   	char outputPOInfoFileName[100];

    char *funcName = "obfuscation"; // 関数名 

    //printf("Open file %s\n", benchName);

    sprintf(outputCNFFileName, "%s_obf.cnf", benchName);
    sprintf(outputCNFStartFileName, "%s_obf.cnf.start", benchName);
    sprintf(outputCNFEndFileName, "%s_obf.cnf.end", benchName);
    sprintf(outputKEYINFOFileName, "%skeyIn.info", benchName);
   	sprintf(outputPIInfoFileName, "%s_obf.piCnfInfo", benchName);
    sprintf(outputPOInfoFileName, "%s_obf.poCnfInfo", benchName);

    out_fp = fpCNF = topgun_open( outputCNFFileName, "w", funcName );
    fpCNFSTART = topgun_open( outputCNFStartFileName, "w", funcName );
    fpCNFEND = topgun_open( outputCNFEndFileName, "w", funcName );
    fpKEYINFO = topgun_open( outputKEYINFOFileName, "w", funcName );
    fpPIINFO = topgun_open( outputPIInfoFileName, "w", funcName );
    fpPOINFO = topgun_open( outputPOInfoFileName, "w", funcName );

/*	char out_fn[128]; //出力ファイル名を入れるchar型配列
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
*/

	/*	ID順にレベルやタイプを取得 */
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		if(2<line->type && line->type<11){		//回路内のゲートの数を数える
			gate++;
		}
		if(1<line->type && line->type<11){		//回路内のゲートの数を数える(ブランチも含める)
			pos++;
		}
		data[i] = line;
		data[i]->level  = line->lv_pi; //ここをポインタに
		data[i]->line_id  = line->line_id;
		data[i]->type = line->type;
		//printf("ID:%lu,入力レベル:%lu,TYPE:%u\n", data[i]->line_id,data[i]->level,data[i]->type);
	}
	//printf("The number of gate is %d\n",gate);

	Ulong size = Line_info.n_line;
	printf("size:%lu\n",size);

	/**** ゲートだけをまとめた配列を作成する ****/
	int gateId[gate];		//この配列を使ってループの開始、折り返しゲートを決める
	int randId[pos];		//muxで繋ぐゲートID(ランダムに選択する)
	int n = 0;				//ゲート数をカウント
	int m = 0;				//ランダムに選択する信号線をカウント
	for (int i = 0; i < Line_info.n_line; i++)
	{
		line = &(Line_head[i]);
		if(2<line->type && line->type<11 && n<gate)
		{
			gateId[n] = line->line_id;
			n++;
		}
		if(1<line->type && line->type<11){		//回路内のゲートの数を数える(ブランチも含める)
			randId[m] = line->line_id;
			m++;
		}
		if(pos == m){
			break;
		}
	}

	/***** ループの開始地点と折り返し地点、muxをで接続するゲートを決める *****/
	int  N = 0;		//作成するループ数を数える
	shuffle(gateId,randId,gate);
	int g = 0;
	while(N < loops && entries == 0)
	{
		srand( (int)time(NULL) );	//乱数SEED設定
		//printf("--------------ROUTE%d--------------\n",N);
		//printf("%d\n", discovery);
		M = 0;
		num = gateId[g];
		start[N] = data[num];

		end[N] = dfs(entry,N,M,loops,length,start[N],route);
		end[N]->endflg = 1;
		end[N]->st = start[N];
		start[N] -> stflg = 1;
		//printf("START_ID%d:%lu\n",N,start[N]->line_id);
		//printf("END_ID:%lu\n",end[N]->line_id);
		//printf("********************************\n");

		if(flag2 == 1){			//経路が見つかった時
			//経路として選択済みにする
			for (int m = 0; m < length; m++)
			{
				route[N][m]->rtflg = 1;
			}
			//printf("entries:%d\n",entries);
			N++;				//次のループ作成のためにNをインクリメント
			entries = 0;		//次のループのために初期化
			//printf("発見\n");
		}
		//printf("--------------ROUTE%d--------------\n\n",N);

		//訪問済みチェック(flag)を初期化して次のdfsに備える
		for(int i = 0;i<Line_info.n_line;i++){
			data[i]->flag = 0;
		}
		g++;	//次の始点を決める
	}

	N=0;
	while(N < loops)
	{
		M=0;
		dfs2(N,M,loops,length,start[N],end[N],route2);
		if(flag2 == 1){			//経路が見つかった時
			for (int m = 0; m < length*2; m++)
			{
				route2[N][m]->rtflg = 1;
				if (route2[N][m]->line_id==end[N]->line_id) break;
			}
			N++;				//次のループ作成のためにNをインクリメント
			entries = 0;		//次のループのために初期化
		}
		//訪問済みチェック(flag)を初期化して次のdfsに備える
		for(int i = 0;i<Line_info.n_line;i++){
			data[i]->flag = 0;
		}
	}


	//取得した経路を出力
/*	for (int i = 0; i < loops; i++)
	{
		for (int j = 0; j < length; j++)
		{
			printf("route[%d][%d]-ID:%lu,n_in:%lu,n_out:%lu,type:%u,end:%lu\n",i,j,route[i][j]->line_id,route[i][j]->n_in,route[i][j]->n_out,route[i][j]->type,route[i][j]->endflg);
		}
	}
*/
	for (int i = 0; i < loops; i++)
	{
		for (int j = 0; j < length*2; j++)
		{
			LINE *rt = route2[i][j];
			printf("route2[%d][%d]-ID:%lu,n_in:%lu,n_out:%lu,type:%u,rt:%lu,end:%lu\n",i,j,rt->line_id,rt->n_in,rt->n_out,rt->type,rt->rtflg,rt->endflg);
			if(rt->line_id == end[i]->line_id){
			 	break;
			}
		}
	}

	LINE *routeNode[loops*length];			//レベル順に経路内のゲートをまとめる
	//LINE *routeNode2[loops*length*2];		//レベル順に経路内のゲートをまとめる(ブランチも含んだ方)
	//int k = 0;
	N = M = 0;
	for(int k = 0;k < loops*length;k++){

		routeNode[k] = route[N][M];
		//printf("route[%d][%d]:%lu\n",N,M,route[N][M]->line_id);
		int id = routeNode[k]->line_id;
		//data[id]->rtflg = 1;
		data[id]->rtcnt += 1;		//経路に含まれた数をカウント(経路間のダブりをチェック)
		if(M < length){
			data[id]->nextgt = route[N][M+1];	//使わんかも
			M++;
		}
		if(M == length)
		{
			N++;
			M = 0;
		}
	}

/*	N = M = 0;
	for(int k = 0;k < loops*length*2;k++){

		routeNode2[k] = route2[N][M];
		//printf("route2[%d][%d]:%lu\n",N,M,route[N][M]->line_id);
		int id = routeNode2[k]->line_id;
		data[id]->rtflg = 1;
		if(route2[N][M]->line_id==end[N]->line_id)
		{
			//data[id]->next = route[N][M];
			N++;
			M = 0;
			if(N == loops) break;
		}
		else M++;
	}

	int element = sizeof(routeNode2)/sizeof(routeNode2[0]);
	printf("element:%d\n",element);*/

	//makeNext(routeNode2,data,start,end,element);

	//ソート前出力
	//printf("---ソート前---\n");
/*	for(int k = 0; k < loops*length; k++){
		printf("前ID:%lu,LV.%lu\n",routeNode[k]->line_id,routeNode[k]->level);
	}*/

/*	for(int k = 0;k < element; k++){
		printf("%d,2ID:%lu,LV.%lu,TYPE:%u,cnt:%lu\n",k,routeNode2[k]->line_id,routeNode2[k]->level,routeNode2[k]->type,routeNode2[k]->rtcnt);
	}
*/
	//TopologicalSort(loops,length,element,routeNode,routeNode2,data);	//経路内のノードをレベル順にソート

	//ソート後出力
	//printf("--ソート後--\n");
/*	for(int k = 0; k < loops*length; k++){
		printf("後ID:%lu,LV.%lu\n",routeNode[k]->line_id,routeNode[k]->level);
	}*/

/*	for(int k = 0;k < element; k++){
		printf("%d,ID:%lu,LV.%lu,TYPE:%u,cnt:%lu\n",k,routeNode2[k]->line_id,routeNode2[k]->level,routeNode2[k]->type,routeNode2[k]->rtcnt);
	}*/


	//追加するmuxの数と、それに伴って増加する信号線の数を数える
	Ulong m_node 	 =  loops;			//MUXの追加によって増えるノードの数
	Ulong m_num 	 =  0;				//1ループに追加するMUXの総数(折り返し地点以外)を数える
	Ulong m_edge =  loops;		//MUXの追加によって増えるエッジの数
	//int loop_m[loops];			//1ループに追加するMUXの総数(折り返し地点以外)

/*	for (int i = 0; i < loops*length; i++)
	{
		//endflgの条件文は変更の必要あり
		//route[][]でM=lengthのとき
		if (routeNode[i]->endflg == 0)
		{
			if (routeNode[i]->n_out==1)					//経路のファンアウトが1のとき
			{
				m_node = m_node + 2;					//MUXを2つ増やすため
				m_num = m_num + 2;						//MUXを2つ増やすため
				m_edge = m_edge + 4;					//入力sが2つ,出力zが2つ増えるため(2つのMUXの入力s,は同じ)->本来は下の式が正しいはずだが,こうするとc1908.benchのcnfの変数の数が正しくなる
				//m_edge = m_edge + 3;					//入力sが1つ,出力zが2つ増えるため(2つのMUXの入力s,は同じ)
			}
			else{										//経路のファンアウトが1以外(2で考えるがそれ以上の場合もある)のとき
				m_node = m_node + 1;					//MUXが1つ
				m_num = m_num + 1;						//MUXを1つ増やすため
				m_edge = m_edge + 2;					//s,zが1つ
			}
		}
	}*/

	for (int i = 0; i < loops; i++)
	{
		for (int j = 0; j < length-1; j++)
			{
				if (route[i][j]->n_out==1)					//経路のファンアウトが1のとき
				{
					m_node = m_node + 2;					//MUXを2つ増やすため
					m_num = m_num + 2;						//MUXを2つ増やすため
					//m_edge = m_edge + 4;					//入力sが2つ,出力zが2つ増えるため(2つのMUXの入力s,は同じ)->下の式の方が正しい気がするけど、変数の数は
					//m_edge = m_edge + 3;					//入力sが1つ,出力zが2つ増えるため(2つのMUXの入力s,は同じ)
					m_edge = m_edge + 1;					//キー入力sが1つ増えるため
				}
				else{										//経路のファンアウトが1以外(2で考えるがそれ以上の場合もある)のとき
					m_node = m_node + 1;					//MUXが1つ
					m_num = m_num + 1;						//MUXを1つ増やすため
					//m_edge = m_edge + 2;					//s,zが1つ
					m_edge = m_edge + 1;					//sが1つ
				}
			}
	}


	LINE *r[m_num];										//追加するMUXとランダムに接続するゲートを格納する
	//printf("r_size = %lu\n", sizeof(r));
	//printf("r_element = %lu\n", sizeof(r)/sizeof(Ulong));

	//フラグの初期化
	for (int i = 0; i < Line_info.n_line; i++) data[i]->flag = 0;

/*	//ルートに含まれるゲートのフラグを1にしてランダムゲートとして選択されないようにする
	for (int i = 0; i < Line_info.n_line; i++)
	{
		for (int k = 0; k < loops*length; k++)
		{
			if(routeNode[k]->line_id==data[i]->line_id) data[i]->flag = 1;
		}
	}*/

	//MUXとランダムに接続するゲートを取得する
	for (int i = 0; i < m_node; i++)
	{
		for (int j = 0; i < gate; j++)
		{
			srand( (int)time(NULL) );	//乱数SEED設定
			num = randId[j];								//numにランダムなゲートのIDを代入

			if (1<data[num]->type && data[num]->type<11 && data[num]->rtflg!=1)	//c7552のとき、ランダムなゲートとして外部出力の信号線が選択されることがあるためそれを無理やつ回避するためのtype
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
		printf("r%d:%lu,type%u,n_out:%lu,rt:%lu\n",i,r[i]->line_id,r[i]->type,r[i]->n_out,r[i]->rtflg);
	}*/
	printf("m_node:%lu\n",m_node);
	Ulong variables  = Line_info.n_line + m_edge + 1;		//MUXを追加した後の回路全体の信号線数(命題変数の数)
	Ulong m_clauses = m_node*6;							//muxの節数が6なため
	Ulong clauses = countClauses() + m_clauses;
	//printf("m_clauses:%lu\n", m_clauses);
	//printf("evClauses:%lu\n", countClauses());

	/**** 以下、cnfに変換していく ****/
	////////////////////////////////////
	//ループの作成。格ゲーと間にMUXを挿入していく
	// @ a : MUXの入力
	// @ b : MUXの入力
	// @ s : MUXの入力(キー)
	// @ z : MUXの出力
	////////////////////////////////////
	g = 0;
	N = 0;
	Ulong s	 = Line_info.n_line + 2;		//muxの入力sに与えるID(cnf内で)
	//Ulong z	 = Line_info.n_line + 3;	//muxの入力zに与えるID
	int k = 0;
	fprintf(out_fp,"p cnf %lu %lu\n",variables,clauses);
	fprintf(fpCNFSTART,"1\n");
    topgun_close(fpCNFSTART, funcName);

	for (int i = 0; i < loops; i++)
	{
		for (int j = 0; j < length*2; j++)
		{
			LINE *rt = route2[i][j];
			printf("ID:%lu,TYPE:%u\n",rt->line_id,rt->type);
			if(2<rt->type&&rt->type<11){
				//経路の折り返し地点の時
				//フィードバックのためのMUXを追加
				if(rt->line_id == end[N]->line_id)
				{
					Ulong a = end[N]->line_id+1;
					Ulong b = start[N]->in[0]->line_id+1;
					Ulong z = b;
					printf("END\n");
					fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,b);			//-z + a + b
					fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,s);			//-z + a + s
					fprintf(out_fp,"-%lu %lu -%lu 0\n", z,b,s);			//-z + b + -s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", z,b,s);			//z + -b + -s
					fprintf(out_fp,"%lu -%lu %lu 0\n", 	z,a,s);			//z + -a + s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", z,a,b);			//z + -a + -b
					fprintf(fpKEYINFO,"%lu\n",s);
					printf("-%lu %lu %lu 0\n",  z,a,b);				//-z + a + b
					printf("-%lu %lu %lu 0\n",	 z,a,s);				//-z + a + s
					printf("-%lu %lu -%lu 0\n", z,b,s);				//-z + b + -s
					printf("%lu -%lu -%lu 0\n", z,b,s);				//z + -b + -s
					printf("%lu -%lu %lu 0\n",  z,a,s);				//z + -a + s
					printf("%lu -%lu -%lu 0\n", z,a,b);				//z + -a + -b
					N++;
					k++;
					s++;
					break;
				}

				//経路内の折り返し地点以外の時
				//経路を開くためのMUXを追加する
				else
				{
					//ファンアウト数が1のときMUXを2つ追加
					if(rt->n_out == 1){
						Ulong a = rt->line_id+1;			//ファンアウトが1の時、出力の信号線IDはそのゲートと同じ
						Ulong b = r[g]->line_id+1;	//
/*						while(r[g]->lv_pi > rt->lv_pi){
							g++;
							b = r[g]->line_id+1;
						}*/
						Ulong z = a;						//一個目の出力zは経路内のゲート(rt)の出力
						printf("2\n");
					//	for (int mux = 0; mux < 2; mux++)
					//	{
							fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,b);				//-z + a + b
							fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,s);				//-z + a + s
							fprintf(out_fp,"-%lu %lu -%lu 0\n", z,b,s);				//-z + b + -s
							fprintf(out_fp,"%lu -%lu -%lu 0\n", z,b,s);				//z + -b + -s
							fprintf(out_fp,"%lu -%lu %lu 0\n", 	z,a,s);				//z + -a + s
							fprintf(out_fp,"%lu -%lu -%lu 0\n", z,a,b);				//z + -a + -b
							printf("-%lu %lu %lu 0\n",  z,a,b);				//-z + a + b
							printf("-%lu %lu %lu 0\n",	 z,a,s);			//-z + a + s
							printf("-%lu %lu -%lu 0\n", z,b,s);				//-z + b + -s
							printf("%lu -%lu -%lu 0\n", z,b,s);				//z + -b + -s
							printf("%lu -%lu %lu 0\n",  z,a,s);				//z + -a + s
							printf("%lu -%lu -%lu 0\n", z,a,b);				//z + -a + -b
							g++;
							a = r[g]->line_id+1;
							b = rt->line_id+1;
/*							while(r[g]->lv_pi > rt->lv_pi){
								g++;
								a = r[g]->line_id+1;
							}*/
							z = a;
							fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,b);				//-z + a + b
							fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,s);				//-z + a + s
							fprintf(out_fp,"-%lu %lu -%lu 0\n", z,b,s);				//-z + b + -s
							fprintf(out_fp,"%lu -%lu -%lu 0\n", z,b,s);				//z + -b + -s
							fprintf(out_fp,"%lu -%lu %lu 0\n", 	z,a,s);				//z + -a + s
							fprintf(out_fp,"%lu -%lu -%lu 0\n", z,a,b);				//z + -a + -b
							printf("-%lu %lu %lu 0\n",  z,a,b);				//-z + a + b
							printf("-%lu %lu %lu 0\n",	 z,a,s);			//-z + a + s
							printf("-%lu %lu -%lu 0\n", z,b,s);				//-z + b + -s
							printf("%lu -%lu -%lu 0\n", z,b,s);				//z + -b + -s
							printf("%lu -%lu %lu 0\n",  z,a,s);				//z + -a + s
							printf("%lu -%lu -%lu 0\n", z,a,b);				//z + -a + -b
							fprintf(fpKEYINFO,"%lu\n",s);
							s++;
					//	}
					}
					//ファンアウト数が1以上の時MUXを1つ追加+1
					else if(rt->n_out > 1){
						Ulong a = route2[i][j+1]->line_id+1;
						Ulong b = r[g]->line_id+1;
/*						while(r[g]->lv_pi > route2[i][j+1]->lv_pi){
							g++;
							b = r[g]->line_id+1;
						}*/
						Ulong z = a;
						printf("1\n");
						fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,b);				//-z + a + b
						fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,s);				//-z + a + s
						fprintf(out_fp,"-%lu %lu -%lu 0\n", z,b,s);				//-z + b + -s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", z,b,s);				//z + -b + -s
						fprintf(out_fp,"%lu -%lu %lu 0\n", 	z,a,s);				//z + -a + s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", z,a,b);				//z + -a + -b
						fprintf(fpKEYINFO,"%lu\n",s);
						printf("-%lu %lu %lu 0\n",  z,a,b);				//-z + a + b
						printf("-%lu %lu %lu 0\n",	 z,a,s);				//-z + a + s
						printf("-%lu %lu -%lu 0\n", z,b,s);				//-z + b + -s
						printf("%lu -%lu -%lu 0\n", z,b,s);				//z + -b + -s
						printf("%lu -%lu %lu 0\n",  z,a,s);				//z + -a + s
						printf("%lu -%lu -%lu 0\n", z,a,b);				//z + -a + -b
						g++;
						s++;
					}
				}
				//s++;
				k++;
			}
		}
	}
	printf("count:%d\n",k);
	//printf("N:%d\n", N);
	//通常のcnf変換
	for(int i = 0;i<Line_info.n_line;i++){
		//printf("%d.ID:%lu,入力レベル:%lu,TYPE:%u,n_in:%lu,n_out:%lu\n", i,data[i]->line_id,data[i]->level,data[i]->type,data[i]->n_in,data[i]->n_out);

		//if(data[i]->rtflg == 1){	//取得したIDがループ経路のとき
		//if(data[i]->rtflg != 1){	//取得したIDがループ経路以外の時
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

			else if(data[i]->type==10){																								//XNORゲートの時
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
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h + i
								data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
								data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
								data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
								data[i]->in[8]->line_id+1);
			}

			else if(data[i]->type==6){																							//NANDゲートの時
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
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",												//-z + -a + -b + -d + -e + -f + -g + -h + -i
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
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 													//-z + a + b + c + d + e + f + g + h + i
							data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
							data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
							data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
							data[i]->in[8]->line_id+1);
			}

			else if(data[i]->type==8){																							//NORゲートの時
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
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h + i
								data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
								data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
								data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
								data[i]->in[8]->line_id+1);
			}

			else if(data[i]->type==9){																						//XORゲートの時
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

	topgun_close(fpCNF, funcName);

    fprintf(fpCNFEND,"%lu\n",variables-1);
    topgun_close(fpCNFEND, funcName);

    /* キーの信号線をファイルへ出力する*/
 /*   for (int i = 0; i < k+1; i++)
    {
    	fprintf(fpKEYINFO,"%lu\n",key[i]);
    	printf("%lu\n",key[i]);
    }*/

    topgun_close(fpKEYINFO, funcName);

    for(Ulong i = 0; i < Line_info.n_line; i++){
		line = &Line_head[i];
		switch ( line->type )
		{
			case TOPGUN_PI:
		    	fprintf(fpPIINFO,"%lu\n", line->line_id + 1);
		    	break;
			case TOPGUN_PO:
		    	fprintf(fpPOINFO,"%lu\n", line->line_id + 1);
		    	break;
			default:
		    	break;
		}
    }

    topgun_close(fpPIINFO, funcName);
    topgun_close(fpPOINFO, funcName);

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