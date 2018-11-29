/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 信号線(Line)の構造に関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

typedef enum str_type   // 信号線構造の型
{
	STR_UNDECIDE = 0,	//!< 外部入力
	STR_RECONV   = 1,	//!< 外部出力
	STR_TYPE_NUM,       //!< 型の数
} STR_TYPE;


