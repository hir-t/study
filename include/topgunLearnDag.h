/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 静的学習をDAGで行う関数に関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

typedef enum learn_state{
	LEARN_STATE_0 = 0, //!< 学習用STATE3_0 
	LEARN_STATE_1 = 1, //!< 学習用STATE3_1 
	N_LEARN_STATE = 2 //!< STATE3_0 と STATE3_1 を学習(正常値のみ)
} LEARN_STATE;

typedef enum dag_way{  //!< DAGを作る時の向き
	DAG_FORWARD   = 0, //!< 順方向
	DAG_BACKWARD  = 1  //!< 逆方向
} DAG_WAY;

typedef enum dag_parent{  //!< DAGを作る時の向き
	DAG_MADA   = 0, //!< 未確認
	DAG_SUMI   = 1  //!< 親は済んだ
} DAG_PARENT;


typedef enum learn_flag{  //!< DAGを作る時の向き
	LEARN_FLAG_NULL   = 0, 
	LEARN_FLAG_DIRECT = 1,  //1
	LEARN_FLAG_LOOP_S = 2,  //2 ループスタート
	LEARN_FLAG_LOOP_C = 4,  //3 ループチェック
	LEARN_FLAG_MAIN   = 8,  //4 ループのためのメインノード
	LEARN_FLAG_NO_USE = 16,  //5 ループのため削除
	LEARN_FLAG_NONE    = 32,  //6 フォワードノードのマージ
	LEARN_FLAG_BRANCH   = 64,   //7 バックワードノードのマージ
	LEARN_FLAG_PARENT   = 128,  //8 フォワードノードのマージ
	LEARN_FLAG_CHILDREN = 256,   //9 バックワードノードのマージ
	LEARN_FLAG_PARE_CHI = 384,  //10 フォワードノードのマージ
	LEARN_FLAG_PARENT_F = 512,   //11 バックワードノードのマージ
	LEARN_FLAG_CHILD_C  = 1024,  //12 childrenのマージ
	LEARN_FLAG_CONTRA   = 2048,  //13
	LEARN_FLAG_CONTRA_E = 4096,  //14
	LEARN_FLAG_LOOP_R   = 8192,   //15 loopの路をたどるときにたてる
	LEARN_FLAG_S_LIST   = 16384,  //16 l
	LEARN_FLAG_R_BRANCH = 32768,  //17 対の枝を削除する
	LEARN_FLAG_SORT     = 65536,   //18 toposort
	LEARN_FLAG_REACH    =131072,   //19 reachable
	LEARN_FLAG_ONLIST   =262144,   //20 on cullent_list
	LEARN_FLAG_NO_REACH =524288,   //21 reachable
	LEARN_FLAG_NO_REACH_NOT =1048576,   //22 reachable
	LEARN_FLAG_FINISH   =2097152,   //23 finish
	LEARN_FLAG_RESEARCH =4194304,   //24 re search
	LEARN_FLAG_DEPTH    =8388608,   //25 
	LEARN_FLAG_DEBUG    =16777216,   //debug
} LEARN_FLAG;


typedef enum learn_mode_flag{  //!< DAGを作る時の向き
	LEARN_MODE_SELECT  = 1,
	LEARN_MODE_REENTRY = 2,
	LEARN_MODE_RESORT  = 4, 
} LEARN_MODE_FLAG;


/*!
  @struct learn_node
  @brief dag用のノード
*/
typedef struct learn_node
{
    Ulong               node_id;	//!< ノードのID
    struct line          *line;		 //!< ノードの信号線のポインタ
    enum state_3         state3;     //!< ノードの信号線の状態を3値( 正常値 )を示す
	Ulong               level;	    //!< topological level sort順
	Ulong               imp_count;	//!< 含意操作した回数
	Ulong               flag;	    //!< ノードの到達非到達を示す
	Ulong              	n_forward;  //!< ノードが指している数
	Ulong              	n_for_org;  //!< ノードが指している数の元の値
	struct learn_node   *reverse;   //!< 対偶のノード
	struct learn_node   *prev;      //!< ノードの双方リスト
	struct learn_node   *next;      //!< ノードの双方リスト
	struct learn_branch *forward;   //!< 直接含意用のブランチ(順)
	struct learn_branch *backward;  //!< 直接含意用のブランチ(逆)
	struct learn_s_list *indirect;  //!< 間接含意用のブランチ
	struct learn_node   *parent;    //!< 親へのポインタ
	struct learn_s_list *children;  //!< 子へのポインタ
} LEARN_NODE;


