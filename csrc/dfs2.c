#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
//int M = 0;    //長さ(深さ)を数える

LINE *dfs2(int M,int loops,int length,LINE *start){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	//int length = 3; //作成するループの長さ
	//int loops = 1; //作成するループの数
	//int N = 0;	  //ループの数を数える
	//int num[loops]; //ループの初期ノードid
	//int nextID = 0; //次に訪れるゲートのid
	//int prevID = 0; //nextの前に訪れたゲートid

	LINE *now;
	//LINE *line;
	//LINE *path[loops][length];
	now = start;
	printf("nowID:%lu,nowTYPE:%u\n",now->line_id,now->type);
	if(now->flag != 1){
		now->flag = 1;
		if(2<now->type && now->type<11){
			M++; //深さをカウント
		}
	}

	printf("M:%d\n", M);
	if(M == length){
		return now;
	}
	else{
		for(int a = 0; a < now->n_out; a++){
			now = now->out[a];
			if(now->flag != 1){
				now = dfs2(M,loops,length,now);
			}
		}
	}


	return now;
}