/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief パターン生成(generate)にかかわる設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#ifndef TOPGUN_ATPG_H
#define TOPGUN_ATPG_H

/*!
  @{
  @name Gene_head.flagに関する定義
*/
#define ATPG_PROPA_E	0x00000001	//!< 1bit目 伝搬完了( ON ) 未完了( OFF )
#define ATPG_JUSTI_S	0x00000002	//!< 2bit目 正当化実施開始( ON ) 未開始( OFF ) 
#define	ATPG_UNT	    0x00000004	//!< 3bit目 テスト生成不能性あり ( ON ) なし ( OFF )
/*!
  @}
*/
/*!
  @{
  @name implication関数における状態の定義
*/
#define ALL_FLAG_ON   0x00000000    /* implication state check flag ( for uncontrol state ) */
#define ALL_FLAG_OFF  0x00000001    /* implication state check flag ( for uncontrol state ) */

#define ONE_FLAG_ON   0x00000000    /* implication state check flag ( for control state ) */
#define ONE_FLAG_OFF  0x00000001    /* implication state check flag ( for control state ) */

#define UNK_FLAG_ON   0x00000000    /* implication state check flag ( for unknown state ) */
#define UNK_FLAG_OFF  0x00000001    /* implication state check flag ( for unknown state ) */

#define OUT_PIN_NUM   0x00000000    /* 含意操作のpin_numにおける出力ピンの番号 */
#define IN0_PIN_NUM   0x00000001    /* 含意操作のpin_numにおける1番目の入力ピンの番号 */
#define IN1_PIN_NUM   0x00000002    /* 含意操作のpin_numにおける2番目の入力ピンの番号 */
/*!
  @}
*/

/*!
  @{
  @name btree.flagにおける状態の定義
*/
#define	ASS_N_0		0x00000001	   //!< 1bit 1:assign normal  0 実施ずみ, 0: 未実施
#define	ASS_N_1		0x00000002	   //!< 2bit 1:assign normal  1 実施ずみ, 0: 未実施
#define	ASS_F_0		0x00000004	   //!< 3bit 1:assign failure 0 実施ずみ, 0: 未実施
#define	ASS_F_1		0x00000008	   //!< 4bit 1:assign failure 1 実施ずみ, 0: 未実施 
#define	ASS_C_N		0x00000003	   //!< 1bit and 2bit normal  state complite
#define	ASS_C_F		0x0000000c	   //!< 3bit and 4bit failure state complite
/*!
  @}
*/




/* assign result */



/* drive_dfrontの帰り値(int) */

#define	DRIVE_CONT  0x00000020	// drive継続 
#define	DRIVE_END   0x00000040	// drive完了
#define	DRIVE_IMPOS 0x00000080	// drive不可能
#define	DRIVE_POINT 0x00000100	// driveする個所 

/* justifiedの帰り値(int) */
/* justified check result */
#define	JUSTI_END	0x00000200	/* justified end (topgun_justified) */
#define	JUSTI_KEEP  0x00000400	/* not justified end */
                                /* justified 継続 */
#define	JUSTI_CONF  0x00000800	/* justified 衝突 */
/* propagate check result */
#define	PROPA_END	0x00001000	/* propagate end (topgun_propagate) */
/* #define	PROPA_CONT	0x00002000	/\* not propagate end *\/ */

/* #define	LIM_BACK_END 0x00004000	/\* end for limit of back track *\/ */
#define	ASS_R_CONF	0x00000010	/* STAT_YYより大きな値である必要あり */


/* dfrontier flag */
#define	DF_NOCHECK	0x00000001	/* 1bit まだ未確認のルート */
#define	DF_ROOT		0x00000002	/* 2bit 現在確認中のルート */
#define	DF_DEADEND	0x00000004	/* 3bit 確認済(行き止まり) */


/* atpg_back_trackの返値(int) */

typedef enum gene_back {
	GENE_BACK_END   = 0,	/* back track 終了(whileのため0) */
	GENE_BACK_CONT  = 1		/* ATPG継続(whileのため1) */
} GENE_BACK;

/* for xor */
#define IMP_XOR		0x0001

/*!
  @enum atpg_check
  @brief atpg_check_endの返値(int)
*/
typedef enum atpg_check {
	ATPG_CONT  = 0,    //!< ATPG継続 
	ATPG_NOFLT = 1	   //!< 残り故障がないので終了
} ATPG_CHECK;

