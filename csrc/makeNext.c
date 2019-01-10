#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
//void makeNext(LINE *[],int,int);
//makeNext(routeNode,loops,length);
void makeNext(LINE *routeNode[],int loops,int length,LINE *dbgate[]){
	//int num = 0;
	for (int i = 0; i < loops*length; i++)
	{
		LINE *now = routeNode[i];
/*		if (now->rtcnt > 1)
		{
			dbgate[num] = now;
			printf("---%d,dbID:%lu---\n", num,dbgate[num]->line_id);
			num++;
		}*/
		if (i % loops ==0) printf("\n");
		printf("nowID:%lu,type:%u,cnt:%lu\n",now->line_id,now->type,now->rtcnt);
		for (int j = 0; j < now->n_out; j++)
		{
			printf("now_out%lu,type%u\n", now->out[j]->line_id,now->out[j]->type);
			if(now->endflg == 1){
				//正確には違う。stのinにすべき
				now->next = now->st->in[0];
				now->next->next = now->st;
				printf("nextID:%lu,type:%u\n",now->next->line_id,now->next->type);
				break;
			}
			else if (now->out[j]->type == 2){
				LINE *now2 = now->out[j];
				for (int k = 0; k < now2->n_out; k++)
				{
					if (now2->out[k]->line_id == routeNode[i+1]->line_id)
					{
						now->next = now2;
						printf("nextID:%lu,type:%u\n",now->next->line_id,now->next->type);
						now2->next = routeNode[i+1];
						printf("2nextID:%lu,type:%u\n",now2->next->line_id,now2->next->type);
						break;
					}
				}
			}
			//else{
				else if (now->out[j]->line_id == routeNode[i+1]->line_id)
				{
					now->next = routeNode[i+1];
					printf("nextID:%lu,type:%u\n",now->next->line_id,now->next->type);
					break;
				}
			//}
		}
	}
/*	for (int i = 0; i < loops*length; i++)
	{
		printf("%d,nowID:%lu,nextID:%lu,nextTYPE:%u\n", i,routeNode[i]->line_id,routeNode[i]->next->line_id,routeNode[i]->next->type);
	}*/
}

/*
*************************************************
* *****************TYPE一覧********************* *
*												*
*    TOPGUN_PI   = 0,	//!< 外部入力				*
*    TOPGUN_PO   = 1,	//!< 外部出力				*
*    TOPGUN_BR   = 2,	//!< ファンアウトブランチ	*
*    TOPGUN_INV  = 3,	//!< インバータ			*
*    TOPGUN_BUF  = 4,	//!< バッファ				*
*    TOPGUN_AND  = 5,	//!< アンド				*
*    TOPGUN_NAND = 6,	//!< ナンド				*
*    TOPGUN_OR   = 7,	//!< オア				*
*    TOPGUN_NOR  = 8,	//!< ノア				*
*    TOPGUN_XOR  = 9,	//!< イクルーシブオア		*
*    TOPGUN_XNOR = 10,	//!< イクルーシブノア		*
*    TOPGUN_BLKI = 11,	//!< 不定入力				*
*    TOPGUN_BLKO = 12,	//!< 不定出力				*
*    TOPGUN_UNK  = 13,	//!< 未確定型				*
*    TOPGUN_NUM_PRIM,    //!< 型の数				*
*												*
* ********************************************* *
*************************************************
*/