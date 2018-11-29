/* 論理シミュレーションを行う */
/* 入力パターンのテキストファイルを用意する	*/
#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void makeCycle(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	LINE *Logic_level[Line_info.n_line];
	LINE *line;
	int loops = 1; //作成するループの数
	int length = 3; //作成するループの長さ

	/*	ID順にレベルやタイプを取得 */
	//printf("----------ソート前-----------\n");
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		Logic_level[i] = line;
		Logic_level[i]->level  = line->lv_pi; //ここをポインタに
		Logic_level[i]->line_id  = line->line_id;
		Logic_level[i]->type = line->type;

		//printf("ID:%lu,入力レベル:%lu,TYPE:%u\n", Logic_level[i]->line_id,Logic_level[i]->level,Logic_level[i]->type);

	}

	/***	取得したものをレベル順にソート ***/
	LINE *temp[Line_info.n_line]; //バブルソート用

	for(int i = 0;i<Line_info.n_line;i++){
		for (int j = 1; j < Line_info.n_line;j++){
			if (Logic_level[j-1]->level > Logic_level[j]->level){

				temp[i] = Logic_level[j-1];
				Logic_level[j-1] = Logic_level[j];
				Logic_level[j] = temp[i];
			}
		}
	}

	/*printf("----------ソート後-----------\n");
	for(int i = 0;i<Line_info.n_line;i++){
		printf("ID:%lu,入力レベル:%lu,TYPE:%u\n",Logic_level[i]->line_id,Logic_level[i]->level,Logic_level[i]->type);
	}
	*/

	/* ここからループを作る */
	int N = 0;
	while(N < loops){
		//for(int M = 0 M < length; M++){
			int num = rand() % Line_info.n_line + 1; //ランダムに選択するゲート番号を選ぶ(信この値は号線なども含む)
			while(num < 2 && 10 < num){ //ゲート以外の場合
				num = rand() % Line_info.n_line + 1; //選択し直す
			}
			printf("ID1:%lu,ID2:%lu\n",Logic_level[num]->line_id,Logic_level[num+length]->line_id);
			N++;
		//}
	}


	
}