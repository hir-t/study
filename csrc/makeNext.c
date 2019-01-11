#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>
//void makeNext(LINE *[],int,int);
//makeNext(routeNode,loops,length);
void makeNext(LINE *routeNode2[],LINE *data[],LINE *start[],LINE *end[],int element){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;

	int N = 0;
	for (int i = 0; i < element; i++)
	{
		for (int j = 0; j < Line_info.n_line; j++)
		{
			if (end[N]->line_id == data[j]->line_id)
			{
				data[j]->next = start[N]->in[0];
				N++;
			}
			else if (data[j]->line_id == routeNode2[i]->line_id)
			{
				data[j]->next = routeNode2[i+1];
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