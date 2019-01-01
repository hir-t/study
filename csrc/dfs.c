#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void dfs(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int length = 3; //作成するループの長さ
	int loops = 1; //作成するループの数
	int M = 1;    //長さ(深さ)を数える
	int N = 0;	  //ループの数を数える
	int num[loops]; //ループの初期ノードid
	int nextID = 0; //次に訪れるゲートのid
	int prevID = 0; //nextの前に訪れたゲートid

	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start[loops];
	LINE *end[loops];
	LINE *path[loops][length];

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
	for (N = 0; N < loops; N++)
	{
		//num[N] = rand() % Line_info.n_line; //ランダムに選択するゲート番号を選ぶ(この値は信号線なども含む)
		num[N] = 10;
		start[N] = data[num[N]];
		while((start[N]->type < 3 && 10 < start[N]->type)){ //ゲート以外の場合																									//(num < line->n_in - length)は修正の必要あり
			num[N] = rand() % Line_info.n_line; //選択し直す
			start[N] = data[num[N]];
		}
		printf("id%d:%lu\n",N,start[N]->line_id);
	}

	for (N = 0; N < loops; N++)								//作成するループの数だけ回す
	{
		printf("N=%d\n",N);
		//for(int i = num[N];i<Line_info.n_line;i++)				//信号線をスタートから数える
		//{
			line = &(Line_head[num[N]]);
			start[N]->flag = 1;							//スタートを訪問済みにする
			path[N][M] = start[N];
			prevID = line->line_id;
			//next = line->out[j]->line_id; //次の信号線番号へ更新

			while(M<length){
				printf("M=%d\n",M);
				line = &(Line_head[num[N]]);

				for (int j = 0;j < line->n_out;j++)
				{
					//prev = data[next]->line_id;
					if(data[prevID] -> flag ==1){
						printf("id%dは訪問済み\n",prevID);
						if(j == line->n_out-1){

							prevID=data[prevID]->prev[0]->line_id;
							printf("check6\n");
							printf("prev_id:%d\n",prevID);

						}
					}
					nextID = data[prevID]->out[j]->line_id;	//次の信号線番号へ更新
					data[nextID]->prev = &data[prevID];
					printf("prev_id:%d\n",prevID);
					printf("next_id:%d\n",nextID);

					while(data[nextID]->type == 2){	//次の信号線がブランチの場合
						printf("check1\n");					//!!!!!!!!!!!!!!２週目のj=1になったとき、信号線16のoutが1つ(=out[0])しかないのにout[1]を読もうとしてバグる
						if(data[nextID]->n_out==1){
							j=0;
						}
						nextID = data[nextID]->out[j]->line_id;
						data[nextID]->prev = &data[prevID];
						printf("prev_id:%d\n",prevID);
						//printf("^^^^prev_id:%lu\n",data[nextID]->prev[0]->line_id);
						printf("next_id:%d\n",nextID);
						//break;
					}

					if(2<data[nextID]->type && data[nextID]->type<11) //次の信号線がゲートの場合
					{
						if(data[nextID]->flag != 1){	//このゲートが未訪問なら
							printf("check2\n");
							data[nextID]->flag = 1;
							M++;						//長さを増やすし
							path[N][M]=data[nextID];      //経路に加える
							prevID = data[nextID]->line_id;
							nextID = data[nextID]->out[j]->line_id;	//次の信号線へ更新
							printf("prev_id:%d\n",prevID);
							printf("next_id:%d\n",nextID);
						}
					}

					if(data[nextID]->out[j]->type == 1)
					{ //外部出力に到達したら
						printf("check3\n");
						if (M == length)
						{
							printf("check4\n");
							break;
						}
						//if(M != length-1){   //さらに長さが足りない時
						else {
							//M--;
							//j++;
							printf("check5\n");
							nextID = prevID;
							printf("prev_id:%d\n",prevID);
							printf("next_id:%d\n",nextID);
						}
						//}
						//printf("Oh no.\n");
						//break;
					}
				}
			}
			end[N] = data[prevID];
		//}
	}
	for (N = 0; N < loops; N++)
	{
		printf("path No.%d\n",N);
		for (int M = 0; M < length; M++)
		{
			printf("%lu\n",path[N][M]->line_id);
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