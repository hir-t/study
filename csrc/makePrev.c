#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>
#include <global.h>
//void makeNext(LINE *[],int,int);
//makeNext(routeNode,loops,length);
void makePrev(LINE *routeNode[],int loops,int length,LINE *pr[]){
	int num = 0;
	for (int i = 1; i < loops*length; i++)
	{
		LINE *now = routeNode[i];
		LINE *pass = routeNode[i-1];
		if (i % loops ==0) printf("\n");
		//printf("passID:%lu,type:%u\n",pass->line_id,pass->type);
		//printf("nowID:%lu,type:%u\n",now->line_id,now->type);

		for (int j = 0; j < now->n_in; j++)
		{

			if(now->stflg == 1){
				if (now->rtcnt > 1)
				{
					now->prev = pr[num] = now->in[0];
					pr[num]->prev = now->in[0];
					num++;
				}
				else
				{
					now->prev = now->in[0];
					now->prev->prev = now->in[0];
				}
				//printf("prevID:%lu,type:%u\n",now->prev->line_id,now->prev->type);
				break;
			}

			else if(now->in[j]->type == 2)
			{
				LINE *now2 = now->in[j];
				for (int k = 0; k < now2->n_in; k++)
				{
					if (now2->in[k]->line_id == pass->line_id)
					{
						if (now->rtcnt > 1)
						{
							now->prev = pr[num] = now2;
							pr[num]->prev = pass;
							num++;
						}
						else
						{
							now->prev = now2;
							now2->prev = pass;
						}
						//printf("prevID:%lu,type:%u\n",now->prev->line_id,now->prev->type);
						//printf("2prevID:%lu,type:%u\n",now2->prev->line_id,now2->prev->type);
						break;
					}
				}
			}
			else{
				if(now->in[j]->line_id == pass->line_id)
				{
					if (now->rtcnt > 1){
						now->prev = pr[num] = pass;
						num++;
					}
					else now->prev = pass;

					//printf("prevID:%lu,type:%u\n",now->prev->line_id,now->prev->type);
					break;
				}
			}
		}
	}
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