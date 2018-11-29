/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 故障リストに関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#ifndef TOPGUN_FLIST_H
#define TOPGUN_FLIST_H

/*!
  故障の状態を示す ＠ FLIST->info
*/
#define TOPGUN_SA0	0x00000001	//!< stack-at 0
#define TOPGUN_SA1	0x00000002	//!< stack-at 1 (state only) 
#define TOPGUN_DCD	0x00000004	//!< 故障分類が未決定( OFF ) or 決定( ON )
#define TOPGUN_EXE	0x00000008	//!< ATPG未実施 ( OFF ) or ATPG実施ずみ( ON )
#define TOPGUN_DTA	0x00000010	//!< ATPGによって非検出 ( OFF ) or 検出故障( ON ) 16
#define TOPGUN_DTS	0x00000020	//!< Fsimによって非検出 ( OFF ) or 検出故障( ON ) 32
#define TOPGUN_RED	0x00000040	//!< テスト生成不能性あり ( OFF) or 回路冗長性あり ( ON ) 64
#define TOPGUN_IUT	0x00000080	//!< 初期化系列による影響なし ( OFF) or 影響あり ( ON ) 128
#define TOPGUN_ABT	0x00000100	//!< 打ち切り故障でない ( OFF ) or 打ち切り故障 ( ON ) 256
#define TOPGUN_DTX	0x00000200	//!< Fsim3(X入り)で検出可能 ( ON ) 512
#define TOPGUN_DTR	0x00000400	//!< ランダムパタンで検出可能 ( ON ) 1024

/*!
  シミュレータで選択できる故障
 */
#define SELECT_DEFAULT 0x00000000	//!< initial state
#define SELECT_ON_SA0  0x00000001	//!< stack-at 0
#define SELECT_ON_SA1  0x00000010	//!< stack-at 1
#define SELECT_ON_SA01 0x00000011	//!< stack-at 0 and 1
#define SELECT_OFF     0x00000100	//!< no selectable fault


/*!
  @struct flist
  故障に関する情報
*/
typedef struct flist
{
    Ulong           f_id;		//!< 故障ID
    Ulong           info;		//!< どの様な故障かの情報(sa0 or sa1 / detect or redun etc )
    Ulong           n_eqf;	    //!< 等価故障数
    Ulong           *eqf;	    //!< 等価故障へのポインタ
    struct line	    *line;	    //!< 故障が設定される信号線へのポインタ
    struct flist    *next;      //!< 同じ信号線へ設定する故障のリスト
} FLIST;

typedef enum flist_type
{
    FLIST_COMMENT,               //!< コメント行
    FLIST_ORG,                   //!< パタン生成すべき故障
    FLIST_CON,                   //!< 等価故障
    FLIST_END,                   //!< ファイルの終端
} FLIST_TYPE;

#endif