/*!
  @enum gene_result
  @brief generate関数中の結果のやり取りの設定
*/

typedef enum gene_result {
	NO_RESULT = 0,     //!< 未実施
	INSERT_FAIL,       //!< 故障挿入に失敗した
	INSERT_PASS,       //!< 故障挿入に成功した
	LIM_BACK_END,      //!< back trackの上限に達した
	PROPA_PASS,        //!< 故障伝搬に成功した
	PROPA_FAIL,        //!< 故障伝搬に失敗した
	PROPA_FAIL_DRIVE,  //!< 故障伝搬に失敗した(D-Drive or X-Path) 
	PROPA_FAIL_ENTER,  //!< 故障伝搬に失敗した(一意活生化)
	PROPA_FAIL_IMP,    //!< 故障伝搬に失敗した(含意操作)
	PROPA_CONT,        //!< 故障伝搬を継続中である
	JUSTI_PASS,        //!< 正当化に成功した
	JUSTI_CONT,        //!< 正当化を継続中である
	JUSTI_FAIL,        //!< 正当化に失敗した
	JUSTI_FAIL_ENTER,  //!< 正当化に失敗した(一意活生化) 
	JUSTI_FAIL_IMP,    //!< 正当化に失敗した(含意操作)
	GENE_CONT,         //!< パターン生成を継続中である
	GENE_END,          //!< パターン生成が完了した 
} GENE_RESULT;

/*!
  @enum condition
  @brief 状態 ( 正常値 or 故障値 )に関する値
*/
typedef enum condition {
	COND_UNKNOWN   = 0, //!< 未設定
	COND_FAILURE   = 1, //!< 故障状態( 故障値 )を扱う
	COND_NORMAL    = 4, //!< 正常状態( 正常値 )を扱う
	COND_REVERSE   = 5, //!< 今と反対の状態にする
	TOPGUN_NUM_COND,
} CONDITION;


/*!
  @enum imp_state
  @brief 含意操作の結果

  @note bitである必要がないはず
*/
typedef enum imp_state {
	IMP_KEEP = 0,	//!< 含意操作を値が保持された(更新されなかった)
	IMP_CONF = 1,	//!< 含意操作で値が衝突が発生した
	IMP_UPDA = 2,   //!< 含意操作で値が更新された
} IMP_STATE;

/*!
  @enum ass_result
  @brief 値割当ての結果

  @note bitである必要がないはず, IMP_STATEと共通化しても良い
*/
typedef enum ass_result {
	ASS_KEEP = 0,	/* keep     on assign */
	ASS_CONF = 1,	/* conflict on assign */
} ASS_RESULT;

/*!
  @enum just_result
  @brief 正当化の結果

  @note bitである必要がないはず
*/
typedef enum just_result {

	JUST_ERROR = 0,	 //!< all 0 どこにも必要ない(PIなど)
	JUST_NONE  = 1,	 //!< ERROR (1)
	JUST_OWN   = 2,	 //!< 現在のlineが正当化の必要あり
	JUST_ALL   = 4,	 //!< 入力信号線すべて正当化の必要あり
	JUST_1IN   = 8,	 //!< 入力のどれか一つに正当化が必要
	JUST_1IN_N = 16, //!< どのin[n]に正当化が必要を設定
} JUST_RESULT;

/*!
  @enum just_method
  @brief 次に正当化する場所を選択する基準

*/
typedef enum just_method {

	JUST_METHOD_LV_PI_HIGH = 0,	 //!<
	JUST_METHOD_LV_PI_LOW  = 1,	 //!<
	NUM_JUST_METHOD        	     //!< 
} JUST_METHOD;


/*!
  @enum f_class
  @brief ATPGによる故障の分類結果に関する値
*/
typedef enum f_class {
	NO_DEFINE = 0,     //!< 未実施
	GENE_NO_FAULT,     //!< 故障が選択できない 
	GENE_DET_A,        //!< ATPGで検出
	GENE_DET_S,        //!< 故障シミュレータで検出
	GENE_RED,          //!< 冗長故障
	GENE_UNT,          //!< テスト生成不能故障
	GENE_IUN,          //!< 初期化テスト生成不能故障
	GENE_ABT,          //!< ATPG打ち切り故障
} F_CLASS;

#define INDIRECT_IMP_LIST_SIZE 1000


