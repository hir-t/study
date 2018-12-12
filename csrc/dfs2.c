#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
//int M = 0;    //長さ(深さ)を数える
int entries;
int discovery;

LINE *dfs2(int entry,int M,int loops,int length,LINE *start){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	entries = entry;
	LINE *now;
	now = start;

	printf("nowID:%lu,nowTYPE:%u\n",now->line_id,now->type);

	if(now->flag != 1){	//現在のゲートが未訪問なら
		now->flag = 1;
		if(2<now->type && now->type<11){
			M++; //深さをカウント
		}
	}

	//printf("M:%d\n", M);
	if(M == length && entries > 0){
		printf("endID:%lu,endTYPE:%u\n",now->line_id,now->type);
		printf("---dfs has finished.---\n");
		discovery = 1;
		return now;
	}
	else{
		for(int a = 0; a < now->n_out; a++){
			now = now->out[a];
			printf("nextID:%lu,nextTYPE:%u\n",now->line_id,now->type);
			if(now->n_in > 1){ //深さ2以上でかつエントリポイントが複数
				//printf("OK\n");
				entries++;
				printf("ID:%lu,entries:%d\n",now->line_id,entries);
			}
			if(now->flag != 1){
				now = dfs2(entries,M,loops,length,now);
				return now;
			}
		}
	}
	printf("dfs hasn't finished.\n");
	return now;
}