#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void TopologicalSort(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	LINE *Logic_level[Line_info.n_line];
	LINE *line;

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

	/*	取得したものをレベル順にソート */
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
	}*/
}