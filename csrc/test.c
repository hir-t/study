#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
LINE *dfs2(int M ,int loops,int length,LINE *start);

void test(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int length = 3; //作成するループの長さ
	int loops = 1; //作成するループの数
	int M = 0;    //長さ(深さ)を数える
	//int N = 0;	  //ループの数を数える
	int num[loops]; //ループの初期ノードid
	//int nextID = 0; //次に訪れるゲートのid
	//int prevID = 0; //nextの前に訪れたゲートid

	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start[loops];
	LINE *end[loops];
	//LINE *path[loops][length];

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

		/***** ループの開始地点と折り返し地点を決める *****/
	for (int i = 0; i < loops; i++)
	{
		//num[N] = rand() % Line_info.n_line; //ランダムに選択するゲート番号を選ぶ(この値は信号線なども含む)
		num[i] = 10;
		start[i] = data[num[i]];
		while((start[i]->type < 3 && 10 < start[i]->type)){ //ゲート以外の場合																									//(num < line->n_in - length)は修正の必要あり
			num[i] = rand() % Line_info.n_line; //選択し直す
			start[i] = data[num[i]];
		}
		printf("id%d:%lu\n",i,start[i]->line_id);
	}

	for(int i = 0; i < loops; i++){
		printf("aaa\n");
		end[i] = dfs2(M,loops,length,start[i]);
		printf("endID:%lu,endTYPE:%u\n",end[i]->line_id,end[i]->type);
	}


}