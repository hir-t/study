/*ゲートのみの深さを数える*/
#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
//int M = 0;    //長さ(深さ)を数える
int entries;			//エントリポイントが複数あるか調べるために使う
int discovery;			//経路発見数を数える
int flag2;				//経路発見したら1にする

LINE *dfs(int entry,int N,int M,int loops,int length,LINE *start,LINE *route[loops][length]){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	//LINE *route[length];				//経路を保存
	entries = entry;
	LINE *now;
	now = start;
	flag2 = 0;
	//printf("nowID:%lu,nowTYPE:%u\n",now->line_id,now->type);

	//if(now->flag != 1 &&now->rtflg != 1){	//現在のゲートが未訪問で、他の経路でも使用されていなければ
	if(now->flag != 1){	//現在のゲートが未訪問
		now->flag = 1;
		if(2<now->type && now->type<11){
			route[N][M] = now;
			//int id = now -> line_id;
			//data[id] ->rtflg = 1;
			M++; //深さをカウント
		}
	}

	//printf("M:%d\n", M);
	if(M == length && entries > 0){				//経路が見つかったとき
		//printf("endID:%lu,endTYPE:%u\n",now->line_id,now->type);
		//printf("*******dfs has finished.********\n");
		discovery++;
		flag2 = 1;
		//printf("discovery:%d\n",discovery);
		//printf("entries:%d\n",entries);
		return now;
	}
	else{
		for(int a = 0; a < now->n_out; a++){
			now = now->out[a];
			//route[M] = now;
			//printf("nextID:%lu,nextTYPE:%u\n",now->line_id,now->type);
			if(now->n_in > 1 && 0 < M && M < length){ //深さ2以上でかつエントリポイントが複数
				//printf("OK\n");
				entries++;
				//printf("ID:%lu,entries:%d\n",now->line_id,entries);
			}
			if(now->flag != 1){
				now = dfs(entries,N,M,loops,length,now,route);
				return now;
			}
		}
	}
	//printf("******dfs hasn't finished.*******\n");
	//printf("discovery:%d\n",discovery);
	//printf("N:%d\n",N);
	entries = 0;
	return now;
}