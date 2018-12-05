//複数のループ作成の場合、ループ部分だけを書き足したい
//今はループを増やすたびに全部を書き直してる

#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void obfuscation(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int length = 1; //作成するループの長さ
	int loops = 2; //作成するループの数
	int M = 0;    //長さ(深さ)を数える
	int N = 0;	  //ループの数を数える
	int num[loops]; //ループの初期ノードid

	LINE *data[Line_info.n_line];
	LINE *line;
	LINE *start[loops];
	LINE *end[loops];

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

	/***** ループの開始地点と折り返し地点を決める *****/

	for (N = 0; N < loops; N++)
	{
		num[N] = rand() % Line_info.n_line; //ランダムに選択するゲート番号を選ぶ(この値は信号線なども含む)
		start[N] = data[num[N]];
		while((start[N]->type < 3 && 10 < start[N]->type)){ //ゲート以外の場合																									//(num < line->n_in - length)は修正の必要あり
			num[N] = rand() % Line_info.n_line; //選択し直す
			start[N] = data[num[N]];
		}
		printf("id%d:%lu\n",N,start[N]->line_id);
	}

	for (N = 0; N < loops; N++)
	{
		for(int i = 0;i<Line_info.n_line;i++)
		{
			line = &(Line_head[i]);
			if(line->line_id == num[N])
			{
				while(M<length){
					line = &(Line_head[num[N]]);
					//Logic_level[num] = line;
					for (int j = 0; j < line->n_out; j++)
					{
						//printf("selectedID:%lu,selectedTYPE:%u,outID:%lu,outTYPE:%u\n",line->line_id,line->type,line->out[j]->line_id,line->out[j]->type);
					}
					num[N] = line->out[0]->line_id; //次の信号線番号へ更新

					if(3<data[num[N]]->out[0]->type && data[num[N]]->out[0]->type<10)
					{ //
						M++;
					}
					if(data[num[N]]->out[0]->type == 1)
					{ //外部出力に到達したら
						break;
					}
				}
				end[N] = data[num[N]];
			}
		}
	}

	/***** ファイルの内容を書き込んでいく　*****/
	for(N = 0; N < loops; N++){
		for(int i = 0;i<Line_info.n_line;i++)
		{
			line = &(Line_head[i]);
			//idが折り返し地点の時
			if(line->line_id == end[N]->line_id)				//idが折り返し地点の時BUFとして、始点へ繋ぐ
			{
				if(line->n_in == 1)
				{

					if(line->type==3)
					{															// INVゲートの時
						fprintf(out_fp,"%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);			//a + c
						fprintf(out_fp,"-%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a + -c
					}
					if(line->type==4)
					{															//BUFゲートの時
						fprintf(out_fp,"-%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a + c
						fprintf(out_fp,"%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//a+ -c
					}
				}
				//入力数が2のとき
				//intput1:a,input2:b,output:c
				if(line->n_in == 2){
					if(line->type==5)
					{																							//ANDゲートの時
							fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id); 								//-c + a
							fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id); 								//-c + b
							fprintf(out_fp,"-%lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//-a + -b + c
					}

					if(line->type==6)
					{																						//NANDゲートの時
						fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//c + a
						fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//c + b
						fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//-a + -b + -c
					}

					if(line->type==7)
					{																						//ORゲートの時
						fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//c + -a
						fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//c + -b
						fprintf(out_fp,"%lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//a + b + -c
					}

					if(line->type==8)
					{																						//NORゲートの時
						fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);								//-c + -a
						fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);								//-c + -b
						fprintf(out_fp,"%lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);			//a + b + c
					}

					if(line->type==9)
					{																						//XORゲートの時
						fprintf(out_fp,"%-lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//-a + b + c
						fprintf(out_fp,"%lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + -b + c
						fprintf(out_fp,"%-lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id); 		//-a + -b + -c
						fprintf(out_fp,"%lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + b + -c
					}

					if(line->type==10)
					{																						//XNORゲートの時
						fprintf(out_fp,"%-lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//-a + b + -c
						fprintf(out_fp,"%lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//a + -b + -c
						fprintf(out_fp,"%-lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//-a + -b + c
						fprintf(out_fp,"%lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + b + c
					}
					//printf("\n");
				}
				fprintf(out_fp,"-%lu %lu 0\n",line->out[0]->line_id,start[N]->in[0]->line_id);		//-a + c
				fprintf(out_fp,"%lu -%lu 0\n",line->out[0]->line_id,start[N]->in[0]->line_id);		//a + -c
			}

			//入力数が1のとき
			//input:a output:b
			else
			{
				if(line->n_in == 1)
				{

					if(line->type==3)
					{															// INVゲートの時
						fprintf(out_fp,"%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);			//a + c
						fprintf(out_fp,"-%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a + -c
					}
					if(line->type==4)
					{															//BUFゲートの時
						fprintf(out_fp,"-%lu %lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//-a + c
						fprintf(out_fp,"%lu -%lu 0\n",line->in[0]->line_id,line->out[0]->line_id);		//a+ -c
					}
				}
				//入力数が2のとき
				//intput1:a,input2:b,output:c
				if(line->n_in == 2)
				{
					if(line->type==5)
					{																							//ANDゲートの時
						fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id); 								//-c + a
						fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id); 								//-c + b
						fprintf(out_fp,"-%lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//-a + -b + c
					}

					if(line->type==6)
					{																						//NANDゲートの時
						fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//c + a
						fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//c + b
						fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//-a + -b + -c
					}

					if(line->type==7)
					{																						//ORゲートの時
						fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);									//c + -a
						fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);									//c + -b
						fprintf(out_fp,"%lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);		//a + b + -c
					}

					if(line->type==8)
					{																						//NORゲートの時
						fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id);								//-c + -a
						fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id,line->in[1]->line_id);								//-c + -b
						fprintf(out_fp,"%lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);			//a + b + c
					}

					if(line->type==9)
					{																						//XORゲートの時
						fprintf(out_fp,"%-lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//-a + b + c
						fprintf(out_fp,"%lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + -b + c
						fprintf(out_fp,"%-lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id); 		//-a + -b + -c
						fprintf(out_fp,"%lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + b + -c
					}

					if(line->type==10)
					{																						//XNORゲートの時
						fprintf(out_fp,"%-lu %lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//-a + b + -c
						fprintf(out_fp,"%lu -%lu -%lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//a + -b + -c
						fprintf(out_fp,"%-lu -%lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   	//-a + -b + c
						fprintf(out_fp,"%lu %lu %lu 0\n", line->in[0]->line_id,line->in[1]->line_id,line->out[0]->line_id);   		//a + b + c
					}
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