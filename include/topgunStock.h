/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief stockバッファに関する設定
	
	@note
		パターン内の状態の表現 3値
		X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#define STOCK_SIZE    100            //!< 貯蔵バッファのサイズ デフォルト値

// STOCK_FAULT.no_pat_x特別な値
#define STOCK_F_N_DET     0xfffffffd   //!< Not Detect
#define STOCK_F_D_DET     0xfffffffe   //!< Double Detect
#define STOCK_F_END       0xffffffff   //!< Detect by One Deteted Patttern

// STOCK_PAT.flagの定義
#define SP_NO_DEFINE      0x00000000	/* 初期値 */
#define SP_ONE_DET        0x00000001	/* このパタンだけ検出できる故障がある */
#define SP_REDUN          0x00000002	/* このパタンがなくても検出できない故障はない */
#define SP_COMPLETE       0x00000004	/* 最終的な分類済み */

typedef enum stock_mode {
	STOCK_NO_INFO = 0,  //!< 未定義(デフォルト動作)
	STOCK_DDET,        //!< Double Detect Mode
	STOCK_ONE_DET,     //!< One Detect Pattern Simulation
	STOCK_REVERSE,     //!< Reverse Order Simulation by Redundant Pattern
} STOCK_MODE;


/*!
  @struct stock_pat_head
  @brief 貯蔵用パタン本体(stock_size分ある)
*/
typedef struct stock_pat
{
    Ulong               **buf_a;       //!< 圧縮バッファa
	Ulong               **buf_b;       //!< 圧縮バッファb
	Ulong               *flag;         //!< パタンごとのフラグ
	struct stock_pat    *next;         //!< 次のstock_patへのポインタ
} STOCK_PAT;

/*!
  @struct stock_pat_head
  @brief 貯蔵用パタンに関する情報
*/
typedef struct stock_pat_head
{
    struct stock_pat    *stock_pat;    //!< 貯蔵しているパタンへのポインタ
	Ulong               total_size;    //!< 現在のパタン数
	Ulong               stock_size;    //!< stock一つあたりのサイズ
	Ulong               width;         //!< パタンごとの幅
} STOCK_PAT_HEAD;

/*!
  @struct stock_pat_head
  @brief 貯蔵用パタンに関する情報
*/
typedef struct stock_info
{
    Ulong               *pat_order;    //!< Fsimを実施する順番
	Ulong               total_size;    //!< 全体のパタン数
	Ulong               n_fault;       //!< 対象となる故障数
	struct stock_fault  *stock_fault;  //!< 故障リストへのポインタ
	struct stock_fault  **all_fault;    //!< 故障リストへのポインタ
} STOCK_INFO;

/*!
  @struct stock_fault
  @brief 貯蔵用パタンの故障検出に関する情報
*/
typedef struct stock_fault
{
    struct line         *line;         //!< 対象故障がある信号線へのポインタ
	Ulong               no_pat_0;      //!< 検出したパタン番号
	Ulong               no_pat_1;      //!< 検出したパタン番号	
	Ulong               flag;          //!< 検出できる故障
	struct stock_fault  *next;         //!< 次へのポインタ
} STOCK_FAULT;
