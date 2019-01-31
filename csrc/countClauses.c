/* 節数を数える */
#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
//#include <string.h>

#include <topgun.h>
#include <topgunLine.h>

Ulong countClauses(void){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	LINE *line;
	int clauses = 0;	//節数を数える
						//以下のswich文で+nしていく
						//n:cnfに変換したときの"()"の数

	for (int i = 0; i < Line_info.n_line; i++)
	{
		line = &(Line_head[i]);

		if (line->n_in == 1)
		{
			switch(line->type)
			{
				//PO
				case 1:
					clauses = clauses+2;
					break;
				//Branch
				case 2:
					clauses = clauses+2;
					break;
				//INV
				case 3:
					clauses = clauses+2;
					break;
				//BUF
				case 4:
					clauses = clauses+2;
					break;
				default:
					//clauses = clauses+2;
					break;
			}
		}
		else if (line->n_in == 2)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+3;
					break;
				//NAND
				case 6:
					clauses = clauses+3;
					break;
				//OR
				case 7:
					clauses = clauses+3;
					break;
				//NOR
				case 8:
					clauses = clauses+3;
					break;
				//XOR
				case 9:
					clauses = clauses+4;
					break;
				//XNOR
				case 10:
					clauses = clauses+4;
					break;
				default:
					//clauses = clauses+3;
					break;
			}
		}
		else if (line->n_in == 3)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+4;
					break;
				//NAND
				case 6:
					clauses = clauses+4;
					break;
				//OR
				case 7:
					clauses = clauses+4;
					break;
				//NOR
				case 8:
					clauses = clauses+4;
					break;
				//XOR
				case 9:
					clauses = clauses+5;
					break;
				//XNOR
				case 10:
					clauses = clauses+5;
					break;
				default:
					//clauses = clauses+4;
					break;
			}
		}
		else if (line->n_in == 4)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+5;
					break;
				//NAND
				case 6:
					clauses = clauses+5;
					break;
				//OR
				case 7:
					clauses = clauses+5;
					break;
				//NOR
				case 8:
					clauses = clauses+5;
					break;
				//XOR
				case 9:
					clauses = clauses+6;
					break;
				//XNOR
				case 10:
					clauses = clauses+6;
					break;
				default:
					//clauses = clauses+5;
					break;
			}
		}
		else if (line->n_in == 5)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+6;
					break;
				//NAND
				case 6:
					clauses = clauses+6;
					break;
				//OR
				case 7:
					clauses = clauses+6;
					break;
				//NOR
				case 8:
					clauses = clauses+6;
					break;
				//XOR
				case 9:
					clauses = clauses+7;
					break;
				//XNOR
				case 10:
					clauses = clauses+7;
					break;
				default:
					//clauses = clauses+6;
					break;
			}
		}
		else if (line->n_in == 6)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+7;
					break;
				//NAND
				case 6:
					clauses = clauses+7;
					break;
				//OR
				case 7:
					clauses = clauses+7;
					break;
				//NOR
				case 8:
					clauses = clauses+7;
					break;
				//XOR
				case 9:
					clauses = clauses+8;
					break;
				//XNOR
				case 10:
					clauses = clauses+8;
					break;
				default:
					//clauses = clauses+7;
					break;
			}
		}
		else if (line->n_in == 7)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+8;
					break;
				//NAND
				case 6:
					clauses = clauses+8;
					break;
				//OR
				case 7:
					clauses = clauses+8;
					break;
				//NOR
				case 8:
					clauses = clauses+8;
					break;
				//XOR
				case 9:
					clauses = clauses+9;
					break;
				//XNOR
				case 10:
					clauses = clauses+9;
					break;
				default:
					//clauses = clauses+8;
					break;
			}
		}
		else if (line->n_in == 8)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+9;
					break;
				//NAND
				case 6:
					clauses = clauses+9;
					break;
				//OR
				case 7:
					clauses = clauses+9;
					break;
				//NOR
				case 8:
					clauses = clauses+9;
					break;
				//XOR
				case 9:
					clauses = clauses+10;
					break;
				//XNOR
				case 10:
					clauses = clauses+10;
					break;
				default:
					//clauses = clauses+9;
					break;
			}
		}
		else if (line->n_in == 9)
		{
			switch(line->type)
			{
				//AND
				case 5:
					clauses = clauses+10;
					break;
				//NAND
				case 6:
					clauses = clauses+10;
					break;
				//OR
				case 7:
					clauses = clauses+10;
					break;
				//NOR
				case 8:
					clauses = clauses+10;
					break;
				//XOR
				case 9:
					clauses = clauses+11;
					break;
				//XNOR
				case 10:
					clauses = clauses+11;
					break;
				default:
					//clauses = clauses+10;
					break;
			}
		}
	}
	return clauses;
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