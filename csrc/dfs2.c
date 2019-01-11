/*ゲート+ブランチの深さを数える*/
#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
//int M = 0;    //長さ(深さ)を数える
int discovery;			//経路発見数を数える
int flag2;				//経路発見したら1にする

LINE *dfs2(int N,int M,int loops,int length,LINE *start,LINE *end,LINE *route2[loops][length*2]){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	LINE *now;
	now = start;
	flag2 = 0;
	//printf("nowID:%lu,nowTYPE:%u\n",now->line_id,now->type);

	//if(now->flag != 1 &&now->rtflg != 1){	//現在のゲートが未訪問で、他の経路でも使用されていなければ
	if(now->flag != 1){	//現在のゲートが未訪問
		now->flag = 1;
		route2[N][M] = now;
		//int id = now -> line_id;
		//data[id] ->rtflg = 1;
		M++; //深さをカウント
	}

	//printf("M:%d\n", M);
	if(now->line_id == end->line_id){				//経路が見つかったとき
		//printf("endID:%lu,endTYPE:%u\n",now->line_id,now->type);
		//printf("*******dfs has finished.%d********\n",N);
		discovery++;
		flag2 = 1;
		now->l_num = M;
		//printf("discovery:%d\n",discovery);
		//printf("entries:%d\n",entries);
		return now;
	}
	else{
		for(int a = 0; a < now->n_out; a++){
			now = now->out[a];
			//route[M] = now;
			//printf("nextID:%lu,nextTYPE:%u\n",now->line_id,now->type);

			if(now->flag != 1){
				now = dfs2(N,M,loops,length,now,end,route2);
				return now;
			}
		}
	}
	//printf("******dfs hasn't finished.*******\n");
	//printf("discovery:%d\n",discovery);
	//printf("N:%d\n",N);
	return now;
}