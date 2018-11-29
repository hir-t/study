/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 信号線(Line)に関する設定

	@note
		topgunState.hが必要

	@author
		Masayoshi Yoshimura
	@date
		2006 / 02 / 10   initialize ( renew )
		2007 / 05 / 10   add implication ID for non-chronological backtrack
		2016 / 12 / 12   initialize (renew )
*/

#define TAC_LINE

#ifndef TOPGUN_STATE_H
#include "topgunState.h"
#endif //TOPGUN_STATE


/*!
  LINE->flag用のdefine
*/
#define LINE_SA0 	0x00000001	//!<  1bit stack-at 0
#define LINE_SA1 	0x00000002	//!<  2bit stack-at 1 */
#define LINE_CHK 	0x00000004	//!<  3bit function flag 1
#define LINE_FLT 	0x00000008	//!<  4bit fault flag
#define IMP_FRONT 	0x00000010	//!<  5bit front implication flag
#define IMP_BACK 	0x00000020	//!<  6bit back implication flag
#define JUSTIFY 	0x00000040	//!<  7bit justify flag
#define JUSTIFY_F 	0x00000080	//!<  8bit justify finish flag
#define JUSTIFY_NO 	0x00000100	//!<  9bit justify normal state
#define JUSTIFY_FL 	0x00000200	//!< 10bit justify failure state
#define BOUND_D 	0x00000400	//!< 11bit bound D tree
#define FP_PATH 	0x00000800	//!< 12bit Fault Propagation Path
#define FP_IAP      0x00001000	//!< 13bit Fault Propagate Potential(IAP)
#define FV_ASS      0x00002000	//!< 14bit Fault Values ASSignable

#define LF_UNIQ     0x00004000	//!< 15bit uniqsence flag

#define CIR_FIX     0x00008000	//!< 16bit with fix value for circuit structure

#define PO_FLAG     0x00010000	//!< 17bit output line

#define IMP_FRONT_C	0x00000018	//!<  4 & 5 bit enter implication check
#define IMP_BACK_C	0x00000028	//!<  4 & 6 bit enter implication check


/*!
  @struct line_info
  @brief 回路の信号線レベルに対する情報をもつ
*/
typedef struct line_info
{
    Ulong 	n_line;		//!< 信号線数
    Ulong 	n_pi;		//!< 外部入力数
    Ulong 	n_po;		//!< 外部出力数
    Ulong 	n_bbpi;		//!< 制御できない外部入力数 ( Black Box )
    Ulong 	n_bbpo;		//!< 制御できない外部出力数 ( Black Box )
    Ulong 	n_fault;	//!< 故障数
    Ulong 	n_fault_atpg; //!< ATPG対象故障数
    Ulong 	n_fault_atpg_count;	//!< 残りATPG対象故障数
    Ulong 	max_lv_pi;	//!< 最大入力段数
    Ulong 	max_lv_po;	//!< 最大入力段数
    Ulong 	*n_lv_pi;	//!< 各段数の信号線数
} LINE_INFO;


/*!
  @enum FSIM_FLT
  line->flist[]の添字として用いる
*/
typedef enum fsim_flt{
    FSIM_SA0 = 0,
    FSIM_SA1 = 1,
} FSIM_FLT;

/*!
  @enum LINE_TYPE
  line->typeで用いる信号線のゲート型
*/
typedef enum line_type
{
    TOPGUN_PI   = 0,	//!< 外部入力
    TOPGUN_PO   = 1,	//!< 外部出力
    TOPGUN_BR   = 2,	//!< ファンアウトブランチ
    TOPGUN_INV  = 3,	//!< インバータ
    TOPGUN_BUF  = 4,	//!< バッファ
    TOPGUN_AND  = 5,	//!< アンド
    TOPGUN_NAND = 6,	//!< ナンド
    TOPGUN_OR   = 7,	//!< オア
    TOPGUN_NOR  = 8,	//!< ノア
    TOPGUN_XOR  = 9,	//!< イクルーシブオア
    TOPGUN_XNOR = 10,	//!< イクルーシブノア
    TOPGUN_BLKI = 11,	//!< 不定入力
    TOPGUN_BLKO = 12,	//!< 不定出力
    TOPGUN_UNK  = 13,	//!< 未確定型
    TOPGUN_NUM_PRIM,    //!< 型の数
} LINE_TYPE;


/*!
  @struct line
  信号線に関する情報
*/
typedef struct line
{
    enum line_type	    type;		//!< 信号線のゲート型
    enum state_9        state9;		//!< 信号線の状態を9値( 正常値/故障値 )を示す
    Ulong               line_id;	//!< 信号線のID
    Ulong               n_in;		//!< 信号線の入力数
    Ulong               n_out;		//!< 信号線の出力数
    struct line         **in;       //!< 入力信号線へのポインタ
    struct line         **out;      //!< 出力信号線へのポインタ
    struct learn_list   *imp_0;     //!< 0に含意された時に一緒に含意されるもの
    struct learn_list   *imp_1;     //!< 1に含意された時に一緒に含意されるもの
    Ulong               flag;		//!< フラグ
    Ulong               lv_pi;      //!< 入力からの信号線段数
    Ulong               lv_po;      //!< 出力からの信号線段数
    Ulong               level;

    Ulong               imp_id_n;   //!< 正常値が含意された値割当て番号
    Ulong               imp_id_f;   //!< 故障値が含意された値割当て番号

    Ulong           	cnt_propa;	//!< 故障伝搬回数

    Ulong           	tm_co;		//!< テスタビリティメジャ 0可制御性
    Ulong           	tm_c0;		//!< テスタビリティメジャ 1可制御性
    Ulong           	tm_c1;      //!< テスタビリティメジャ 可観測性
    Ulong           	tm_bb_co;   //!< テスタビリティメジャ 故障挿入時の0可制御性
    Ulong           	tm_bb_c0;   //!< テスタビリティメジャ 故障挿入時の1可制御性
    Ulong           	tm_bb_c1;   //!< テスタビリティメジャ 故障挿入時の可観測性
    struct flist    	*flist[2];  //!< 故障リストへのポインタ

    Ulong           	fault_set_flag;
    Ulong           	fprop_flag;
    Ulong           	mp_flag;
    Ulong           	n_val_a;
    Ulong           	n_val_b; /* 3値用 */
    Ulong           	f_val_a;
    Ulong           	f_val_b; /* 3値用 */
    struct line         *event_line;

    Ulong             value; //シミュレート用論理値

    Ulong           	*out_info;	  //!< 自身からの到達可能外部出力(stemだけ)
    Ulong           	*reconv_info; //!< 接続してる枝が再収斂しているか(stemだけ)

    struct ffrg_node    *ffrg;

} LINE;

typedef struct logic_level{
  Ulong level;
  Ulong id;
  Ulong type;
  Ulong value;
}LOGIC_LEVEL;

/*!
  @struct line_stack
  lineをスタックする構造体
*/
typedef struct line_stack
{
    struct line   	    *line;  //!< 信号線へのポインタ
    struct line_stack  	*next;  //!< 次ぎへのポインタ
} LINE_STACK;

/*!
  @struct line_list
  @brief 信号線(line)で双方向リストを用いるための構造体
*/
typedef struct line_list
{
    struct line			*line;  //!< 信号線へのポインタ
    struct line_list	*prev;  //!< リスト前方へのポインタ
    struct line_list	*next;  //!< リスト後方へのポインタ
} LINE_LIST;

