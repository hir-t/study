/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief Fanout Free Regin Graphのためのヘッダ
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#define FFRG_MAX_EDGE_SIZE   100
#define FFRG_MAX_EDGE_SIZE_S 100

typedef enum ffrg_flag{  //!< DAGを作る時の向き
	FFRG_FLAG_NULL   = 0, 
	FFRG_FLAG_MULTI  = 1,  //1 for check multiple edges
} FFRG_FLAG;

/*!
  @struct ffrg
  @brief ffrgのヘッダ
*/
typedef struct ffrg
{
    Ulong               n_node;	    //!< ノードのID数
	struct ffrg_node    *node;      //!< ノードの実態
} FFRG;

/*!
  @struct ffrg_node
  @brief ffrg用のノード
*/
typedef struct ffrg_node
{
    Ulong               node_id;	//!< ノードのID
	Ulong              	n_edge;     //!< ノードが接続してるノード数
	Ulong              	flag;       //!< ノードのフラグ
    struct line         *line;		//!< ノードの信号線のポインタ
	struct ffrg_node    **edge;     //!< 接続関係を示す枝
} FFRG_NODE;

/*!
  @struct ffrg_node
  @brief ffrg用の枝
*/
typedef struct ffrg_edge_list {
	Ulong               n_edge; //エッジ数
	Ulong               n_size; //エッジリスト数
	Ulong               n_size_s; //search_list数
	struct line         **edge;  //!< エッジへのポインタ
	struct line         **search;  //!< エッジへのポインタ
} FFRG_EDGE_LIST;