/*!
  @struct learn_node
  @brief 等価ノード情報
*/
typedef struct learn_eq_head
{
	struct learn_eq_info *eq_info;   //!< 同値の信号線とStateのリスト

} LEARN_EQ_HEAD;

/*!
  @struct learn_branch
  @brief dag用のブランチ
*/
typedef struct learn_branch {
	Ulong               flag;   //!< 同値の見極め用
	Ulong               count;   //!< 同値の見極め用
	Ulong               loop;   //!< 同値の見極め用
	struct learn_node   *node;  //!< ノードへのポインタ
	struct learn_branch *next;  //!< 次のリストへのポインタ
} LEARN_BRANCH;

/*!
  @struct learn_s_list
  @brief nodeの単方向リストのリスト
*/
typedef struct learn_b_list {
	struct learn_branch  *b_list; //!< リストへのポインタ
	struct learn_b_list  *next;   //!< 次のリストへのポインタ
} LEARN_B_LIST;

/*!
  @struct learn_s_list
  @brief nodeの単方向リスト
*/
typedef struct learn_s_list {
	struct learn_node   *node;  //!< ノードへのポインタ
	struct learn_s_list *next;  //!< 次のリストへのポインタ
} LEARN_S_LIST;

/*!
  @struct learn_s_list
  @brief nodeの単方向リストのリスト
*/
typedef struct learn_s_list2 {
	struct learn_s_list  *s_list; //!< リストへのポインタ
	struct learn_s_list2 *next;   //!< 次のリストへのポインタ
} LEARN_S_LIST2;

/*!
  @struct learn_node_list
  @brief dag用の双方向リスト
*/
typedef struct learn_node_list {
	struct learn_node        *node;  //!< ノードへのポインタ
	struct learn_node_list   *prev;  //!< 次ぎのリストへのポインタ
	struct learn_node_list   *next;  //!< 次ぎのリストへのポインタ
} LEARN_NODE_LIST;

/*!
  @struct learn_branch
  @brief dag用のブランチ
*/
typedef struct learn_topo {
	Ulong                   level;        //!< レベル
	struct learn_node       *top_node;    //!< そのレベルの先頭ノードへのポインタ
	struct learn_topo       *next_level;  //!< 次ぎのレベルへのポインタ
} LEARN_TOPO;


typedef struct learn_dag_info {
	Ulong                   num_node;
	Ulong                   max_level;
	Ulong                   continue_flag;
	struct learn_s_list    *indirect_node_list;
} LEARN_DAG_INFO;

typedef struct learn_dag_return {
	Ulong                   n_add_direct_node;
	Ulong                   redo_flag;
	LEARN_MODE_FLAG         mode_flag;
	LEARN_S_LIST            *add_indirect_list;
} LEARN_DAG_RETURN;


typedef struct learn_flag_info {
	LEARN_NODE              **list;
	Ulong                   count;          /* 登録済みの数 ( = 0 なし ) */
} LEARN_FLAG_INFO;

typedef struct learn_search_info {
	LEARN_NODE              **list;
	Ulong                   enter_id;       /* 登録済みの数 ( = 0 なし ) */
	Ulong                   get_id;         /* 取得済みの数 ( = 0 なし ) */
	Ulong                   number_id;      /* 登録済みの数 ( = 0 なし ) */
} LEARN_SEARCH_INFO;