/*!
  @struct gene_head
  @brief パターン生成に関する情報
*/
typedef struct gene_head
{
    Ulong               flag;            //!< パターン生成に関するフラグ
	Ulong               limit_back;      //!< バックトラック数の上限
	Ulong               back_all;        //!< 現在のバックトラック数
	Ulong               back_drive;      //!< 故障伝搬中のバックトラック数
	Ulong               back_drive_just; //!< 伝搬したパスが正当化で失敗した数
    struct line         *fault_line;     //!< 現在パターン生成の対象である故障
	struct line         *propa_line;     //!< 故障を伝搬させている信号線
    struct dfront       *dfront_t;       //!< Dフロンティアツリーの先頭ノード
    struct dfront       *dfront_c;	     //!< Dフロンティアツリーの現在処理しているノード
	Ulong               n_imp_info_list; //!< 確保した含意操作リストの数
	Ulong               n_enter_imp_list; //!< 含意操作リストの現在の登録数
    struct imp_info     *imp_info_list;  //!< 含意操作リスト
    struct btree		*btree;		     //!< バックトラックツリー
	Ulong               last_n;          //!< 当該で正常値を含意できたリストの終点
	Ulong               cnt_n;           //!< 当該で正常値を含意できたリストの回数
	Ulong               last_f;          //!< 当該で故障値を含意できたリストの終点
	Ulong               cnt_f;           //!< 当該で故障値を含意できたリストの回数
	struct line         **imp_list_n;
	struct line         **imp_list_f;
	struct line_stack   *cir_fix;        //!<* 回路構成による固定値の情報
} GENE_HEAD;


/*!
  @struct ass_list
  @brief 値割り当てに関する情報
*/
typedef struct ass_list
{
    struct line		    *line;           //!< 値を割り当てる信号線
	enum   state_3      ass_state3;      //!< 割り当てる値
	enum   condition    condition;       //!< 割り当てるのは信号線の正常値 or 故障値
    struct ass_list     *next;           //!< 次ぎへのポインタ(単方向リスト)
} ASS_LIST;

/*!
  @struct btree
  @brief バックトラックツリーのデータ構造

  @note 現在は正当化の部分だけ使用
*/
typedef struct btree
{
	Ulong               flag;		     //!< back track treeのflag 
	enum	condition   condition;       //!< 正常値 or 故障値 
	enum    state_3     ass_state3;      //!< 信号線に割り当てた値
    struct	line		*line;           //!< 値を割り当てる信号線
#ifndef OLD_IMP2
	Ulong               last_n;          //!< 当該で正常値を含意できたリストの終点
	Ulong               cnt_n;           //!< 当該で正常値を含意できたリストの回数
	Ulong               last_f;          //!< 当該で故障値を含意できたリストの終点
	Ulong               cnt_f;           //!< 当該で故障値を含意できたリストの回数
#else
    struct	imp_trace   *imp_trace;      //!< 当該で含意操作できた信号線と正or故
#endif /* OLD_IMP2 */
    struct	just_info	*just_e; 	     //!< 登録された正当化信号線 
    struct	just_info	*just_d; 	     //!< 削除された正当化信号線
	struct	just_info	*just_flag;      //!< JUSTIFY_NO, _FLの記録
    struct	btree		*next;		     //!< back track treeの子へのポインタ
    struct	btree		*prev;		     //!< back track treeの親へのポインタ
} BTREE;
	
/*!
  @struct dfront
  @brief Dフロンティアのデータ構造

  @note 回路の分岐にあわせてn分岐させている
  @note ass_listは存在する数によっては持ち方をかえた方が良い 
*/
typedef struct dfront
{
	Ulong               flag;		    //!< dfrontierのflag
	struct	line		*line;		    //!< FFRの先頭信号線へのポインタ
    struct	ass_list    *ass_list;	    //!< 割当た値と信号線 
#ifndef OLD_IMP2
	Ulong               last_n;          //!< 当該で正常値を含意できたリストの終点
	Ulong               cnt_n;           //!< 当該で正常値を含意できたリストの回数
	Ulong               last_f;          //!< 当該で故障値を含意できたリストの終点
	Ulong               cnt_f;           //!< 当該で故障値を含意できたリストの回数
#else
    struct	imp_trace   *imp_trace;      //!< 当該で含意操作できた信号線と正or故
#endif /* OLD_IMP2 */
	struct	dfront		*prev;		    //!< 親へのポインタ ルートはNULL
    struct	dfront		**next;		    //!< 子供へのポインタ(数はlineのn_out)
} DFRONT;

