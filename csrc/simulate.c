#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
void simulate(){
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

	/*シミュレーション*/
	int Num_of_Input = 0; //入力数

	for (int i = 0; i < Line_info.n_line;i++){
		if(Logic_level[i]->type == 0){ //外部入力数を数える
			Num_of_Input++;
		}
	}
	//printf("外部入力数:%d\n",Num_of_Input);

	int input_num[Num_of_Input]; //入力用ビット列

	/* テストファイルを読み込む*/
	char in_fn[128]; //入力ファイル名を入れるchar型配列
	FILE *in_fp; //入力ファイル・ポインタ
	printf("Input test file name : ");
	scanf("%s",in_fn); //ファイル名をchar型配列fnへ入力

	in_fp = fopen(in_fn,"r"); //読み出しモードでファイルを開く
	if(in_fp == NULL){ //fopenに失敗した場合
		printf("The file %s is not exist.\n", in_fn); //失敗した旨を出力
		printf("Input test file name : ");
		scanf("%s",in_fn); //ファイル名をchar型配列fnへ入力
	}
	int bit;
	int testTimes = 0;
	int pattern;
	while((pattern=fgetc(in_fp)) != EOF){ //ファイルの終わりに達するまで1文字ずつ入力
		if (pattern=='\n'){ //入力された文字が改行コードのとき
			testTimes++; //行数をインクリメント
		}
	}

	int a = 0;
	while((fscanf(in_fp,"%d",&bit)) != EOF){ //ファイルの終わりに達するまで1文字ずつ入力
		if(a == Num_of_Input){
				break;
		}
		input_num[a] = bit;
		printf("bit = %d\n", bit);
		a++;
	}



	/*printf("%dビットの入力ビット列(0or1)を入力\n",Num_of_Input);
	for(int i = 0; i<Num_of_Input; i++){
		printf("%dビット目\n",i);
		scanf("%d",&input_num[i]);
		while(input_num[i]<0 || 1<input_num[i]){
			printf("ERROR:input number is under 0 or over 1.\n");
			printf("再入力:%dビット目\n",i);
			scanf("%d",&input_num[i]);
		}
	}

	//Ulong value;
	printf("-----入力ビット列-----\n");
	for(int i = 0; i<Num_of_Input; i++){
		printf("%d", input_num[i]);
	}
	printf("\n");*/

	Ulong value[line->n_in]; //AND,NAND演算の際使用

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
	printf("------演算結果------\n");
	for (int i = 0; i < Line_info.n_line;i++){
		line = &(Line_head[i]);
		printf("ID:%lu,入力レベル:%lu,TYPE:%u,論理値:%lu\n",line->line_id,line->level,line->type,line->value);
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

