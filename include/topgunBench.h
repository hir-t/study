/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief .bench I/Fに関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/*!
  @{
  @name .benchフォーマットに関するキーワード
*/
#define BENCH_STR_INPUT       "INPUT"    //!< input
#define BENCH_STR_OUTPUT      "OUTPUT"   //!< output
#define BENCH_STR_GATE_CONECT '='        //!< ゲートの接続
#define BENCH_STR_COMMENT     '#'        //!< コメント行
/*!
  @}
*/

/*!
  @{
  @name .benchフォーマットにおけるゲート名表記
 */
#define BENCH_GATE_AND        "AND"     //!< ANDゲート
#define BENCH_GATE_NAND       "NAND"    //!< NANDゲート
#define BENCH_GATE_OR         "OR"      //!< ORゲート
#define BENCH_GATE_NOR        "NOR"     //!< NORゲート
#define BENCH_GATE_NOT        "NOT"     //!< NOTゲート ( = INVゲート )
#define BENCH_GATE_BUF        "BUF"     //!< BUFFERゲート
#define BENCH_GATE_INV        "INV"     //!< INVゲート
#define BENCH_GATE_XOR        "XOR"     //!< EX-ORゲート
#define BENCH_GATE_XNOR       "XNOR"    //!< EX-NORゲート
/*!
  @}
*/

#define BENCH_NAME_SIZE 1024            //!< 1つの名前当りの最大文字数

/*!
  @enum BENCH_CELL_DEFINE
  @brief .bench 形式のcellの型
*/
typedef enum bench_cell_define{
	BENCH_CELL_INPUT = 0,       //!< input
	BENCH_CELL_OUTPUT,          //!< output
	BENCH_CELL_UNKNOWN,         //!< その他
} BENCH_CELL_DEFINE;

/*!
  @enum BENCH_ANALYZE_HEADER
  @brief .bench 形式のfileヘッダの型
*/
typedef enum bench_analyze_header{
	BENCH_COMMENT = 1,         //!< コメント行
	BENCH_END,                 //!< ファイルの終端
	BENCH_INPUT,               //!<  外部入力ピン
	BENCH_OUTPUT,              //!<  外部出力ピン
	BENCH_GATE,                //!<  ゲート
} BENCH_ANALYZE_HEADER;

/*!
  @enum BENCH_ANALYZE_LINE_END
  @breif .bench 形式の行末の判定
*/
typedef enum bench_analyze_line_end{
	BENCH_LINE_END = 1,        //!< 行末
	BENCH_LINE_CONTINUE,       //!< 行末でない
} BENCH_ANALYZE_LINE_END;

