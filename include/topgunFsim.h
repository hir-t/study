/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 故障シミュレータに関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#ifndef TOPGUN_FSIM_H
#define TOPGUN_FSIM_H

/* TOPGUN Global Define */
#define BYTE 8          /* 8 bit = 1 byte */

/* line->fault_set_flag @ Fsim */
#define ALL_BIT_OFF     0x00000000

/* line->mp_flag @ Fsim */
#define FSIM_EVENT_OFF  0
#define FSIM_EVENT_ON   1


#define STATE_X_CODE 0x00
#define STATE_0_CODE 0x01
#define STATE_1_CODE 0x02
#define STATE_U_CODE 0x03


#define STATE_0 0
#define STATE_1 1

/* for 32bit 2値用論理値 */
#define PARA_V0_BIT 0x00000000
#define PARA_V1_BIT 0xffffffff

#define BIT0_ON     0x00000001

#define RAND_BIT    0x00100000 /* なるべく上位の方をとる */

#define FSIM_ALL_ON 0xffffffff


#define NO_PROP_FAULT 0

#define FSIM_VAL3   1 /* 1 : val3を使う, 1以外 : val3を使わない */
#define FSIM_S_COMP 1 /* 1 : staic compactionを使う, 1以外 : 使わない */

/*!
  @enum fsim_mode
  @brief 故障シミュレータのモード番号
*/
typedef enum fsim_mode {
	FSIM_NO_INFO = 0,  //!< 未定義(デフォルト動作)
	FSIM_PPSFP,        //!< パラレルパターンシングルフォールトプロパゲーション
	FSIM_SPPFP,        //!< シングルパターンパラレルフォールトプロパゲーション
	FSIM_SPSFP,        //!< シングルパターンシングルフォールトプロパゲーション
	FSIM_PPSFP_VAL3,   //!< シングルパターンシングルフォールトプロパゲーション
	FSIM_RANDOM,       //!< シングルパターンシングルフォールトプロパゲーション
	FSIM_OUTPUT,       //!< generate完了後残りパターンをシミュレーションする
	FSIM_STOCK         //!< 生成後静的圧縮をする
} FSIM_MODE;

/*!
  @struct fsim_head
  @brief 故障シミュレータヘッダ

  @note そのうち故障シミュレータ用の情報(info)と回路(head)に分割する
*/
typedef struct fsim_head
{
    Ulong 	bit_size;	      //!< bitサイズパラレルモード時の並列数となる。動作させるコンピュータのCPUアーキテクチャに依存する
	Ulong   detect_mask;      //!< パラレルモード時に規定の数に足りない場合の、マスクさせるための変数( global の必要がないかも)
} FSIM_HEAD;

/*!
  @struct event_list
  @brief 値を伝搬させるためのイベントに関する構造体
*/
typedef struct event_list {
	Ulong			n_event; //!< イベント数
    struct line		**event; //!< イベント(信号線へのポインタ)
} EVENT_LIST;

/*!
  @struct fsim_info
  @brief flight_atpgとのI/F用データ

  @note
  すべて故障シミュレータを動作させるための設定情報
  n_detectは個々に変わる恐れも少なく、累積数で判断するため、fsim_infoでなくfsim_headにあるべきかも
*/
typedef struct fsim_info {
	enum fsim_mode      mode;           //!< 故障シミュレータの動作モード
	struct cc_pattern   *cc_pat;        //!< 3値表現の組合せパターン(1個だけの場合)
    struct cc_pattern   **cc_pat_wodge; //!< 3値表現の組合せパターン(N個のパターン、パラレルパターンモード用)
	Ulong				n_cc_pat;       //!< 故障を検出したとする数(N detection用)
	Ulong				n_detect;       //!< 故障を検出したとする数(N detection用)
	Ulong				num_pat;        //!< 故障を検出したパタン数
	Ulong				num_waste_pat;  //!< 故障を検出しないパタン数(後で不要になった)
	Ulong				num_rand_pat;   //!< ランダムパタン故障シミュレーションのパタン数
	Ulong				fsim3;          //!< 3値を実行するか
	Ulong				static_comp;    //!< ポスト処理での静的圧縮を実行するか
} FSIM_INFO;


#endif
