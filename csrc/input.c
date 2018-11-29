#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>

void input(int input[32][5]){
	int count = 0;
	int line = 0;
	int data; //読み込んだ数字
	//int i = 0;
	//int input[32][5];

	printf("入力ファイル名:");
	scanf("%s",in_fn); //ファイル名をchar型配列fnへ入力

	in_fp = fopen(in_fn,"r"); //読み出しモードでファイルを開く

	if(in_fp == NULL){ //fopenに失敗した場合
		printf("ファイル「%s」が開けない.\n", in_fn); //失敗した旨を出力
		exit(1); //終了
	}

  	while((fscanf(in_fp,"%d",&data)) != EOF){ //ファイルの終わりに達するまで1文字ずつ入力
  		input[line][count] = data;
  		//printf("%d ", data);
  		count ++;
  		if(count == 5){
  			count = 0;
  			line++;
  		}
	}
}