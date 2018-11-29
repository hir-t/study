/*544872 田中寛生*/

#include <stdio.h> //printf,scanf, fopen,fgetc,fclose
#include <stdlib.h> //exit
#define MAX 128 //ファイル名の長さは128文字以下であると仮定

int main(void){
	char fn[MAX];
	FILE *fp;
	int stringCount = 0; //文字数をカウント
	int lineCount = 0; //行数をカウント

	printf("入力ファイル名:");
	scanf("%s",fn);

	fp = fopen(fn,"r");
	if(fp == NULL){
		printf("ファイル"%s"が開けない.\n", fn);
		exit(1);
	}

	while(fgetc(fp) != EOF){
		stringCount ++;
		if (fgetc(fp)=="\n"){
			lineCount++;
		}
	}

	fclose(fp);
	printf("%sの中身は%d文字,%d行.\n",stringCount,lineCount);
	exit(0);
}