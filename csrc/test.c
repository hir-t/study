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
	//int N = 0;	  //ループの数を数える
	int num = 0; //ループの初期ノードid
	int gate = 0;
	int count = 0;
	//int nextID = 0; //次に訪れるゲートのid
	//int prevID = 0; //nextの前に訪れたゲートid
	//int entry = 0;

	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start[loops];
	LINE *end[loops];
	//LINE *path[loops][length];
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
	int gateId[gate];
	int n = 0;
	for (int i = 0; i < Line_info.n_line; i++)
	{
		line = &(Line_head[i]);
		if(2<line->type && line->type<11)
		{		//回路内のゲートの数を数える
			gateId[n] = line->line_id;
			n++;
		}
		if(gate == n){
			break;
		}
	}

	for(int i = 0;i<gate;i++){
		printf("GATE:%d\n",gateId[i]);
	}

	/***** ループの開始地点と折り返し地点を決める *****/

	/* 乱数SEED設定 */
	srand( (int)time(NULL) );
	int i = 0;
	//for (int i = 0; i < loops; i++)
	while((i != loops && entries < 1) || (i != loops && discovery !=1))
	{
		printf("--------------TAKE%d--------------\n",i);
		M = 0;
		//num = 4;
		num = rand() % gate; //ランダムに選択するゲート番号を選ぶ(この値は信号線なども含む)
		printf("num:%d\n",gateId[num]);
		//num[i] = 10;
		start[i] = data[gateId[num]];

		printf("STARTid%d:%lu\n",i,start[i]->line_id);

		end[i] = dfs2(entry,M,loops,length,start[i]);

		//printf("endID:%lu,endTYPE:%u\n",end[i]->line_id,end[i]->type);
/*		if(entries < 1 || discovery != 1){ //複数のエントリポイントが見つからなかった場合、または経路が短かった場合
			i = i-1;	 //もう一度探索し直すために、カウント変数を減らす
			count++;
		}*/
		
		if(count == 2){
			printf("!!!! ERROR !!!!\n");
			break;
		}
		i++;
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