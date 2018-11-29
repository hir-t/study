/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 圧縮バッファに関する設定

	@note
		@verbatim
		パターン内の状態の表現 2値
		0 ( = 0 )  1 ( = 1 )
		パターン内の状態の表現 3値
		X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )
		@endverbatim
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#define BIT_1_ON      0x00000001      //!< 1bit目だけ1である
#define COMP_BUF_SIZE 100             //!< 圧縮バッファのサイズ デフォルト値
//#define STOCK_SIZE    1000            //!< 貯蔵バッファのサイズ デフォルト値

/*!
  @{
  @name comp_info->resultに関する定義
*/
#define COMPACT_OK  0x00000000      /* compact enable */
#define COMPACT_NG  0xffffffff      /* compact conflict */
/*!
  @}
*/





/*!
  @{
  @name comp_info->comp_mode flagに関する定義
*/
#define COMP_NO_INFO 0x00000000      /* 初期状態 */
#define COMP_GET     0x00000001      /* 圧縮バッファから1つ取ってくる */
#define COMP_GET_ALL 0x00000002      /* 圧縮バッファからbit幅分取ってくる */
#define COMP_INSERT  0x00000004      /* 圧縮バッファにパターンを入れる */
#define COMP_FAST    0x00000008      /* 圧縮可能な箇所にすぐ入れる */
#define COMP_REST_X  0x00000010      /* 残りのXの数が最も多いようにする */
/*!
  @}
*/


/*!
  @enum comp_check_all_x
  @brief パターンがすべてXかをチェックする
*/
typedef enum comp_check_all_x {
	COMP_PAT_ALL_X = 0,     //!< すべてXである
	COMP_PAT_NOT_X = 1,     //!< すべてXではない
} COMP_CHECK_ALL_X;

/* 圧縮開始に関するcompact_on関数の返り値 */
typedef enum comp_on_check {
	COMP_ON_STOP = 0,      //!< しない
	COMP_ON_START = 1,     //!< 圧縮する
} COMP_ON_CHECK;

/*!
  @struct comp_head
  @brief 圧縮バッファに関する情報
*/
typedef struct comp_head
{
    Ulong               **buf_a;       //!< 圧縮バッファa
	Ulong               **buf_b;       //!< 圧縮バッファb 
	Ulong               *num_x_state;  //!< 圧縮バッファの中でXの数
    Ulong               flag;          //!< flag for atpg
	Ulong               width;         //!< 圧縮バッファの幅
	Ulong               buf_size;      //!< 圧縮バッファのサイズ
	Ulong               min_n_x_state; //!< 圧縮バッファの中でXの数の最小値
	Ulong               min_x_buf_id;  //!< 圧縮バッファの中で最小XをもつバッファのID
} COMP_HEAD;

/*!
  @struct cc_pattern
  @brief 圧縮バッファにおけるパターン
*/
typedef struct cc_pattern
{
    Ulong               *pattern_a;    //!< パターン a
	Ulong               *pattern_b;    //!< パターン b
} CC_PATTERN;

/*!
  @struct comp_info
  @brief  動的圧縮に関する情報
*/
typedef struct comp_info
{
	Ulong               mode;           //!< 動的圧縮の動作モード
	Ulong               result;         //!< 動的圧縮の結果
	Ulong               n_cc_pat;       //!< 並列パタン数
	struct cc_pattern   *cc_pat;        //!< 3値組合せパターン(1)
    struct cc_pattern   **cc_pat_wodge; //!< 3値組合せパターン(N)
} COMP_INFO;

