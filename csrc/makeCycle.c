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
/*
	LINE *temp[Line_info.n_line]; //バブルソート用

	for(int i = 0;i<Line_info.n_line;i++){
		for (int j = 1; j < Line_info.n_line;j++){
			if (Logic_level[j-1]->level > Logic_level[j]->level){

				temp[i] = Logic_level[j-1];
				Logic_level[j-1] = Logic_level[j];
				Logic_level[j] = temp[i];

			}
		}
	}*/
	/*
	printf("----------ソート後-----------\n");
	for(int i = 0;i<Line_info.n_line;i++){
		printf("ID:%lu,入力レベル:%lu,TYPE:%u,i:%d\n",Logic_level[i]->line_id,Logic_level[i]->level,Logic_level[i]->type,i);
	}*/


	/* ここからループを作る */
	int N = 0;
	int M = 0;
	int first;
	while(N < loops){
		//int num = 10; //ループ作成の開始地点の信号線番号
		int num = rand() % Line_info.n_line; //ランダムに選択するゲート番号を選ぶ(信この値は号線なども含む)
		while((Logic_level[num]->type < 3 && 10 < Logic_level[num]->type)){ //ゲート以外の場合																									//(num < line->n_in - length)は修正の必要あり
			num = rand() % Line_info.n_line; //選択し直す
		}

		first = num;

		//for(int M = 0 ;M < length; M++){
		while(M < length){
			line = &(Line_head[num]);
			Logic_level[num] = line;
			for (int j = 0; j < line->n_out; j++){
				Logic_level[num] = line;
				printf("selectedID:%lu,selectedTYPE:%u,outID:%lu,outTYPE:%u\n",Logic_level[num]->line_id,Logic_level[num]->type,line->out[j]->line_id,line->out[j]->type);
				//printf("outID:%lu,outTYPE:%u,out2ID:%lu,out2TYPE:%u\n",line->out[j]->line_id,line->out[j]->type,line->out[j]->out[j]->line_id,line->out[j]->out[j]->type);
			}

			num = line->out[0]->line_id; //次の信号線番号へ更新

			if(3<line->out[0]->type && line->out[0]->type<10){ //
				M++;
			}
			if(line->out[0]->type == 1){ //外部出力に到達したら
				break;
			}

		//次は選択したゲートのlength(今はlength=3)先のゲートまで出力できるようにする
		}
		line->out[0]->line_id = Logic_level[first]->line_id;
		printf("first:%lu\n",Logic_level[first]->line_id);
		N++;
	}

	/*** 接続チェック ***/
	for(int i=0; i < Line_info.n_line; i++){
		line = &(Line_head[i]);
		printf("-------------------------\n");
		printf("line_id:%lu\n",line->line_id);
		printf("type:%u\n",line->type );

		for (int j = 0; j < line->n_in; j++){
			printf("in:%lu\n",line->in[j]->line_id);
		}

		for (int j = 0; j < line->n_out; j++){
			printf("out:%lu\n",line->out[j]->line_id);
		}

	}
	printf("-------------------------\n");

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
*/