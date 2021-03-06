/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief CELL形式に関する設定
	
	@note

	CELL形式はゲートレベルネットリストとATPG用ネットリストとの間をつなぐためのものである。ルールチェックの実施やパターン生成や故障シミュレータのために回路変換の元となる。
	回路分散時にはここから展開するのが正解ではないだろうか？

	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


/*!
  @{
  @name cellのハッシュテーブルに関する設定

  @note 名前の文字数の制限は解除すべき課題
*/
#define CELL_HASH 999991 			//!< cell_head用ハッシュ値
#define CELL_NAME_SIZE 1024         //!< 名前の文字数の最大値
/*!
  @}
*/

/*!
  @enum cell_flag
  @brief Cellの故障が定義されているか否かを示すフラグ
*/
typedef enum cell_flag
{
	Fault_off       = 0,           //!< 入力にも出力にも定義されていない
	Fault_out_on    = 1,           //!< 出力だけ定義されている
	Fault_in_on     = 2,           //!< 入力だけ定義されている
	Fault_in_out_on = 3,           //!< 入力にも出力にも定義されている
} CELL_FLAG;


/*!
  @enum cell_type
  @brief Cellフォーマット上のゲートタイプ
*/
typedef enum cell_type
{
	CELL_PI   = 0,
	CELL_PO,
	CELL_BR,
	CELL_INV,
	CELL_BUF,
	CELL_AND,
	CELL_NAND,
	CELL_OR,
	CELL_NOR,
	CELL_XOR,
	CELL_XNOR,
	CELL_SEL,
	CELL_FF,
	CELL_BLK,   /* 読み込み時 */
	CELL_BLKI,  /* 変換後 */
	CELL_BLKO,	/* 変換後 */
	CELL_UNK,
	CELL_NUM_PRIM
} CELL_TYPE;


/*!
  @struct cell_head
  セルレベルに対する情報をもつ
*/
typedef struct cell_head
{
    Ulong           n_id;		//!< セル数
    Ulong           n_pi;		//!< 外部入力数
    Ulong           n_po;		//!< 外部出力数
    Ulong           n_bbpi;		//!< 制御できない外部入力数 ( Black Box )
    Ulong           n_bbpo;		//!< 制御できない外部出力数 ( Black Box )
    struct cell     **cell;     /* cells */
} CELL_HEAD;

/*!
  @struct cell
  セルに関する情報
*/
typedef struct cell {
	char			*name;		//!< セル線名
	Ulong			id;			//!< セルID
	CELL_FLAG		flag;		//!<  flag
	CELL_TYPE		type;		//!< セルのタイプ
	Ulong			n_in;		//!< 入力数
	Ulong			n_out;		//!< 出力数
	struct cell_io	*in;		//!< 入力セルへのポインタ
	struct cell_io  *out;		//!< 出力セルへのポインタ
    struct cell	    *next;		//!< ハッシュ表の次ぎへのポインタ
} CELL;


/*!
  @struct cell_io
  セルの接続に関する情報

  @note
  入出力数の初期値がわからないので追加していく形式になっている
*/
typedef struct cell_io {
	struct cell    *cell;		/*	接続信号線		*/
	struct cell_io *next;
} CELL_IO;