/*!
  @struct dfront_list
  @brief Dフロンティアのリスト構造
*/
typedef struct dfront_list			
{
    struct	dfront		*dfront;	    //!< Dforntierへのポインタ
    struct	dfront_list	*next;		    //!< 次ぎへのポインタ
} DFRONT_LIST;

/*!
  @struct imp_info
  @brief 含意操作の入力と結果

  @note  pin_numは表引きをするためにコード化されている
*/
typedef struct imp_info			    
{
    enum   state_3      new_state3;	   //!< 含意操作する原因の値
	enum   condition    condition;	   //!< 正常値 or 故障値
	Ulong	        	pin_num;       //!< どのpinか0(out),1(in0),2(in1),3, ... 
	Ulong	        	max_imp_id;    //!< 含意の起源となっている値割り当ての最大数
	struct line         *line;	       //!< 含意操作する信号線へのポインタ 
	struct imp_info     *next;	       //!< 次へのポインタ
} IMP_INFO;


/*!
  @struct just_info
  @brief 正当化が必要な信号線とその状態の情報 
*/
typedef struct just_info
{
	enum   condition    condition;	   //!< 正常値 or 故障値
	struct line         *line;	       //!< 正当化する信号線へのポインタ
	struct just_info    *next;	       //!< 次へのポインタ
} JUST_INFO;


/*!
  @struct imp_result_info
  @brief 含意操作による結果
*/
typedef struct imp_result_info
{
	enum   imp_state    i_state;       //!< 含意操作の実施結果
	enum   state_3      state3;       //!< 変化するstate ( X -> 0 or 1 or U )
	Ulong				pin_num;      //!< 変化するpin番号
} IMP_RESULT_INFO;

/*!
  @struct imp_result
  @brief 含意操作による結果
*/
typedef struct imp_result
{
	enum   imp_state    i_state;       //!< 含意操作の実施結果
	Ulong				imp_id;        //!< 衝突した含意操作のID
} IMP_RESULT;

/*!
  @struct imp_trace
  @brief 含意操作の記録

  @note  バックトラックや再初期化に用いるため、含意操作した箇所を記録する
*/
typedef struct imp_trace
{
	enum   condition    condition;   //!< 含意操作した状態 正常値(STAT_N) or 故障値(STAT_F) */
	struct line         *line;       //!< 含意操作した信号線へのポインタ
	struct imp_trace    *next;       //!< 次ぎへのポインタ
} IMP_TRACE;


/*!
  @struct gene_info
  @brief パターン生成に関する情報
*/
typedef struct gene_info
{
	enum   f_class      f_class;     //!< 故障の分類結果
	enum   gene_result  result;      //!< パターン生成実行結果
	struct flist        *fault;      //!< パターン生成対象故障へのポインタ
	struct cc_pattern   *cc_pat;     //!< 故障に対して生成したパターン(3値)
} GENE_INFO;


/*!
  @struct learn_list
  @brief 静的学習に関する情報
*/
typedef struct learn_list
{
    struct line		    *line;           //!< 値を割り当てる信号線
	enum   state_3      ass_state3;      //!< 割り当てる値
    struct learn_list   *next;           //!< 次ぎへのポインタ(単方向リスト)
} LEARN_LIST;

/*!
  @struct learn_list_list
  @brief 静的学習に関する情報
*/
typedef struct indirect_imp_info
{
	Ulong               list_size;       //!<
	Ulong               enter_id;        //!<
	Ulong               get_id;          //!<
    struct learn_list   **l_list;        //!< learn_listへのポインタ
	struct learn_list   *next;           //!< 次ぎへのポインタ(単方向リスト)
} INDIRECT_IMP_INFO;

/*!
  @enum learn_gate_type
  @brief 学習する/される信号線か
*/
typedef enum learn_type {
	LEARN_OFF = 0,     //!< 未実施
	LEARN_ON,          //!< 学習を実施する/実施される
} LEARN_TYPE;

/*!
  @enum back_track_result
  @brief バックトラックした結果を返す
*/
typedef struct back_result {
	enum   gene_back    result;
	Ulong               back_imp_id; //!< バックトラックした段数
} BACK_RESULT;

#endif
