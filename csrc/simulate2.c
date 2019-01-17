/* 論理シミュレーションを行う */
/* 入力パターンのテキストファイルを用意する	*/
#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void simulate2(char *fileName){
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
	/*** シミュレーション ***/
	int Num_of_Input = 0; //入力数

	for (int i = 0; i < Line_info.n_line;i++){
		if(Logic_level[i]->type == 0){ //外部入力数を数える
			Num_of_Input++;
		}
	}
	//printf("外部入力数:%d\n",Num_of_Input);
	int times = 1; //テスト回数
	for (int i = 0; i < Num_of_Input; ++i){
		times = times*2;
	}
	int input_num[Num_of_Input]; //入力用ビット列

	/* テストファイルを読み込む*/
	char in_fn[128]; //入力ファイル名を入れるchar型配列
	FILE *in_fp; //入力ファイル・ポインタ
	sprintf(in_fn, "%s", fileName);

	// ファイルを開く
    in_fp= fopen( in_fn, "r");
    if ( in_fp == NULL ) {
        printf("File Open Error %s\n", fileName);
        exit(0);
    }

	/*printf("Input test file name : ");
	scanf("%s",in_fn); //ファイル名をchar型配列fnへ入力*/

	/*in_fp = fopen(in_fn,"r"); //読み出しモードでファイルを開く
	if(in_fp == NULL){ //fopenに失敗した場合
		printf("The file %s is not exist.\n", in_fn); //失敗した旨を出力
		printf("Input test file name : ");
		scanf("%s",in_fn); //ファイル名をchar型配列fnへ入力
	}*/

	int data;
	int bit = 0;
	int count = 0;
	//int pattern = 0;
	Ulong value[line->n_in]; //AND,NAND演算の際使用

	while((fscanf(in_fp,"%d",&data)) != EOF){ //ファイルの終わりに達するまで1文字ずつ入力
  		input_num[bit] = data;
  		//printf("%d ", data);
  		bit ++;
  		if(bit == 5){
  			for (int i = 0; i < Line_info.n_line;i++){
				if(Logic_level[i]->type == 0){ //TYPEが外部入力のとき
					Logic_level[i]->value = input_num[i]; //コマンドライン引数を代入
				}
				if(Logic_level[i]->type == 1){ //TYPEが外部出力のとき
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						Logic_level[i]->value = Logic_level[i]->in[j]->value; //入力のIDが持つ論理値を代入
					}
				}
				if(Logic_level[i]->type == 2){ //TYPEがファンアウトブランチのとき
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						Logic_level[i]->value = Logic_level[i]->in[j]->value; //入力のIDが持つ論理値を代入
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
					}
				}
				if(Logic_level[i]->type == 3){ //TYPEがインバータのとき
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						if(Logic_level[i]->in[j]->value == 0){ //入力が0のとき
							Logic_level[i]->value = 1; //入力のIDが持つ論理値を反転して代入
						}
						if(Logic_level[i]->in[j]->value == 1){ //入力が1のとき
							Logic_level[i]->value = 0; //入力のIDが持つ論理値を反転して代入
						}
					}
				}
				if(Logic_level[i]->type == 4){ //TYPEがバッファのとき
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						Logic_level[i]->value = Logic_level[i]->in[j]->value; //入力のIDが持つ論理値を代入
					}
				}
				if(Logic_level[i]->type == 5){ //TYPEがANDのとき
					Ulong stack = 0; //ゲート１つに対する全入力の論理値を加算
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						value[j] = Logic_level[i]->in[j]->value; //入力のIDが持つ論理値を反転して代入
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						stack += value[j]; //入力の論理値を全て足す
					}
					//printf("ID:%lu,stack:%lu\n", Logic_level[i]->line_id,stack); //stackの値チェック
					if(stack == Logic_level[i]->n_in){
						Logic_level[i] -> value = 1;
					}
					else{
						Logic_level[i] -> value = 0;
					}
				}
				if(Logic_level[i]->type == 6){ //TYPEがNANDのとき
					Ulong stack = 0; //ゲート１つに対する全入力の論理値を加算
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						value[j] = Logic_level[i]->in[j]->value; //入力の論理値を全て格納
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						stack += value[j]; //入力の論理値を全て足す
					}
					//printf("ID:%lu,stack:%lu\n", Logic_level[i]->line_id,scack); //stackの値チェック
					if(stack == Logic_level[i]->n_in){
						Logic_level[i] -> value = 0;
					}
					else{
						Logic_level[i] -> value = 1;
					}
				}
				if(Logic_level[i]->type == 7){ //TYPEがORのとき
					Ulong stack = 0; //ゲート１つに対する全入力の論理値を加算
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						value[j] = Logic_level[i]->in[j]->value; //入力の論理値を全て格納
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						stack += value[j]; //入力の論理値を全て足す
					}
					//printf("ID:%lu,stack:%lu\n", Logic_level[i]->line_id,flag); //stackの値チェック
					if(stack >= 1){
						Logic_level[i] -> value = 1;
					}
					else{
						Logic_level[i] -> value = 0;
					}
				}
				if(Logic_level[i]->type == 8){ //TYPEがNORのとき
					Ulong stack = 0; //ゲート１つに対する全入力の論理値を加算
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						value[j] = Logic_level[i]->in[j]->value; //入力の論理値を全て格納
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						stack += value[j]; //入力の論理値を全て足す
					}
					//printf("ID:%lu,stack:%lu\n", Logic_level[i]->line_id,flag); //stackの値チェック
					if(stack >= 1){
						Logic_level[i] -> value = 0;
					}
					else{
						Logic_level[i] -> value = 1;
					}
				}
				if(Logic_level[i]->type == 9){ //TYPEがXORのとき
					Ulong stack = 0; //ゲート１つに対する全入力の論理値を加算
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						value[j] = Logic_level[i]->in[j]->value; //入力の論理値を全て格納
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						stack += value[j]; //入力の論理値を全て足す
					}
					//printf("ID:%lu,stack:%lu\n", Logic_level[i]->line_id,flag); //stackの値チェック
					if(stack % 2 == 1){ //1が奇数個の時に1
						Logic_level[i] -> value = 1;
					}
					else{ //1が偶数この時に0
						Logic_level[i] -> value = 0;
					}
				}
				if(Logic_level[i]->type == 10){ //TYPEがXNORのとき
					Ulong stack = 0; //ゲート１つに対する全入力の論理値を加算
					for (int j = 0; j < Logic_level[i]->n_in; j++){
						value[j] = Logic_level[i]->in[j]->value; //入力の論理値を全て格納
						//printf("ID%lu,in_ID:%lu,TYPE:%u,論理値:%lu\n",Logic_level[i]->line_id,Logic_level[i]->in[j]->line_id,Logic_level[i]->type,Logic_level[i]->in[j]->value); //入力のIDとその論理値をチェック
						stack += value[j]; //入力の論理値を全て足す
					}
					//printf("ID:%lu,stack:%lu\n", Logic_level[i]->line_id,flag); //stackの値チェック
					if(stack % 2 == 1){ //1が奇数個の時に0
						Logic_level[i] -> value = 0;
					}
					else{ //1が偶数この時に0
						Logic_level[i] -> value = 1;
					}
				}
			}
			printf("------演算結果%d------\n",count);
			for (int k = 0; k < Line_info.n_line;k++){
				line = &(Line_head[k]);
				printf("ID:%lu,入力レベル:%lu,TYPE:%u,論理値:%lu\n",line->line_id,line->level,line->type,line->value);
			}
  			bit = 0;
  			count++;
  		}
	}
}

