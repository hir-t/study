/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 静的学習をDAGで行う関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include<stdio.h>

#include "topgun.h"
#include "topgunState.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunLearnDag.h"
#include "topgunMemory.h"
#include "topgunError.h"
#include "topgunTime.h"
#include "topgunTest.h" 

static LEARN_NODE ***Node_head;
static LEARN_TOPO Topo_top;
static LEARN_DAG_INFO Learn_dag_info;
static LEARN_NODE **Loop_head;

static LEARN_FLAG_INFO Contra_info =
	{ NULL,
	  0
	};

static LEARN_FLAG_INFO Direct_info =
	{ NULL,
	  0
	};

static LEARN_SEARCH_INFO Search_info =
	{ NULL,
	  0,
	  0,
	  0	  
	};

static LEARN_SEARCH_INFO Search_add_info =
	{ NULL,
	  0,
	  0,
	  0	  
	};

extern LINE_INFO  Line_info;
extern LINE       ***Lv_pi_head;
extern GENE_HEAD  Gene_head;
extern  LINE      *Line_head;

extern void static_learn_dag ( Ulong );
extern void static_learn_dag_d ( void );
static void static_learn_dag_init ( void );
static LEARN_STATE static_learn_dag_state3 ( STATE_3 );
//static LEARN_STATE static_learn_dag_invert_state3 ( STATE_3 );
//static void static_learn_dag_make_direct ( void );
//static LEARN_S_LIST *static_learn_dag_make_list ( void );
//static void static_learn_dag_make_indirect ( LINE *, STATE_3 );
//static void static_learn_dag_enter_result ( LINE *, STATE_3 );
//static void topgun_print_mes_dag_result_indirect ( LINE *, STATE_3 );

static void static_learn_dag_init_allocate ( void );
static void static_learn_dag_init_dag_connect ( void );
static void static_learn_dag_make_direct_branch_input ( LINE *, STATE_3, DAG_WAY, STATE_3 );
static void static_learn_dag_make_direct_branch ( LEARN_NODE *, LEARN_NODE * );

static void static_learn_dag_topologicalsort( void );
static void static_learn_dag_direct ( Ulong );
static void static_learn_dag_direct_reentry( Ulong );
static LEARN_S_LIST *static_learn_dag_direct_node ( LEARN_NODE *, LEARN_DAG_RETURN * );
static void static_learn_dag_direct_node_new( LEARN_NODE *, LEARN_DAG_RETURN * );
static void static_learn_dag_up_flag( LEARN_NODE * );
void static_learn_dag_up_flag_old( LEARN_NODE * );

//static LEARN_NODE *static_learn_dag_direct_get_list( Ulong );
//static void static_learn_dag_direct_enter_list( LEARN_NODE * );
//static void static_learn_dag_direct_delete_list( void );

static void static_learn_dag_down_flag ( LEARN_NODE * );
void static_learn_dag_down_flag_old ( LEARN_NODE * );
static void static_learn_dag_print_topolotical_result( void );
//static void static_learn_dag_print_learn_state ( LEARN_STATE );
static void static_learn_dag_delete_topo_level_list ( LEARN_NODE * );
static void static_learn_dag_insert_topo_level_list ( LEARN_NODE * );
static LEARN_S_LIST *static_learn_dag_loop_check_search( LEARN_NODE *);
//void test_dag_topo_graph_flag_0 ( void );
static LEARN_S_LIST *static_learn_dag_make_loop_list( LEARN_NODE * );
static LEARN_S_LIST *static_learn_dag_node_merge ( LEARN_S_LIST *, LEARN_NODE *, Ulong );
static void static_learn_dag_node_merge_new( LEARN_S_LIST *, LEARN_NODE *, Ulong );
static void  test_dag_graph_flag_loop_s_c(void);
extern void topgun_print_mes_learn_list_result_dag ( LINE *, STATE_3 );
extern void static_learn_print_learn_result_dag ( void );
static LEARN_S_LIST *static_learn_dag_make_no_reachable_list( void );
static LEARN_NODE *static_learn_dag_search_max_level_node ( LEARN_S_LIST * );
static LEARN_NODE *static_learn_dag_search_top_node( LEARN_S_LIST * );
static void static_learn_dag_redo_all_topologicalsort ( void );
static LEARN_BRANCH *static_learn_dag_make_forward_list( LEARN_NODE * );
static LEARN_S_LIST *static_learn_dag_make_backward_list( LEARN_NODE * );
static LEARN_S_LIST *static_learn_dag_make_backward_list_parent( LEARN_NODE * );
static LEARN_NODE *static_learn_dag_search_max_level_node ( LEARN_S_LIST * );
static LEARN_TYPE static_learn_dag_check_contraposition( LEARN_NODE *, LEARN_NODE *);
LEARN_TYPE static_learn_dag_check_contraposition_old( LEARN_NODE *, LEARN_NODE *);
//static void static_learn_dag_contra_enter_list( LEARN_NODE *);
//static LEARN_NODE *static_learn_dag_contra_get_list( Ulong );
//static void static_learn_dag_contra_delete_list( void );

void static_learn_dag_check_contraposition_down_flag( LEARN_NODE * );
static LEARN_S_LIST *static_learn_dag_recheck_unreachable_list( LEARN_S_LIST * );
static void static_learn_dag_make_connect( LEARN_S_LIST *, LEARN_S_LIST * );
static LEARN_BRANCH *static_learn_dag_merge_b_list ( LEARN_B_LIST *, LEARN_NODE * );
static LEARN_S_LIST *static_learn_dag_merge_s_list( LEARN_S_LIST2 *, LEARN_NODE *  );
static Ulong static_learn_dag_branch_count( LEARN_BRANCH * );
static void static_learn_dag_branch_count_finish( LEARN_NODE * );
static void static_learn_dag_delete_reverse_branch ( LEARN_NODE * );
static void static_learn_dag_reup_flag( LEARN_NODE * );
static void static_learn_dag_direct_only_one_sort( Ulong );
static void static_learn_dag_direct_dynamic_sort( Ulong );
static void static_learn_dag_direct_dynamic_sort_reentry( Ulong );
static void static_learn_dag_direct_dynamic_resort( Ulong );
static void static_learn_dag_direct_dynamic_resort_new( Ulong );
static void static_learn_dag_make_fix_list( ASS_LIST * );
static void static_learn_dag_onflag_research( LEARN_S_LIST * );
static void static_learn_dag_onflag_research_new( void );
static void static_learn_dag_branch_count_finish_with_check( LEARN_NODE * );

static void static_learn_dag_indirect_select( LEARN_NODE * );
void static_learn_dag_indirect_select_to_all_node( void );

static void topgun_print_mes_node( LEARN_NODE * );
static void topgun_print_mes_dag_new_fix_line( LINE * );
static void topgun_print_mes_dag_illegal_node( LEARN_NODE * );
static void topgun_print_mes_dag_n_add_node( Ulong );
static void topgun_print_mes_dag_illegal_add_node( LEARN_S_LIST * );
static void topgun_print_mes_dag_hatena_node( LEARN_NODE * );
static void topgun_print_mes_dag_finish_node( LEARN_NODE * );
static void topgun_print_mes_dag_onlist_node( LEARN_NODE * );
static void topgun_print_mes_dag_pass_node( LEARN_NODE * );
static void topgun_print_mes_dag_exe_node( LEARN_NODE * );
static void topgun_print_mes_dag_select_node_add( LEARN_NODE *, Ulong );
static void topgun_print_mes_dag_select_node( LEARN_NODE * );
static void topgun_print_mes_reentry( LEARN_NODE * );
static void topgun_print_mes_reentry_org( LEARN_NODE * );
static void topgun_print_mes_reentry_child( LEARN_NODE * );
static void topgun_print_mes_reentry_from( LEARN_NODE * );
static void static_learn_dag_search_add_list_remalloc( void );

//未使用のためコメントアウトする
//static void static_learn_dag_levelize( void );
//static void static_learn_dag_research_node_character( void );

#ifdef LEARN_IMP_COUNT
#ifdef LEARN_OUTPUT
static void topgun_print_mes_dag_fix( void );
#endif /* LEARN_IMP_COUNT */
#endif /* LEARN_OUTPUT */

static void test_dag_node_learn_do ( Ulong );
static void test_dag_node_connect( void );
static void test_dag_node_n_forward();
static void topgun_print_mes_dag_select_node( LEARN_NODE * );
static void topgun_print_mes_dag_rest_check( void );
static void topgun_print_mes_dag_next_entry( LEARN_NODE * );
static void topgun_print_mes_dag_reentry( LEARN_NODE * );
static void topgun_print_mes_dag_reentry_next( LEARN_NODE * );
extern void test_dag_node_branch ( LEARN_NODE * );
extern void test_dag_node_2_node ( LEARN_NODE * );

//static void topgun_test_node_children ( LEARN_NODE *, char * );

/* for debug */
extern void topgun_print_mes_node_connect_search( LEARN_NODE *);
extern void topgun_print_mes_Topo_top( void );
extern void topgun_print_mes_dag_redo_sort( LEARN_NODE * );
extern void topgun_print_mes_already_search_node( LEARN_NODE * );
extern void topgun_print_mes_dag_iteration( Ulong );
extern void topgun_print_mes_search_node( LEARN_NODE *);
extern void topgun_print_mes_dont_search_node( LEARN_NODE * );

/* no use */
extern void static_learn_dag_reflesh( void );
extern void static_learn_dag_redo_topologicalsort ( LEARN_NODE *, LEARN_NODE * );
extern LEARN_TYPE static_learn_dag_check_family( LEARN_NODE *, LEARN_NODE * );
extern void test_dag_topo_list( void );


/* topgun_learn.c */
extern  LEARN_TYPE static_learn_line_type_check(LINE_TYPE );
extern  void static_learn_print_learn_result( void );
extern  void static_learn_line_count_all( ASS_LIST *,  STATE_3 );
extern  void static_learn_line_count_indirect( ASS_LIST *,  STATE_3 );
extern  LEARN_TYPE static_learn_equal_learn_list();

/* topgun_state.c */
extern IMP_RESULT gene_enter_state( ASS_LIST *); /* stateに値を設定する関数 */
extern void atpg_reflesh_state( LINE *, LEARN_STATE );
extern STATE_3 atpg_get_state9_2_normal_state3 ( STATE_9 ); /* 9値から正常値を取り出す */
extern STATE_3 atpg_invert_state3 ( STATE_3 );



/* topgun_implication.c */
extern IMP_RESULT implication ( void ) ; /* 含意操作実行関数 */

/* topgun_reflesh.c */
extern void atpg_imptrace_reflesh ( void );

/* topgun_uty.c */
extern void utility_renew_parameter_with_env( char *, Ulong *, Ulong );

/* topgun_test.c */
extern void test_all_state_xx ( void ) ;
extern void test_line_null ( LINE *, char * );
extern void test_line_in_null ( LINE *, char * );

/* topgun_print.c */
void    topgun_print_mes( int, int );
void    topgun_print_mes_learn_list_result ( LINE *, STATE_3 );
void    topgun_print_mes_sp( Ulong );
void    topgun_print_mes_char( char * );
void    topgun_print_mes_type( LINE * );
void    topgun_print_mes_state3( STATE_9 );
void    topgun_print_mes_id( LINE * );
void    topgun_print_mes_n( void );
void    topgun_print_mes_state9_n( STATE_9 );
void    topgun_print_mes_ulong_2( Ulong );
void    topgun_print_mes_state3_only( STATE_3 );
void    topgun_print_mes_compare_ulong ( Ulong, Ulong );
void    topgun_print_mes_learn_state3( LINE *, STATE_3, LINE *, STATE_3 );

static Ulong loop_count = 0;
static Ulong add_branch = 0;


/*!
  @brief DAGを用いた学習を実施する

  @param [in] void
  @return void

*/

void static_learn_dag
(
 Ulong iteration
 ){

	Ulong i;
	Ulong learn_sort;
	
	topgun_print_mes( 12, 0 );
	topgun_print_mes_n();
	
	{
		Ulong j;
		ASS_LIST ass_list;
		
		ass_list.condition = COND_NORMAL; /* 学習は正常値に対してするので */
		ass_list.next      = NULL;

		for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
			for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

				/* 静的学習の対象となる信号線 */
				ass_list.line = Lv_pi_head[ i ][ j ];

//				static_learn_line_count( &ass_list, STATE3_0 );
//				static_learn_line_count( &ass_list, STATE3_1 );
			}
		}
	}

	
	static_learn_dag_init();

	topgun_print_mes( 12, 1 );
	topgun_print_mes_n();

	utility_renew_parameter_with_env ( "TOPGUN_LEARN_SORT", &learn_sort, 0 );


	if ( ( learn_sort == 0 ) ||
		 ( learn_sort == 1 ) ) {
		static_learn_dag_topologicalsort();

		static_learn_dag_print_topolotical_result();
	}

	topgun_print_mes( 12, 2 );
	topgun_print_mes_n();

	switch ( learn_sort ) {
	case 0:
		//printf("mode 0\n");
		static_learn_dag_direct( iteration ); //!< 隣接以外の直接含意を求める
		break;
	case 1:
		//printf("mode 1\n");
		static_learn_dag_direct_only_one_sort( iteration ); //!< 隣接以外の直接含意を求める
		break;
	case 2:
		//printf("mode 2\n");
		static_learn_dag_direct_dynamic_sort( iteration ); //!< 隣接以外の直接含意を求める
		break;
	case 3:
		//printf("mode 3\n");
		static_learn_dag_direct_reentry( iteration ); //!< 隣接以外の直接含意を求める
		break;
	case 4:
		//printf("mode 4\n");
		static_learn_dag_direct_dynamic_sort_reentry ( iteration ); //!< 隣接以外の直接含意を求める
		break;
	case 5:
		//printf("mode 5\n");
		static_learn_dag_direct_dynamic_resort ( iteration ); //!< 隣接以外の直接含意を求める
		break;
	case 6:
		//printf("mode 6\n");
		static_learn_dag_direct_dynamic_resort_new ( iteration ); //!< 隣接以外の直接含意を求める
		//static_learn_dag_levelize( ); //!< できたグラフにレベルを付ける
		break;
	}
	//static_learn_dag_indirect_select_to_all_node();
	//static_learn_dag_research_node_character();
	//exit(0);
}


/*!
  @brief DAGの初期化

  @param [in] void
  @return void
*/

void static_learn_dag_init
(
 void
 ){

	topgun_4_utility_enter_start_time ( &Time_head.learn_init );	
	static_learn_dag_init_allocate();
	static_learn_dag_init_dag_connect();

	Topo_top.top_node   = NULL;
	Topo_top.level      = 0;
	Topo_top.next_level = NULL;
	
	Learn_dag_info.max_level = 0;
	Learn_dag_info.continue_flag = 0;
	topgun_4_utility_enter_end_time ( &Time_head.learn_init );
}

/*!
  @brief DAGのメモリ確保とポインタNULLをでの初期化

  @param [in] void
  @return void

*/

void static_learn_dag_init_allocate
(
 void
 ){

	LEARN_NODE *null_node = NULL;
	LEARN_NODE *new_node  = NULL;

	LEARN_TYPE learn_type;

	LINE       *line      = NULL;
	
	Ulong i;           //!< 信号線数のカウンタ
	Ulong node_id  = 0;  //!< node_idのカウンタ


	char *func_name = "static_learn_dag_init_allocate"; /* 関数名 */

	/* Node_head I/Fの初期化 */
	Node_head = ( LEARN_NODE *** )topgunMalloc
		( FMT_LEARN_NODE_PP, sizeof( LEARN_NODE ** ), N_LEARN_STATE, func_name ); //!< 0,1で"2個"

	Node_head[ LEARN_STATE_0 ] = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_P, sizeof( LEARN_NODE *), Line_info.n_line, func_name ); //!< NULLもあるけどまず全部とる
	Node_head[ LEARN_STATE_1 ] = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_P, sizeof( LEARN_NODE* ), Line_info.n_line, func_name ); //!< NULLもあるけどまず全部とる

	for ( i = 0; i < Line_info.n_line ; i++ ) {
		Node_head[ LEARN_STATE_0 ][ i ] = NULL;
		Node_head[ LEARN_STATE_1 ][ i ] = NULL;
	}


	/* 学習非対象用ノード */
	null_node = ( LEARN_NODE * )topgunMalloc
		( FMT_LEARN_NODE, sizeof( LEARN_NODE ), 1, func_name );

	null_node->node_id     = node_id++;
	null_node->line        = NULL;
	null_node->state3      = STATE3_C;
	null_node->level       = 0;
	null_node->imp_count   = 0;
	null_node->flag        = 0;
	null_node->n_forward   = 0;
	null_node->n_for_org   = 0;
	null_node->reverse     = NULL;
	null_node->prev        = NULL;
	null_node->next        = NULL;
	null_node->forward     = NULL;
	null_node->backward    = NULL;
	null_node->indirect    = NULL;
	null_node->parent      = NULL;
	null_node->children    = NULL;

	
	for ( i = 0; i < Line_info.n_line ; i++ ) {


		learn_type = static_learn_line_type_check ( Line_head[ i ].type );
		
		if ( LEARN_ON == learn_type ) {

			line =  &( Line_head[ i ] );

			//!< STATE_0に対して

			new_node = ( LEARN_NODE * )topgunMalloc
				( FMT_LEARN_NODE, sizeof( LEARN_NODE ), 1, func_name ); 
			
			new_node->node_id     = node_id++;
			new_node->line        = line;
			new_node->state3      = STATE3_0;
			new_node->level       = 0;
			new_node->imp_count   = 0;
			new_node->flag        = 0;
			new_node->n_forward   = 0;
			new_node->n_for_org   = 0;
			new_node->reverse     = NULL;
			new_node->prev        = NULL;
			new_node->next        = NULL;
			new_node->forward     = NULL;
			new_node->backward    = NULL;
			new_node->indirect    = NULL;
			new_node->parent      = NULL;
			new_node->children    = NULL;


			/* node_headへの接続 */
			Node_head[ LEARN_STATE_0 ][ i ] = new_node;


			new_node = ( LEARN_NODE * )topgunMalloc
				( FMT_LEARN_NODE, sizeof( LEARN_NODE ), 1, func_name ); 
			
			new_node->node_id    = node_id++;
			new_node->line        = line;
			new_node->state3      = STATE3_1;
			new_node->level       = 0;
			new_node->imp_count   = 0;
			new_node->flag       = 0;
			new_node->n_forward  = 0;
			new_node->n_for_org  = 0;
			new_node->reverse     = NULL;
			new_node->prev       = NULL;
			new_node->next       = NULL;
			new_node->forward    = NULL;
			new_node->backward   = NULL;
			new_node->indirect   = NULL;
			new_node->parent      = NULL;
			new_node->children    = NULL;
			
			/* node_headへの接続 */
			Node_head[ LEARN_STATE_1 ][ i ] = new_node;

		/* reverseの設定 */
			Node_head[ LEARN_STATE_0 ][ i ]->reverse = Node_head[ LEARN_STATE_1 ][ i ];
			Node_head[ LEARN_STATE_1 ][ i ]->reverse = Node_head[ LEARN_STATE_0 ][ i ];
		}
		else {
			/* 含意に無関係な信号線 */
			Node_head[ LEARN_STATE_0 ][ i ] = null_node;
			Node_head[ LEARN_STATE_1 ][ i ] = null_node;
		}
	}

	Learn_dag_info.num_node = node_id;


	Loop_head = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_P, sizeof( LEARN_NODE* ),
		  ( Learn_dag_info.num_node / 2 ), func_name ); //ループチェックでたどれるノードは、全ノード数の半分以下 */

	for ( i = 0 ; i < Learn_dag_info.num_node / 2 ; i++ ) {
		Loop_head[ i ] = NULL;
	}

	/* 学習用のノードが必要かどうかの判定用リスト */
	Contra_info.list = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_PP, sizeof( LEARN_NODE * ), ( Learn_dag_info.num_node / 2) , func_name ); //!< 0,1で"2個"

	for ( i = 0; i < Learn_dag_info.num_node / 2 ; i++ ) {
		Contra_info.list[ i ] = NULL;
	}
	
	/* Directノードかどうかの判定用リスト */
	Direct_info.list = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_PP, sizeof( LEARN_NODE * ), Learn_dag_info.num_node , func_name ); //!< 0,1で"2個"

	for ( i = 0; i < Learn_dag_info.num_node ; i++ ) {
		Direct_info.list[ i ] = NULL;
	}

	/* Searchするノードかどうかの判定用リスト */
	Search_info.list = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_PP, sizeof( LEARN_NODE * ), ( Learn_dag_info.num_node + 1 ), func_name ); //!< 0,1で"2個"

	for ( i = 0; i < ( Learn_dag_info.num_node + 1 ); i++ ) {
		Search_info.list[ i ] = NULL;
	}
	
	/* Searchするノードかどうかの判定用リスト */
	Search_add_info.list = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_PP, sizeof( LEARN_NODE * ), ( Learn_dag_info.num_node * 2 ), func_name ); //!< 0,1で"2個"

	for ( i = 0; i < ( Learn_dag_info.num_node * 2 ) ; i++ ) {
		Search_add_info.list[ i ] = NULL;
	}
	Search_add_info.number_id = ( Learn_dag_info.num_node * 2 ) ; 
}

/*!
  @brief DAGのn_forwadを再度設定する

  @param [in] void
  @return void

*/

void static_learn_dag_reflesh
(
 void
 ){

	Ulong i;           //!< 信号線数のカウンタ

	//char *func_name = "static_learn_dag_reflesh"; /* 関数名 */
	
	for ( i = 0; i < Line_info.n_line ; i++ ) {
		Node_head[ LEARN_STATE_0 ][ i ]->n_forward
			= Node_head[ LEARN_STATE_0 ][ i ]->n_for_org;
		Node_head[ LEARN_STATE_0 ][ i ]->imp_count   = 0;
		
		Node_head[ LEARN_STATE_1 ][ i ]->n_forward
			= Node_head[ LEARN_STATE_1 ][ i ]->n_for_org;
		Node_head[ LEARN_STATE_1 ][ i ]->imp_count   = 0;
	}
}

/*!
  @brief DAGの接続関係による初期化

  @param [in] void
  @return void

*/

void static_learn_dag_init_dag_connect
(
 void
 ){
	Ulong i; //!< 信号線数
	LINE  *line = NULL;
	
	char *func_name = "static_learn_dag_init_dag_connect"; /* 関数名 */

	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		line = &(Line_head[ i ]);

		test_line_null ( line, func_name );

		switch ( line->type ) {
		case TOPGUN_AND:
			/* line-0に対して */
			/* 0入力からforwardの枝をはる */
			/* 0入力へbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_0, DAG_BACKWARD, STATE3_0 );

			/* line-1に対して */
			/* 1入力へforwardの枝をはる */
			/* 1入力からbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_1, DAG_FORWARD, STATE3_1 );
			
			break;
		case TOPGUN_NAND:
			/* line-0に対して */
			/* 1入力へforwardの枝をはる */
			/* 1入力からbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_0, DAG_FORWARD, STATE3_1 );

			/* line-1に対して */
			/* 0入力からforwardの枝をはる */
			/* 0入力へbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_1, DAG_BACKWARD, STATE3_0 );

			break;
		case TOPGUN_OR:
			/* line-0に対して */
			/* 0入力へforwardの枝をはる */
			/* 0入力からbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_0, DAG_FORWARD, STATE3_0 );
			
			/* line-1に対して */
			/* 1入力からforwardの枝をはる */
			/* 1入力へbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_1, DAG_BACKWARD, STATE3_1 );
			break;

		case TOPGUN_NOR:
			/* line-0に対して */
			/* 1入力からforwardの枝をはる */
			/* 1入力へbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_0, DAG_BACKWARD, STATE3_1 );
			
			/* line-1に対して */
			/* 0入力へforwardの枝をはる */
			/* 0入力からbackwardの枝をはる */
			static_learn_dag_make_direct_branch_input( line, STATE3_1, DAG_FORWARD, STATE3_0 );
			break;			

		case TOPGUN_PI:
			/* そのまま */
			break;
		case TOPGUN_PO:
		case TOPGUN_BR:
		case TOPGUN_INV:
		case TOPGUN_BUF: 
		case TOPGUN_BLKI:
		case TOPGUN_BLKO:
			/* DAGに現れない */
			break;
		case TOPGUN_UNK:			
			/* なにもしない */
			break;
		case TOPGUN_XOR:
		case TOPGUN_XNOR:
		default:
			topgun_error( FEC_PRG_LINE_TYPE, func_name );
			break;
		}
	}
}


/*!
  @brief 到達可能か判定するリストへの登録

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void extern inline static_learn_dag_direct_enter_list
(
 LEARN_NODE *node
 ){

	LEARN_NODE **list  =  Direct_info.list;
	Ulong      count   =  Direct_info.count;
	LEARN_BRANCH *forward =  node->forward;
															
	while ( forward ) {

		list[ count++ ] = forward->node;

		forward = forward->next;
	}
	Direct_info.count = count;
}

void extern inline static_learn_dag_direct_enter_list_new
(
 LEARN_NODE *node
 ){

	Direct_info.list[ Direct_info.count ] = node;
	Direct_info.count++;
}


/*!
  @brief 到達可能か判定するリストのノードの読み出し

  @param [in] count  読み出すノードのリストの位置
  @return *node      読み出すノードのポインタ

*/

LEARN_NODE extern inline *static_learn_dag_direct_get_list
(
 Ulong count
 ){
	return ( Direct_info.list[ count ] );
}

/*!
  @brief 対偶が必要か否か判定するリストへの登録

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void extern inline static_learn_dag_search_enter_list
(
 LEARN_NODE *node
 ){
	/* 0は常にNULL */
	Search_info.enter_id++;
	Search_info.list[ Search_info.enter_id ] = node;
}

/*!
  @brief 到達可能か判定するリストのノードの読み出し

  @param [in] count  読み出すノードのリストの位置
  @return *node      読み出すノードのポインタ

*/

LEARN_NODE extern inline *static_learn_dag_search_get_list
(
 void
 ){

	LEARN_NODE *node = Search_info.list[ Search_info.enter_id];
	
	if (  Search_info.enter_id != 0 ) {
		Search_info.list[ Search_info.enter_id] = NULL; // 念のため
		
		Search_info.enter_id--;
	}
	return ( node );
}

/*!
  @brief 探索が必要か否か判定するリストへの初期化

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void static_learn_dag_search_delete_list
(
 void
 ){
	/*
	  Ulong i;

	for ( i = 1 ; i < ( Search_info.enter_id + 1 ) ; i++ ) {

		Search_info.list[ i ]->flag &= ~LEARN_FLAG_FINISH;
		Search_info.list[ i ]->flag &= ~LEARN_FLAG_ONLIST;
		Search_info.list[ i ]->n_forward = Search_info.list[ i ]->n_for_org;
		Search_info.list[ i ] = NULL;
	}
	*/
	Search_info.enter_id = 0;
	Search_info.get_id = 0;
}


/*!
  @brief 到達可能か判定するリストのノードの読み出し

  @param [in] count  読み出すノードのリストの位置
  @return *node      読み出すノードのポインタ

*/

LEARN_NODE extern inline *static_learn_dag_search_add_get_list
(
 void
 ){
	LEARN_NODE *node = Search_add_info.list[ Search_add_info.enter_id];

	Search_add_info.list[ Search_add_info.enter_id] = NULL; // 念のため

	if (  Search_add_info.enter_id != 0 ){
		Search_add_info.enter_id--;
		//printf("get_id %lu\n",Search_add_info.enter_id);
	}
	//exit(0);
	
	return ( node );
}

/*!
  @brief 追加探索が必要か否か判定するリストへの初期化

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void static_learn_dag_search_add_delete_list
(
 void
 ){
	Ulong i;

	for ( i = 0 ; i < Search_add_info.enter_id ; i++ ) {
		Search_add_info.list[ i ] = NULL;
	}
	Search_add_info.enter_id = 0;
}

/*!
  @brief 対偶が必要か否か判定するリストへの登録

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void extern inline static_learn_dag_search_add_enter_list
(
 LEARN_NODE *node
 ){
	Search_add_info.enter_id++;

	if ( Search_add_info.enter_id == Search_add_info.number_id ) {
		static_learn_dag_search_add_list_remalloc();
	}
	
	//printf("enter_id %lu %ld\n",Search_add_info.enter_id, 	Learn_dag_info.num_node );
	Search_add_info.list[ Search_add_info.enter_id ] = node;
}


/*!
  @brief 対偶が必要か否か判定するリストへの登録

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void static_learn_dag_search_add_list_remalloc
(
 void
 ){

	LEARN_NODE **new_node_list = NULL;
	Ulong       num_node_list =  Search_add_info.number_id * 2;
	Ulong       i;

	char *func_name = "static_learn_dag_search_add_list_remalloc"; /* 関数名 */

	printf("before %lu ->", Search_add_info.number_id );
	
	/* リストが足りなくなったので、追加する */
	new_node_list = ( LEARN_NODE ** )topgunMalloc
		( FMT_LEARN_NODE_PP, sizeof( LEARN_NODE * ), ( num_node_list ), func_name ); //!< 0,1で"2個"

	for ( i = 0; i < Search_add_info.number_id ; i++ ) {
		new_node_list[ i ] = Search_add_info.list[ i ]; 
	}
	for ( i = Search_add_info.number_id ; i < num_node_list ; i++ ) {
		new_node_list[ i ] = NULL;
	}

	topgunFree( Search_add_info.list, FMT_LEARN_S_LIST, Search_add_info.number_id, func_name );
	
	Search_add_info.list = new_node_list;
	Search_add_info.number_id = num_node_list;

	printf("after %lu\n", Search_add_info.number_id );
}



/*!
  @brief 対偶が必要か否か判定するリストへの登録

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void extern inline static_learn_dag_contra_enter_list
(
 LEARN_NODE *node
 ){

	LEARN_NODE **contra_list  =  Contra_info.list;
	Ulong      count          =  Contra_info.count;
	LEARN_BRANCH *forward     =  node->forward;
															
	while ( forward ) {

		contra_list[ count++ ] = forward->node;

		forward = forward->next;
	}
	Contra_info.count = count;
}

void extern inline static_learn_dag_contra_enter_list_new
(
 LEARN_NODE *node
 ){
	Contra_info.list[ Contra_info.count ] = node;
	Contra_info.count++;
}

/*!
  @brief 対偶が必要か否か判定するリストのノードの読み出し

  @param [in] count  読み出すノードのリストの位置
  @return *node      読み出すノードのポインタ

*/

LEARN_NODE extern inline *static_learn_dag_contra_get_list
(
 Ulong count
 ){
	return ( Contra_info.list[ count ] );
}

/*!
  @brief 対偶が必要か否か判定するリストのノードの読み出し

  @param [in] count  読み出すノードのリストの位置
  @return *node      読み出すノードのポインタ

*/

void extern inline static_learn_dag_contra_delete_list
(
 void
 ){
	LEARN_NODE **contra_list = Contra_info.list;
	Ulong      count         = Contra_info.count;
	Ulong      i             = 0;

	for ( i = 0 ; i < count ; i++ ) {

		contra_list[ i ]->flag &= ~LEARN_FLAG_CONTRA;

		/* 念のため */
		contra_list[ i ] = NULL;
	}

	Contra_info.count = 0;
}

/*!
  @brief 到達可能か判定するリストのノードの読み出し

  @param [in] count  読み出すノードのリストの位置
  @return *node      読み出すノードのポインタ

*/

void static_learn_dag_direct_delete_list
(
 void
 ){
	LEARN_NODE **list = Direct_info.list;
	Ulong      count  = Direct_info.count;
	Ulong      i      = 0;

	for ( i = 0 ; i < count ; i++ ) {

		/* 念のため */
		list[ i ] = NULL;
	}

	Direct_info.count = 0;
}


void static_learn_dag_make_direct_branch_input
(
 LINE *line,
 STATE_3 line_state3,
 DAG_WAY dag_way,
 STATE_3 in_state3
 ){
	Ulong i; //!< 入出力信号線数
	Ulong inv_cnt = 0; //!< 最大信号線数
	LINE  *in;
	LEARN_TYPE learn_type;
	LEARN_STATE line_state, in_state;
	LEARN_NODE *line_node, *in_node;
	STATE_3 enter_in_state3 = in_state3;
	

	char *func_name = "static_learn_dag_make_direct_branch_input"; /* 関数名 */
	
	test_line_in_null ( line , func_name );

	
	for ( i = 0 ; i < line->n_in ; i++ ) {

		in = line->in[ i ];
		inv_cnt = 0;
		learn_type = static_learn_line_type_check ( in->type );
		if ( in->type == TOPGUN_INV ) {
			inv_cnt++;
		}

		while ( learn_type == LEARN_OFF ) {
			/* bufとかbranchとかをDAGから除去する */
			
			if ( in->n_in == 0 ) {
				/* BLKIの場合 */
       				return;
			}
			
			in = in->in[ 0 ];
			learn_type = static_learn_line_type_check ( in->type );
			if ( in->type == TOPGUN_INV ) {
				inv_cnt++;
			}
		}

		if ( ( inv_cnt % 2 ) == 1 ) {
			/* invertが奇数個ある場合 */
			enter_in_state3 = atpg_invert_state3 ( in_state3 );
		}
		else {
			enter_in_state3 = in_state3;
		}
		
		line_state = static_learn_dag_state3 ( line_state3 );
		in_state   = static_learn_dag_state3 ( enter_in_state3 );
		line_node  = Node_head[ line_state ][ line->line_id ];
		in_node    = Node_head[ in_state   ][ in->line_id   ];

		
		if ( dag_way == DAG_FORWARD ) {
			static_learn_dag_make_direct_branch ( line_node, in_node );
		}
		else {
			static_learn_dag_make_direct_branch ( in_node, line_node );
		}
	}
}

void static_learn_dag_make_direct_branch
(
 LEARN_NODE   *from_node,
 LEARN_NODE   *to_node
 ){

	LEARN_BRANCH *new_branch;
	LEARN_NODE   *mini_id_node;
	LEARN_S_LIST *children;

	char *func_name = "static_learn_dag_make_direct_branch"; /* 関数名 */

	Learn_dag_info.continue_flag++;

	topgun_print_mes_char( "2-L-B " );

	mini_id_node = from_node;
	children = from_node->children;
	while ( children ) {

		if ( children->node->node_id > mini_id_node->node_id ) {
			mini_id_node = children->node;
		}
		children = children->next;
	}
	
	topgun_print_mes_ulong_2( mini_id_node->level );
	topgun_print_mes_type( mini_id_node->line );
	topgun_print_mes_id( mini_id_node->line );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_state3_only( mini_id_node->state3);
	topgun_print_mes_char( " -> " );


	mini_id_node = to_node;
	children = to_node->children;
	while ( children ) {

		if ( children->node->node_id > mini_id_node->node_id ) {
			mini_id_node = children->node;
		}
		children = children->next;
	}
	
	topgun_print_mes_ulong_2( mini_id_node->level );
	topgun_print_mes_type( mini_id_node->line );
	topgun_print_mes_id( mini_id_node->line );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_state3_only( mini_id_node->state3);
	topgun_print_mes_sp( 1 );
	topgun_print_mes_compare_ulong ( from_node->level, to_node->level );
	
	topgun_print_mes_n();
	
	/* 順方向をつける */
	new_branch = ( LEARN_BRANCH * )topgunMalloc
		( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );

	new_branch->node   = to_node;
	new_branch->flag   = 0;
	new_branch->next   = from_node->forward;
	from_node->forward = new_branch;

	new_branch->loop   = loop_count;
	new_branch->count  = add_branch++;
	
	from_node->n_forward++;
	from_node->n_for_org++;

	/* 逆方向 */
	new_branch = ( LEARN_BRANCH * )topgunMalloc
		( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );

	new_branch->node   = from_node;
	new_branch->flag   = 0;
	new_branch->next   = to_node->backward;
	to_node->backward  = new_branch;

	new_branch->loop   = loop_count;
	new_branch->count  = add_branch++;
	
}

void static_learn_dag_make_direct_branch_print
(
 LEARN_NODE   *from_node,
 LEARN_NODE   *to_node
 ){

	LEARN_NODE   *mini_id_node;
	LEARN_S_LIST *children;

	/* char *func_name = "static_learn_dag_make_direct_branch"; *//* 関数名 */

	Learn_dag_info.continue_flag++;

	topgun_print_mes_char( "2-L-B add " );

	mini_id_node = from_node;
	children = from_node->children;
	while ( children ) {

		if ( children->node->node_id > mini_id_node->node_id ) {
			mini_id_node = children->node;
		}
		children = children->next;
	}
	
	topgun_print_mes_ulong_2( mini_id_node->level );
	topgun_print_mes_type( mini_id_node->line );
	topgun_print_mes_id( mini_id_node->line );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_state3_only( mini_id_node->state3);
	topgun_print_mes_char( " -> " );


	mini_id_node = to_node;
	children = to_node->children;
	while ( children ) {

		if ( children->node->node_id > mini_id_node->node_id ) {
			mini_id_node = children->node;
		}
		children = children->next;
	}
	
	topgun_print_mes_ulong_2( mini_id_node->level );
	topgun_print_mes_type( mini_id_node->line );
	topgun_print_mes_id( mini_id_node->line );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_state3_only( mini_id_node->state3);
	topgun_print_mes_sp( 1 );
	topgun_print_mes_compare_ulong ( from_node->level, to_node->level );
	
	topgun_print_mes_n();
	
}

/*!
  @brief DAGのトポロジカルソート用データの初期化

  @param [in] void
  @return void

*/

void static_learn_dag_topologicalsort
(
 void
 ){

	Ulong i; //!< 信号線数
	Ulong current_level         = 0;
	LEARN_TOPO *current_topo    = NULL;
	LEARN_TOPO *learn_topo      = NULL; 
	LEARN_NODE *node            = NULL;
	LEARN_NODE *top_node        = NULL;
	LEARN_NODE *current_node    = NULL;
	LEARN_NODE *prev_level_node = NULL;
	
	LEARN_BRANCH *back          = NULL;

	LEARN_S_LIST *current_list  = NULL;
	LEARN_S_LIST *learn_s_list = NULL;
	LEARN_S_LIST *current_level_list = NULL;
	LEARN_NODE   *prev_node;
	Ulong mini_pi_level = FL_ULMAX;
	Ulong next_mini_level = FL_ULMAX;

	char *func_name = "static_learn_dag_topologicalsort"; /* 関数名 */

	//topgun_4_utility_enter_start_time ( &Time_head.learn_init_sort );	
	
	/* 一段目のリストを作成する */
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];

		if ( node->node_id != 0 ) { /* 対象外ということ */
			
			if ( node->n_forward == 0 ) {

				node->level = Topo_top.level;
				
				learn_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
				learn_s_list->node = node;
				learn_s_list->next = current_level_list;
				current_level_list = learn_s_list;

			}

			node = Node_head[ LEARN_STATE_1 ][ i ];
			if ( node->n_forward == 0 ) {

				node->level = Topo_top.level;

				/* 双方向リストの作成 */
				learn_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
				learn_s_list->node = node;
				learn_s_list->next = current_level_list;
				current_level_list = learn_s_list;

			}
		}
	}
	
	current_list = current_level_list;
	mini_pi_level = 0;
	next_mini_level = FL_ULMAX;
	prev_node = NULL;

	while ( 1 ) {
	
		while ( current_list ) {

			current_node = current_list->node;
			if ( ! ( current_node->flag & LEARN_FLAG_SORT ) ){
				if ( current_node->line->lv_pi == mini_pi_level ) {

					current_node->flag |= LEARN_FLAG_SORT;

					/* 双方向リストの作成 */
					current_node->prev = prev_node;
					current_node->next = NULL;
					if ( prev_node != NULL ) {
						prev_node->next = current_node;
					}
					else {
						Topo_top.top_node = current_node;
					}
					prev_node = current_node;
				}

				else {
					/* まだ未処理がある　*/
					if ( current_list->node->line->lv_pi < next_mini_level  ) {
						next_mini_level = current_list->node->line->lv_pi;
					}
				}
			}
			current_list = current_list->next;
		}

		/* mini_pi_levelが終了 */

		
		current_list = current_level_list;
		
		if ( next_mini_level == FL_ULMAX ) {
			/* 全部終了レベルが終了 */
			while ( current_list ) {
				current_list->node->flag &= ~LEARN_FLAG_SORT;
				learn_s_list = current_list;
				current_list = current_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
			current_level_list = NULL;
			break;
		}
		else {
			mini_pi_level = next_mini_level;
			next_mini_level = FL_ULMAX;
		}
	}
	
	current_topo = &( Topo_top );

	/* 2段目以降を作成する */
	while ( 1 ) {

		prev_level_node = current_topo->top_node;
		
		/* 次のレベル用を構築する */
		top_node      = NULL;
		current_level = current_topo->level +1;

		current_level_list = NULL;
		
		while ( 1 ) {
		
			back = prev_level_node->backward;

			while ( back ) {
				back->node->n_forward--;

				if ( back->node->n_forward == 0 ) {

					back->node->level = current_level;

					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
						
					learn_s_list->node = back->node;
					learn_s_list->next = current_level_list;
					current_level_list = learn_s_list;
				}
				back = back->next;
			}

			if ( ( prev_level_node->next == NULL ) ||
				 ( prev_level_node->level != prev_level_node->next->level ) ) {
				break;
			}
			else {
				prev_level_node = prev_level_node->next; 
			}
		}
		
		current_list = current_level_list;
		mini_pi_level = 0;
		next_mini_level = FL_ULMAX;
		top_node = NULL;
		
		while ( 1 ) {
	
			while ( current_list ) {
			
				current_node = current_list->node;
				if ( ! ( current_node->flag & LEARN_FLAG_SORT ) ){
					if ( current_node->line->lv_pi == mini_pi_level ) {

						current_node->flag |= LEARN_FLAG_SORT;

						/* 双方向リストの作成 */
						current_node->prev = prev_node;
						current_node->next = NULL;
						prev_node->next   = current_node;
						if ( top_node == NULL ) {
							top_node = current_node;
						}
						prev_node  = current_node;
					}
					else {
						/* まだ未処理がある　*/
						if ( current_list->node->line->lv_pi < next_mini_level  ) {
							next_mini_level = current_list->node->line->lv_pi;
						}
					}
				}
				current_list = current_list->next;
			}

			/* 今のPIレベルが終了 */
			current_list = current_level_list;
		
			if ( next_mini_level == FL_ULMAX ) {
				/* 全部PIレベルが終了 */
				while ( current_list ) {
					current_list->node->flag &= ~LEARN_FLAG_SORT;
					learn_s_list = current_list;
					current_list = current_list->next;
								topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
				}
				current_level_list = NULL;
				break;
			}
			else {
				mini_pi_level = next_mini_level;
				next_mini_level = FL_ULMAX;
			}
		}
		
		if ( top_node == NULL ) {
			/* 次のレベルがない == 終了 */
			/* 初期の時は有効 */
			break;
		}
		
		learn_topo = ( LEARN_TOPO * )topgunMalloc
			( FMT_LEARN_TOPO, sizeof( LEARN_TOPO ), 1, func_name );
		
		learn_topo->level        = current_level;
		learn_topo->top_node     = top_node;
		learn_topo->next_level   = NULL;
		current_topo->next_level = learn_topo;

		current_topo = current_topo->next_level;
	}
	Learn_dag_info.max_level = current_level; /* max_levelの初期化 */

	test_dag_node_connect();

	topgun_print_mes_Topo_top();

	//topgun_4_utility_enter_end_time ( &Time_head.learn_init_sort );
}

/*!
  @brief DAGの一度決めた処理順でずっと行う
  

  @param [in] void
  @return void

*/
void static_learn_dag_direct_only_one_sort
(
 Ulong iteration
 ){

	LEARN_NODE *current_node = NULL;

	LEARN_DAG_RETURN dag_return;
	LEARN_S_LIST *tmp;
	
	Ulong continue_flag = 0;
	
	dag_return.redo_flag = 0;
	dag_return.mode_flag = 0;

	/* 最初はレベル1から始める */
	current_node = Topo_top.top_node;
	
	/* すべての信号線に対して */
	while ( 1 ) {
		
		topgun_print_mes_dag_iteration( loop_count );
		topgun_print_mes_Topo_top();
		
		while ( current_node ){
			if ( current_node->imp_count == loop_count ) {

				/* 子は実施しない */
				if ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) {
					
					topgun_print_mes_search_node( current_node );
					current_node->imp_count++;
					dag_return.n_add_direct_node = 0;
					tmp = static_learn_dag_direct_node ( current_node, &dag_return );
				
					continue_flag += dag_return.n_add_direct_node;
				}
				else {
					topgun_print_mes_dont_search_node( current_node );
				}
			}
			else {
				topgun_print_mes_already_search_node( current_node );
			}

			current_node = current_node->next;
		}
		loop_count++;
		
		/* 終了条件の確認 */
		if ( continue_flag == 0 ) {
			break;
		}
		continue_flag = 0;

		if ( loop_count >= iteration ) {
			break;
		}
		
		/* levelの更新 */
		current_node = Topo_top.top_node;

	}
	topgun_print_mes_Topo_top();
	
	test_dag_node_learn_do ( loop_count );
	
	printf("2-L-E loop %ld\n",loop_count);
}

/*!
  @brief
  

  @param [in] void
  @return void

*/
void static_learn_dag_direct_dynamic_sort
(
 Ulong iteration
 ){

	LEARN_NODE *current_node = NULL;
	LEARN_NODE *back_node = NULL;

	LEARN_DAG_RETURN dag_return;
	
	Ulong continue_flag = 0;
	Ulong count_try_learn = 0;
	Ulong i;
	
	LEARN_S_LIST *current_list = NULL;
	LEARN_S_LIST *learn_s_list = NULL;

	LEARN_S_LIST *add_list = NULL;
	LEARN_S_LIST *tmp_add_list = NULL;
	LEARN_S_LIST *tmp;
	
	LEARN_BRANCH *backward = NULL;

	char *func_name = "static_learn_dag_direct_dynamic_sort"; /* 関数名 */
	
	dag_return.redo_flag = 0;
	dag_return.mode_flag = 1;
	dag_return.add_indirect_list = NULL;

	/* すべての信号線に対して */
	while ( 1 ) {
		
		//printf("2-L-3 loop %ld\n",loop_count);
		topgun_print_mes_dag_iteration( loop_count );
		
		/* まず一段目のリスト(current_list)を作成する */
		for ( i = 0 ; i < Line_info.n_line ; i++ ) {

			current_node = Node_head[ LEARN_STATE_0 ][ i ];
			
			if ( current_node->node_id != 0 ) { /* 対象外ということ */
			
				if ( (  current_node->n_for_org == 0 ) &&
					 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {

					//printf("add    stack node ID %ld\n",current_node->node_id);
					
					current_node->level = Topo_top.level;
				
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = current_node;
					learn_s_list->next = current_list;
					current_list = learn_s_list;
				}
				else {
					current_node->flag &= ~LEARN_FLAG_ONLIST;
					current_node->n_forward = current_node->n_for_org; 
				}
				current_node->flag &= ~LEARN_FLAG_FINISH;
					
				current_node = Node_head[ LEARN_STATE_1 ][ i ];
				if ( ( current_node->n_for_org == 0 ) &&
					 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {

					//printf("add    stack node ID %ld\n",current_node->node_id);
					
					current_node->level = Topo_top.level;
					
					/* 双方向リストの作成 */
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = current_node;
					learn_s_list->next = current_list;
					current_list = learn_s_list;
				}
				else {
					current_node->flag &= ~LEARN_FLAG_ONLIST;
					current_node->n_forward = current_node->n_for_org; 
				}
				current_node->flag &= ~LEARN_FLAG_FINISH;
			}
		}
	

		while ( current_list ) {

			if ( add_list == NULL ) {
				/* 最初はレベル1から始める */
				current_node = current_list->node;

				learn_s_list = current_list;
				current_list = current_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
			else {
				/* add_list(間接含意の枝の根元ですでに処理済みの場合)が存在する場合は先にする */
				current_node = add_list->node;
				learn_s_list = add_list;
				add_list     = add_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
		
			/* 選択しない条件 */
			/* その1 stackにつまれている間に枝を張られた場合 */
			/* その2 子供でない */
			if ( ( current_node->n_forward != 0 ) ||
				 ( current_node->flag & LEARN_FLAG_CHILDREN ) ) {

				//printf("delete stack node ID %ld\n",current_node->node_id);
				
				current_node->flag &= ~LEARN_FLAG_ONLIST;
				continue;
			}
			current_node->flag |= LEARN_FLAG_FINISH;

			//printf("select stack node ID %ld\n",current_node->node_id);
			
			topgun_print_mes_search_node( current_node );
			current_node->imp_count++;
			dag_return.n_add_direct_node = 0;
			tmp = static_learn_dag_direct_node ( current_node, &dag_return );
			count_try_learn++;
			continue_flag += dag_return.n_add_direct_node;

			if ( dag_return.add_indirect_list != NULL ) {

				/* learn_s_list を連結する */

				if ( add_list == NULL ) {
					add_list = dag_return.add_indirect_list;
				}
				else {
					tmp_add_list = add_list;
					while ( tmp_add_list->next ) {
						tmp_add_list = tmp_add_list->next;
					}
					tmp_add_list->next = dag_return.add_indirect_list;
				}
				dag_return.add_indirect_list = NULL;
			}

			backward = current_node->backward;
			while ( backward ) {

				back_node = backward->node;
				back_node->n_forward--;

				if ( ( back_node->n_forward == 0 ) &&
					 ( ! ( back_node->flag & LEARN_FLAG_ONLIST ) ) ) {

					back_node->flag |= LEARN_FLAG_ONLIST;

					//printf("add    stack node ID %ld\n",back_node->node_id);

					/* 双方向リストの作成 */
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = back_node;
					learn_s_list->next = current_list;
					current_list = learn_s_list;
				}
				backward = backward->next;
			}
		}

		loop_count++;
		if ( continue_flag == 0 ) {
			break;
		}
		continue_flag = 0;

		if ( loop_count >= iteration ) {
			break;
		}
		
	}
	topgun_print_mes_Topo_top();
	
	test_dag_node_learn_do ( loop_count );
	
	printf("2-L-E loop %ld\n",loop_count);
	printf("2-L-E try  %ld\n",count_try_learn);
}

/*!
  @brief DAGの一度決めた処理順でずっと行う
  

  @param [in] void
  @return void

*/
void static_learn_dag_direct_dynamic_sort_reentry
(
 Ulong iteration
 ){

	LEARN_NODE *current_node = NULL;
	LEARN_NODE *back_node = NULL;

	LEARN_DAG_RETURN dag_return;
	
	Ulong continue_flag = 0;
	Ulong count_try_learn = 0;
	Ulong total_learn = 0;
	Ulong i;
	
	LEARN_S_LIST *current_list = NULL;
	LEARN_S_LIST *learn_s_list = NULL;

	LEARN_S_LIST *add_list = NULL;
	LEARN_S_LIST *tmp_add_list = NULL;

	LEARN_S_LIST *search_add_list = NULL;
	
	LEARN_BRANCH *backward = NULL;

	char *func_name = "static_learn_dag_direct_dynamic_sort_reentry"; /* 関数名 */
	
	dag_return.redo_flag = 0;
	dag_return.mode_flag = 3;
	dag_return.add_indirect_list = NULL;

	/* すべての信号線に対して */
	while ( 1 ) {
		
		printf("2-L-3 loop %ld\n",loop_count);
		topgun_print_mes_dag_iteration( loop_count );
		
		/* まず一段目のリスト(current_list)を作成する */
		for ( i = 0 ; i < Line_info.n_line ; i++ ) {

			current_node = Node_head[ LEARN_STATE_0 ][ i ];

			if ( current_node->node_id != 0 ) { /* 対象外ということ */
			
				if ( (  current_node->n_for_org == 0 ) &&
					 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {

					//printf("add    stack node ID %ld\n",current_node->node_id);
					
					current_node->level = Topo_top.level;
				
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = current_node;
					learn_s_list->next = current_list;
					current_list = learn_s_list;
				}
				else {
					current_node->flag &= ~LEARN_FLAG_ONLIST;
					current_node->n_forward = current_node->n_for_org; 
				}
				current_node->flag &= ~LEARN_FLAG_FINISH;
					
				current_node = Node_head[ LEARN_STATE_1 ][ i ];
				if ( ( current_node->n_for_org == 0 ) &&
					 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {

					//printf("add    stack node ID %ld\n",current_node->node_id);
					
					current_node->level = Topo_top.level;
					
					/* 双方向リストの作成 */
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = current_node;
					learn_s_list->next = current_list;
					current_list = learn_s_list;
				}
				else {
					current_node->flag &= ~LEARN_FLAG_ONLIST;
					current_node->n_forward = current_node->n_for_org; 
				}
				current_node->flag &= ~LEARN_FLAG_FINISH;
			}
		}
	

		while ( current_list || add_list  ) {

			test_dag_node_n_forward ();

		
#ifdef ADD_FIRST
			/* add_listを先にする */
			if ( add_list == NULL ) {
				/* 最初はレベル1から始める */
				current_node = current_list->node;
			
				learn_s_list = current_list;
				current_list = current_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
			else {
				/* add_list(間接含意の枝の根元ですでに処理済みの場合)が存在する場合は先にする */
				current_node = add_list->node;
				
				learn_s_list = add_list;
				add_list     = add_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
#else
			/* add_listを後にする */
			if ( current_list == NULL ) {
				/* add_list(間接含意の枝の根元ですでに処理済みの場合)が存在する場合でcurrent_listがNULLの場合にする */
				current_node = add_list->node;
				
				learn_s_list = add_list;
				add_list     = add_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
			else {
				/* 最初はレベル1から始める */
				current_node = current_list->node;
			
				learn_s_list = current_list;
				current_list = current_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
#endif /* ADD_FIRST */			
			current_node->flag &= ~LEARN_FLAG_ONLIST;
			
			/* 選択しない条件 */
			/* その1 stackにつまれている間に枝を張られた場合 */
			/* その2 子供でない */
			if ( ( current_node->flag & LEARN_FLAG_CHILDREN ) ||
				 ( current_node->flag & LEARN_FLAG_FINISH ) ){

				//printf("delete stack node ID %ld\n",current_node->node_id);
				
				continue;
			}
			current_node->flag |= LEARN_FLAG_FINISH;

			topgun_print_mes_dag_select_node(current_node);
			
			topgun_print_mes_search_node( current_node );
			current_node->imp_count++;
			dag_return.n_add_direct_node = 0;
			
			search_add_list = static_learn_dag_direct_node ( current_node, &dag_return );

			topgun_print_mes_dag_select_node_add(current_node, dag_return.n_add_direct_node );
			
			count_try_learn++;
			continue_flag += dag_return.n_add_direct_node;

			if ( dag_return.add_indirect_list != NULL ) {

				/* learn_s_list を連結する */
				if ( add_list == NULL ) {
					add_list = dag_return.add_indirect_list;
				}
				else {
					tmp_add_list = add_list;
					while ( tmp_add_list->next ) {
						tmp_add_list = tmp_add_list->next;
					}
					tmp_add_list->next = dag_return.add_indirect_list;
				}
				dag_return.add_indirect_list = NULL;
			}

			backward = current_node->backward;
			while ( backward ) {
				back_node = backward->node;
				static_learn_dag_branch_count_finish( back_node );

				if ( ( back_node->n_forward == 0 ) &&
					 ( ! ( back_node->flag & LEARN_FLAG_ONLIST ) ) &&
					 ( ! ( back_node->flag & LEARN_FLAG_FINISH ) ) ){

					back_node->flag |= LEARN_FLAG_ONLIST;

					//printf("add    stack node ID %ld\n",back_node->node_id);

					/* 双方向リストの作成 */
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = back_node;
					learn_s_list->next = current_list;
					current_list = learn_s_list;

				}
				backward = backward->next;
			}

			/* マージによる到達不可能ノードの処理 */
			if ( search_add_list != NULL ) {
				if ( current_list == NULL ) {
					current_list = search_add_list;
				}
				else {
					static_learn_dag_make_connect ( current_list, search_add_list );
				}
			}

			topgun_print_mes_dag_next_entry( current_node );
		}

		topgun_print_mes_dag_rest_check();
		
		printf("2-L-3 current_try   %8ld\n",count_try_learn );
		printf("2-L-3 current_learn %8ld\n",(continue_flag/2));

		total_learn += ( continue_flag / 2 );
		loop_count++;
		if ( continue_flag == 0 ) {
			break;
		}
		else {
			printf("2-L-L add node %8ld\n", continue_flag );
		}
		continue_flag = 0;

		if ( loop_count >= iteration ) {
			break;
		}


	}
	topgun_print_mes_Topo_top();
	
	test_dag_node_learn_do ( loop_count );
	printf("2-L-E loop %ld\n",loop_count);
	printf("2-L-E try  %ld\n",count_try_learn);
	
	printf("SL %lu ( %lu / %lu )\n",total_learn/2,count_try_learn, loop_count);
	
}

/*!
  @brief DAGの一度決めた処理順でずっと行う
  

  @param [in] void
  @return void

*/
void static_learn_dag_direct_dynamic_resort_new
(
 Ulong iteration
 ){

	LEARN_NODE *current_node = NULL;
	LEARN_NODE *back_node = NULL;

	LEARN_DAG_RETURN dag_return;
	
	Ulong continue_flag = 0;
	Ulong continue_flag2 = 1;
	Ulong count_try_learn = 0;
	Ulong total_learn = 0;
	Ulong i;
	
	LEARN_S_LIST *current_list = NULL;

	LEARN_S_LIST *search_add_list = NULL;
	
	LEARN_BRANCH *backward = NULL;

	//char *func_name = "static_learn_dag_direct_dynamic_sort"; /* 関数名 */

	topgun_4_utility_enter_start_time ( &Time_head.learn_dynamic_resort );	

	/* dag_returnの初期化 */
	dag_return.redo_flag = 0;
	dag_return.mode_flag = 7;
	dag_return.add_indirect_list = NULL;

	/* とりあえず回数を調査することを主眼にしたコードを作成する(2006/7/13) */
	/* malloc/freeを減らす修正(2006/10/25) */

	/* まず一段目のリスト(current_list)を作成する */
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		current_node = Node_head[ LEARN_STATE_0 ][ i ];

		if ( current_node->node_id != 0 ) { /* 対象外ということ */
			
			if ( (  current_node->n_for_org == 0 ) &&
				 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
				
				//printf("add    stack node ID %ld\n",current_node->node_id);
 					
				current_node->level = Topo_top.level;
				
				static_learn_dag_search_enter_list ( current_node );
				
			}
			else {
				current_node->flag &= ~LEARN_FLAG_ONLIST;
				current_node->n_forward = current_node->n_for_org; 
			}
			current_node->flag &= ~LEARN_FLAG_FINISH;
			current_node->flag |= LEARN_FLAG_RESEARCH;

			
			
			current_node = Node_head[ LEARN_STATE_1 ][ i ];
			if ( ( current_node->n_for_org == 0 ) &&
				 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
				
				//printf("add    stack node ID %ld\n",current_node->node_id);
					
				current_node->level = Topo_top.level;

				static_learn_dag_search_enter_list ( current_node );
				
			}
			else {
				current_node->flag &= ~LEARN_FLAG_ONLIST;
				current_node->n_forward = current_node->n_for_org; 
			}
			current_node->flag &= ~LEARN_FLAG_FINISH;
			current_node->flag |= LEARN_FLAG_RESEARCH;
		}
	}
	
	while ( 1 ) { //iteration
		
		//printf("2-L-3 loop %ld\n",loop_count);
		topgun_print_mes_dag_iteration( loop_count );

		if ( continue_flag2 == 0 ) {

			/* 今は使っていない */
			/*
			printf("2-L-3 loop %ld+alpha\n",loop_count);
			for ( i = 0 ; i < Line_info.n_line ; i++ ) {

				current_node = Node_head[ LEARN_STATE_0 ][ i ];

				if ( current_node->node_id != 0 ) { // 対象外ということ
			
					if ( (  current_node->n_for_org == 0 ) &&
						 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
				
						current_node->level = Topo_top.level;
				
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				
						learn_s_list->node = current_node;
						learn_s_list->next = current_list;
						current_list = learn_s_list;
					}
					else {
						current_node->flag &= ~LEARN_FLAG_ONLIST;
						current_node->n_forward = current_node->n_for_org; 
					}
					current_node->flag &= ~LEARN_FLAG_FINISH;
					current_node->flag |= LEARN_FLAG_RESEARCH;
			
					current_node = Node_head[ LEARN_STATE_1 ][ i ];
					if ( ( current_node->n_for_org == 0 ) &&
						 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
						
					
						current_node->level = Topo_top.level;
						
						// 双方向リストの作成 
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
						learn_s_list->node = current_node;
						learn_s_list->next = current_list;
						current_list = learn_s_list;
					}
					else {
						current_node->flag &= ~LEARN_FLAG_ONLIST;
						current_node->n_forward = current_node->n_for_org; 
					}
					current_node->flag &= ~LEARN_FLAG_FINISH;
					current_node->flag |= LEARN_FLAG_RESEARCH;
				}
			}
			*/
		}
		else {
			if ( Search_info.enter_id == 0 ){

				/* loop2以降 */
			
				/* 少なくともadd_listのbackward側のFINISHを */
				/* 全部のFINISH FLAGを元に戻す必要がある */
				/* add_listはスタート地点 */

			
				/* まずadd_listからbackward側にLEARN_FLAG_RESEARCHをたてる */

				static_learn_dag_onflag_research_new( );
				static_learn_dag_search_add_delete_list();

				/*  */
			
				for ( i = 0 ; i < Line_info.n_line ; i++ ) {

					current_node = Node_head[ LEARN_STATE_0 ][ i ];

					if ( current_node->node_id != 0 ) { /* 対象外ということ */
			
						if ( (  current_node->n_for_org == 0 ) &&
							 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
							
							current_node->level = Topo_top.level;

							static_learn_dag_search_enter_list ( current_node );
				
						}
						else {
							current_node->flag &= ~LEARN_FLAG_ONLIST;
							current_node->n_forward = current_node->n_for_org; 
						}
						current_node->flag &= ~LEARN_FLAG_FINISH;
			
						current_node = Node_head[ LEARN_STATE_1 ][ i ];
						if ( ( current_node->n_for_org == 0 ) &&
							 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
					
							current_node->level = Topo_top.level;

							static_learn_dag_search_enter_list ( current_node );

						}
						else {
							current_node->flag &= ~LEARN_FLAG_ONLIST;
							current_node->n_forward = current_node->n_for_org; 
						}
						current_node->flag &= ~LEARN_FLAG_FINISH;
					}
				}
			}
		}


		/* 本番開始 */

		if ( Search_info.enter_id == 0 ) {
			/* 一つも登録できない場合 */
			break;
		}

		while ( 1 ) {  //登録がなくなるまで繰り返す

			test_dag_node_n_forward ();

			// add_listを後にする
			current_node = static_learn_dag_search_get_list();

			if ( current_node == NULL ) {
				// add_list(間接含意の枝の根元ですでに処理済みの場合)が存在する場合でcurrent_listがNULLの場合にする
				//current_node = static_learn_dag_search_add_get_list();
				// resortモードでも実質使っていない 2006/11/02
				break;
			}

			current_node->flag &= ~LEARN_FLAG_ONLIST;
			
			/* 選択しない条件 */
			/* その1 stackにつまれている間に枝を張られた場合 */
			/* その2 子供でない */
			if ( ( current_node->flag & LEARN_FLAG_CHILDREN ) ||
				 ( current_node->flag & LEARN_FLAG_FINISH ) ) {

				//printf("delete stack node ID %ld\n",current_node->node_id);

				continue;
			}

			
			current_node->flag |= LEARN_FLAG_FINISH;

			if ( current_node->flag & LEARN_FLAG_RESEARCH ) {

				topgun_print_mes_dag_select_node(current_node);
				topgun_print_mes_search_node( current_node );

				current_node->imp_count++;
				dag_return.n_add_direct_node = 0;
			
				/* 実際に探索する */
				static_learn_dag_direct_node_new ( current_node, &dag_return );
				/* add_listは中で登録する(2006/10/25) */

				topgun_print_mes_dag_select_node_add ( current_node, dag_return.n_add_direct_node );
				topgun_print_mes_dag_exe_node ( current_node );
				
				count_try_learn++;
				continue_flag += dag_return.n_add_direct_node;
				current_node->flag &= ~LEARN_FLAG_RESEARCH;


				/* add_listは中で登録する */
				/*
				if ( dag_return.add_indirect_list != NULL ) {

					// add_list を連結する
					if ( add_list == NULL ) {
						add_list = dag_return.add_indirect_list;
					}
					else {
						tmp_add_list = add_list;
						while ( tmp_add_list->next ) {
							tmp_add_list = tmp_add_list->next;
						}
						tmp_add_list->next = dag_return.add_indirect_list;
					}
					dag_return.add_indirect_list = NULL;
				}
				*/
			}
			else {
				//printf("2-L-R no  research node %8ld\n",current_node->node_id );
			}

			if ( dag_return.redo_flag == 10 ) {
				/* ループが発生して、最後まで探索をしていない場合 */

				current_node->flag &= ~LEARN_FLAG_FINISH;
				current_node->flag |= LEARN_FLAG_ONLIST;
				current_node->flag |= LEARN_FLAG_RESEARCH;

				/* 再度入力する */
				static_learn_dag_search_enter_list ( current_node );
				
			}
			else {
				backward = current_node->backward;
				while ( backward ) {
					back_node = backward->node;
					
					static_learn_dag_branch_count_finish_with_check( back_node );

					if ( ( back_node->n_forward == 0 ) &&
						 ( ! ( back_node->flag & LEARN_FLAG_ONLIST ) ) &&
						 ( ! ( back_node->flag & LEARN_FLAG_FINISH ) ) ){

						back_node->flag |= LEARN_FLAG_ONLIST;
					
						//printf("2-L-R put onlist   node %8ld\n",back_node->node_id );
						//printf("add    stack node ID %ld\n",back_node->node_id);

						/* 双方向リストの作成 */

						static_learn_dag_search_enter_list ( back_node );

					}
					else {
						if ( back_node->n_forward != 0 ) {
							topgun_print_mes_dag_pass_node( back_node );
						}
						else if ( back_node->flag & LEARN_FLAG_ONLIST ) {
							topgun_print_mes_dag_onlist_node( back_node );
						}
						else if ( back_node->flag & LEARN_FLAG_FINISH ) {
							topgun_print_mes_dag_finish_node( back_node );
						}
						else {
							topgun_print_mes_dag_hatena_node( back_node );
						}
					}
					backward = backward->next;
				}
			}

			/* マージによる到達不可能ノードの処理 */
			topgun_print_mes_dag_illegal_add_node( search_add_list );
			
			if ( search_add_list != NULL ) {
				if ( current_list == NULL ) {
					current_list = search_add_list;
				}
				else {
					static_learn_dag_make_connect ( current_list, search_add_list );
				}
				search_add_list = NULL;
			}

			topgun_print_mes_dag_next_entry( current_node );
		} //n回目の探索が終了

		topgun_print_mes_dag_rest_check();

		//printf("2-L-3 current_try   %8ld\n",count_try_learn );
		//printf("2-L-3 current_learn %8ld\n",(continue_flag/2));

		total_learn += ( continue_flag / 2 );
		
		loop_count++;
		if ( continue_flag == 0 ) {

			if ( continue_flag2 == 0 ) {
				break;
			}
			else {
				break;
				continue_flag2 = 0;
			}
		}
		else {
			continue_flag2 = 1;
			topgun_print_mes_dag_n_add_node	( continue_flag );
		}
		continue_flag = 0;

		if ( loop_count >= iteration ) {
			break;
		}

		/* 次の探索のため */
		static_learn_dag_search_delete_list();
		
	} // iterationによる判断

	
	topgun_print_mes_Topo_top();
	
	test_dag_node_learn_do ( loop_count );
	
	printf("SL %lu ( %lu / %lu )\n",total_learn/2,count_try_learn, loop_count);

	topgun_4_utility_enter_end_time ( &Time_head.learn_dynamic_resort );	
}

/*!
  @brief DAGの一度決めた処理順でずっと行う
  

  @param [in] void
  @return void

*/
void static_learn_dag_direct_dynamic_resort
(
 Ulong iteration
 ){

	LEARN_NODE *current_node = NULL;
	LEARN_NODE *back_node = NULL;

	LEARN_DAG_RETURN dag_return;
	
	Ulong continue_flag = 0;
	Ulong continue_flag2 = 1;
	Ulong count_try_learn = 0;
	Ulong total_learn = 0;
	Ulong i;
	
	LEARN_S_LIST *current_list = NULL;
	LEARN_S_LIST *learn_s_list = NULL;

	LEARN_S_LIST *add_list = NULL;
	LEARN_S_LIST *tmp_add_list = NULL;

	LEARN_S_LIST *search_add_list = NULL;
	
	LEARN_BRANCH *backward = NULL;

	char *func_name = "static_learn_dag_direct_dynamic_sort"; /* 関数名 */

	topgun_4_utility_enter_start_time ( &Time_head.learn_dynamic_resort );	
	
	dag_return.redo_flag = 0;
	dag_return.mode_flag = 7;
	dag_return.add_indirect_list = NULL;

	/* とりあえず回数を調査することを主眼にしたコードを作成する(2006/7/13) */

	/* まず一段目のリスト(current_list)を作成する */
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		current_node = Node_head[ LEARN_STATE_0 ][ i ];

		if ( current_node->node_id != 0 ) { /* 対象外ということ */
			
			if ( (  current_node->n_for_org == 0 ) &&
				 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
				
				//printf("add    stack node ID %ld\n",current_node->node_id);
 					
				current_node->level = Topo_top.level;
				
				learn_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				
				learn_s_list->node = current_node;
				learn_s_list->next = current_list;
				current_list = learn_s_list;
			}
			else {
				current_node->flag &= ~LEARN_FLAG_ONLIST;
				current_node->n_forward = current_node->n_for_org; 
			}
			current_node->flag &= ~LEARN_FLAG_FINISH;
			current_node->flag |= LEARN_FLAG_RESEARCH;
			
			current_node = Node_head[ LEARN_STATE_1 ][ i ];
			if ( ( current_node->n_for_org == 0 ) &&
				 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
				
				//printf("add    stack node ID %ld\n",current_node->node_id);
					
				current_node->level = Topo_top.level;
					
				/* 双方向リストの作成 */
				learn_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
				learn_s_list->node = current_node;
				learn_s_list->next = current_list;
				current_list = learn_s_list;
			}
			else {
				current_node->flag &= ~LEARN_FLAG_ONLIST;
				current_node->n_forward = current_node->n_for_org; 
			}
			current_node->flag &= ~LEARN_FLAG_FINISH;
			current_node->flag |= LEARN_FLAG_RESEARCH;
		}
	}
	
	while ( 1 ) {
		
		printf("2-L-3 loop %ld\n",loop_count);
		topgun_print_mes_dag_iteration( loop_count );

		if ( continue_flag2 == 0 ) {

			printf("2-L-3 loop %ld+alpha\n",loop_count);
			for ( i = 0 ; i < Line_info.n_line ; i++ ) {

				current_node = Node_head[ LEARN_STATE_0 ][ i ];

				if ( current_node->node_id != 0 ) { /* 対象外ということ */
			
					if ( (  current_node->n_for_org == 0 ) &&
						 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
				
						current_node->level = Topo_top.level;
				
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				
						learn_s_list->node = current_node;
						learn_s_list->next = current_list;
						current_list = learn_s_list;
					}
					else {
						current_node->flag &= ~LEARN_FLAG_ONLIST;
						current_node->n_forward = current_node->n_for_org; 
					}
					current_node->flag &= ~LEARN_FLAG_FINISH;
					current_node->flag |= LEARN_FLAG_RESEARCH;
			
					current_node = Node_head[ LEARN_STATE_1 ][ i ];
					if ( ( current_node->n_for_org == 0 ) &&
						 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
						
					
						current_node->level = Topo_top.level;
						
						/* 双方向リストの作成 */
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
						learn_s_list->node = current_node;
						learn_s_list->next = current_list;
						current_list = learn_s_list;
					}
					else {
						current_node->flag &= ~LEARN_FLAG_ONLIST;
						current_node->n_forward = current_node->n_for_org; 
					}
					current_node->flag &= ~LEARN_FLAG_FINISH;
					current_node->flag |= LEARN_FLAG_RESEARCH;
				}
			}
		}
		else {
		if ( current_list == NULL ) {

			/* loop2以降 */
			
			/* 少なくともadd_listのbackward側のFINISHを */
			/* 全部のFINISH FLAGを元に戻す必要がある */
			/* add_listはスタート地点 */

			
			/* まずadd_listからbackward側にLEARN_FLAG_RESEARCHをたてる */

			static_learn_dag_onflag_research( add_list ) ;

			while ( add_list ) {
				tmp_add_list = add_list;
				add_list = add_list->next;
				topgunFree ( tmp_add_list, FMT_LEARN_S_LIST, 1, func_name );
			}
			tmp_add_list = NULL;


			/*  */
			
			for ( i = 0 ; i < Line_info.n_line ; i++ ) {

				current_node = Node_head[ LEARN_STATE_0 ][ i ];

				if ( current_node->node_id != 0 ) { /* 対象外ということ */
			
					if ( (  current_node->n_for_org == 0 ) &&
						 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
				
						current_node->level = Topo_top.level;
				
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				
						learn_s_list->node = current_node;
						learn_s_list->next = current_list;
						current_list = learn_s_list;
					}
					else {
						current_node->flag &= ~LEARN_FLAG_ONLIST;
						current_node->n_forward = current_node->n_for_org; 
					}
					current_node->flag &= ~LEARN_FLAG_FINISH;
			
					current_node = Node_head[ LEARN_STATE_1 ][ i ];
					if ( ( current_node->n_for_org == 0 ) &&
						 ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) ) {
						
					
						current_node->level = Topo_top.level;
						
						/* 双方向リストの作成 */
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
						learn_s_list->node = current_node;
						learn_s_list->next = current_list;
						current_list = learn_s_list;
					}
					else {
						current_node->flag &= ~LEARN_FLAG_ONLIST;
						current_node->n_forward = current_node->n_for_org; 
					}
					current_node->flag &= ~LEARN_FLAG_FINISH;
				}
			}
		}
		}

		if ( current_list == NULL ) {
			break;
		}

		while ( current_list ) {

			test_dag_node_n_forward ();

			/* add_listを後にする */
			if ( current_list == NULL ) {
				/* add_list(間接含意の枝の根元ですでに処理済みの場合)が存在する場合でcurrent_listがNULLの場合にする */
				current_node = add_list->node;
				
				learn_s_list = add_list;
				add_list     = add_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
			else {
				/* 最初はレベル1から始める */
				current_node = current_list->node;
			
				learn_s_list = current_list;
				current_list = current_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}

			current_node->flag &= ~LEARN_FLAG_ONLIST;
			
			/* 選択しない条件 */
			/* その1 stackにつまれている間に枝を張られた場合 */
			/* その2 子供でない */
			if ( ( current_node->flag & LEARN_FLAG_CHILDREN ) ||
				 ( current_node->flag & LEARN_FLAG_FINISH ) ) {

				//printf("delete stack node ID %ld\n",current_node->node_id);
				
				continue;
			}
			current_node->flag |= LEARN_FLAG_FINISH;

			if ( current_node->flag & LEARN_FLAG_RESEARCH ) {

				topgun_print_mes_dag_select_node(current_node);
				topgun_print_mes_search_node( current_node );
				current_node->imp_count++;
				dag_return.n_add_direct_node = 0;
			
				/* 実際に探索する */
				search_add_list = static_learn_dag_direct_node ( current_node, &dag_return );

				topgun_print_mes_dag_select_node_add ( current_node, dag_return.n_add_direct_node );
				topgun_print_mes_dag_exe_node ( current_node );
				
				count_try_learn++;
				continue_flag += dag_return.n_add_direct_node;
				current_node->flag &= ~LEARN_FLAG_RESEARCH;

				if ( dag_return.add_indirect_list != NULL ) {

					/* add_list を連結する */
					if ( add_list == NULL ) {
						add_list = dag_return.add_indirect_list;
					}
					else {
						tmp_add_list = add_list;
						while ( tmp_add_list->next ) {
							tmp_add_list = tmp_add_list->next;
						}
						tmp_add_list->next = dag_return.add_indirect_list;
					}
					dag_return.add_indirect_list = NULL;
				}
			}
			else {
				//printf("2-L-R no  research node %8ld\n",current_node->node_id );
			}

			if ( dag_return.redo_flag == 10 ) {
				/* ループが発生して、最後まで探索をしていない場合 */

				current_node->flag &= ~LEARN_FLAG_FINISH;
				current_node->flag |= LEARN_FLAG_ONLIST;
				current_node->flag |= LEARN_FLAG_RESEARCH;
				
				learn_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
				learn_s_list->node = current_node;
				learn_s_list->next = current_list;
				current_list = learn_s_list;
				
			}
			else {
				backward = current_node->backward;
				while ( backward ) {
					back_node = backward->node;
					static_learn_dag_branch_count_finish_with_check( back_node );

					if ( ( back_node->n_forward == 0 ) &&
						 ( ! ( back_node->flag & LEARN_FLAG_ONLIST ) ) &&
						 ( ! ( back_node->flag & LEARN_FLAG_FINISH ) ) ){

						back_node->flag |= LEARN_FLAG_ONLIST;
					
						//printf("2-L-R put onlist   node %8ld\n",back_node->node_id );
						//printf("add    stack node ID %ld\n",back_node->node_id);

						/* 双方向リストの作成 */
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
						learn_s_list->node = back_node;
						learn_s_list->next = current_list;
						current_list = learn_s_list;

					}
					else {
						if ( back_node->n_forward != 0 ) {
							topgun_print_mes_dag_pass_node( back_node );
						}
						else if ( back_node->flag & LEARN_FLAG_ONLIST ) {
							topgun_print_mes_dag_onlist_node( back_node );
						}
						else if ( back_node->flag & LEARN_FLAG_FINISH ) {
							topgun_print_mes_dag_finish_node( back_node );
						}
						else {
							topgun_print_mes_dag_hatena_node( back_node );
						}
					}
					backward = backward->next;
				}
			}

			/* マージによる到達不可能ノードの処理 */
			topgun_print_mes_dag_illegal_add_node( search_add_list );
			if ( search_add_list != NULL ) {
				if ( current_list == NULL ) {
					current_list = search_add_list;
				}
				else {
					static_learn_dag_make_connect ( current_list, search_add_list );
				}
				search_add_list = NULL;
			}

			topgun_print_mes_dag_next_entry( current_node );
		}

		topgun_print_mes_dag_rest_check();

		//printf("2-L-3 current_try   %8ld\n",count_try_learn );
		//printf("2-L-3 current_learn %8ld\n",(continue_flag/2));

		total_learn += ( continue_flag / 2 );
		
		loop_count++;
		if ( continue_flag == 0 ) {

			if ( continue_flag2 == 0 ) {
				break;
			}
			else {
				break;
				continue_flag2 = 0;
			}
		}
		else {
			continue_flag2 = 1;
			topgun_print_mes_dag_n_add_node	( continue_flag );
		}
		continue_flag = 0;

		if ( loop_count >= iteration ) {
			break;
		}
	}
	topgun_print_mes_Topo_top();
	
	test_dag_node_learn_do ( loop_count );
	
	printf("SL %lu ( %lu / %lu )\n",total_learn/2,count_try_learn, loop_count);
	//printf("2-L-E loop %ld\n",loop_count);
	//printf("2-L-E try  %ld\n",count_try_learn);

	topgun_4_utility_enter_end_time ( &Time_head.learn_dynamic_resort );	
}

/*!
  @brief 隣接以外の直接含意を求める

  @param [in] void
  @return void

*/

void static_learn_dag_direct
(
 Ulong iteration
 ){
	LEARN_NODE *current_node = NULL;
	LEARN_TOPO *current_topo = NULL;

	LEARN_DAG_RETURN dag_return;
	
	Ulong continue_flag = 0;
	Ulong count_try_learn = 0;
	
	dag_return.redo_flag = 0;
	dag_return.mode_flag = 0;
	dag_return.add_indirect_list = NULL;
	
	/* 最初はレベル1から始める */
	current_node = Topo_top.top_node;
	
	/* すべての信号線に対して */
	while ( 1 ) {
		
		topgun_print_mes_dag_iteration( loop_count );
		topgun_print_mes_Topo_top();
		
		while ( current_node ){
			if ( current_node->imp_count == loop_count ) {

				/* 子は実施しない */
				if ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) {
					
					topgun_print_mes_search_node( current_node );
					current_node->imp_count++;
					dag_return.n_add_direct_node = 0;
					static_learn_dag_direct_node ( current_node, &dag_return );
					count_try_learn++;
					continue_flag += dag_return.n_add_direct_node;
				}
				else {
					topgun_print_mes_dont_search_node( current_node );
				}
			}
			else {
				topgun_print_mes_already_search_node( current_node );
			}

			if ( dag_return.redo_flag != 0 ) {
				current_node->imp_count--;

				topgun_print_mes_dag_redo_sort( current_node );

				/* 再ソート */
				static_learn_dag_redo_all_topologicalsort();
				/* 再ソートしたら最初から */
				current_topo = &( Topo_top );
				current_node = current_topo->top_node;
				dag_return.redo_flag       = 0;

			}
			else {
				current_node = current_node->next;
			}
		}
		
		loop_count++;
		/* 終了条件の確認 */
		if ( continue_flag == 0 ) {
			break;
		}
		if ( loop_count >= iteration ) {
			break;
		}
		continue_flag = 0;

		
		/* levelの更新 */
		current_node = Topo_top.top_node;

	}
	topgun_print_mes_Topo_top();
	
	loop_count++;
	test_dag_node_learn_do ( loop_count );
	
	printf("2-L-E loop %ld\n",loop_count);
	printf("2-L-E try  %ld\n",count_try_learn);
}

/*!
  @brief 隣接以外の直接含意を求める

  @param [in] void
  @return void

*/

void static_learn_dag_direct_reentry
(
 Ulong iteration
 ){
	LEARN_NODE *current_node = NULL;
	LEARN_TOPO *current_topo = NULL;

	LEARN_DAG_RETURN dag_return;
	
	Ulong continue_flag = 0;
	Ulong count_try_learn = 0;
	
	dag_return.redo_flag = 0;
	dag_return.mode_flag = 2;
	dag_return.add_indirect_list = NULL;
	
	/* 最初はレベル1から始める */
	current_node = Topo_top.top_node;
	
	/* すべての信号線に対して */
	while ( 1 ) {
		
		topgun_print_mes_dag_iteration( loop_count );
		topgun_print_mes_Topo_top();
		
		while ( current_node ){
			if ( current_node->imp_count == loop_count ) {

				/* 子は実施しない */
				if ( ! ( current_node->flag & LEARN_FLAG_CHILDREN ) ) {
					
					topgun_print_mes_search_node( current_node );
					current_node->imp_count++;
					dag_return.n_add_direct_node = 0;
					static_learn_dag_direct_node ( current_node, &dag_return );
					count_try_learn++;
					continue_flag += dag_return.n_add_direct_node;
				}
				else {
					topgun_print_mes_dont_search_node( current_node );
				}
			}
			else {
				topgun_print_mes_already_search_node( current_node );
			}

			if ( dag_return.redo_flag != 0 ) {
				current_node->imp_count--;

				topgun_print_mes_dag_redo_sort( current_node );

				/* 再ソート */
				static_learn_dag_redo_all_topologicalsort();
				/* 再ソートしたら最初から */
				current_topo = &( Topo_top );
				current_node = current_topo->top_node;
				dag_return.redo_flag       = 0;

			}
			else {
				current_node = current_node->next;
			}
		}
		
		loop_count++;
		/* 終了条件の確認 */
		if ( continue_flag == 0 ) {
			break;
		}
		if ( loop_count >= iteration ) {
			break;
		}
		continue_flag = 0;

		
		/* levelの更新 */
		current_node = Topo_top.top_node;

	}
	topgun_print_mes_Topo_top();
	
	loop_count++;
	test_dag_node_learn_do ( loop_count );
	
	printf("2-L-E loop %ld\n",loop_count);
	printf("2-L-E try  %ld\n",count_try_learn);
}

/*!
  @brief 間接含意の探索終了後にグラフの構造を調査する

  @param [in] void
  @return void

*/

/*
// 未使用のためコメントアウトする
void static_learn_dag_levelize
(
 void
 ){
	Ulong      i = 0; //カウンタ
	Ulong      current_level = 1; //レベルの最小値
	Ulong      get_count = 0;
	LEARN_NODE *node = NULL;
	LEARN_BRANCH *backward = NULL;
	LEARN_BRANCH *forward = NULL;


	static_learn_dag_direct_delete_list();
	
	// contra_clear
	// n_forwardを元にもどす
	for ( i = 0; i < Line_info.n_line ; i++ ) {
		Node_head[ LEARN_STATE_0 ][ i ]->n_forward
			= Node_head[ LEARN_STATE_0 ][ i ]->n_for_org;
		
		Node_head[ LEARN_STATE_1 ][ i ]->n_forward
			= Node_head[ LEARN_STATE_1 ][ i ]->n_for_org;
	}


	// まずはレベル付け
	for ( i = 0; i < Line_info.n_line ; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];
		if ( node->node_id != 0 ) {

			if ( node->n_for_org == 0 ) {

				//printf("Node %3lu -> ",node->node_id );
				node->level = current_level;  // levelの設定

				backward = node->backward;
				while ( backward ) {
					backward->node->n_forward--; 

					if ( backward->node->n_forward == 0 ) {
						//printf("%3lu for oo ",backward->node->node_id );
						static_learn_dag_direct_enter_list_new( backward->node );
					}
					else {
						//printf("%3lu for %2lu ",backward->node->node_id,  backward->node->n_forward);
					}
					backward = backward->next;
				}
				//printf("\n");
			}
		}
		node = Node_head[ LEARN_STATE_1 ][ i ];
		if ( node->node_id != 0 ) {

			if ( node->n_for_org == 0 ) {

				//printf("Node %3lu -> ",node->node_id );
				
				node->level = current_level;  // levelの設定

				backward = node->backward;
				while ( backward ) {
					backward->node->n_forward--; 

					if ( backward->node->n_forward == 0 ) {
						//printf("%3lu for oo ",backward->node->node_id );
						static_learn_dag_direct_enter_list_new( backward->node );
					}
					else {
						//printf("%3lu for %2lu ",backward->node->node_id,  backward->node->n_forward);
					}
					backward = backward->next;
				}
				//printf("\n");
			}
		}
	}
	
	node = static_learn_dag_direct_get_list( get_count++ );
	
	while ( node ) {

		//入力側(backward)のノードの最大レベルを求める
		current_level = 0;
		forward = node->forward;
		while ( forward ) {
			if ( current_level < forward->node->level ) {
				current_level = forward->node->level;
			}
			forward = forward->next;
		}
		node->level = current_level+1;  // levelの設定

		printf("Node %3lu -> ",node->node_id );
		
		backward = node->backward;
		while ( backward ) {

			backward->node->n_forward--; 
			
			if ( backward->node->n_forward == 0 ) {
				printf("%3lu for oo ",backward->node->node_id );
				static_learn_dag_direct_enter_list_new( backward->node );
			}
			else {
				printf("%3lu for %2lu ",backward->node->node_id,
					   backward->node->n_forward);
			}
			backward = backward->next;
		}
		printf("\n");
		node = static_learn_dag_direct_get_list( get_count++ );
	}
	

	// n_forwardを元にもどす
	for ( i = 0; i < Line_info.n_line ; i++ ) {
		Node_head[ LEARN_STATE_0 ][ i ]->n_forward
			= Node_head[ LEARN_STATE_0 ][ i ]->n_for_org;
		
		Node_head[ LEARN_STATE_1 ][ i ]->n_forward
			= Node_head[ LEARN_STATE_1 ][ i ]->n_for_org;
	}
		
	static_learn_dag_direct_delete_list();
}
*/

void extern inline test_dag_topo_graph_flag_0
(
 void 
 ){
#ifdef _DEBUG_PRT_ATPG_

	LEARN_NODE      *node;
	Ulong           flag;
	
	//char *func_name = "static_learn_dag_topo_graph_flag_0"; /* 関数名 */
	
	node = Topo_top.top_node;

	while ( node ) {

		if ( node->prev != NULL ) {
			if ( ! ( ( node->prev->next->line->line_id == node->line->line_id ) &&
					 ( node->prev->next->state3 == node->state3 ) ) ) {
						 printf("2-L-e prev LV %2ld ID %8ld ",node->level,node->line->line_id );
						 topgun_print_mes_state3_only( node->state3);
						 topgun_print_mes_n();
					 }
		}
		if ( node->next != NULL ) {
			if ( ! ( ( node->next->prev->line->line_id == node->line->line_id ) &&
					 ( node->next->prev->state3 == node->state3 ) ) ) {
						 printf("2-L-e next LV %2ld ID %8ld ",node->level,node->line->line_id );
						 topgun_print_mes_state3_only( node->state3);
						 topgun_print_mes_n();
					 }
		}
#ifdef NO_USE
		if ( node->level != level ) {
			printf("2-L-E leve LV %2ld ID %8ld ",node->level,node->line->line_id );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n();
		}
#endif /* NO_USE */
		flag = ( node->flag & ~LEARN_FLAG_PARE_CHI ) ; /* nouseのフラグは残るから */
		if ( flag != 0 ) {
			printf("2-L-E LV %2ld ID %8ld ",node->level,node->line->line_id );
			topgun_print_mes_state3_only( node->state3);
			printf("  %2ld ",flag );
			printf("  %2ld ",node->flag );
			topgun_print_mes_n();
		}
		node = node->next;
	}

#endif /* _DEBUG_PRT_ATPG_ */
}

/*!
  @brief 隣接以外の直接含意を求める

  @param [in] void
  @return void

*/

void static_learn_dag_direct_node_new
(
 LEARN_NODE *current_node,
 LEARN_DAG_RETURN *dag_return
 ){

	ASS_LIST      ass_list;
	ASS_LIST      *add_list;
	ASS_LIST      *tmp_ass_list;

	IMP_RESULT   imp_result;

	
	LEARN_NODE   *reverse_node      = NULL; //!< current_nodeの対のノード
	LEARN_NODE   *top_node          = NULL; //!< 含意可能で到達不可能なノード
	LEARN_NODE   *reverse_top_node  = NULL; //!< top_nodeの対のノード

	LEARN_NODE   *from_result_node  = NULL;
	LEARN_NODE   *to_result_node    = NULL;
	
	LEARN_S_LIST *no_branch_list    = NULL;
	LEARN_S_LIST *learn_s_list      = NULL;
	
	LEARN_S_LIST *loop_list         = NULL; //!< ループを構成しているノードリスト
	LEARN_S_LIST *free_loop_list;

	LEARN_S_LIST *indirect_node_list = NULL;
	LEARN_S_LIST *backward_list      = NULL;

	LEARN_BRANCH *branch            = NULL;

	LEARN_LIST   *learn_list        = NULL; //!< 間接含意のリスト

	LEARN_TYPE   learn_type;

	LINE         *from_line         = NULL;
	LINE         *to_line           = NULL;
	STATE_3      from_state3;
	STATE_3      to_state3;

	char *func_name = "static_learn_dag_direct_node"; /* 関数名 */

	topgun_4_utility_enter_start_time ( &Time_head.learn_direct_node );
	topgun_4_utility_enter_start_time ( &Time_head.learn_search );

	// initial imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;
	
	dag_return->n_add_direct_node = 0;  //!< 幾つ枝を足したか
	dag_return->redo_flag         = 0;  //!< redoを実施したか
	
	ass_list.line       = current_node->line;
	ass_list.condition  = COND_NORMAL;
	ass_list.ass_state3 = current_node->state3;
	ass_list.next       = NULL;

	/* 対偶用のノード */
	reverse_node       = current_node->reverse;
	if ( reverse_node->flag & LEARN_FLAG_CHILDREN ) {
		reverse_node = reverse_node->parent;
	}

	topgun_4_utility_enter_start_time ( &Time_head.learn_imp );	
	imp_result = gene_enter_state ( &ass_list );

	if ( IMP_CONF != imp_result.i_state ) {
		imp_result = implication();
	}
	topgun_4_utility_enter_end_time ( &Time_head.learn_imp );

	
	if ( IMP_CONF != imp_result.i_state ) {
		/* 含意操作が成功した場合 */
	
		/* 枝をたどり、たどった節にflag( DIRECT )をたてる */
		static_learn_dag_up_flag ( current_node );
		
		/* 到達不可能な信号線のリストを求める */
		no_branch_list = static_learn_dag_make_no_reachable_list();
					
		/* フラグがたっていない箇所をレベルが高い順に調べる */
		while ( 1 ) {

			/* 未チェックのリストからレベルの一番高いものを選ぶ */
			if ( ! ( dag_return->mode_flag & LEARN_MODE_SELECT ) ) {
				top_node = static_learn_dag_search_max_level_node ( no_branch_list );
			}
			else if ( dag_return->mode_flag & LEARN_MODE_SELECT ) {
				top_node = static_learn_dag_search_top_node ( no_branch_list );
			}

			if ( top_node == NULL ) {
				/* 全部フラグがたった */

				/* no_branch_listのメモリをfreeする */

				while ( no_branch_list ) {
					learn_s_list = no_branch_list;
					no_branch_list = no_branch_list->next;
					topgunFree ( learn_s_list, FMT_LEARN_S_LIST, 1, func_name ); 
				}
				break;
			}
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
			else {
				printf("2-L-R top node n%8ld line %5ld\n"
					   ,top_node->node_id,top_node->line->line_id );
			}
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */

			if ( dag_return->redo_flag != 0 ) {
				
				dag_return->redo_flag = 10; /* for REENTRY MODE */
				
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
				printf("2-L-R ehehe?\n");
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
				
				/* 構造が変化した */
				while ( no_branch_list ) {
					learn_s_list = no_branch_list;
					no_branch_list = no_branch_list->next;
					topgunFree ( learn_s_list, FMT_LEARN_S_LIST, 1, func_name ); 
				}
				break;
			}
		
			/* 対偶の場所を覚える */
			reverse_top_node = top_node->reverse;


			/* flagをたてる */
			top_node->flag |= LEARN_FLAG_DIRECT;

			//learn_type = static_learn_dag_check_family( current_node, top_node );

			/* 線をひく */
			topgun_4_utility_enter_start_time ( &Time_head.learn_add_br );
			static_learn_dag_make_direct_branch( current_node, top_node );
			topgun_4_utility_enter_end_time ( &Time_head.learn_add_br );

			if ( current_node->flag & LEARN_FLAG_CHILDREN ) {
				from_result_node = current_node->parent;
			}
			else {
				from_result_node = current_node;
			}
			
			if (top_node->flag & LEARN_FLAG_CHILDREN ) {
				to_result_node = top_node->parent;
			}
			else {
				to_result_node = top_node;
			}
			
			if ( to_result_node->flag & LEARN_FLAG_FINISH ) {
					/* すでにto_result_nodeからの探索が終了している場合 */
				if ( from_result_node->n_forward > 0 ) {
					from_result_node->n_forward--;
				}
			}
			
			dag_return->n_add_direct_node++;

			if (! ( dag_return->mode_flag & LEARN_MODE_SELECT ) ){
				if ( ! ( current_node->level > top_node->level ) ) {
					dag_return->redo_flag = 1;
				}
			}

			while ( 1 ) {
				
				/* ループがある場合の作業 */
				loop_list = static_learn_dag_loop_check_search( current_node );
			
				if ( loop_list != NULL ) {
					/* loopが成立している */
					topgun_4_utility_enter_start_time ( &Time_head.learn_loop_ope );
					
					/* loopを構成するノードを一つにマージする */
					/* とりあえずレベルの一番高いノードにマージする */
					static_learn_dag_node_merge_new( loop_list, current_node, 0 );

					/* ここではsearch_tmp_listは必ずNULL */

					/* ノードのマージに対応してフラグを変更する */
					static_learn_dag_reup_flag ( current_node );

				
					/* 再度ソートを仕直すトリガーをセットする */
					dag_return->redo_flag = 2;

					while ( loop_list ) {
						free_loop_list = loop_list;
						loop_list = loop_list->next;
						topgunFree ( free_loop_list, FMT_LEARN_S_LIST, 1, func_name ); 
					}
					
					topgun_4_utility_enter_end_time ( &Time_head.learn_loop_ope );

				}
				else {
					break;
				}
			}
		
			if ( dag_return->redo_flag == 0 ) {
				/* つづける場合だけやる */
				/* その先もフラグをたてる */
				static_learn_dag_up_flag ( 	top_node );
			}

			/* 対偶(間接含意)はすぐには線をひく */
			/* 覚えるんはとりあえずおぼえとく */

			if ( reverse_top_node->flag & LEARN_FLAG_CHILDREN ) {
				reverse_top_node = reverse_top_node->parent;
			}
			if ( reverse_node->flag & LEARN_FLAG_CHILDREN ) {
				reverse_node = reverse_node->parent;
			}
		
			learn_type = static_learn_dag_check_contraposition
				( reverse_top_node, reverse_node );
			//static_learn_dag_check_contraposition_down_flag( reverse_top_node );

		
			if ( learn_type == LEARN_ON ) {
				/* 枝をはる */

				from_line   = reverse_top_node->line;
				from_state3 = reverse_top_node->state3;
				to_line     = reverse_node->line;
				to_state3   = reverse_node->state3;

				learn_list = ( LEARN_LIST * )topgunMalloc
					( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					
				learn_list->line       = to_line;
				learn_list->ass_state3 = to_state3;

				if ( from_state3 == STATE3_0 ) {
					learn_list->next = from_line->imp_0;
					from_line->imp_0 = learn_list;
				}
				else if ( from_state3 == STATE3_1 ){
					learn_list->next = from_line->imp_1;
					from_line->imp_1 = learn_list;
				}

				/* 線をひく */
				static_learn_dag_make_direct_branch( reverse_top_node, reverse_node );
				static_learn_dag_make_direct_branch_print( reverse_top_node, reverse_node );

				if ( reverse_top_node->flag & LEARN_FLAG_CHILDREN ) {
					from_result_node = reverse_top_node->parent;
				}
				else {
					from_result_node = reverse_top_node;
				}			
				if ( reverse_node->flag & LEARN_FLAG_CHILDREN ) {
					to_result_node = reverse_node->parent;
				}
				else {
					to_result_node = reverse_node;
				}
				
				if ( to_result_node->flag & LEARN_FLAG_FINISH ) {
					/* すでにto_result_nodeからの探索が終了している場合 */
					if ( from_result_node->n_forward > 0 ) {
						from_result_node->n_forward--;
					}
				}


				if ( dag_return->mode_flag & LEARN_MODE_RESORT ) {
					/* RESORT modeの場合 */
					topgun_print_mes_dag_reentry( from_result_node );

					static_learn_dag_search_add_enter_list ( from_result_node );

					/* 
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					learn_s_list->node = from_result_node;
					learn_s_list->next = indirect_node_list;
					indirect_node_list = learn_s_list;
					*/
				}
				else if ( dag_return->mode_flag & LEARN_MODE_REENTRY ) {
					/* REENTRY modeの場合 */
					/* 間接含意を覚えたノードを覚える */
					
					if ( from_result_node->flag & LEARN_FLAG_FINISH ) {

						from_result_node->flag &= ~LEARN_FLAG_FINISH;

						topgun_print_mes_dag_reentry( from_result_node );
					
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
						learn_s_list->node = from_result_node;
						learn_s_list->next = indirect_node_list;
						indirect_node_list = learn_s_list;
					}
				}

				
				dag_return->n_add_direct_node++;

				if ( ! ( dag_return->mode_flag & LEARN_MODE_SELECT ) ){
					if ( ! ( reverse_top_node->level > reverse_node->level ) ) {

						if ( dag_return->redo_flag == 0 ) {
							dag_return->redo_flag = 1;
						}
					}
				}

				while ( 1 ) {
					/* ループがある場合の作業 */
					loop_list = static_learn_dag_loop_check_search( reverse_top_node );

					if ( loop_list != NULL ) {
						/* loopが成立している */
						topgun_4_utility_enter_start_time ( &Time_head.learn_loop_ope );
					
						/* loopを構成するノードを一つにマージする */
						static_learn_dag_node_merge_new( loop_list, reverse_top_node, 1 );

						/* ノードのマージに対応してフラグを変更する */
						static_learn_dag_reup_flag ( reverse_top_node );
					
						/* 再度ソートを仕直す */
						dag_return->redo_flag = 2;
						//static_learn_dag_redo_all_topologicalsort();

						while ( loop_list ) {
							free_loop_list = loop_list;
							loop_list = loop_list->next;
							topgunFree ( free_loop_list, FMT_LEARN_S_LIST, 1, func_name ); 
						}
						topgun_4_utility_enter_end_time ( &Time_head.learn_loop_ope );
					}
					else {
						break;
					}
				}
			
				loop_list = NULL;

				if ( dag_return->redo_flag == 0 ) {
					/* つづける場合だけやる */
					no_branch_list = static_learn_dag_recheck_unreachable_list( no_branch_list );
				}
			}
		}

		/* 枝をたどり、たどった節のflagを戻す */
		static_learn_dag_down_flag ( current_node );

		test_dag_topo_graph_flag_0();
	}

	topgun_4_utility_enter_end_time ( &Time_head.learn_search );
	topgun_4_utility_enter_start_time ( &Time_head.learn_finish );
	

	/* 含意操作の結果をrefleshする */
	atpg_imptrace_reflesh();
#ifndef OLD_IMP2
#else	
	Gene_head.imp_trace = NULL;
#endif /* OLD_IMP2 */	

	/* 値割り当ての結果を元にもどす */
	//atpg_reflesh_state( ass_list.line, ass_list.condition );

	add_list = ass_list.next;

	while ( add_list ) {

		tmp_ass_list = add_list;
		add_list = add_list->next;
		topgunFree ( tmp_ass_list, FMT_ASS_LIST, 1, func_name ); 
	}
	if ( IMP_CONF == imp_result.i_state ) {
		/* fix listを作成する */
		static_learn_dag_make_fix_list( &ass_list );

	}

	/* indirect_node_listから実際に再度実施する必要のあるノードをリストアップする */
	dag_return->add_indirect_list = indirect_node_list;

	/* message for debug */
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	if ( dag_return->n_add_direct_node != 0 ) {
		printf("2-L-P n_add %5ld\n", dag_return->n_add_direct_node );
		learn_s_list = indirect_node_list;
		while( learn_s_list ) {
			printf("2-L-P add n%5ld l%5ld\n"
				   ,learn_s_list->node->node_id
				   ,learn_s_list->node->line->line_id);
			learn_s_list = learn_s_list->next;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */

	if ( ! ( dag_return->mode_flag & LEARN_MODE_RESORT ) ){
		/* newの場合はたぶんこない */
		printf("hen!!\n");
		
		/* 再帰的にbackward方向へLEARN_FLAG_FINISHフラグを元に戻す */
		while ( indirect_node_list ) {
			branch = indirect_node_list->node->backward;
			indirect_node_list = indirect_node_list->next;
			while ( branch ) {
			
				if ( branch->node->flag & LEARN_FLAG_FINISH ) {
				
					branch->node->flag &= ~LEARN_FLAG_FINISH;

					topgun_print_mes_dag_reentry( branch->node );
				
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					learn_s_list->node = branch->node;
					learn_s_list->next = backward_list;
					backward_list = learn_s_list;
				
				}
				static_learn_dag_branch_count_finish( branch->node );
				branch = branch->next;
			}
		}
		while ( backward_list ) {
			learn_s_list = backward_list;
			backward_list = backward_list->next;

			branch = learn_s_list->node->backward;

			topgunFree ( learn_s_list, FMT_LEARN_S_LIST, 1, func_name ); 

			while ( branch ) {
			
				if ( branch->node->flag & LEARN_FLAG_FINISH ) {
				
					branch->node->flag &= ~LEARN_FLAG_FINISH;

					topgun_print_mes_dag_reentry_next( branch->node );

					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					learn_s_list->node = branch->node;
					learn_s_list->next = backward_list;
					backward_list = learn_s_list;
				
				}
				static_learn_dag_branch_count_finish( branch->node );
				//branch->node->n_forward++;
				branch = branch->next;
			}
		}
	}

	test_all_state_xx();

	topgun_4_utility_enter_end_time ( &Time_head.learn_finish );
	topgun_4_utility_enter_end_time ( &Time_head.learn_direct_node );

}

/*!
  @brief 隣接以外の直接含意を求める

  @param [in] void
  @return void

*/

LEARN_S_LIST *static_learn_dag_direct_node
(
 LEARN_NODE *current_node,
 LEARN_DAG_RETURN *dag_return
 ){

	ASS_LIST      ass_list;
	ASS_LIST      *add_list;
	ASS_LIST      *tmp_ass_list;

	IMP_RESULT   imp_result;

	
	LEARN_NODE   *reverse_node      = NULL; //!< current_nodeの対のノード
	LEARN_NODE   *top_node          = NULL; //!< 含意可能で到達不可能なノード
	LEARN_NODE   *reverse_top_node  = NULL; //!< top_nodeの対のノード

	LEARN_NODE   *from_result_node  = NULL;
	LEARN_NODE   *to_result_node    = NULL;
	
	LEARN_S_LIST *no_branch_list    = NULL;
	LEARN_S_LIST *learn_s_list      = NULL;
	
	LEARN_S_LIST *loop_list         = NULL; //!< ループを構成しているノードリスト
	LEARN_S_LIST *free_loop_list;

	LEARN_S_LIST *indirect_node_list = NULL;
	LEARN_S_LIST *backward_list      = NULL;

	LEARN_BRANCH *branch            = NULL;

	LEARN_LIST   *learn_list        = NULL; //!< 間接含意のリスト

	LEARN_TYPE   learn_type;

	LEARN_S_LIST *search_add_list   = NULL;  //!< 返り値
	LEARN_S_LIST *search_tmp_list   = NULL;  //!< 一次受け
	
	LINE         *from_line         = NULL;
	LINE         *to_line           = NULL;
	STATE_3      from_state3;
	STATE_3      to_state3;

	char *func_name = "static_learn_dag_direct_node"; /* 関数名 */

	topgun_4_utility_enter_start_time ( &Time_head.learn_direct_node );
	topgun_4_utility_enter_start_time ( &Time_head.learn_search );

	// initial imp_result
	imp_result.i_state = IMP_KEEP;
	imp_result.imp_id  = 0;
	
	dag_return->n_add_direct_node = 0;  //!< 幾つ枝を足したか
	dag_return->redo_flag         = 0;  //!< redoを実施したか
	
	ass_list.line       = current_node->line;
	ass_list.condition  = COND_NORMAL;
	ass_list.ass_state3 = current_node->state3;
	ass_list.next       = NULL;

	/* 対偶用のノード */
	reverse_node       = current_node->reverse;
	if ( reverse_node->flag & LEARN_FLAG_CHILDREN ) {
		reverse_node = reverse_node->parent;
	}

	topgun_4_utility_enter_start_time ( &Time_head.learn_imp );	
	imp_result = gene_enter_state ( &ass_list );

	if ( IMP_CONF != imp_result.i_state ) {
		imp_result = implication();
	}
	topgun_4_utility_enter_end_time ( &Time_head.learn_imp );

	
	if ( IMP_CONF != imp_result.i_state ) {
		/* 含意操作が成功した場合 */
	
		/* 枝をたどり、たどった節にflag( DIRECT )をたてる */
		static_learn_dag_up_flag ( current_node );
		
		/* 到達不可能な信号線のリストを求める */
		no_branch_list = static_learn_dag_make_no_reachable_list();
					
		/* フラグがたっていない箇所をレベルが高い順に調べる */
		while ( 1 ) {

			/* 未チェックのリストからレベルの一番高いものを選ぶ */
			if ( ! ( dag_return->mode_flag & LEARN_MODE_SELECT ) ) {
				top_node = static_learn_dag_search_max_level_node ( no_branch_list );
			}
			else if ( dag_return->mode_flag & LEARN_MODE_SELECT ) {
				top_node = static_learn_dag_search_top_node ( no_branch_list );
			}

			if ( top_node == NULL ) {
				/* 全部フラグがたった */

				/* no_branch_listのメモリをfreeする */

				while ( no_branch_list ) {
					learn_s_list = no_branch_list;
					no_branch_list = no_branch_list->next;
					topgunFree ( learn_s_list, FMT_LEARN_S_LIST, 1, func_name ); 
				}
				break;
			}
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
			else {
				printf("2-L-R top node n%8ld line %5ld\n"
					   ,top_node->node_id,top_node->line->line_id );
			}
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */

			if ( dag_return->redo_flag != 0 ) {
				
				dag_return->redo_flag = 10; /* for REENTRY MODE */
				
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
				printf("2-L-R ehehe?\n");
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
				
				/* 構造が変化した */
				while ( no_branch_list ) {
					learn_s_list = no_branch_list;
					no_branch_list = no_branch_list->next;
					topgunFree ( learn_s_list, FMT_LEARN_S_LIST, 1, func_name ); 
				}
				break;
			}
		
			/* 対偶の場所を覚える */
			reverse_top_node = top_node->reverse;


			/* flagをたてる */
			top_node->flag |= LEARN_FLAG_DIRECT;

			//learn_type = static_learn_dag_check_family( current_node, top_node );

			/* 線をひく */
			topgun_4_utility_enter_start_time ( &Time_head.learn_add_br );
			static_learn_dag_make_direct_branch( current_node, top_node );
			topgun_4_utility_enter_end_time ( &Time_head.learn_add_br );

			if ( current_node->flag & LEARN_FLAG_CHILDREN ) {
				from_result_node = current_node->parent;
			}
			else {
				from_result_node = current_node;
			}
			
			if (top_node->flag & LEARN_FLAG_CHILDREN ) {
				to_result_node = top_node->parent;
			}
			else {
				to_result_node = top_node;
			}
			
			if ( to_result_node->flag & LEARN_FLAG_FINISH ) {
					/* すでにto_result_nodeからの探索が終了している場合 */
				if ( from_result_node->n_forward > 0 ) {
					from_result_node->n_forward--;
				}
			}
			
			dag_return->n_add_direct_node++;

			if (! ( dag_return->mode_flag & LEARN_MODE_SELECT ) ){
				if ( ! ( current_node->level > top_node->level ) ) {
					dag_return->redo_flag = 1;
				}
			}

			while ( 1 ) {
				
				/* ループがある場合の作業 */
				loop_list = static_learn_dag_loop_check_search( current_node );
			
				if ( loop_list != NULL ) {
					/* loopが成立している */
					topgun_4_utility_enter_start_time ( &Time_head.learn_loop_ope );
					
					/* loopを構成するノードを一つにマージする */
					/* とりあえずレベルの一番高いノードにマージする */
					search_tmp_list = static_learn_dag_node_merge( loop_list, current_node, 0 );

					/* ここではsearch_tmp_listは必ずNULL */

					/* ノードのマージに対応してフラグを変更する */
					static_learn_dag_reup_flag ( current_node );

				
					/* 再度ソートを仕直すトリガーをセットする */
					dag_return->redo_flag = 2;

					while ( loop_list ) {
						free_loop_list = loop_list;
						loop_list = loop_list->next;
						topgunFree ( free_loop_list, FMT_LEARN_S_LIST, 1, func_name ); 
					}
					
					topgun_4_utility_enter_end_time ( &Time_head.learn_loop_ope );

				}
				else {
					break;
				}
			}
		
			if ( dag_return->redo_flag == 0 ) {
				/* つづける場合だけやる */
				/* その先もフラグをたてる */
				static_learn_dag_up_flag ( 	top_node );
			}

			/* 対偶(間接含意)はすぐには線をひく */
			/* 覚えるんはとりあえずおぼえとく */

			if ( reverse_top_node->flag & LEARN_FLAG_CHILDREN ) {
				reverse_top_node = reverse_top_node->parent;
			}
			if ( reverse_node->flag & LEARN_FLAG_CHILDREN ) {
				reverse_node = reverse_node->parent;
			}
		
			learn_type = static_learn_dag_check_contraposition
				( reverse_top_node, reverse_node );
			//static_learn_dag_check_contraposition_down_flag( reverse_top_node );

		
			if ( learn_type == LEARN_ON ) {
				/* 枝をはる */

				from_line   = reverse_top_node->line;
				from_state3 = reverse_top_node->state3;
				to_line     = reverse_node->line;
				to_state3   = reverse_node->state3;

				learn_list = ( LEARN_LIST * )topgunMalloc
					( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					
				learn_list->line       = to_line;
				learn_list->ass_state3 = to_state3;

				if ( from_state3 == STATE3_0 ) {
					learn_list->next = from_line->imp_0;
					from_line->imp_0 = learn_list;
				}
				else if ( from_state3 == STATE3_1 ){
					learn_list->next = from_line->imp_1;
					from_line->imp_1 = learn_list;
				}

				/* 線をひく */
				static_learn_dag_make_direct_branch( reverse_top_node, reverse_node );
				static_learn_dag_make_direct_branch_print( reverse_top_node, reverse_node );

				if ( reverse_top_node->flag & LEARN_FLAG_CHILDREN ) {
					from_result_node = reverse_top_node->parent;
				}
				else {
					from_result_node = reverse_top_node;
				}			
				if ( reverse_node->flag & LEARN_FLAG_CHILDREN ) {
					to_result_node = reverse_node->parent;
				}
				else {
					to_result_node = reverse_node;
				}
				
				if ( to_result_node->flag & LEARN_FLAG_FINISH ) {
					/* すでにto_result_nodeからの探索が終了している場合 */
					if ( from_result_node->n_forward > 0 ) {
						from_result_node->n_forward--;
					}
				}


				if ( dag_return->mode_flag & LEARN_MODE_RESORT ) {
					/* RESORT modeの場合 */
					topgun_print_mes_dag_reentry( from_result_node );
					
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					learn_s_list->node = from_result_node;
					learn_s_list->next = indirect_node_list;
					indirect_node_list = learn_s_list;
				}
				else if ( dag_return->mode_flag & LEARN_MODE_REENTRY ) {
					/* REENTRY modeの場合 */
					/* 間接含意を覚えたノードを覚える */
					
					if ( from_result_node->flag & LEARN_FLAG_FINISH ) {

						from_result_node->flag &= ~LEARN_FLAG_FINISH;

						topgun_print_mes_dag_reentry( from_result_node );
					
						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
						learn_s_list->node = from_result_node;
						learn_s_list->next = indirect_node_list;
						indirect_node_list = learn_s_list;
					}
				}

				
				dag_return->n_add_direct_node++;

				if ( ! ( dag_return->mode_flag & LEARN_MODE_SELECT ) ){
					if ( ! ( reverse_top_node->level > reverse_node->level ) ) {

						if ( dag_return->redo_flag == 0 ) {
							dag_return->redo_flag = 1;
						}
					}
				}

				while ( 1 ) {
					/* ループがある場合の作業 */
					loop_list = static_learn_dag_loop_check_search( reverse_top_node );

					if ( loop_list != NULL ) {
						/* loopが成立している */
						topgun_4_utility_enter_start_time ( &Time_head.learn_loop_ope );
					
						/* loopを構成するノードを一つにマージする */
						search_tmp_list = static_learn_dag_node_merge( loop_list, reverse_top_node, 1 );

						/* search_add_listを連結する */
						if ( search_tmp_list != NULL ) {
							if ( search_add_list == NULL ) {
								search_add_list = search_tmp_list;
							}
							else {
								static_learn_dag_make_connect ( search_add_list, search_tmp_list );
								search_tmp_list = NULL;
							}
						}


						/* ノードのマージに対応してフラグを変更する */
						static_learn_dag_reup_flag ( reverse_top_node );
					
						/* 再度ソートを仕直す */
						dag_return->redo_flag = 2;
						//static_learn_dag_redo_all_topologicalsort();

						while ( loop_list ) {
							free_loop_list = loop_list;
							loop_list = loop_list->next;
							topgunFree ( free_loop_list, FMT_LEARN_S_LIST, 1, func_name ); 
						}
						topgun_4_utility_enter_end_time ( &Time_head.learn_loop_ope );
					}
					else {
						break;
					}
				}
			
				loop_list = NULL;

				if ( dag_return->redo_flag == 0 ) {
					/* つづける場合だけやる */
					no_branch_list = static_learn_dag_recheck_unreachable_list( no_branch_list );
				}
			}
		}

		/* 枝をたどり、たどった節のflagを戻す */
		static_learn_dag_down_flag ( current_node );

		test_dag_topo_graph_flag_0();
	}

	topgun_4_utility_enter_end_time ( &Time_head.learn_search );
	topgun_4_utility_enter_start_time ( &Time_head.learn_finish );
	

	/* 含意操作の結果をrefleshする */
	atpg_imptrace_reflesh();
#ifndef OLD_IMP2
#else	
	Gene_head.imp_trace = NULL;
#endif /* OLD_IMP2 */	

	/* 値割り当ての結果を元にもどす */
	//atpg_reflesh_state( ass_list.line, ass_list.condition );

	add_list = ass_list.next;

	while ( add_list ) {

		tmp_ass_list = add_list;
		add_list = add_list->next;
		topgunFree ( tmp_ass_list, FMT_ASS_LIST, 1, func_name ); 
	}
	if ( IMP_CONF == imp_result.i_state ) {
		/* fix listを作成する */
		static_learn_dag_make_fix_list( &ass_list );

	}

	/* indirect_node_listから実際に再度実施する必要のあるノードをリストアップする */
	dag_return->add_indirect_list = indirect_node_list;

	/* message for debug */
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	if ( dag_return->n_add_direct_node != 0 ) {
		printf("2-L-P n_add %5ld\n", dag_return->n_add_direct_node );
		learn_s_list = indirect_node_list;
		while( learn_s_list ) {
			printf("2-L-P add n%5ld l%5ld\n"
				   ,learn_s_list->node->node_id
				   ,learn_s_list->node->line->line_id);
			learn_s_list = learn_s_list->next;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */

	if ( ! ( dag_return->mode_flag & LEARN_MODE_RESORT ) ){
		/* 再帰的にbackward方向へLEARN_FLAG_FINISHフラグを元に戻す */
		while ( indirect_node_list ) {
			branch = indirect_node_list->node->backward;
			indirect_node_list = indirect_node_list->next;
			while ( branch ) {
			
				if ( branch->node->flag & LEARN_FLAG_FINISH ) {
				
					branch->node->flag &= ~LEARN_FLAG_FINISH;

					topgun_print_mes_dag_reentry( branch->node );
				
					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					learn_s_list->node = branch->node;
					learn_s_list->next = backward_list;
					backward_list = learn_s_list;
				
				}
				static_learn_dag_branch_count_finish( branch->node );
				branch = branch->next;
			}
		}
		while ( backward_list ) {
			learn_s_list = backward_list;
			backward_list = backward_list->next;

			branch = learn_s_list->node->backward;

			topgunFree ( learn_s_list, FMT_LEARN_S_LIST, 1, func_name ); 

			while ( branch ) {
			
				if ( branch->node->flag & LEARN_FLAG_FINISH ) {
				
					branch->node->flag &= ~LEARN_FLAG_FINISH;

					topgun_print_mes_dag_reentry_next( branch->node );

					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_LIST, sizeof( LEARN_LIST ), 1, func_name );
					learn_s_list->node = branch->node;
					learn_s_list->next = backward_list;
					backward_list = learn_s_list;
				
				}
				static_learn_dag_branch_count_finish( branch->node );
				//branch->node->n_forward++;
				branch = branch->next;
			}
		}
	}

	
	test_all_state_xx();

	topgun_4_utility_enter_end_time ( &Time_head.learn_finish );
	topgun_4_utility_enter_end_time ( &Time_head.learn_direct_node );

	return ( search_add_list );
}

/*!
  @brief 最初のトポロジカルレベルソートをする

  @param [in] void
  @return void

*/

void static_learn_dag_topo_init
(
 void
 ){


}

/*!
  @brief 枝をたどり、たどった節にflagをたてる

  @param [in] *start_node  開始するノードへのポインタ
  @return void

*/
/*
void static_learn_dag_up_flag
(
 LEARN_NODE *start_node
 ){
	
	LEARN_NODE   *current_node = NULL;
	Ulong        search_count = 0;

	//char *func_name = "static_learn_up_flag"; //関数名 
	topgun_4_utility_enter_start_time ( &Time_head.learn_upflag );

	Direct_info.count = 0;
	
	static_learn_dag_direct_enter_list( start_node );
	current_node = static_learn_dag_direct_get_list( search_count++ );
	
	while ( current_node ) {
		
		if ( ! ( current_node->flag & LEARN_FLAG_DIRECT ) ){
			current_node->flag |= LEARN_FLAG_DIRECT;

			static_learn_dag_direct_enter_list( current_node );

		}
		current_node = static_learn_dag_direct_get_list( search_count++ );
	}
	static_learn_dag_direct_delete_list();

	topgun_4_utility_enter_end_time ( &Time_head.learn_upflag );
}
*/

void static_learn_dag_up_flag
(
 LEARN_NODE *start_node
 ){
	
	LEARN_NODE   *current_node = NULL;
	Ulong        search_count = 0;
	LEARN_BRANCH *forward = NULL;
	
	//char *func_name = "static_learn_up_flag"; // 関数名 
	topgun_4_utility_enter_start_time ( &Time_head.learn_upflag );

	Direct_info.count = 0;

	forward = start_node->forward;
	while ( forward ) {
		forward->node->flag |= LEARN_FLAG_DIRECT;
		static_learn_dag_direct_enter_list_new( forward->node );
		forward = forward->next;
	}

	current_node = static_learn_dag_direct_get_list( search_count++ );
	
	while ( current_node ) {
		
		forward =  current_node->forward;

		while ( forward ) {

			if ( ! ( forward->node->flag & LEARN_FLAG_DIRECT ) ){
				forward->node->flag |= LEARN_FLAG_DIRECT;

				static_learn_dag_direct_enter_list_new( forward->node );
			}
			forward = forward->next;
		}
		current_node = static_learn_dag_direct_get_list( search_count++ );
	}
	static_learn_dag_direct_delete_list();

	topgun_4_utility_enter_end_time ( &Time_head.learn_upflag );
}


/*!
  @brief 枝をたどり、たどった節にflagをたてる

  @param [in] *start_node  開始するノードへのポインタ
  @return void

*/

void static_learn_dag_up_flag_old
(
 LEARN_NODE *start_node
 ){
	LEARN_BRANCH *initial_search = start_node->forward;
	LEARN_BRANCH *current_search = NULL;
	LEARN_BRANCH *next_stack = NULL;
	LEARN_BRANCH *new_branch;
	LEARN_BRANCH *search = NULL;


	char *func_name = "static_learn_up_flag"; /* 関数名 */
	
	while ( initial_search ) {
		current_search = initial_search;
		initial_search = initial_search->next;
		
		if ( ! ( current_search->node->flag & LEARN_FLAG_DIRECT ) ){
			current_search->node->flag |= LEARN_FLAG_DIRECT;

			next_stack = current_search->node->forward;

			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				new_branch->flag   = 0;
				
				next_stack = next_stack->next;

			}
		}
	}
	
	while ( search ) {

		current_search = search;
		search = search->next;
		
		if ( ! ( current_search->node->flag & LEARN_FLAG_DIRECT ) ) {
			current_search->node->flag |= LEARN_FLAG_DIRECT;

			next_stack = current_search->node->forward;

			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				new_branch->flag   = 0;

				
				next_stack = next_stack->next;

			}
		}
		topgunFree ( current_search, FMT_LEARN_BRANCH, 1, func_name ); 
	}
}

/*!
  @brief ノードのマージに対応してフラグを変更する

  @param [in] *parent  親になったノードへのポインタ
  @return void

*/

void static_learn_dag_reup_flag
(
 LEARN_NODE *parent 
 ){

	DAG_PARENT   flag = DAG_MADA;
	LEARN_S_LIST *children;
	
	//char *func_name = "static_learn_reup_flag"; /* 関数名 */

	children = parent->children;

	if ( parent->flag & LEARN_FLAG_DIRECT ) {
	
		while ( children ) {
			/* 悪影響ないので全部する */
			children->node->flag &= ~LEARN_FLAG_DIRECT;
			children = children->next;
		}
	}
	else {
		while ( children ) {
			if ( children->node->flag & LEARN_FLAG_DIRECT ) {
				flag = DAG_SUMI;
				children->node->flag &= ~LEARN_FLAG_DIRECT;
			}
			children = children->next;
		}
		if ( flag == DAG_SUMI ) {
			static_learn_dag_up_flag( parent );
		}
	}
}


/*!
  @brief 枝をたどり、たどった節のflagを戻す

  @param [in] *start_node  開始するノードへのポインタ
  @return void

*/
/*
void static_learn_dag_down_flag
(
 LEARN_NODE *start_node
 ){
	LEARN_NODE   *current_node = NULL;
	Ulong search_count = 0;

	//char *func_name = "static_learn_down_flag"; //関数名 

	// とりあえずupとdownは分離 
	
	// startがNO_USEの場合の対応

	topgun_4_utility_enter_start_time ( &Time_head.learn_downflag );

	Direct_info.count = 0;
	
	start_node->flag &= ~LEARN_FLAG_DIRECT;

	static_learn_dag_direct_enter_list( start_node );
	
	current_node = static_learn_dag_direct_get_list( search_count++ );

	while ( current_node ) {
		if ( current_node->flag & LEARN_FLAG_DIRECT ) {
			current_node->flag &= ~LEARN_FLAG_DIRECT;
			
			static_learn_dag_direct_enter_list( current_node );
		}
		current_node = static_learn_dag_direct_get_list( search_count++ );
	}
	static_learn_dag_direct_delete_list();
	
	test_dag_topo_graph_flag_0();

	topgun_4_utility_enter_end_time ( &Time_head.learn_downflag );
}
*/
void static_learn_dag_down_flag
(
 LEARN_NODE *start_node
 ){
	LEARN_NODE   *current_node = NULL;
	Ulong search_count = 0;
	LEARN_BRANCH *forward = NULL;

	//char *func_name = "static_learn_down_flag"; // 関数名 

	// とりあえずupとdownは分離 
	
	// startがNO_USEの場合の対応 

	topgun_4_utility_enter_start_time ( &Time_head.learn_downflag );

	Direct_info.count = 0;

	forward = start_node->forward;
	while ( forward ) {
		forward->node->flag &= ~LEARN_FLAG_DIRECT;
		static_learn_dag_direct_enter_list_new( forward->node );
		forward = forward->next;
	}
	current_node = static_learn_dag_direct_get_list( search_count++ );

	while ( current_node ) {
		forward = current_node->forward;
		while ( forward ) {
			if ( forward->node->flag & LEARN_FLAG_DIRECT ) {
				forward->node->flag &= ~LEARN_FLAG_DIRECT;
			
				static_learn_dag_direct_enter_list_new( forward->node );
			}
			forward = forward->next;
		}
		current_node = static_learn_dag_direct_get_list( search_count++ );
	}
	static_learn_dag_direct_delete_list();
	
	test_dag_topo_graph_flag_0();

	topgun_4_utility_enter_end_time ( &Time_head.learn_downflag );
}

/*!
  @brief 枝をたどり、たどった節のflagを戻す

  @param [in] *start_node  開始するノードへのポインタ
  @return void

*/

void static_learn_dag_down_flag_old
(
 LEARN_NODE *start_node
 ){
	LEARN_S_LIST *new_s_list;
	LEARN_S_LIST *search = NULL;
	LEARN_S_LIST *current_search = NULL;

	LEARN_BRANCH *next_stack = NULL;
	LEARN_BRANCH *initial_search = NULL;

	char *func_name = "static_learn_down_flag"; /* 関数名 */

	/* startがNO_USEの場合の対応 */
	initial_search = start_node->forward;
	start_node->flag &= ~LEARN_FLAG_DIRECT;

	while ( initial_search ) {
		if ( initial_search->node->flag & LEARN_FLAG_DIRECT ) {
			initial_search->node->flag &= ~LEARN_FLAG_DIRECT;
			
			next_stack = initial_search->node->forward;

			while ( next_stack ) {

				new_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
	
				new_s_list->node   = next_stack->node;
				
				new_s_list->next   = search;
				search             = new_s_list;

				
				next_stack = next_stack->next;

			}
		}
		initial_search = initial_search->next;
	}
	
	while ( search ) {

		current_search = search;
		search = search->next;
		
		if ( current_search->node->flag & LEARN_FLAG_DIRECT ) {
			current_search->node->flag &= ~LEARN_FLAG_DIRECT;

			next_stack = current_search->node->forward;

			while ( next_stack ) {

				new_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
	
				new_s_list->node   = next_stack->node;
				
				new_s_list->next   = search;
				search             = new_s_list;
				
				next_stack = next_stack->next;

			}
		}
		topgunFree ( current_search, FMT_LEARN_S_LIST, 1, func_name ); 
	}
	test_dag_topo_graph_flag_0();
}



void static_learn_dag_redo_topologicalsort
(
 LEARN_NODE *from_node,
 LEARN_NODE *to_node
){

	LEARN_S_LIST *redo_curr = NULL;
	LEARN_S_LIST *redo_top  = NULL;
	LEARN_S_LIST *redo_list = NULL;

	LEARN_S_LIST *backward_list;
	LEARN_NODE   *curr_node;

	char *func_name = "static_learn_dag_redo_topologicalsort"; /* 関数名 */

	/* いまのトポロジカルレベルリストから切り出す */
	static_learn_dag_delete_topo_level_list ( from_node );
	
	/* 新しいレベルに変更する */
	from_node->level = ( to_node->level ) + 1;

	if ( Learn_dag_info.max_level < from_node->level ) {
		Learn_dag_info.max_level = from_node->level;
	}
	
	/* 新しいレベルに挿入する */
	static_learn_dag_insert_topo_level_list ( from_node );
	//test_dag_topo_list();

	if ( from_node->flag & LEARN_FLAG_PARENT ) {
		backward_list = static_learn_dag_make_backward_list_parent ( from_node );
	}
	else {
		backward_list = static_learn_dag_make_backward_list ( from_node );
	}
	
	while ( backward_list ) {

		curr_node = backward_list->node;

		if ( ! ( from_node->level <  curr_node->level ) ){
			/* いまのトポロジカルレベルリストから切り出す */
			static_learn_dag_delete_topo_level_list ( curr_node );

			
			curr_node->level = ( from_node->level ) + 1 ;

			if ( Learn_dag_info.max_level < curr_node->level ) {
				Learn_dag_info.max_level = curr_node->level;
			}

			/* 新しいレベルに挿入する */
			static_learn_dag_insert_topo_level_list ( curr_node );
			//test_dag_topo_list();

			/* 次ぎ積む */
			if ( curr_node->flag & LEARN_FLAG_PARENT ) {
				redo_list = static_learn_dag_make_backward_list_parent ( curr_node );
			}
			else {
				redo_list = static_learn_dag_make_backward_list ( curr_node );
			}

			/* 連結する */
			if ( redo_top == NULL ) {
				redo_top = redo_list;
			}
			else {
				static_learn_dag_make_connect( redo_top, redo_list );
			}
		}
		backward_list = backward_list->next;
	}

	
	/* 2段目以降を再ソートする */
	while ( redo_top ) {

		redo_curr = redo_top;
		redo_top  = redo_top->next;

		if ( redo_curr->node->flag & LEARN_FLAG_PARENT ) {
			backward_list = static_learn_dag_make_backward_list_parent ( redo_curr->node );
		}
		else {
			backward_list = static_learn_dag_make_backward_list ( redo_curr->node );
		}
		

		while ( backward_list ) {

			curr_node = backward_list->node;
		
			if ( ! ( redo_curr->node->level < curr_node->level  ) ) {

				/* いまのトポロジカルレベルリストから切り出す */
				static_learn_dag_delete_topo_level_list ( curr_node );

				curr_node->level = ( redo_curr->node->level + 1 );

				if ( Learn_dag_info.max_level < curr_node->level ) {
					Learn_dag_info.max_level = curr_node->level;
				}

				/* 新しいレベルに挿入する */
				static_learn_dag_insert_topo_level_list ( curr_node );
				test_dag_topo_list();

				/* 次ぎ積む */
				if ( curr_node->flag & LEARN_FLAG_PARENT ) {
					redo_list = static_learn_dag_make_backward_list_parent ( curr_node );
				}
				else {
					redo_list = static_learn_dag_make_backward_list ( curr_node );
				}
				
				/* 連結する */
				if ( redo_top == NULL ) {
					redo_top = redo_list;
				}
				else {
					static_learn_dag_make_connect( redo_top, redo_list );
				}
			}
			backward_list = backward_list->next;
		}
		topgunFree( redo_curr, FMT_LEARN_S_LIST, 1, func_name );
	}
}

void static_learn_dag_redo_all_topologicalsort
(
 void
)
{

	Ulong i; //!< 信号線数
	Ulong current_level         = 0;
	LEARN_TOPO *current_topo    = NULL;
	LEARN_TOPO *learn_topo      = NULL;
	
	LEARN_NODE *node            = NULL;
	LEARN_NODE *top_node        = NULL;
	LEARN_NODE *current_node    = NULL;
	LEARN_NODE *prev_level_node = NULL;
	LEARN_NODE *back_node       = NULL;

	LEARN_BRANCH *forward       = NULL;

	LEARN_S_LIST *backward_list = NULL;
	LEARN_S_LIST *children      = NULL;
	LEARN_S_LIST *current_list  = NULL;

	LEARN_S_LIST *current_level_list = NULL;
	LEARN_S_LIST *learn_s_list  = NULL;

	Ulong mini_pi_level = FL_ULMAX;
	Ulong next_mini_level = FL_ULMAX;
	LEARN_NODE *prev_node;
	

	char *func_name = "static_learn_dag_redo_all_topologicalsort"; /* 関数名 */

	/* 以前のTopo_top以下のfree忘れあり */
	topgun_4_utility_enter_start_time ( &Time_head.learn_redo_sort );	
	
	/* n_forwardを元にもどす */
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];

		if ( node->node_id != 0 ) { /* 対象外ということ */

			node->n_forward = node->n_for_org;
			node->level = 0;
			node->prev = NULL;
			node->next = NULL;

			/* 子供から来るものは減らしとく */
#ifndef FLILGHT_TEST
			if ( node->flag & LEARN_FLAG_PARENT ) {

				node->n_forward = 0;

				children = node->children;

				/* まずflagをたてる */
				while ( children ) {

					children->node->flag |= LEARN_FLAG_CHILD_C;
					children = children->next;
				}
				
				/* チェックする */
				forward = node->forward;
				while ( forward ) {

					if ( ! ( forward->node->flag & LEARN_FLAG_CHILD_C ) ){
						node->n_forward++;
					}
					forward = forward->next;
				}

				children = node->children;

				/* flagを戻す */
				while ( children ) {

					children->node->flag &= ~LEARN_FLAG_CHILD_C;
					children = children->next;
				}
			}
#endif /* FLILGHT_TEST */
		}
		node = Node_head[ LEARN_STATE_1 ][ i ];
					
		if ( node->node_id != 0 ) { /* 対象外ということ */

			node->n_forward = node->n_for_org;
			node->level = 0;
			node->prev = NULL;
			node->next = NULL;

#ifndef	FLILGHT_TEST
			/* 子供から来るものは減らしとく */			
			if ( node->flag & LEARN_FLAG_PARENT ) {

				node->n_forward = 0;

				children = node->children;

				/* まずflagをたてる */
				while ( children ) {

					children->node->flag |= LEARN_FLAG_CHILD_C;
					children = children->next;
				}
				
				/* チェックする */
				forward = node->forward;
				while ( forward ) {

					if ( ! ( forward->node->flag & LEARN_FLAG_CHILD_C ) ){
						node->n_forward++;
					}
					forward = forward->next;
				}

				children = node->children;

				/* flagを戻す */
				while ( children ) {

					children->node->flag &= ~LEARN_FLAG_CHILD_C;
					children = children->next;
				}
			}
#endif	/* FLILGHT_TEST */
		}
	}

	current_level_list = NULL;
	
	/* 一段目のリストを作成する */
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];

		if ( node->node_id != 0 ) { /* 対象外ということ */

			if ( node->flag & LEARN_FLAG_CHILDREN ) {
				; /* 子には何もしない */
			}
			else {

				node->prev = NULL;
				node->next = NULL;

				if ( node->n_forward == 0 ) {

					node->level = Topo_top.level;
				

					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = node;
					learn_s_list->next = current_level_list;
					current_level_list = learn_s_list;
				}
			}
			node = Node_head[ LEARN_STATE_1 ][ i ];
			if ( node->flag & LEARN_FLAG_CHILDREN ) {
				; /* 子には何もしない */
			}
			else {
				node->prev = NULL;
				node->next = NULL;
				
				if ( node->n_forward == 0 ) {

					node->level = Topo_top.level;


					learn_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
					
					learn_s_list->node = node;
					learn_s_list->next = current_level_list;
					current_level_list = learn_s_list;
				}
			}
		}
	}


	current_list = current_level_list;
	mini_pi_level = 0;
	next_mini_level = FL_ULMAX;
	prev_node = NULL;

	while ( 1 ) {
	
		while ( current_list ) {

			current_node = current_list->node;
			if ( ! ( current_node->flag & LEARN_FLAG_SORT ) ){

#ifdef LEARN_PI_SORT
				if ( current_node->line->lv_pi == mini_pi_level ) {
#endif /* LEARN_PI_SORT */

					current_node->flag |= LEARN_FLAG_SORT;

					/* 双方向リストの作成 */
					current_node->prev = prev_node;
					current_node->next = NULL;
					if ( prev_node != NULL ) {
						prev_node->next = current_node;
					}
					else {
						Topo_top.top_node = current_node;
					}
					prev_node = current_node;
					
#ifdef LEARN_PI_SORT
				}
				else {
					/* まだ未処理がある　*/
					if ( current_list->node->line->lv_pi < next_mini_level  ) {
						next_mini_level = current_list->node->line->lv_pi;
					}
				}
#endif /* LEARN_PI_SORT */
				
			}
			current_list = current_list->next;
		}

		/* mini_pi_levelが終了 */

		
		current_list = current_level_list;
		
		if ( next_mini_level == FL_ULMAX ) {
			/* 全部終了レベルが終了 */
			while ( current_list ) {
				current_list->node->flag &= ~LEARN_FLAG_SORT;
				learn_s_list = current_list;
				current_list = current_list->next;
				topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
			}
			current_level_list = NULL;
			break;
		}
		else {
			mini_pi_level = next_mini_level;
			next_mini_level = FL_ULMAX;
		}
	}
	
	current_topo = &( Topo_top );

	/* 2段目以降を作成する */
	while ( 1 ) {

		prev_level_node = current_topo->top_node;
		
		/* 次のレベル用を構築する */
		top_node      = NULL;
		current_level = current_topo->level +1;

		
		current_level_list = NULL;
		
		while ( 1 ) {

			if ( prev_level_node->flag & LEARN_FLAG_PARENT ) {
				backward_list = static_learn_dag_make_backward_list_parent ( prev_level_node );
			}
			else {
				backward_list = static_learn_dag_make_backward_list ( prev_level_node );
			}

			while ( backward_list ) {
				back_node = backward_list->node;
				back_node->n_forward--;


				if ( back_node->flag & LEARN_FLAG_CHILDREN ) {
					;/* 子供は必要ない */		
				}
				else {
					if ( back_node->n_forward == 0 ) {

						back_node->level = current_level;

						learn_s_list = ( LEARN_S_LIST * )topgunMalloc
							( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
						
						learn_s_list->node = back_node;
						learn_s_list->next = current_level_list;
						current_level_list = learn_s_list;

#ifdef _DEBUG_PRT_ATPG_
						printf("2-L-R level %2ld node ID %5ld\n",back_node->level, back_node->node_id);
#endif /* _DEBUG_PRT_ATPG_ */
					}
					else {
#ifdef _DEBUG_PRT_ATPG_
						printf("2-L-N Mi    %2ld node ID %5ld\n",back_node->n_forward,back_node->node_id);
#endif /* _DEBUG_PRT_ATPG_ */
					}
				}
				current_list = backward_list;
				backward_list = backward_list->next;

				topgunFree( current_list, FMT_LEARN_S_LIST, 1, func_name );
			}

			if ( ( prev_level_node->next == NULL ) ||
				 ( prev_level_node->level != prev_level_node->next->level ) ){
				break;
			}
			else {
				prev_level_node = prev_level_node->next; 
			}
		}


		current_list = current_level_list;
		mini_pi_level = 0;
		next_mini_level = FL_ULMAX;
		top_node = NULL;
		
		while ( 1 ) {
	
			while ( current_list ) {
			
				current_node = current_list->node;
				if ( ! ( current_node->flag & LEARN_FLAG_SORT ) ){
					
#ifdef LEARN_PI_SORT					
					if ( current_node->line->lv_pi == mini_pi_level ) {
#endif /* LEARN_PI_SORT */
						
						current_node->flag |= LEARN_FLAG_SORT;

						/* 双方向リストの作成 */
						current_node->prev = prev_node;
						current_node->next = NULL;
						prev_node->next   = current_node;
						if ( top_node == NULL ) {
							top_node = current_node;
						}
						prev_node  = current_node;
#ifdef LEARN_PI_SORT						
					}
					else {
						/* まだ未処理がある　*/
						if ( current_list->node->line->lv_pi < next_mini_level  ) {
							next_mini_level = current_list->node->line->lv_pi;
						}
					}
#endif /* LEARN_PI_SORT */					
				}
				current_list = current_list->next;
			}

			/* 今のPIレベルが終了 */
			current_list = current_level_list;
		
			if ( next_mini_level == FL_ULMAX ) {
				/* 全部PIレベルが終了 */
				while ( current_list ) {
					current_list->node->flag &= ~LEARN_FLAG_SORT;
					learn_s_list = current_list;
					current_list = current_list->next;
								topgunFree( learn_s_list, FMT_LEARN_S_LIST, 1, func_name );
				}
				current_level_list = NULL;
				break;
			}
			else {
				mini_pi_level = next_mini_level;
				next_mini_level = FL_ULMAX;
			}
		}

		if ( top_node == NULL ) {
			/* 次のレベルがない == 終了 */
			/* 初期の時は有効 */
			break;
		}
		
		learn_topo = ( LEARN_TOPO * )topgunMalloc
			( FMT_LEARN_TOPO, sizeof( LEARN_TOPO ), 1, func_name );
		
		learn_topo->level        = current_level;
		learn_topo->top_node     = top_node;
		learn_topo->next_level   = NULL;
		current_topo->next_level = learn_topo;

		current_topo = current_topo->next_level;
	}
	Learn_dag_info.max_level = current_level; /* max_levelの再初期化 */

	test_dag_node_connect();

	topgun_print_mes_Topo_top();
	
	topgun_4_utility_enter_end_time ( &Time_head.learn_redo_sort );
}

void static_learn_dag_delete_topo_level_list
(
 LEARN_NODE *from_node
 ){
	LEARN_TOPO *current_topo = NULL;
	LEARN_NODE *prev = from_node->prev;
	LEARN_NODE *next = from_node->next;
	Ulong      i;

	//char *func_name = "static_learn_dag_delete_topo_level_list"; /* 関数名 */

	/* 切り出すノードはレベルが更新される前である */

	/* Topo_top のつなぎ直し */
	current_topo = & ( Topo_top );
	
	for ( i = 0 ; i < from_node->level ; i++ ){
		current_topo = current_topo->next_level;
	}

	if ( current_topo->top_node->node_id == from_node->node_id ) {

		/* 移動させるノードが先頭である場合 */
		if ( from_node->level == from_node->next->level ) {

			/* 移動させるノードの次ぎも同じレベル */
			current_topo->top_node = from_node->next;
		}
		else {
			/* 移動させるノードと同じレベルのノードはない */
			current_topo->top_node = NULL;
		}
	}


	/* リストの接続 ( 抜き ) */
	if ( ( prev == NULL ) && ( next == NULL ) ) {
		/* 何もしない */
		/* ないはず */
	}
	else if ( prev == NULL ) {
		next->prev = NULL;
	}
	else if ( next == NULL ) {

		prev->next = NULL;
	}
	else {

		prev->next = next;
		next->prev = prev;
	}
}


void static_learn_dag_insert_topo_level_list
(
 LEARN_NODE *from_node
 ){
	
	LEARN_TOPO *level_list = NULL;            //from_nodeのレベルリスト
	LEARN_TOPO *pre_level_list = &(Topo_top); //from_nodeの一つ前のレベルリスト
	LEARN_TOPO *last_top_list = NULL;
	LEARN_NODE *front_inserted_node = NULL;
	LEARN_NODE *rear_inserted_node  = NULL;
	LEARN_NODE *search_node  = NULL;
	
	Ulong      check_level = 0;
	Ulong      i = 0;

	char *func_name = "static_learn_dag_insert_topo_level_list"; /* 関数名 */

	
	/* 先頭をつなぎ直す */ /* insertなのでfrom_node->levelは必ず1以上 */

	for ( i = 0 ; i < ( from_node->level - 1 ); i++ ) {
		if ( pre_level_list->top_node != NULL ) {
			last_top_list = pre_level_list;
		}
		pre_level_list = pre_level_list->next_level;
	}
	if ( pre_level_list->top_node != NULL ) {
		last_top_list = pre_level_list;
	}

	level_list = pre_level_list->next_level;

	if ( ( level_list != NULL ) &&
		 ( level_list->top_node != NULL ) ) {

		front_inserted_node = level_list->top_node;
		rear_inserted_node  = front_inserted_node->prev;
			
		from_node->next     = front_inserted_node;
		from_node->prev     = rear_inserted_node;

		rear_inserted_node->next = from_node;
		front_inserted_node->prev = from_node;

		level_list->top_node = from_node;
	}
	else {
		
		if ( level_list == NULL ) {

			/* 新なレベルを作る */
			level_list = ( LEARN_TOPO * )topgunMalloc
				( FMT_LEARN_TOPO, sizeof( LEARN_TOPO ), 1, func_name );

			level_list->level        = from_node->level;
			level_list->top_node     = from_node;
			level_list->next_level   = NULL;

			pre_level_list->next_level = level_list;
		}

		check_level = last_top_list->top_node->level;

		search_node = last_top_list->top_node;

		while ( 1 ) {

			if ( ( search_node->next == NULL ) ||
				 ( search_node->next->level != check_level ) ){
				break;
			}
			search_node = search_node->next;
		}

		
		front_inserted_node = search_node->next;
		rear_inserted_node  = search_node;
			
		from_node->next     = front_inserted_node;
		from_node->prev     = rear_inserted_node;

		rear_inserted_node->next = from_node;
		if ( front_inserted_node != NULL ) {
			front_inserted_node->prev = from_node;
		}

		level_list->top_node = from_node;
	}
		
}

/*!
  @brief loopがあるかチェックする。またあればループのノードリストを返す

  @param [in] *start_node  開始するノードへのポインタ
  @return *loop_list ループのノードリスト(空の場合はループがない)

*/


LEARN_S_LIST *static_learn_dag_loop_check_search_old
(
 LEARN_NODE *from_node
 ){

	LEARN_BRANCH *forward   = NULL;

	LEARN_S_LIST *loop_list = NULL;
	LEARN_S_LIST *check_top  = NULL;
	LEARN_S_LIST *check_bottom = NULL;
	LEARN_S_LIST *check_list = NULL;
	LEARN_S_LIST *node_list = NULL;
	LEARN_S_LIST *current_list = NULL;
	
	LEARN_NODE   *start_node   = from_node;
	LEARN_NODE   *current_node;
	
	char *func_name = "static_learn_dag_loop_check_search"; /* 関数名 */

	/* まずloopがあるかの判定をする */

	topgun_test_node_children ( start_node, func_name );

	start_node->flag |= LEARN_FLAG_LOOP_S;
	forward = start_node->forward;

	/* parentにすべてノードを集めるので不要
	if ( start_node->flag & LEARN_FLAG_PARENT ) {
		forward = static_learn_dag_make_forward_list( start_node );
	}
	else {
		forward = start_node->forward;
	}
	*/

	/* depth first の探索　*/
	
	while ( forward ) {

		topgun_test_node_children ( forward->node, func_name );
		current_node = forward->node;
		
		if ( ! ( current_node->flag & LEARN_FLAG_LOOP_C ) ) {
			current_node->flag |= LEARN_FLAG_LOOP_C;
			
			/* 次ぎ積む */
			node_list = ( LEARN_S_LIST * )topgunMalloc
				( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				 
			node_list->node   = current_node;
			node_list->next   = check_top;
			check_top         = node_list;
			
			if ( check_bottom == NULL ) {
				check_bottom = node_list;
			}
		}
		forward = forward->next;
	}

	check_list = check_top;

	while ( check_list ) {

		current_list = check_list;
		check_bottom = NULL;
		
		forward = current_list->node->forward;

		while ( forward ) {

			topgun_test_node_children ( forward->node, func_name );
			current_node = forward->node;
		
			if ( current_node->flag & LEARN_FLAG_LOOP_S ) {
				/* ループ発生 */
				/* リストの作成 */
				loop_list = static_learn_dag_make_loop_list( current_node );
				
				break;
			}
			else if ( ! ( current_node->flag & LEARN_FLAG_LOOP_C ) ) {

				current_node->flag |= LEARN_FLAG_LOOP_C;
					
				node_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );

				/* 最後尾に追加する */
				/* まずcheck_bottomのリストを作成する */
				node_list->node  = current_node;
				node_list->next  = check_bottom;
				check_bottom     = node_list;
			}
			forward = forward->next;
		}
		if ( check_bottom == NULL ) {
			check_list = check_list->next;
		}
		else {
			static_learn_dag_make_connect ( check_list, check_bottom );
			check_list = check_list->next;
		}
		if ( loop_list != NULL ) {
			break;
		}
	}

	/* flagを戻す */
	start_node->flag &= ~LEARN_FLAG_LOOP_S;
		
	check_list = check_top;
	
	while ( check_list ) {

		current_list = check_list;
		check_list   = check_list->next;

		current_list->node->flag &= ~LEARN_FLAG_LOOP_C;

		topgunFree( current_list, FMT_LEARN_S_LIST, 1, func_name );

	}

	test_dag_graph_flag_loop_s_c();
	
	return ( loop_list );
}

/*!
  @brief loopがあるかチェックする。またあればループのノードリストを返す
         あらかじめメモリを確保しとくバージョン

  @param [in] *start_node  開始するノードへのポインタ
  @return *loop_list ループのノードリスト(空の場合はループがない)

*/


LEARN_S_LIST *static_learn_dag_loop_check_search
(
 LEARN_NODE *from_node
 ){
	/* new */
	LEARN_NODE   *start_node   = from_node;
	LEARN_NODE   *current_node = NULL;
	LEARN_BRANCH *forward      = NULL;

	LEARN_S_LIST *loop_list = NULL;           /* 返り値 : Loopがあった場合-> list
           												  Loopがない場合  -> NULL */
		
	Ulong        loop_check_id = 0;           /* Loop_head用でのチェック実行用のカウンタ */
	Ulong        loop_enter_id = 0;           /* Loop_head用への登録場所のカウンタ */
	Ulong        i = 0;                       /* カウンタ */

	/* old */

	/*

	LEARN_S_LIST *check_top  = NULL;
	LEARN_S_LIST *check_bottom = NULL;
	LEARN_S_LIST *check_list = NULL;
	LEARN_S_LIST *node_list = NULL;
	LEARN_S_LIST *current_list = NULL;

	*/
	
	//char *func_name = "static_learn_dag_loop_check_search"; /* 関数名 */

	/* まずloopがあるかの判定をする */

	topgun_4_utility_enter_start_time ( &Time_head.learn_loop_check );	

	topgun_test_node_children ( start_node, func_name );

	Loop_head[ loop_enter_id++ ] = start_node;
	start_node->flag |= LEARN_FLAG_LOOP_S;

	forward = start_node->forward;

#ifdef LEARN_WIDTH
	/* 幅優先探索になっている */
	while ( loop_enter_id != loop_check_id ) {

		forward = Loop_head[ loop_check_id++ ]->forward;

		while ( forward ) {

			topgun_test_node_children ( forward->node, func_name );
			current_node = forward->node;
		
			if ( current_node->flag & LEARN_FLAG_LOOP_S ) {
				/* ループ発生 */
				/* リストの作成 */
				loop_list = static_learn_dag_make_loop_list( current_node );
				
				break;
			}
			if ( ! ( current_node->flag & LEARN_FLAG_LOOP_C ) ) {

				current_node->flag |= LEARN_FLAG_LOOP_C;

				/* 最後尾に追加する */
				Loop_head[ loop_enter_id++ ] = current_node;
				
			}
			forward = forward->next;
		}
		if ( loop_list != NULL ) {
			break;
		}
	}
#else
	/* 深さ優先探索にしたい */
	while ( loop_enter_id != loop_check_id ) {

		forward = Loop_head[ loop_check_id++ ]->forward;

		while ( forward ) {

			topgun_test_node_children ( forward->node, func_name );
			current_node = forward->node;
		
			if ( current_node->flag & LEARN_FLAG_LOOP_S ) {
				/* ループ発生 */
				/* リストの作成 */
				loop_list = static_learn_dag_make_loop_list( current_node );
				
				break;
			}
			if ( ! ( current_node->flag & LEARN_FLAG_LOOP_C ) ) {

				current_node->flag |= LEARN_FLAG_LOOP_C;

				/* 最後尾に追加する */
				Loop_head[ loop_enter_id++ ] = current_node;
				
			}
			forward = forward->next;
		}
		if ( loop_list != NULL ) {
			break;
		}
	}
#endif /* LEARN_WIDTH */

	/* flagを戻す＠start_node */
	Loop_head[ 0 ]->flag &= ~LEARN_FLAG_LOOP_S;
	
	for ( i = 1 ; i < loop_enter_id ; i++ ) {
		/* flagを戻す */
		Loop_head[ i ]->flag &= ~LEARN_FLAG_LOOP_C;

	}

	//test_dag_graph_flag_loop_s_c();

	topgun_4_utility_enter_end_time ( &Time_head.learn_loop_check );
	
	return ( loop_list );
}


/*!
  @brief loopのノードリストを返す

  @param [in] *start_node  開始するノードへのポインタ
  @return *loop_list   ループのノードリスト

*/

LEARN_S_LIST *static_learn_dag_make_loop_list
(
 LEARN_NODE *start_node
 ){

	LEARN_S_LIST    *loop_list; /* 返り値のポインタ */
	LEARN_S_LIST    *node_list; /* 返り値のポインタ */

	LEARN_S_LIST    *back;
	LEARN_S_LIST    *top_list = NULL;
	LEARN_S_LIST    *add_back = NULL;
		
	LEARN_NODE      *node;
	
	char *func_name = "static_learn_dag_make_loop_list"; /* 関数名 */
	
	topgun_4_utility_enter_start_time ( &Time_head.learn_loop_mk );
	
	topgun_print_mes_char( "2-L-L Loop START " );
	topgun_print_mes_ulong_2( start_node->node_id );
	topgun_print_mes_ulong_2( start_node->level );
	topgun_print_mes_type( start_node->line );
	topgun_print_mes_id( start_node->line );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_state3_only( start_node->state3);
	topgun_print_mes_n();

	/* parentにすべてノードを集めるので不要
	if ( start_node->flag & LEARN_FLAG_PARENT ) {
		back = static_learn_dag_make_backward_list_parent ( start_node );
	}
	else {
		back = static_learn_dag_make_backward_list ( start_node );
	}
	*/
	back = static_learn_dag_make_backward_list ( start_node );
	top_list = back;

		
	node_list = ( LEARN_S_LIST * )topgunMalloc
		( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );

	node_list->node   = start_node;
	node_list->next   = NULL;

	loop_list = node_list;
	
	while ( back ) {
		
		node = back->node;

		topgun_test_node_children ( node, func_name );
		
		if ( ( node->flag & LEARN_FLAG_LOOP_C ) &&
			 ( ! ( node->flag & LEARN_FLAG_LOOP_R ) ) ){

			topgun_print_mes_char( "2-L-L Loop NEXT  " );
			topgun_print_mes_ulong_2( node->node_id );
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n();			
			
			node->flag |= LEARN_FLAG_LOOP_R;
			
			node_list = ( LEARN_S_LIST * )topgunMalloc
				( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );

			node_list->node   = node;
			node_list->next   = loop_list;
			loop_list         = node_list;

			/* parentにすべてノードを集めるので不要
			if ( node->flag & LEARN_FLAG_PARENT ) {
				add_back = static_learn_dag_make_backward_list_parent ( node );
			}
			else {
				add_back = static_learn_dag_make_backward_list ( node );
			}
			*/
			add_back = static_learn_dag_make_backward_list ( node );
			
			/* 連結する */
			static_learn_dag_make_connect( back, add_back );
		}
		else if ( node->flag & LEARN_FLAG_LOOP_S ) {

			topgun_print_mes_char( "2-L-L Loop END   " );
			topgun_print_mes_ulong_2( node->node_id );
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n();
			
			break;
		}
		else {
			;
		}
		back = back->next;
	}

	/* フラグをたおす */

	back = top_list;
	while ( back ) {

		node = back->node;

		topgun_test_node_children( node, func_name );

		if ( ( node->flag & LEARN_FLAG_LOOP_C ) &&
			 ( node->flag & LEARN_FLAG_LOOP_R ) ){

			node->flag &= ~LEARN_FLAG_LOOP_R;

		}

		node_list = back;
		back = back->next;

		topgunFree ( node_list, FMT_LEARN_S_LIST, 1, func_name ); 
	}		

	if ( loop_list->next == NULL ) {
		printf("GYA!\n");
		topgun_error( FEC_PRG_LEARN_DAG, func_name );
	}


	/*
	node_list = loop_list;
	while ( node_list ) {

		node_list = node_list->next;
	}
	*/

	topgun_4_utility_enter_end_time ( &Time_head.learn_loop_mk );
	
	return ( loop_list );
}


/*!
  @brief loop中のノードをマージする

  @param [in] *loop_list ループを構成するノードリスト
  @return void

*/

LEARN_S_LIST *static_learn_dag_node_merge
(
 LEARN_S_LIST *loop_list,
 LEARN_NODE *merge_node,
 Ulong mode_flag 
 ){
	LEARN_S_LIST *add_list        = NULL;
	LEARN_S_LIST *all_add_list    = NULL;
	LEARN_S_LIST *add_list_search = NULL;
	LEARN_S_LIST *check_list      = NULL;
	LEARN_S_LIST *prev            = NULL;

	LEARN_S_LIST *child_list       = NULL;
	
	LEARN_S_LIST2 *children_list  = NULL;

	LEARN_B_LIST *new_b_list      = NULL;
	LEARN_B_LIST *forward_list    = NULL;
	LEARN_B_LIST *backward_list   = NULL;
	
	LEARN_S_LIST2 *new_s_list2  = NULL;

	LEARN_S_LIST *search_add_list = NULL;

	LEARN_NODE   *parent_node     = NULL;
	LEARN_BRANCH *back;

	Ulong        parent_level;

	Ulong        current_mode_flag = 1; /* 初期値(必ず上書きしてから使用) */

	char *func_name = "static_learn_dag_node_merge"; /* 関数名 */

	
	/* どのノードにマージするかを决める */
	/* ->枝の根本に固定する */
	/* ->多数のループがあった時に便利 */
	parent_node = merge_node;
	parent_node->flag |= LEARN_FLAG_PARENT;
	parent_level = merge_node->level;

	/* node->parent, node->childrenを更新する */
	parent_node->flag |= LEARN_FLAG_PARENT;
	all_add_list = NULL;
	check_list = loop_list;
	prev       = NULL;

	while ( check_list ) {

		topgun_print_mes_node( check_list->node );
		
		topgun_test_node_children ( check_list->node, func_name );

		/* 子供になるノードの枝を集める */

		new_b_list = ( LEARN_B_LIST * )topgunMalloc
			( FMT_LEARN_B_LIST, sizeof( LEARN_B_LIST ), 1, func_name );
		
		new_b_list->b_list = check_list->node->forward;
		new_b_list->next   = forward_list;
		forward_list       = new_b_list;

		check_list->node->n_for_org = 0;
		check_list->node->n_forward = 0;
		check_list->node->forward = NULL; 


		new_b_list = ( LEARN_B_LIST * )topgunMalloc
			( FMT_LEARN_B_LIST, sizeof( LEARN_B_LIST ), 1, func_name );
		
		new_b_list->b_list = check_list->node->backward;
		new_b_list->next   = backward_list;
		backward_list      = new_b_list;

		check_list->node->backward = NULL;
		
		
		if ( check_list->node->flag & LEARN_FLAG_PARENT ) {
			/* 既に親であった */

			new_s_list2 = ( LEARN_S_LIST2 * )topgunMalloc
				( FMT_LEARN_S_LIST2, sizeof( LEARN_S_LIST2 ), 1, func_name );
			
			new_s_list2->s_list = check_list->node->children;
			new_s_list2->next   = children_list;
			children_list       = new_s_list2;

			add_list_search = check_list->node->children;
			check_list->node->children = NULL;

			/* 親の子供も新しい親にする */
			
			while ( add_list_search ) {
				/* 新しい親にかえる */
				add_list_search->node->parent = parent_node;
				add_list_search = add_list_search->next;
			}

			
			if ( check_list->node->node_id != parent_node->node_id ) {
				/* 新しい親でない */
				check_list->node->flag &= ~LEARN_FLAG_PARENT;
				check_list->node->flag |= LEARN_FLAG_CHILDREN;
				check_list->node->flag &= ~LEARN_FLAG_DIRECT;
				check_list->node->parent = parent_node;

				add_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				add_list->node = check_list->node;
				add_list->next = child_list;

				child_list = add_list;
			}
		}
		else{
			/* 初めて子になった */
			check_list->node->flag |= LEARN_FLAG_CHILDREN;
			check_list->node->flag &= ~LEARN_FLAG_DIRECT;
			check_list->node->parent = parent_node;

			add_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
			add_list->node = check_list->node;
			add_list->next = child_list;

			child_list     = add_list;
		}
		check_list = check_list->next;
	}

	new_s_list2 = ( LEARN_S_LIST2 * )topgunMalloc
		( FMT_LEARN_S_LIST2, sizeof( LEARN_S_LIST2 ), 1, func_name );
			
	new_s_list2->s_list = child_list;
	new_s_list2->next   = children_list;
	children_list       = new_s_list2;

	

	/* forward, backward, childrenの重複をはぶく */
	
	/* 先にchildren */
	parent_node->children = static_learn_dag_merge_s_list ( children_list, parent_node );
	children_list = NULL;
	
	/* forward */
	parent_node->forward   = static_learn_dag_merge_b_list ( forward_list, parent_node );

	/* backward */
	parent_node->backward   = static_learn_dag_merge_b_list ( backward_list, parent_node );

	/* n_forward */
	static_learn_dag_branch_count_finish ( parent_node );
	/* n_for_org */
	parent_node->n_for_org = static_learn_dag_branch_count ( parent_node->forward );
	forward_list = NULL;
	backward_list = NULL;

	static_learn_dag_delete_reverse_branch ( parent_node );

	back = parent_node->backward;

	if (! ( parent_node->flag & LEARN_FLAG_FINISH ) ){
		/* 条件2が不成立 */
		mode_flag = 0;
	}
	
	while ( back ) {

		/* 現在調べているbackに対するmode_flag */
		current_mode_flag = mode_flag;
		
#ifdef _DEBUG_PRT_ATPG_
		if ( parent_node->flag & LEARN_FLAG_FINISH ) {
			printf("M_F");
		}
		else {
			printf("M_M");
		}
		if ( back->node->flag & LEARN_FLAG_FINISH ) {
			/* 条件3が不成立 */
			printf("_F ");
		}
		else {
			/* 条件3が成立 */
			printf("_M ");
		}
#endif /* _DEBUG_PRT_ATPG_ */

		if ( back->node->flag & LEARN_FLAG_FINISH ) {
			/* 条件3が不成立 */
			current_mode_flag = 0;
		}


		if ( back->node->flag & LEARN_FLAG_ONLIST ) {
			current_mode_flag = 0;
		}

		/*
		  ブランチの追加によるループの発生に伴い、
		  間接含意の調査を実行すべきノードが、
		  通常の探索方法※では実施されない。

		  次の条件がすべて成立した場合に、問題が発生する
		  対策として、間接含意の探索実行リストを作成する
		  
		  1. 間接含意として学習した枝によってループが発生している
		  2. 親ノードが処理済みである
		  3. 子ノードが未処理である
		  4. 子ノードのn_forward値は開始前に1以上である
		  5. 子ノードのn_forward値は終了後に0である
		*/
		/*
		  ※通常の探索方法
		  1. 出力が1つもないノードを選択し、実行リストへ登録する
		  2. 実行リストが空でないのであれば、実行リストから一つ選択する。
             もし実行リストが空であれば、終了する
		  3. 選択したノードから間接含意の探索を実施する
		  4. 選択したノードへの枝を持つノードの入力数を一つ減らす
		  5. 選択したノードへの枝を持つノードの入力数が0であれば、
		     実行リストへ登録する
		  6. 2.へ戻る
		*/

		if ( back->node->n_forward == 0 ) {
			/* 条件4が不成立 */
			current_mode_flag = 0;
		}
		
		static_learn_dag_branch_count_finish ( back->node );

		if ( back->node->n_forward != 0 ) {
			/* 条件5が不成立 */
			current_mode_flag = 0;
		}

		if ( current_mode_flag == 1 ) {
			/* 条件1〜5がすべて成立 */
			topgun_print_mes_dag_illegal_node( back->node );

			add_list = ( LEARN_S_LIST * )topgunMalloc
				( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
			
			add_list->node  = back->node;
			add_list->next  = search_add_list;
			search_add_list = add_list;			
		}
		
		back = back->next;
	}

	topgun_print_mes_node( parent_node );

	{
		LEARN_BRANCH *branch;
		branch = parent_node->forward;
		while ( branch ) {
			topgun_print_mes_node( branch->node );
			branch = branch->next;
		}
		branch = parent_node->backward;
		while ( branch ) {
			topgun_print_mes_node( branch->node );
			branch = branch->next;
		}
	}
	
	
	topgun_print_mes ( 12, 11 );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( merge_node->level );
	topgun_print_mes_type( merge_node->line );
	topgun_print_mes_id( merge_node->line );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_state3_only( merge_node->state3 );
	topgun_print_mes_n();

#ifdef _DEBUG_PRT_ATPG_

	{
		LEARN_S_LIST *current_list    = NULL;

		current_list = parent_node->children;
		while ( current_list ) {
			if ( current_list->node != NULL ) {
				if ( current_list->node->parent != NULL ) {
					if ( current_list->node->parent->node_id != parent_node->node_id ){
						printf("2-L-E bad parent\n");
					}
				}
				else{
					printf("2-L-E no parent\n");
				}
			}
			else {
				printf("2-L-E no parent node\n");
			}
			current_list = current_list->next;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */

	return ( search_add_list );
}

/*!
  @brief loop中のノードをマージする

  @param [in] *loop_list ループを構成するノードリスト
  @return void

*/

void static_learn_dag_node_merge_new
(
 LEARN_S_LIST *loop_list,
 LEARN_NODE *merge_node,
 Ulong mode_flag 
 ){
	LEARN_S_LIST *add_list        = NULL;
	LEARN_S_LIST *all_add_list    = NULL;
	LEARN_S_LIST *add_list_search = NULL;
	LEARN_S_LIST *check_list      = NULL;
	LEARN_S_LIST *prev            = NULL;

	LEARN_S_LIST *child_list       = NULL;
	
	LEARN_S_LIST2 *children_list  = NULL;

	LEARN_B_LIST *new_b_list      = NULL;
	LEARN_B_LIST *forward_list    = NULL;
	LEARN_B_LIST *backward_list   = NULL;
	
	LEARN_S_LIST2 *new_s_list2  = NULL;

	LEARN_NODE   *parent_node     = NULL;
	LEARN_BRANCH *back;

	Ulong        parent_level;

	Ulong        current_mode_flag = 1; /* 初期値(必ず上書きしてから使用) */

	char *func_name = "static_learn_dag_node_merge"; /* 関数名 */

	
	/* どのノードにマージするかを决める */
	/* ->枝の根本に固定する */
	/* ->多数のループがあった時に便利 */
	parent_node = merge_node;
	parent_node->flag |= LEARN_FLAG_PARENT;
	parent_level = merge_node->level;

	/* node->parent, node->childrenを更新する */
	parent_node->flag |= LEARN_FLAG_PARENT;
	all_add_list = NULL;
	check_list = loop_list;
	prev       = NULL;

	while ( check_list ) {

		topgun_print_mes_node( check_list->node );
		
		topgun_test_node_children ( check_list->node, func_name );

		/* 子供になるノードの枝を集める */

		new_b_list = ( LEARN_B_LIST * )topgunMalloc
			( FMT_LEARN_B_LIST, sizeof( LEARN_B_LIST ), 1, func_name );
		
		new_b_list->b_list = check_list->node->forward;
		new_b_list->next   = forward_list;
		forward_list       = new_b_list;

		check_list->node->n_for_org = 0;
		check_list->node->n_forward = 0;
		check_list->node->forward = NULL; 


		new_b_list = ( LEARN_B_LIST * )topgunMalloc
			( FMT_LEARN_B_LIST, sizeof( LEARN_B_LIST ), 1, func_name );
		
		new_b_list->b_list = check_list->node->backward;
		new_b_list->next   = backward_list;
		backward_list      = new_b_list;

		check_list->node->backward = NULL;
		
		
		if ( check_list->node->flag & LEARN_FLAG_PARENT ) {
			/* 既に親であった */

			new_s_list2 = ( LEARN_S_LIST2 * )topgunMalloc
				( FMT_LEARN_S_LIST2, sizeof( LEARN_S_LIST2 ), 1, func_name );
			
			new_s_list2->s_list = check_list->node->children;
			new_s_list2->next   = children_list;
			children_list       = new_s_list2;

			add_list_search = check_list->node->children;
			check_list->node->children = NULL;

			/* 親の子供も新しい親にする */
			
			while ( add_list_search ) {
				/* 新しい親にかえる */
				add_list_search->node->parent = parent_node;
				add_list_search = add_list_search->next;
			}

			
			if ( check_list->node->node_id != parent_node->node_id ) {
				/* 新しい親でない */
				check_list->node->flag &= ~LEARN_FLAG_PARENT;
				check_list->node->flag |= LEARN_FLAG_CHILDREN;
				check_list->node->flag &= ~LEARN_FLAG_DIRECT;
				check_list->node->parent = parent_node;

				add_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				add_list->node = check_list->node;
				add_list->next = child_list;

				child_list = add_list;
			}
		}
		else{
			/* 初めて子になった */
			check_list->node->flag |= LEARN_FLAG_CHILDREN;
			check_list->node->flag &= ~LEARN_FLAG_DIRECT;
			check_list->node->parent = parent_node;

			add_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
			add_list->node = check_list->node;
			add_list->next = child_list;

			child_list     = add_list;
		}
		check_list = check_list->next;
	}

	new_s_list2 = ( LEARN_S_LIST2 * )topgunMalloc
		( FMT_LEARN_S_LIST2, sizeof( LEARN_S_LIST2 ), 1, func_name );
			
	new_s_list2->s_list = child_list;
	new_s_list2->next   = children_list;
	children_list       = new_s_list2;

	

	/* forward, backward, childrenの重複をはぶく */
	
	/* 先にchildren */
	parent_node->children = static_learn_dag_merge_s_list ( children_list, parent_node );
	children_list = NULL;
	
	/* forward */
	parent_node->forward   = static_learn_dag_merge_b_list ( forward_list, parent_node );

	/* backward */
	parent_node->backward   = static_learn_dag_merge_b_list ( backward_list, parent_node );

	/* n_forward */
	static_learn_dag_branch_count_finish ( parent_node );
	/* n_for_org */
	parent_node->n_for_org = static_learn_dag_branch_count ( parent_node->forward );
	forward_list = NULL;
	backward_list = NULL;

	static_learn_dag_delete_reverse_branch ( parent_node );

	back = parent_node->backward;

	if (! ( parent_node->flag & LEARN_FLAG_FINISH ) ){
		/* 条件2が不成立 */
		mode_flag = 0;
	}
	
	while ( back ) {

		/* 現在調べているbackに対するmode_flag */
		current_mode_flag = mode_flag;
		
#ifdef _DEBUG_PRT_ATPG_
		if ( parent_node->flag & LEARN_FLAG_FINISH ) {
			printf("M_F");
		}
		else {
			printf("M_M");
		}
		if ( back->node->flag & LEARN_FLAG_FINISH ) {
			/* 条件3が不成立 */
			printf("_F ");
		}
		else {
			/* 条件3が成立 */
			printf("_M ");
		}
#endif /* _DEBUG_PRT_ATPG_ */

		if ( back->node->flag & LEARN_FLAG_FINISH ) {
			/* 条件3が不成立 */
			current_mode_flag = 0;
		}


		if ( back->node->flag & LEARN_FLAG_ONLIST ) {
			current_mode_flag = 0;
		}

		/*
		  ブランチの追加によるループの発生に伴い、
		  間接含意の調査を実行すべきノードが、
		  通常の探索方法※では実施されない。

		  次の条件がすべて成立した場合に、問題が発生する
		  対策として、間接含意の探索実行リストを作成する
		  
		  1. 間接含意として学習した枝によってループが発生している
		  2. 親ノードが処理済みである
		  3. 子ノードが未処理である
		  4. 子ノードのn_forward値は開始前に1以上である
		  5. 子ノードのn_forward値は終了後に0である
		*/
		/*
		  ※通常の探索方法
		  1. 出力が1つもないノードを選択し、実行リストへ登録する
		  2. 実行リストが空でないのであれば、実行リストから一つ選択する。
             もし実行リストが空であれば、終了する
		  3. 選択したノードから間接含意の探索を実施する
		  4. 選択したノードへの枝を持つノードの入力数を一つ減らす
		  5. 選択したノードへの枝を持つノードの入力数が0であれば、
		     実行リストへ登録する
		  6. 2.へ戻る
		*/

		if ( back->node->n_forward == 0 ) {
			/* 条件4が不成立 */
			current_mode_flag = 0;
		}
		
		static_learn_dag_branch_count_finish ( back->node );

		if ( back->node->n_forward != 0 ) {
			/* 条件5が不成立 */
			current_mode_flag = 0;
		}

		if ( current_mode_flag == 1 ) {
			/* 条件1〜5がすべて成立 */
			topgun_print_mes_dag_illegal_node( back->node );

			static_learn_dag_search_enter_list ( back->node );

		}
		
		back = back->next;
	}

	/*
	topgun_print_mes_node( parent_node );
	{
		LEARN_BRANCH *branch;
		branch = parent_node->forward;
		while ( branch ) {
			topgun_print_mes_node( branch->node );
			branch = branch->next;
		}
		branch = parent_node->backward;
		while ( branch ) {
			topgun_print_mes_node( branch->node );
			branch = branch->next;
		}
	}
	*/
	
	
	topgun_print_mes ( 12, 11 );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( merge_node->level );
	topgun_print_mes_type( merge_node->line );
	topgun_print_mes_id( merge_node->line );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_state3_only( merge_node->state3 );
	topgun_print_mes_n();

#ifdef _DEBUG_PRT_ATPG_

	{
		LEARN_S_LIST *current_list    = NULL;

		current_list = parent_node->children;
		while ( current_list ) {
			if ( current_list->node != NULL ) {
				if ( current_list->node->parent != NULL ) {
					if ( current_list->node->parent->node_id != parent_node->node_id ){
						printf("2-L-E bad parent\n");
					}
				}
				else{
					printf("2-L-E no parent\n");
				}
			}
			else {
				printf("2-L-E no parent node\n");
			}
			current_list = current_list->next;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
	
}


/*!
  @brief 対偶が必要か否か判定する 

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/
/*
LEARN_TYPE static_learn_dag_check_contraposition
(
 LEARN_NODE *start_node,
 LEARN_NODE *end_node
 ){

	LEARN_NODE   *current_node = NULL;

	LEARN_TYPE   learn_type = LEARN_ON;
	Ulong        search_count = 0;

	//char *func_name = "static_learn_check_contraposition"; //関数名

	topgun_4_utility_enter_start_time ( &Time_head.learn_contra );
	
	end_node->flag |= LEARN_FLAG_CONTRA_E;


	static_learn_dag_contra_enter_list( start_node );

	current_node = static_learn_dag_contra_get_list( search_count++ );

	while( current_node ) {

		if ( current_node->flag & LEARN_FLAG_CONTRA_E ) {
			learn_type = LEARN_OFF;

			break;
		}
		if ( ! ( current_node->flag & LEARN_FLAG_CONTRA ) ){
			current_node->flag |= LEARN_FLAG_CONTRA;
			
			static_learn_dag_contra_enter_list( current_node );
			
		}
		current_node = static_learn_dag_contra_get_list( search_count++ );
	}
	static_learn_dag_contra_delete_list();
		
	end_node->flag &= ~LEARN_FLAG_CONTRA_E;

	topgun_4_utility_enter_end_time ( &Time_head.learn_contra );
	
	return ( learn_type );
}
*/

LEARN_TYPE static_learn_dag_check_contraposition
(
 LEARN_NODE *start_node,
 LEARN_NODE *end_node
 ){

	LEARN_NODE   *current_node = NULL;

	LEARN_TYPE   learn_type = LEARN_ON;
	LEARN_BRANCH *forward = NULL;
	Ulong        search_count = 0;
	
	//char *func_name = "static_learn_check_contraposition"; // 関数名

	topgun_4_utility_enter_start_time ( &Time_head.learn_contra );
	
	end_node->flag |= LEARN_FLAG_CONTRA_E;

	forward = start_node->forward;
	while ( forward ) {
		forward->node->flag |= LEARN_FLAG_CONTRA;
		static_learn_dag_contra_enter_list_new( forward->node );
		forward = forward->next;
	}

	current_node = static_learn_dag_contra_get_list( search_count++ );

	while( current_node ) {


		forward = current_node->forward;
		
		while ( forward ) {

			if ( forward->node->flag & LEARN_FLAG_CONTRA_E ) {
				learn_type = LEARN_OFF;

				break;
			}
			if ( ! ( forward->node->flag & LEARN_FLAG_CONTRA ) ){
				forward->node->flag |= LEARN_FLAG_CONTRA;
			
				static_learn_dag_contra_enter_list_new( forward->node );
			
			}
			forward = forward->next;
		}

		if ( learn_type == LEARN_OFF ) {
			break;
		}
		current_node = static_learn_dag_contra_get_list( search_count++ );
	}
	static_learn_dag_contra_delete_list();
		
	end_node->flag &= ~LEARN_FLAG_CONTRA_E;

	topgun_4_utility_enter_end_time ( &Time_head.learn_contra );
	
	return ( learn_type );
}


/*!
  @brief 対偶が必要か否か判定する 

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

LEARN_TYPE static_learn_dag_check_contraposition_old
(
 LEARN_NODE *start_node,
 LEARN_NODE *end_node
 ){

	LEARN_BRANCH *initial_search = NULL;
	LEARN_BRANCH *current_search = NULL;
	LEARN_BRANCH *next_stack = NULL;
	LEARN_BRANCH *next_stack_top = NULL;
	LEARN_BRANCH *new_branch;
	LEARN_BRANCH *search = NULL;

	LEARN_TYPE   learn_type = LEARN_ON;

	char *func_name = "static_learn_dag_check_contraposition"; /* 関数名 */
	
	end_node->flag |= LEARN_FLAG_CONTRA_E;

	while ( initial_search ) {
		current_search = initial_search;
		initial_search = initial_search->next;

		if ( current_search->node->flag & LEARN_FLAG_CONTRA_E ) {
			learn_type = LEARN_OFF;
			
			break;
		}
			
		if ( ! ( current_search->node->flag & LEARN_FLAG_CONTRA ) ){
			current_search->node->flag |= LEARN_FLAG_CONTRA;

			next_stack = current_search->node->forward;
			next_stack_top = NULL;
			
			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				
				next_stack = next_stack->next;

			}
			next_stack = next_stack_top;
			while ( next_stack ) {
				new_branch = next_stack;
				next_stack = next_stack->next;
				topgunFree ( new_branch, FMT_LEARN_BRANCH, 1, func_name );
			}
		}
	}


	while ( search ) {

		current_search = search;
		search = search->next;
		
		if ( current_search->node->flag & LEARN_FLAG_CONTRA_E ) {
			learn_type = LEARN_OFF;

			topgunFree ( current_search, FMT_LEARN_BRANCH, 1, func_name ); 

			while ( search ) {
				current_search = search;
				search = search->next;
			}
			break;
		}

		if ( ! ( current_search->node->flag & LEARN_FLAG_CONTRA ) ) {
			current_search->node->flag |= LEARN_FLAG_CONTRA;

			next_stack = current_search->node->forward;
			next_stack_top = NULL;

			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				
				next_stack = next_stack->next;

			}
			next_stack = next_stack_top;
			while ( next_stack ) {
				new_branch = next_stack;
				next_stack = next_stack->next;
				topgunFree ( new_branch, FMT_LEARN_BRANCH, 1, func_name ); 
			}
		}
		topgunFree ( current_search, FMT_LEARN_BRANCH, 1, func_name ); 
	}
	
	end_node->flag &= ~LEARN_FLAG_CONTRA_E;
	return ( learn_type );
}

/*!
  @brief 対偶が必要か否か判定に用いたフラグを元にもどす

  @param [in] *start_node  開始するノードへのポインタ
  @return void

*/

void static_learn_dag_check_contraposition_down_flag
(
 LEARN_NODE *start_node
 ){

	LEARN_BRANCH *initial_search;
	LEARN_BRANCH *initial_search_top = NULL;
	LEARN_BRANCH *current_search = NULL;
	LEARN_BRANCH *next_stack = NULL;
	LEARN_BRANCH *next_stack_top = NULL;
	LEARN_BRANCH *new_branch;
	LEARN_BRANCH *search = NULL;

	char *func_name = "static_learn_dag_check_contrapositoin_down_flag"; /* 関数名 */

	if( start_node->flag & LEARN_FLAG_PARENT ) {
		initial_search = static_learn_dag_make_forward_list( start_node );
		initial_search_top = initial_search;
	}
	else {
		initial_search = start_node->forward;
		initial_search_top = NULL;
	}

	/* initial_searchに対して */
	while ( initial_search ) {
		current_search = initial_search;
		initial_search = initial_search->next;

		if ( current_search->node->flag & LEARN_FLAG_CONTRA ) {
			current_search->node->flag &= ~LEARN_FLAG_CONTRA;


			if( start_node->flag & LEARN_FLAG_PARENT ) {
				next_stack = static_learn_dag_make_forward_list( current_search->node );
				next_stack_top = next_stack;
			}
			else {
				next_stack = current_search->node->forward;
				next_stack_top = NULL;
			}
			
			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				
				next_stack = next_stack->next;

			}
			next_stack = next_stack_top;
			while ( next_stack ) {
				new_branch = next_stack;
				next_stack = next_stack->next;
				topgunFree ( new_branch, FMT_LEARN_BRANCH, 1, func_name ); 
			}
		}
	}
	initial_search = initial_search_top;
	while ( initial_search ) {
		current_search = initial_search;
		initial_search = initial_search->next;
		topgunFree ( current_search, FMT_LEARN_BRANCH, 1, func_name );
	}


	while ( search ) {

		current_search = search;
		search = search->next;
		
		if ( current_search->node->flag & LEARN_FLAG_CONTRA ) {
			current_search->node->flag &= ~LEARN_FLAG_CONTRA;


			if( start_node->flag & LEARN_FLAG_PARENT ) {
				next_stack = static_learn_dag_make_forward_list( current_search->node );
				next_stack_top = next_stack;
			}
			else {
				next_stack = current_search->node->forward;
				next_stack_top = NULL;
			}

			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				
				next_stack = next_stack->next;

			}
			next_stack = next_stack_top;
			while ( next_stack ) {
				new_branch = next_stack;
				next_stack = next_stack->next;
				topgunFree ( new_branch, FMT_LEARN_BRANCH, 1, func_name ); 
			}
		}
		topgunFree ( current_search, FMT_LEARN_BRANCH, 1, func_name ); 
	}
}

/*!
  @brief 対偶が必要か否か判定に用いたフラグを元にもどす

  @param [in] *start_node  開始するノードへのポインタ
  @return void

*/

void static_learn_dag_check_contraposition_down_flag_old
(
 LEARN_NODE *start_node
 ){

	LEARN_BRANCH *initial_search;
	LEARN_BRANCH *initial_search_top = NULL;
	LEARN_BRANCH *current_search = NULL;
	LEARN_BRANCH *next_stack = NULL;
	LEARN_BRANCH *next_stack_top = NULL;
	LEARN_BRANCH *new_branch;
	LEARN_BRANCH *search = NULL;

	char *func_name = "static_learn_dag_check_contrapositoin_down_flag"; /* 関数名 */

	if( start_node->flag & LEARN_FLAG_PARENT ) {
		initial_search = static_learn_dag_make_forward_list( start_node );
		initial_search_top = initial_search;
	}
	else {
		initial_search = start_node->forward;
		initial_search_top = NULL;
	}

	/* initial_searchに対して */
	while ( initial_search ) {
		current_search = initial_search;
		initial_search = initial_search->next;

		if ( current_search->node->flag & LEARN_FLAG_CONTRA ) {
			current_search->node->flag &= ~LEARN_FLAG_CONTRA;


			if( start_node->flag & LEARN_FLAG_PARENT ) {
				next_stack = static_learn_dag_make_forward_list( current_search->node );
				next_stack_top = next_stack;
			}
			else {
				next_stack = current_search->node->forward;
				next_stack_top = NULL;
			}
			
			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				
				next_stack = next_stack->next;

			}
			next_stack = next_stack_top;
			while ( next_stack ) {
				new_branch = next_stack;
				next_stack = next_stack->next;
				topgunFree ( new_branch, FMT_LEARN_BRANCH, 1, func_name ); 
			}
		}
	}
	initial_search = initial_search_top;
	while ( initial_search ) {
		current_search = initial_search;
		initial_search = initial_search->next;
		topgunFree ( current_search, FMT_LEARN_BRANCH, 1, func_name );
	}


	while ( search ) {

		current_search = search;
		search = search->next;
		
		if ( current_search->node->flag & LEARN_FLAG_CONTRA ) {
			current_search->node->flag &= ~LEARN_FLAG_CONTRA;


			if( start_node->flag & LEARN_FLAG_PARENT ) {
				next_stack = static_learn_dag_make_forward_list( current_search->node );
				next_stack_top = next_stack;
			}
			else {
				next_stack = current_search->node->forward;
				next_stack_top = NULL;
			}

			while ( next_stack ) {

				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_branch->node   = next_stack->node;
				
				new_branch->next   = search;
				search             = new_branch;

				
				next_stack = next_stack->next;

			}
			next_stack = next_stack_top;
			while ( next_stack ) {
				new_branch = next_stack;
				next_stack = next_stack->next;
				topgunFree ( new_branch, FMT_LEARN_BRANCH, 1, func_name ); 
			}
		}
		topgunFree ( current_search, FMT_LEARN_BRANCH, 1, func_name ); 
	}
}


/*!
  @brief 同じ等価ノード内かを判定する

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

Ulong static_learn_dag_branch_count
(
 LEARN_BRANCH *branch
 ){
	Ulong count = 0;

	while ( branch ) {
		count++;
		branch = branch->next;
	}
	return ( count );
}
/*!
  @brief 

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

void static_learn_dag_branch_count_finish
(
 LEARN_NODE *node
 ){
	LEARN_BRANCH *branch = node->forward;
	Ulong count = 0;

#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-C node recalc %4ld %2ld", node->node_id, node->n_forward );
#endif /* _DEBUG_PRT_ATPG_ */

	while ( branch ) {

		if ( ! ( branch->node->flag & LEARN_FLAG_FINISH ) ){
			count++;
		}
		branch = branch->next;
	}
	node->n_forward = count;


#ifdef _DEBUG_PRT_ATPG_
	printf(" -> %2ld",node->n_forward );
	if ( node->flag & LEARN_FLAG_FINISH ) {
		printf(" F ");
	}
	else {
		printf(" M ");
	}
	printf("\n");
#endif /* _DEBUG_PRT_ATPG_ */
}

void static_learn_dag_branch_count_finish_with_check
(
 LEARN_NODE *node
 ){
	LEARN_BRANCH *branch = node->forward;
	Ulong count = 0;

#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-C node recalc %4ld %2ld", node->node_id, node->n_forward );
#endif /* _DEBUG_PRT_ATPG_ */

	while ( branch ) {

		if ( ! ( branch->node->flag & LEARN_FLAG_FINISH ) ){
			count++;
		}
		branch = branch->next;
	}
	node->n_forward = count;


#ifdef _DEBUG_PRT_ATPG_
	printf(" -> %2ld",node->n_forward );
	if ( node->flag & LEARN_FLAG_FINISH ) {
		printf(" F ");
	}
	else {
		printf(" M ");
	}
	printf("\n");
#endif /* _DEBUG_PRT_ATPG_ */
}

/*!
  @brief 同じ等価ノード内かを判定する

  @param [in] *start_node  開始するノードへのポインタ
  @param [in] *end_node    終了するノードへのポインタ
  @return void

*/

LEARN_TYPE static_learn_dag_check_family
(
 LEARN_NODE *start_node,
 LEARN_NODE *end_node
 ){

	/* 両方共子の場合 */
	if ( ( start_node->flag & LEARN_FLAG_CHILDREN ) &&
		 ( end_node->flag & LEARN_FLAG_CHILDREN ) ) {
		if ( start_node->parent->node_id ==
			 end_node->parent->node_id ) {
			return ( LEARN_OFF );
		}
		else {
			return ( LEARN_ON );
		}
	}
	/* 親子の場合 */
	if ( ( start_node->flag & LEARN_FLAG_PARENT ) &&
		 ( end_node->flag & LEARN_FLAG_CHILDREN ) ) {
		if ( start_node->node_id == end_node->parent->node_id ) {
			return ( LEARN_OFF );
		}
		else {
			return ( LEARN_ON );
		}
	}
	
	if ( ( start_node->flag & LEARN_FLAG_CHILDREN ) &&
		 ( end_node->flag & LEARN_FLAG_PARENT ) ) {
		if ( start_node->parent->node_id == end_node->node_id ) {
			return ( LEARN_OFF );
		}
		else {
			return ( LEARN_ON );
		}
	}
	return ( LEARN_ON );
}


LEARN_STATE static_learn_dag_state3
(
STATE_3 state3
){
#ifndef TOPGUN_TEST	
	char *func_name = "static_learn_dag_state3"; /* 関数名 */
#endif	

	switch ( state3 ) {
	case STATE3_0:
		return ( LEARN_STATE_0 );
	case STATE3_1:
		return ( LEARN_STATE_1 );
	default:
#ifndef TOPGUN_TEST		
		topgun_error( FEC_PRG_LINE_STAT, func_name );
#endif
		break;
	}
	return ( N_LEARN_STATE ); //!< こない
}

/*!
  @brief 値を反転させる( 0 <-> 1 )

  @param [in] state3 3値の値
  @return 含意操作用2値の値(反転値)

*/

LEARN_STATE static_learn_dag_invert_state3
(
 STATE_3 state3
 ){
#ifndef TOPGUN_TEST
	char *func_name = "static_learn_dag_state3"; /* 関数名 */
#endif /* TOPGUN_TEST */
	
	switch ( state3 ) {
	case STATE3_0:
		return ( LEARN_STATE_1 ); //!< 反転させる
#ifndef TOPGUN_TEST
	case STATE3_1:
		return ( LEARN_STATE_0 );
	default:
		topgun_error( FEC_PRG_LINE_STAT, func_name );
#else
	default:
		return ( LEARN_STATE_0 );
#endif
	}
	return ( N_LEARN_STATE ); //!< こない
}


/*!
  @brief グラフでたどれない箇所を調べる

  @param [in] void
  @return たどれない箇所のリスト

*/

LEARN_S_LIST *static_learn_dag_make_no_reachable_list
(
 void 
 ){
	LEARN_NODE   *node              = NULL; //!< テンポラリ
	LEARN_STATE  learn_state;
	LEARN_S_LIST *unreachable_list    = NULL;
	LEARN_S_LIST *new_s_list        = NULL;

	STATE_3       normal_state3     = STATE3_C;
	
	char *func_name = "static_learn_dag_state3"; /* 関数名 */


#ifndef OLD_IMP2

	Ulong i = 0;
	Ulong pos = 0;

	topgun_4_utility_enter_start_time ( &Time_head.learn_no_reach );

	
	/* 1つの値のみの含意なので1からlast_nまでが範囲となる */
	/* 0でなく1なのは、自分自身をのぞくため */
	for ( pos = Gene_head.last_n, i = 1 ; i < Gene_head.cnt_n; i++, pos-- ) {
		
		/* 含意操作の結果からグラフでたどれない箇所をしらべる */
		normal_state3 = atpg_get_state9_2_normal_state3
			( Gene_head.imp_list_n[ pos ]->state9 );
		learn_state =  static_learn_dag_state3 ( normal_state3 );

		node = Node_head[ learn_state ][ Gene_head.imp_list_n[ pos ]->line_id ];

		if ( node->node_id != 0 ) {
					
			if ( ! (  node->flag & LEARN_FLAG_DIRECT ) ) {

				if ( ! ( node->flag & LEARN_FLAG_CHILDREN ) ) {
					/* 直接行けない信号線がある */

					new_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );

					new_s_list->node = node;
					new_s_list->next = unreachable_list;
					unreachable_list = new_s_list;
#ifdef _DEBUG_PRT_ATPG_
#ifdef _DEBUG_PRT_LEARN_
					printf("2-L-R no reachable n%8ld %5ld\n"
						   ,node->node_id,node->line->line_id);
#endif /* _DEBUG_PRT_LEARN_ */
#endif /* _DEBUG_PRT_ATPG_ */
				}
			}
		}
	}
	topgun_4_utility_enter_end_time ( &Time_head.learn_no_reach );	
#else
	IMP_TRACE    *imp_trace         = NULL;
	
	imp_trace = Gene_head.imp_trace;
	
	while ( 1 ) {

		if ( ( imp_trace == NULL )  ||
			 ( imp_trace->next == NULL ) ) {
			break;
		}
		
		/* 含意操作の結果からグラフでたどれない箇所をしらべる */

		normal_state3 = atpg_get_state9_2_normal_state3 ( imp_trace->line->state9 );
		learn_state =  static_learn_dag_state3 ( normal_state3 );

		node = Node_head[ learn_state ][ imp_trace->line->line_id ];

		if ( node->node_id != 0 ) {
					
			if ( ! (  node->flag & LEARN_FLAG_DIRECT ) ) {

				if ( ! ( node->flag & LEARN_FLAG_CHILDREN ) ) {
					/* 直接行けない信号線がある */

					new_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );

					new_s_list->node = node;
					new_s_list->next = unreachable_list;
					unreachable_list = new_s_list;
#ifdef _DEBUG_PRT_ATPG_
#ifdef _DEBUG_PRT_LEARN_
					printf("2-L-R no reachable n%8ld %5ld\n"
						   ,node->node_id,node->line->line_id);
#endif /* _DEBUG_PRT_LEARN_ */
#endif /* _DEBUG_PRT_ATPG_ */
				}
			}
		}
		imp_trace = imp_trace->next;
	}
#endif /* OLD_IMP2 */
	

	return ( unreachable_list );
}

/*!
  @brief フラグがたっていないリストを更新する

  @param [in] no_branch_list
  @return 

*/

LEARN_S_LIST *static_learn_dag_recheck_unreachable_list
(
 LEARN_S_LIST *no_branch_list 
 ){
	LEARN_S_LIST *return_list = NULL;
	LEARN_S_LIST *no_branch_list_search = NULL;
	LEARN_S_LIST *current_node_list = NULL;
	
	char *func_name = "static_learn_recheck_unreachable_list"; /* 関数名 */
	
	/* no_branch_listを更新する */
	no_branch_list_search = no_branch_list;
			
	while ( no_branch_list_search ) {

		current_node_list = no_branch_list_search;
		no_branch_list_search = no_branch_list_search->next;
					
		if ( current_node_list->node->flag & LEARN_FLAG_DIRECT ) {
			
			topgunFree( current_node_list, FMT_LEARN_BRANCH, 1, func_name );
					
		}
		else {
			current_node_list->next = return_list;
			return_list = current_node_list;
		}
	}
	return ( return_list );
}

/*!
 リストからレベルの最も高いノードをとりだす

  @param [in] node_list
  @return 最も高いノード

*/

LEARN_NODE *static_learn_dag_search_max_level_node
(
 LEARN_S_LIST *node_list
 ){
	Ulong      top_level = 0;
	LEARN_NODE *top_node = NULL;

#ifndef TOPGUN_TEST	
	char *func_name = "static_learn_dag_search_max_level_node";	/* 関数名 */
#endif /* TOPGUN_TEST */
	
	while ( node_list ) {

		topgun_test_node_children ( node_list->node, func_name );

		/* childrenはnode_mergeで発生する */
		if ( node_list->node->flag & LEARN_FLAG_CHILDREN ) {
			return ( NULL );
		}
		else {
			if ( top_level < node_list->node->level ) {
			
				top_level = node_list->node->level;
				top_node  = node_list->node;
			}
		}
		node_list = node_list->next;
	}

	return ( top_node );
}


/*!
 リストからレベルを用いずレベルの最も高いノードをとりだす

  @param [in] node_list
  @return 最も高いノード

*/

LEARN_NODE *static_learn_dag_search_top_node
(
 LEARN_S_LIST *org_node_list
 ){
	LEARN_NODE *top_node = NULL;
	LEARN_NODE *current_node = NULL;
	LEARN_S_LIST *node_list;
	LEARN_BRANCH *forward;

	//char *func_name = "static_learn_dag_search_max_level_node";	/* 関数名 */

	node_list = org_node_list;
	while ( node_list ) {
		node_list->node->flag |= LEARN_FLAG_NO_REACH;
		node_list = node_list->next;
	}

	node_list = org_node_list;
	while ( node_list ) {
		forward = node_list->node->forward;
		
		while ( forward ) {
			current_node = forward->node;

			if ( current_node->flag & LEARN_FLAG_NO_REACH ) {
				current_node->flag |= LEARN_FLAG_NO_REACH_NOT;
			}
			forward = forward->next;
		}
		node_list = node_list->next;
	}

	node_list = org_node_list;
	while ( node_list ) {
		node_list->node->flag &= ~LEARN_FLAG_NO_REACH;
		if ( node_list->node->flag & LEARN_FLAG_NO_REACH_NOT ) {
			node_list->node->flag &= ~LEARN_FLAG_NO_REACH_NOT;
		}
		else {
			top_node = node_list->node;
		}
		node_list = node_list->next;
	}

	return ( top_node );
}


/*!
  複数のブランチリストをマージして一つのブランチリストにする

  @param [in] *b_list
  @return branch

*/

LEARN_BRANCH *static_learn_dag_merge_b_list
(
 LEARN_B_LIST *b_list,
 LEARN_NODE   *org_node
 ){

	LEARN_B_LIST *free_b_list    = NULL;
	LEARN_B_LIST *current_b_list = NULL;
	
	LEARN_BRANCH *branch         = NULL;  /* 返り値 */
	LEARN_BRANCH *new_branch     = NULL;
	LEARN_BRANCH *free_branch    = NULL;
	LEARN_BRANCH *current_branch = NULL;

	LEARN_S_LIST *children;

	char *func_name = "static_learn_dag_merge_b_list"; /* 関数名 */


	org_node->flag |= LEARN_FLAG_BRANCH;
	children = org_node->children;

	while ( children ) {
		children->node->flag |= LEARN_FLAG_BRANCH;
		children = children->next;
	}
	
	current_b_list = b_list;
	
	while ( current_b_list ){

		current_branch = current_b_list->b_list; 
		
		while ( current_branch ) {

			if ( ( ! ( current_branch->node->flag & LEARN_FLAG_BRANCH ) ) &&

				 ( current_branch->node->node_id != org_node->node_id ) ) {

				current_branch->node->flag |= LEARN_FLAG_BRANCH;
			
				new_branch = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
				new_branch->node = current_branch->node;
				new_branch->next = branch;
				new_branch->count = 0;
				new_branch->loop = loop_count;
				branch           = new_branch;
			}
			free_branch = current_branch;
			current_branch = current_branch->next;
			
			topgunFree ( free_branch, FMT_LEARN_BRANCH, 1, func_name ); 
		}
		free_b_list    = current_b_list;
		current_b_list = current_b_list->next;
		
		topgunFree ( free_b_list, FMT_LEARN_B_LIST, 1, func_name ); 
	}

	current_branch = branch;
	while( current_branch ) {
		current_branch->node->flag &= ~LEARN_FLAG_BRANCH;
		current_branch = current_branch->next;
	}

	org_node->flag &= ~LEARN_FLAG_BRANCH;
	children = org_node->children;

	while ( children ) {
		children->node->flag &= ~LEARN_FLAG_BRANCH;
		children = children->next;
	}

	
	return ( branch ); 
}

/*!
  ノードリストをマージして一つのノードリストにする

  @param [in] *s_list
  @return branch

*/

LEARN_S_LIST *static_learn_dag_merge_s_list
(
 LEARN_S_LIST2 *s_list2,
 LEARN_NODE    *org_node
 ){

	LEARN_S_LIST  *s_list          = NULL;
	LEARN_S_LIST  *new_s_list      = NULL;
	LEARN_S_LIST  *free_s_list     = NULL;
	LEARN_S_LIST  *current_s_list  = NULL;

	LEARN_S_LIST2 *free_s_list2    = NULL;
	LEARN_S_LIST2 *current_s_list2 = NULL;

	char *func_name = "static_learn_dag_merge_s_list"; /* 関数名 */

	current_s_list2 = s_list2;

	while ( current_s_list2 ){

		current_s_list = current_s_list2->s_list; 
		
		while ( current_s_list ) {

			if ( ( ! ( current_s_list->node->flag & LEARN_FLAG_S_LIST ) ) &&
				 ( current_s_list->node->node_id != org_node->node_id ) ) {

				current_s_list->node->flag |= LEARN_FLAG_S_LIST;
			
				new_s_list = ( LEARN_S_LIST * )topgunMalloc
						( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
				new_s_list->node = current_s_list->node;
				new_s_list->next = s_list;
				s_list           = new_s_list;
			}
			free_s_list = current_s_list;
			current_s_list = current_s_list->next;
			
			topgunFree ( free_s_list, FMT_LEARN_S_LIST, 1, func_name ); 
		}
		free_s_list2    = current_s_list2;
		current_s_list2 = current_s_list2->next;
		
		topgunFree ( free_s_list2, FMT_LEARN_S_LIST2, 1, func_name ); 
	}


	current_s_list = s_list;
	while ( current_s_list ) {
		current_s_list->node->flag &= ~LEARN_FLAG_S_LIST;
		current_s_list = current_s_list->next;
	}
	return ( s_list ); 
}


/*!
 等価のノードからforwadしているノードのリストを作成する

  @param [in] *parent
  @return forward_list

*/

LEARN_BRANCH *static_learn_dag_make_forward_list
(
 LEARN_NODE *parent
 ){
	LEARN_BRANCH *forward_list = NULL;
	LEARN_BRANCH *new_s_list = NULL;
	LEARN_BRANCH *forward;
	
	LEARN_S_LIST *children;

	
	char *func_name = "static_learn_dag_state3"; /* 関数名 */

	/* まず自分の等価ノードにフラグをたてる */
	parent->flag |= LEARN_FLAG_PARENT_F;

	children = parent->children;
	while ( children ) {
		
		children->node->flag |= LEARN_FLAG_PARENT_F;
		children = children->next;
		
	}
	
	forward = parent->forward;

	/* forward_listを作成 */
	while  ( forward ) {

		/* 親から */
		if ( ! ( forward->node->flag & LEARN_FLAG_PARENT_F ) ) {
			forward->node->flag |= LEARN_FLAG_PARENT_F;
			
			new_s_list = ( LEARN_BRANCH * )topgunMalloc
				( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
			new_s_list->node = forward->node;
			new_s_list->next = forward_list;
			forward_list     = new_s_list;
		}
		
		forward = forward->next;
	}
	
	children = parent->children;
	/* 子から */
	while ( children ) {

		forward = children->node->forward;
		
		while ( forward ) {

			if ( ! ( forward->node->flag & LEARN_FLAG_PARENT_F ) ) {

				forward->node->flag |= LEARN_FLAG_PARENT_F;
				
				new_s_list = ( LEARN_BRANCH * )topgunMalloc
					( FMT_LEARN_BRANCH, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_s_list->node = forward->node;
				new_s_list->next = forward_list;
				forward_list     = new_s_list;

			}
			
			forward = forward->next;
		}
		children = children->next;
	}

	children = parent->children;
	while ( children ) {

		forward = children->node->forward;
		
		while ( forward ) {

			if ( forward->node->flag & LEARN_FLAG_PARENT_F ) {

				forward->node->flag &= ~LEARN_FLAG_PARENT_F;
			}
			forward = forward->next;
		}
		children = children->next;
	}

	forward = forward_list;
	while ( forward ) {
		forward->node->flag &= ~LEARN_FLAG_PARENT_F;
		forward = forward->next;
	}

	children = parent->children;
	while ( children ) {
		children->node->flag &= ~LEARN_FLAG_PARENT_F;
		children = children->next;
	}
	
	parent->flag &= ~LEARN_FLAG_PARENT_F;
	
	
	
	return ( forward_list ); 
}

/*!
 マージしたノードへの枝の重複を省く

  @param [in] *parent
  @return void

*/

void static_learn_dag_delete_reverse_branch
(
 LEARN_NODE *parent
 ){

	LEARN_BRANCH *branch_top;
	LEARN_BRANCH *branch;
	LEARN_BRANCH *r_branch;
	LEARN_BRANCH *current_branch;
	LEARN_BRANCH *new_branch = NULL;
	LEARN_S_LIST *children;
	Ulong        del_count = 0;
	DAG_PARENT   parent_flag = DAG_MADA;

	char *func_name = "static_learn_dag_delete_reverse_branch"; /* 関数名 */
	

	children = parent->children;

	topgun_print_mes_char("2-L-D parent ");
	topgun_print_mes_ulong_2( parent->node_id );
	topgun_print_mes_n();

	parent->flag |= LEARN_FLAG_R_BRANCH;
	
	while ( children ) {

		topgun_print_mes_char("2-L-D child  ");
		topgun_print_mes_ulong_2( children->node->node_id );
		topgun_print_mes_n();
		
		children->node->flag |= LEARN_FLAG_R_BRANCH;
		children = children->next;

	}

	topgun_print_mes_char("2-L-D forward\n");
	
	branch = parent->forward;

	while ( branch ) {
		
		r_branch = branch->node->backward;
		new_branch = NULL;
		branch_top = NULL;

		topgun_print_mes_char("2-L-D forward node ");
		topgun_print_mes_ulong_2( branch->node->node_id );
		topgun_print_mes_n();

		parent_flag = DAG_MADA;
		
		while ( r_branch ) {

			current_branch = r_branch;
			r_branch = r_branch->next;

			topgun_print_mes_char("2-L-D current node ");
			topgun_print_mes_ulong_2( current_branch->node->node_id );
			
			if ( current_branch->node->flag & LEARN_FLAG_R_BRANCH ) {
				if ( parent_flag == DAG_MADA ) {
					if ( current_branch->node->node_id == parent->node_id ) {
						parent_flag = DAG_SUMI;
						
						topgun_print_mes_char(" -> parent(mada)\n");
						if ( new_branch == NULL ){
							new_branch = current_branch;
							new_branch->next = NULL;
							branch_top = new_branch;
						}
						else {
							new_branch->next = current_branch;
							new_branch = current_branch;
							new_branch->next = NULL;
						}
					}
					else {
						topgun_print_mes_char(" -> children(mada)\n");
						current_branch->node = parent;
						parent_flag = DAG_SUMI;
						if ( new_branch == NULL ){
							new_branch = current_branch;
							new_branch->next = NULL;
							branch_top = new_branch;
						}
						else {
							new_branch->next = current_branch;
							new_branch = current_branch;
							new_branch->next = NULL;
						}
					}
				}
				else {
					topgun_print_mes_char(" -> SUMI \n");
					topgunFree ( current_branch, FMT_LEARN_BRANCH, 1, func_name );
				}
			}
			else {
				topgun_print_mes_char(" -> other \n");
				if ( new_branch == NULL ){
					new_branch = current_branch;
					new_branch->next = NULL;
					branch_top = new_branch;
				}
				else {
					new_branch->next = current_branch;
					new_branch = current_branch;
					new_branch->next = NULL;
				}
			}
		}

		branch->node->backward = branch_top;
		
		branch = branch->next;
	}

	branch = parent->backward;

	topgun_print_mes_char("2-L-D backward\n");
	
	while ( branch ) {

		r_branch = branch->node->forward;
		new_branch = NULL;
		branch_top = NULL;

		topgun_print_mes_char("2-L-D backward node ");
		topgun_print_mes_ulong_2( branch->node->node_id );
		topgun_print_mes_n();
		
		del_count = 0;
		parent_flag = DAG_MADA;
		
		while ( r_branch ) {

			current_branch = r_branch;
			r_branch = r_branch->next;

			topgun_print_mes_char("2-L-D current node ");
			topgun_print_mes_ulong_2( current_branch->node->node_id );

			if ( current_branch->node->flag & LEARN_FLAG_R_BRANCH ) {
				if ( parent_flag == DAG_MADA ) {
					if ( current_branch->node->node_id == parent->node_id ) {
						parent_flag = DAG_SUMI;
						
						topgun_print_mes_char(" -> parent(mada)\n");
						if ( new_branch == NULL ){
							new_branch = current_branch;
							new_branch->next = NULL;
							branch_top = new_branch;
						}
						else {
							new_branch->next = current_branch;
							new_branch = current_branch;
							new_branch->next = NULL;
						}
						
					}
					else {
						
						topgun_print_mes_char(" -> children(mada)\n");
						
						parent_flag = DAG_SUMI;
						current_branch->node = parent;
						if ( new_branch == NULL ){
							new_branch = current_branch;
							new_branch->next = NULL;
							branch_top = new_branch;
						}
						else {
							new_branch->next = current_branch;
							new_branch = current_branch;
							new_branch->next = NULL;
						}
					}
				}
				else {
					topgun_print_mes_char(" -> SUMI \n");
					del_count++;
					topgunFree ( current_branch, FMT_LEARN_BRANCH, 1, func_name );
				}
			}
			else {
				topgun_print_mes_char(" -> other \n");
				if ( new_branch == NULL ){
					new_branch = current_branch;
					new_branch->next = NULL;
					branch_top = new_branch;
				}
				else {
					new_branch->next = current_branch;
					new_branch = current_branch;
					new_branch->next = NULL;
				}
			}
		}
		branch->node->forward = branch_top;
		branch->node->n_for_org -= del_count;
		branch = branch->next;
	}


	children = parent->children;
	
	while ( children ) {

		children->node->flag &= ~LEARN_FLAG_R_BRANCH;
		children = children->next;
	}

	parent->flag &= ~LEARN_FLAG_R_BRANCH;
}

/*!
  ノードからbackwardしているノードのリストを作成する

  @param [in] *parent
  @return backward_list

*/

LEARN_S_LIST *static_learn_dag_make_backward_list
(
 LEARN_NODE *from_node
 ){
	LEARN_S_LIST *backward_list = NULL;
	LEARN_S_LIST *new_s_list = NULL;
	LEARN_BRANCH *backward;
	
	char *func_name = "static_learn_dag_make_backward_list"; /* 関数名 */

	/* まず自分の等価ノードにフラグをたてる */

#ifdef _DEBUG_PRT_ATPG_
	//printf("2-L-W node ID %5ld\n",from_node->node_id);
#endif /* _DEBUG_PRT_ATPG_ */
	backward = from_node->backward;
	while  ( backward ) {

#ifdef _DEBUG_PRT_ATPG_
		//printf("2-L-W to   ID %5ld\n",backward->node->node_id);
#endif /* _DEBUG_PRT_ATPG_ */
		
		new_s_list = ( LEARN_S_LIST * )topgunMalloc
			( FMT_LEARN_S_LIST, sizeof( LEARN_BRANCH ), 1, func_name );
		
		new_s_list->node = backward->node;
		new_s_list->next = backward_list;
		backward_list     = new_s_list;
		
		backward = backward->next;
	}

	return ( backward_list );
}

/*!
  add_listから到達可能なノードにLEARN_FLAG_RESEARCHをたてる
*/

void static_learn_dag_onflag_research_new
(
 void
 ){
	
	LEARN_BRANCH *backward = NULL;
	LEARN_NODE   *current_node = NULL;
	LEARN_NODE   *back_node = NULL;
	
	//char *func_name = "static_learn_dag_onflag_research"; /* 関数名 */

	/* まず自分の等価ノードにフラグをたてる */

	Search_add_info.get_id = 0;
	
	current_node = static_learn_dag_search_add_get_list();
	
	while ( current_node ) {

		if ( current_node->flag & LEARN_FLAG_CHILDREN ) {

			topgun_print_mes_reentry_org ( current_node );
			
			current_node = current_node->parent;
			
			topgun_print_mes_reentry_child ( current_node );
		}
		else {

			topgun_print_mes_reentry_child ( current_node );
			
		}

		
		current_node->flag |= LEARN_FLAG_RESEARCH;
		backward = current_node->backward;

		while  ( backward ) {

			back_node = backward->node;
			
			if ( ! ( back_node->flag & LEARN_FLAG_RESEARCH ) ){

				topgun_print_mes_reentry ( back_node );

				back_node->flag |= LEARN_FLAG_RESEARCH;
				
				static_learn_dag_search_add_enter_list ( back_node );

			}
			backward = backward->next;
		}
		current_node = static_learn_dag_search_add_get_list();
	}
}

/*!
  add_listから到達可能なノードにLEARN_FLAG_RESEARCHをたてる
*/

void static_learn_dag_onflag_research
(
 LEARN_S_LIST *add_list
 ){
	
	LEARN_S_LIST *backward_list = NULL;
	LEARN_S_LIST *new_s_list = NULL;
	LEARN_BRANCH *backward = NULL;
	LEARN_NODE   *current_node = NULL;
	LEARN_NODE   *back_node = NULL;
	
	char *func_name = "static_learn_dag_onflag_research"; /* 関数名 */

	/* まず自分の等価ノードにフラグをたてる */

	while ( add_list ) {

		current_node = add_list->node;

		if ( current_node->flag & LEARN_FLAG_CHILDREN ) {

			topgun_print_mes_reentry_org ( current_node );
			
			current_node = current_node->parent;
			
			topgun_print_mes_reentry_child ( current_node );
		}
		else {

			topgun_print_mes_reentry_child ( current_node );
			
		}

		
		current_node->flag |= LEARN_FLAG_RESEARCH;
		backward = current_node->backward;

		while  ( backward ) {

			back_node = backward->node;
			
			if ( ! ( back_node->flag & LEARN_FLAG_RESEARCH ) ){

				topgun_print_mes_reentry ( back_node );

				back_node->flag |= LEARN_FLAG_RESEARCH;
				
				new_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_BRANCH ), 1, func_name );
		
				new_s_list->node = back_node;
				new_s_list->next = backward_list;
				backward_list     = new_s_list;

			}
			backward = backward->next;
		}
		add_list = add_list->next;
	}

	while ( backward_list ) {
		
		current_node = backward_list->node;
		backward = current_node->backward;

		topgun_print_mes_reentry_from( current_node );
		
		new_s_list = backward_list;
		backward_list = backward_list->next;
		topgunFree ( new_s_list, FMT_LEARN_S_LIST, 1, func_name );
		
		while  ( backward ) {
			
			back_node = backward->node;
			
			if ( ! ( back_node->flag & LEARN_FLAG_RESEARCH ) ){

				topgun_print_mes_reentry ( back_node ) ;
				
				back_node->flag |= LEARN_FLAG_RESEARCH;
				
				new_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_BRANCH ), 1, func_name );
		
				new_s_list->node = back_node;
				new_s_list->next = backward_list;
				backward_list     = new_s_list;

			}
			backward = backward->next;
		}

	}
}



/*!
  親ノードからbackwardしているノードのリストを作成する

  @param [in] *parent
  @return backward_list

*/

LEARN_S_LIST *static_learn_dag_make_backward_list_parent
(
 LEARN_NODE *parent
 ){
	LEARN_S_LIST *backward_list = NULL;
	LEARN_S_LIST *new_s_list = NULL;
	LEARN_BRANCH *backward;
	
	LEARN_S_LIST *children;

	
	char *func_name = "static_learn_dag_make_backward_list"; /* 関数名 */

	/* まず自分の等価ノードにフラグをたてる */
	/* -> 枝の数が大事なので、ループ外にいってるものはぜんぶ繋げる */

	parent->flag |= LEARN_FLAG_PARENT_F;

	children = parent->children;
	
	while ( children ) {
		
		children->node->flag |= LEARN_FLAG_PARENT_F;
		children = children->next;
		
	}
	
	backward = parent->backward;
	while  ( backward ) {

		if ( ! ( backward->node->flag & LEARN_FLAG_PARENT_F ) ) {
			
			new_s_list = ( LEARN_S_LIST * )topgunMalloc
				( FMT_LEARN_S_LIST, sizeof( LEARN_BRANCH ), 1, func_name );
	
			new_s_list->node = backward->node;
			new_s_list->next = backward_list;
			backward_list     = new_s_list;
		}
		
		backward = backward->next;
	}
	
	children = parent->children;
	while ( children ) {

		backward = children->node->backward;
		
		while ( backward ) {

			if ( ! ( backward->node->flag & LEARN_FLAG_PARENT_F ) ) {

				new_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_s_list->node = backward->node;
				new_s_list->next = backward_list;
				backward_list     = new_s_list;

			}
			
			backward = backward->next;
		}
		children = children->next;
	}

#ifdef NO_USE
	children = parent->children;
	while ( children ) {

		backward = children->node->backward;
		
		while ( backward ) {

			if ( backward->node->flag & LEARN_FLAG_PARENT_F ) {

				backward->node->flag &= ~LEARN_FLAG_PARENT_F;
				
			}
			
			backward = backward->next;
		}
		children = children->next;
	}



	backward = parent->backward;
	while ( backward ) {
		backward->node->flag &= ~LEARN_FLAG_PARENT_F;
		backward = backward->next;
	}
#endif /* NO_USE */
	children = parent->children;
	while ( children ) {
		children->node->flag &= ~LEARN_FLAG_PARENT_F;
		children = children->next;
	}
	parent->flag &= ~LEARN_FLAG_PARENT_F;
	
	return ( backward_list ); 
}

/*!
  @brief 全ノードに対して不要な間接含意が存在するかを調べる

  @param [in] *mae
  @param [in] *ato
  @return void

*/

void static_learn_dag_indirect_select_to_all_node
(
 void 
 ){
	Ulong i;
	LEARN_NODE *node;
	LINE *line;
	
	topgun_4_utility_enter_start_time ( &Time_head.learn_select );
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		line = &( Line_head[ i ] );

		if ( line->imp_0 != NULL ) {
			node    = Node_head[ LEARN_STATE_0 ][ line->line_id   ];
			static_learn_dag_indirect_select ( node );
		}
		if ( line->imp_1 != NULL ) {
			node    = Node_head[ LEARN_STATE_1 ][ line->line_id   ];
			
			static_learn_dag_indirect_select ( node );
		}		
	}
	topgun_4_utility_enter_end_time ( &Time_head.learn_select );
}

/*
// 未使用のためコメントアウトする
void static_learn_dag_research_node_character
(
 void 
 ){
	Ulong i;
	LEARN_NODE *node;
	LINE *line;

	Ulong all_node = 0;
	Ulong real_node = 0;
	Ulong dual_connect = 0;
	Ulong multi_connect = 0;
	Ulong level_0   = 0;
	Ulong n_back    = 0;
	LEARN_BRANCH *branch = NULL;
	
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		line = &( Line_head[ i ] );

		node    = Node_head[ LEARN_STATE_0 ][ line->line_id   ];

		if ( node != NULL ) {

			all_node++;
			if ( ( node->node_id != 0 ) &&
				 (! ( node->flag & LEARN_FLAG_CHILDREN ) ) ){

				real_node++;

				n_back = 0;
				branch = node->backward;
				while ( branch ) {
					n_back++;
					branch = branch->next;
				}
				printf("ID %2lu lv %2lu for %3lu back %3lu "
					   ,node->node_id, node->level, node->n_for_org,n_back);

				
				printf("F ");
				branch = node->forward;
				while ( branch ) {
					printf("%2lu ",branch->node->node_id);
					branch = branch->next;
				}
				printf("B ");
				branch = node->backward;
				while ( branch ) {
					printf("%2lu ",branch->node->node_id);
					branch = branch->next;
				}
				printf("\n");
				
				if ( node->n_for_org == 0 ) {
					level_0++;
				}
				if ( ( node->n_for_org > 1 ) &&
					 ( n_back > 1 ) ){
					dual_connect++;
				}
				if ( node->n_for_org > 1 ) {
					multi_connect++;
				}
			}
		}			
		node    = Node_head[ LEARN_STATE_1 ][ line->line_id   ];
		if ( node != NULL ) {

			all_node++;
			if ( ( node->node_id != 0 ) &&
				 (! ( node->flag & LEARN_FLAG_CHILDREN ) ) ){

				n_back = 0;
				branch = node->backward;
				while ( branch ) {
					n_back++;
					branch = branch->next;
				}
				printf("ID %2lu lv %2lu for %3lu back %3lu "
					   ,node->node_id, node->level, node->n_for_org,n_back);

				
				printf("F ");
				branch = node->forward;
				while ( branch ) {
					printf("%2lu ",branch->node->node_id);
					branch = branch->next;
				}
				printf("B ");
				branch = node->backward;
				while ( branch ) {
					printf("%2lu ",branch->node->node_id);
					branch = branch->next;
				}
				printf("\n");
				
				real_node++;
				if ( node->n_for_org == 0 ) {
					level_0++;
				}
				if ( ( node->n_for_org > 1 ) &&
					 ( n_back > 1 ) ){
					dual_connect++;
				}
				if ( node->n_for_org > 1 ) {
					multi_connect++;
				}
			}
		}			
	}
	printf("all_node    %lu\n",all_node);
	printf("real_node   %lu\n",real_node);
	printf("level_0     %lu\n",level_0);
	printf("multi       %lu\n",multi_connect);
	printf("dual_multi  %lu\n",dual_connect);
	//exit(0);
}
*/

// 2006/10/23コメントアウト(処理がすべてあるかないかをprintfで表示するだけのため) 
void static_learn_dag_indirect_select
(
 LEARN_NODE *start_node
 ){

	char *func_name = "static_learn_dag_indirect_select";

	LEARN_BRANCH *forward = NULL;
	LEARN_S_LIST *new_s_list = NULL;
	LEARN_S_LIST *search_list = NULL;
	LEARN_S_LIST *search_list_top = NULL;
	LEARN_S_LIST *add_list;
	LEARN_LIST   *learn_list = NULL;
	LEARN_NODE   *learn_node = NULL;
	LEARN_S_LIST *children;


	if ( start_node->flag & LEARN_FLAG_CHILDREN ) {
		forward = start_node->parent->forward;
	}
	else {
		forward = start_node->forward;
	}

	while ( forward ) {

		new_s_list = ( LEARN_S_LIST * )topgunMalloc
			( FMT_LEARN_S_LIST, sizeof( LEARN_BRANCH ), 1, func_name );
	
		new_s_list->node = forward->node;
		new_s_list->next = search_list;
		search_list     = new_s_list;

		forward = forward->next;
	}

	search_list_top = search_list;

	while ( search_list ) {

		forward = search_list->node->forward;
		add_list = NULL;

		while ( forward ) {

			if ( ! ( forward->node->flag & LEARN_FLAG_REACH ) ) {

				forward->node->flag |= LEARN_FLAG_REACH;
				
				new_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_BRANCH ), 1, func_name );
	
				new_s_list->node = forward->node;
				new_s_list->next = add_list;
				add_list     = new_s_list;
			}
			forward = forward->next;
		}
		static_learn_dag_make_connect ( search_list, add_list );
		search_list = search_list->next;
	}

	if ( start_node->state3 == STATE3_0 ) {
		learn_list = start_node->line->imp_0;
	}
	else {
		learn_list = start_node->line->imp_1;
	}

	while ( learn_list ) {

		learn_node = Node_head
			[ static_learn_dag_state3 ( learn_list->ass_state3 ) ]
			[ learn_list->line->line_id ];

		if ( learn_node->flag & LEARN_FLAG_REACH ) {
			// don't need this flag
			printf("node can delete branch node_id %6ld -> %6ld "
				   ,start_node->node_id, learn_node->node_id ); 
		}
		else {
			printf("node     need   branch node_id %6ld -> %6ld " 
			   ,start_node->node_id, learn_node->node_id );
		}

		if ( start_node->flag & LEARN_FLAG_CHILDREN ) {
			forward = start_node->parent->forward;
			printf(" (p) "); 
		}
		else {
			forward = start_node->forward;
			printf("     ");
		}
		while ( forward ) {
			if ( forward->node->node_id == learn_node->node_id ) {
				printf("loop %4ld count %6ld\n",forward->loop, forward->count);
				break;
			}
			forward = forward->next;
		}

		if ( start_node->flag & LEARN_FLAG_CHILDREN ) {

			if ( start_node->parent->node_id == learn_node->node_id ) {
				printf("child  <-> parent\n"); 
			}
			else {
				children = start_node->parent->children;
				while ( children ) {
					if ( children->node->node_id == learn_node->node_id ) {
						printf("child  <-> child\n"); 
						break;
					}
					children = children->next;
				}
			}
		}
		else if ( start_node->flag & LEARN_FLAG_PARENT ) {
			children = start_node->children;
			while ( children ) {
				if ( children->node->node_id == learn_node->node_id ) {
					printf("parent <-> child\n");
					break;
				}
				children = children->next;
			}
		}
		learn_list = learn_list->next;
	}

	/* flagを戻す */
	search_list = search_list_top;
	while ( search_list ) {

		new_s_list = search_list;
		search_list->node->flag &= ~LEARN_FLAG_REACH;
		search_list = search_list->next;

		topgunFree ( new_s_list, FMT_LEARN_S_LIST, 1, func_name );
		
	}
}



/*!
  二つのリストをくっつける

  @param [in] *mae
  @param [in] *ato
  @return void

*/

void static_learn_dag_make_connect
(
 LEARN_S_LIST *mae,
 LEARN_S_LIST *ato
 ){

	while ( mae->next ) {
		mae = mae->next;
	}
	mae->next = ato;
}


LEARN_NODE *static_learn_dag_line_state_2_node
(
 LINE *org_line,
 STATE_3 state3
 ){

	LINE *line = org_line;
	Ulong odd_cnt = 0;
	char  stop_flag = 1;
	LEARN_NODE *node;
	
	while ( stop_flag ) {
		switch ( line->type ) {
		case TOPGUN_BR:
		case TOPGUN_BUF:
			line = line->in[0];
			break;
		case TOPGUN_INV:
			line = line->in[0];
			odd_cnt++;
			break;
		default:
			stop_flag = 0;
			break;
		}
	}

	if ( ( ( state3 == STATE3_0 ) && ( ( odd_cnt % 2 ) == 0 ) ) ||
		 ( ( state3 == STATE3_1 ) && ( ( odd_cnt % 2 ) == 1 ) ) ) {
		node = Node_head[ LEARN_STATE_0 ][ line->line_id ];
	}
	else {
		node = Node_head[ LEARN_STATE_1 ][ line->line_id ];
	}
	if ( node->flag & LEARN_FLAG_CHILDREN ) {
		node = node->parent;
	}
	return (  node );
}

void atpg_line_justified_extend_mini_level_node
(
 ASS_LIST *org_ass_list
 ){
	LINE  *line = org_ass_list->line;
	STATE_3 state3 = org_ass_list->ass_state3;

	Ulong current_level = 0;

	LEARN_BRANCH *backward = NULL;
	LEARN_NODE   *node = NULL;
	LEARN_NODE   *save = NULL;

	//char *func_name = "static_learn_dag_indirect_select";

	node = static_learn_dag_line_state_2_node ( line, state3 );

	while ( node ) {

		current_level = 0;
		backward = node->backward;
		save = node;
		
		node = NULL;


		while ( backward ) {
			if ( current_level < backward->node->level ) {
				current_level = backward->node->level;
				node = backward->node;
			}
			backward = backward->next;
		}
	}
	org_ass_list->line       = save->line;
	org_ass_list->ass_state3 = save->state3;
}


/*!
  fixリストへ登録する

  @return void

*/

void static_learn_dag_make_fix_list
(
 ASS_LIST *ass_list
 ){
#ifndef OLD_IMP2

	Ulong      i = 0;
	Ulong      pos = 0;
	LINE_STACK *line_stack = NULL;
	
	ASS_LIST fix_insert_list;
	IMP_RESULT imp_result;
	
	char *func_name = "static_learn_dag_make_fix_list";

	// initial imp_result
	imp_result.i_state = IMP_CONF;
	imp_result.imp_id  = 0;
	
	fix_insert_list.line       = ass_list->line;
	fix_insert_list.ass_state3 = atpg_invert_state3( ass_list->ass_state3 );
	fix_insert_list.condition  = COND_NORMAL;
	fix_insert_list.next       = NULL;

	/* ass_list つくって、imp_trace覚えとく? */
	imp_result = gene_enter_state( &fix_insert_list );
	if ( IMP_CONF != imp_result.i_state ) {
		imp_result = implication();
	}
	if ( IMP_CONF == imp_result.i_state ) {
		/* confの逆値を設定しているのにconfとは変 */
		//topgun_error( FEC_PRG_LINE_STAT, func_name );
		for ( pos = Gene_head.last_n, i = 0 ; i < Gene_head.cnt_n; i++, pos-- ) {
		    atpg_reflesh_state( Gene_head.imp_list_n[ pos ], (LEARN_STATE) COND_NORMAL );
		}
	}
	else {
	/* 正常値はcir_fixに登録する */
	for ( pos = Gene_head.last_n, i = 0 ; i < Gene_head.cnt_n; i++, pos-- ) {
		
		topgun_print_mes_dag_new_fix_line( Gene_head.imp_list_n[ pos ] );
										   
		Gene_head.imp_list_n[ pos ]->flag |= CIR_FIX;

		line_stack = ( LINE_STACK * )topgunMalloc
			( FMT_LINE_STACK, sizeof( LINE_STACK ), 1, func_name );
		
		line_stack->line  = Gene_head.imp_list_n[ pos ];
		line_stack->next  = Gene_head.cir_fix;
		Gene_head.cir_fix = line_stack;
	}
	}
	/* 故障値は元に戻す(ないはずだけど) */
	for ( pos = Gene_head.last_f, i = 0 ; i < Gene_head.cnt_f; i++, pos-- ) {
		atpg_reflesh_state( Gene_head.imp_list_f[ pos ], (LEARN_STATE) COND_FAILURE );
	}
	Gene_head.last_n = 0;
	Gene_head.last_f = 0;	
	Gene_head.cnt_n  = 0;
	Gene_head.cnt_f  = 0;

#else	
	ASS_LIST fix_insert_list;
	IMP_RESULT imp_result = IMP_CONF;
	IMP_TRACE  *imp_trace = NULL;
	IMP_TRACE  *current_imp_trace = NULL;
	
	char *func_name = "static_learn_dag_make_fix_list";

	fix_insert_list.line       = ass_list->line;
	fix_insert_list.ass_state3 = atpg_invert_state3( ass_list->ass_state3 );
	fix_insert_list.condition  = COND_NORMAL;
	fix_insert_list.next       = NULL;

	/* ass_list つくって、imp_trace覚えとく? */
	imp_result = gene_enter_state( &fix_insert_list );
	if ( IMP_CONF != imp_result) {
		imp_result = implication();
	}
	if ( IMP_CONF == imp_result ) {
		/* confの逆値を設定しているのにconfとは変 */
		topgun_error( FEC_PRG_LINE_STAT, func_name );
	}

	/* 故障値は元に戻し、cir_fixに登録する */
	imp_trace = Gene_head.imp_trace;
	while ( imp_trace ) {
		
		current_imp_trace = imp_trace;
		imp_trace = imp_trace->next;
		
		if ( COND_NORMAL == current_imp_trace->condition ) {

			topgun_print_mes_dag_new_fix_line( current_imp_trace->line );
			
			current_imp_trace->line->flag |= CIR_FIX;
			current_imp_trace->next = Gene_head.cir_fix;
			Gene_head.cir_fix       = current_imp_trace;
		}
		else {
			atpg_reflesh_state( current_imp_trace->line, COND_FAILURE );
			topgunFree( current_imp_trace, FMT_IMP_TRACE, 1, func_name );
		}
	}
	Gene_head.imp_trace = NULL;
#endif /* OLD_IMP2 */	
}
 

void static_learn_dag_print_topolotical_result
(
 void 
 ){
#ifdef _DEBUG_PRT_ATPG_

	LEARN_NODE      *node;
	
	//char *func_name = "static_learn_dag_print_topolotical_result"; /* 関数名 */


	node = Topo_top.top_node;
		
	while ( node ) {

		printf("2-L-T LV %2ld Node ID %8ld Line ID %8ld "
			   ,node->level,node->node_id, node->line->line_id );
		if ( node->state3 == STATE3_0 ) {
			printf("0\n");
		}
		else {
			printf("1\n");
		}
		node = node->next;
	}

#endif /* _DEBUG_PRT_ATPG_ */
}

void test_dag_node_connect
(
 void
 ){
#ifdef _DEBUG_PRT_ATPG_

	Ulong i;
	LEARN_NODE *node;
	
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];

		if ( node->node_id != 0 ){

			if ( ! ( node->flag & LEARN_FLAG_CHILDREN ) ) {

				test_dag_node_branch( node );
				test_dag_node_2_node( node );
				
			}
			node = Node_head[ LEARN_STATE_1 ][ i ];
			if ( ! ( node->flag & LEARN_FLAG_CHILDREN ) ) {

				test_dag_node_branch( node );
				test_dag_node_2_node( node );
				
			}
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void test_dag_node_branch
(
 LEARN_NODE *test_node
 ){
#ifdef _DEBUG_PRT_ATPG_

	LEARN_BRANCH *branch;
	Ulong branch_count = 0;

	branch = test_node->forward;

	while ( branch ) {
		branch_count++;
		branch = branch->next;
	}

	if ( branch_count != test_node->n_for_org ) {
		printf("2-L-E NODE ID %2ld %ld != %ld",test_node->node_id, test_node->n_for_org, branch_count );
		topgun_print_mes_n();
	}

#endif /* _DEBUG_PRT_ATPG_ */
}
	
void test_dag_node_2_node
(
 LEARN_NODE *test_node
 ){
#ifdef _DEBUG_PRT_ATPG_

	LEARN_BRANCH *branch;
	LEARN_BRANCH *to_branch;
	LEARN_NODE   *to_node;
	Ulong flag = 0;

	branch = test_node->forward;

	while ( branch ) {

		to_node = branch->node;

		to_branch = to_node->backward;

		flag = 0;
		while ( to_branch ) {

			if ( to_branch->node->node_id == test_node->node_id ) {
				flag = 1;
				break;
			}
			to_branch = to_branch->next;
		}
		if ( flag == 0 ) {
			printf("2-L-E NODE ID %2ld -> %2ld, NOT <-",test_node->node_id, to_node->node_id );
			topgun_print_mes_n();
		}
		branch = branch->next;
	}

	branch = test_node->backward;

	while ( branch ) {

		to_node = branch->node;

		to_branch = to_node->forward;

		flag = 0;
		while ( to_branch ) {

			if ( to_branch->node->node_id == test_node->node_id ) {
				flag = 1;
				break;
			}
			to_branch = to_branch->next;
		}
		if ( flag == 0 ) {
			printf("2-L-E NODE ID %2ld <- %2ld, NOT ->",test_node->node_id, to_node->node_id );
			topgun_print_mes_n();
		}
		branch = branch->next;
	}

#endif /* _DEBUG_PRT_ATPG_ */
}


void test_dag_graph_flag_loop_s_c
(
 void 
 ){
#ifdef _DEBUG_PRT_ATPG_
	Ulong      i;
	LEARN_NODE *node;
	
	for ( i = 0 ; i < Line_info.n_line; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];
		if ( node->node_id == 0 ) {
			
		}
		else if ( node->flag & LEARN_FLAG_LOOP_S ){
			topgun_print_mes_char( "2-L-C BAG Loop check start " );
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n();
		}
		else if ( node->flag & LEARN_FLAG_LOOP_C ){
			topgun_print_mes_char( "2-L-C BAG Loop check flag  " );
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n();
		}
		
		node = Node_head[ LEARN_STATE_1 ][ i ];

		if ( node->node_id == 0 ){

		}
		else if ( node->flag & LEARN_FLAG_LOOP_S ){
			topgun_print_mes_char( "2-L-C BAG Loop check start " );
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n();
		}
		else if ( node->flag & LEARN_FLAG_LOOP_C ){
			topgun_print_mes_char( "2-L-C BAG Loop check flag  " );
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n();
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
}


void test_dag_topo_list
(
 void 
 ){
#ifdef _DEBUG_PRT_ATPG_

	LEARN_TOPO *topo;
	LEARN_NODE *node;

	topo = & ( Topo_top );

	node = topo->top_node;

	topgun_print_mes_char( "2-L-D Dump list - node  \n" );
	while ( node ) {

		if ( ( node->prev == NULL ) ||
			 ( node->next == NULL ) ||
			 ( node->prev->level != node->level ) ||
			 ( node->next->level != node->level ) ){
			 
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n( );
		}
		node = node->next;
	}

	topgun_print_mes_char( "2-L-D Dump list - Topo  \n" );
	while ( topo ) {
		node = topo->top_node;
		if ( node != NULL ) {
			topgun_print_mes_ulong_2( node->level );
			topgun_print_mes_type( node->line );
			topgun_print_mes_id( node->line );
			topgun_print_mes_sp( 1 );
			topgun_print_mes_state3_only( node->state3);
			topgun_print_mes_n(  );
		}
		topo = topo->next_level;
	}
#endif /* _DEBUG_PRT_ATPG_ */
}


void test_dag_node_learn_do
(
 Ulong loop_count
 ){
#ifdef _DEBUG_PRT_ATPG_

	Ulong i;
	LEARN_NODE *node;
	//char buf[256];

	for ( i = 0 ; i < Line_info.n_line; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];
		if ( node->node_id == 0 ) {
			;
		}
		else {

			if ( node->imp_count == loop_count ) {
				//topgun_print_mes_char( "2-L   implication node      ");
				//topgun_print_mes_ulong_2( node->imp_count );
				//topgun_print_mes_sp( 1 );
				//topgun_print_mes_ulong_2( node->node_id );
				//topgun_print_mes_sp( 1 );
				//topgun_print_mes_ulong_2( node->level );
				//topgun_print_mes_type( node->line );
				//topgun_print_mes_id( node->line );
				//topgun_print_mes_sp( 1 );
				//topgun_print_mes_state3_only( node->state3);
				//topgun_print_mes_n();
			}
			else if  (! ( node->flag & LEARN_FLAG_CHILDREN ) ) {
				topgun_print_mes_char( "2-L-E Don't implication node");
				topgun_print_mes_ulong_2( node->imp_count );
				topgun_print_mes_sp( 1 );
				topgun_print_mes_ulong_2( node->node_id );
				topgun_print_mes_sp( 1 );
				topgun_print_mes_ulong_2( node->level );
				topgun_print_mes_type( node->line );
				topgun_print_mes_id( node->line );
				topgun_print_mes_sp( 1 );
				topgun_print_mes_state3_only( node->state3);
				topgun_print_mes_n();
			}
			else {
				;
			}
			node = Node_head[ LEARN_STATE_1 ][ i ];
			if ( node->imp_count == loop_count ) {
				//topgun_print_mes_char( "2-L   implication node      ");
				//topgun_print_mes_ulong_2( node->imp_count );
				//topgun_print_mes_sp( 1 );
				//topgun_print_mes_ulong_2( node->node_id );
				//topgun_print_mes_sp( 1 );
				//topgun_print_mes_ulong_2( node->level );
				//topgun_print_mes_type( node->line );
				//topgun_print_mes_id( node->line );
				//topgun_print_mes_sp( 1 );
				//topgun_print_mes_state3_only( node->state3);
				//topgun_print_mes_n();
			}
			else if  (! ( node->flag & LEARN_FLAG_CHILDREN ) ) {
				topgun_print_mes_char( "2-L-E Don't implication node");
				topgun_print_mes_ulong_2( node->imp_count );
				topgun_print_mes_sp( 1 );
				topgun_print_mes_ulong_2( node->node_id );
				topgun_print_mes_sp( 1 );
				topgun_print_mes_ulong_2( node->level );
				topgun_print_mes_type( node->line );
				topgun_print_mes_id( node->line );
				topgun_print_mes_sp( 1 );
				topgun_print_mes_state3_only( node->state3);
				topgun_print_mes_n();
			}
			else {
				;
			}
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void test_dag_node_n_forward
(
 void
 ){
#ifdef _DEBUG_PRT_ATPG_

	Ulong i;
	Ulong count;
	LEARN_NODE *node;
	LEARN_BRANCH *branch;

	for ( i = 0 ; i < Line_info.n_line; i++ ) {

		node = Node_head[ LEARN_STATE_0 ][ i ];
		if ( node->node_id != 0 ) {
			count = 0;
			branch = node->forward;
			while ( branch ) {
				if ( ! ( branch->node->flag & LEARN_FLAG_FINISH ) ){
					count++;
				}
				branch = branch->next;
			}
			if ( node->n_forward != count ) {
				printf("2-L-E node %4ld n_forward %2ld count %2ld\n",
					   node->node_id, node->n_forward, count);
				branch = node->forward;
				while ( branch ) {
					if ( ! ( branch->node->flag & LEARN_FLAG_FINISH ) ){
						printf("2-L-E      mada -> %4ld\n", branch->node->node_id );
					}
					else {
						printf("2-L-E      sumi -> %4ld\n", branch->node->node_id );
					}
					branch = branch->next;
				}
			}
			node = Node_head[ LEARN_STATE_1 ][ i ];
			count = 0;
			branch = node->forward;
			while ( branch ) {
				if ( ! ( branch->node->flag & LEARN_FLAG_FINISH ) ){
					count++;
				}
				branch = branch->next;
			}
			if ( node->n_forward != count ) {
				printf("2-L-E node %4ld n_forward %2ld count %2ld\n",
					   node->node_id, node->n_forward, count);
				branch = node->forward;
				while ( branch ) {
					if ( ! ( branch->node->flag & LEARN_FLAG_FINISH ) ){
						printf("2-L-E      mada -> %4ld\n", branch->node->node_id );
					}
					else {
						printf("2-L-E      sumi -> %4ld\n", branch->node->node_id );
					}
					branch = branch->next;
				}

			}
			count = 0;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
}

void static_learn_print_learn_result_dag
(
 void
 ){
	Ulong i;
	Ulong j;

	ASS_LIST ass_list;
	//LEARN_TYPE learn_type;
	
	topgun_print_mes( 12, 8 );
	topgun_print_mes_n();
	
	for ( i = 0 ; i < Line_info.max_lv_pi ; i++ ) {
		for ( j = 0 ; j < Line_info.n_lv_pi[ i ] ; j++ ) {

			/* 静的学習の対象となる信号線 */
			ass_list.line = Lv_pi_head[ i ][ j ];

			//learn_type = static_learn_line_type_check ( ass_list.line->type ) ;

			//if ( learn_type == LEARN_ON ) {
				
				/* 学習結果を出力する */
					
				topgun_print_mes_learn_list_result_dag ( ass_list.line, STATE3_0 );
				topgun_print_mes_learn_list_result_dag ( ass_list.line, STATE3_1 );
				//}
		}
	}
}

void topgun_print_mes_learn_list_result_dag
(
 LINE *ass_line,    
 STATE_3 ass_state3 
 ){
	//#ifdef _DEBUG_PRT_ATPG_
	LEARN_LIST *learn_list = NULL;
	LEARN_BRANCH *forward;
	LEARN_BRANCH *forward_top;
	LEARN_TYPE learn_type;
	//LEARN_LIST current;


	//char *func_name = "topgun_print_mes_learn_list_result_dag"; /* 関数名 */
	
	if ( ass_state3 == STATE3_0 ) {
		learn_list = ass_line->imp_0;
		forward_top = Node_head[ LEARN_STATE_0 ][ ass_line->line_id ]->forward;
	}
	else {
		learn_list = ass_line->imp_1;
		forward_top = Node_head[ LEARN_STATE_1 ][ ass_line->line_id ]->forward;
	}


	while ( learn_list ) {

		learn_type = LEARN_ON;
		forward = forward_top;
		
		//while ( forward ) {

		//current.line       = forward->node->line;
		//current.ass_state3 = forward->node->state3;
			
			/* 全く同一かを調べる */
			//learn_type = static_learn_equal_learn_list ( &current, learn_list->line, learn_list->ass_state3 );
		
		//if ( learn_type == LEARN_OFF ) {
		//break;
		//}
		//forward = forward->next;
		//}

		//if ( learn_type == LEARN_ON ) {
			printf("LEARN_RES");
			//}
			//else {
			//printf("LEARN_LES");
			//}
		/* メッセージ出力 */
		if ( ass_state3 == STATE3_0 ) {
			printf(" %8ld   state0 ",ass_line->line_id);
			if ( learn_list->ass_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",learn_list->line->line_id);
			}
			else {
				printf("-> %8ld state1\n",learn_list->line->line_id);
			}
		}
		else {
			printf(" %8ld   state1 ",ass_line->line_id);
			if ( learn_list->ass_state3 == STATE3_0 ) {
				printf("-> %8ld state0\n",learn_list->line->line_id);
			}
			else {
				printf("-> %8ld state1\n",learn_list->line->line_id);
			}
		}

		learn_list = learn_list->next;
	}

	//#endif /* _DEBUG_PRT_ATPG_ */
}


void test_node_children
(
 LEARN_NODE *node,
 char *org_func_name
 ){

#ifndef TOPGUN_TEST
	
	//char *func_name = "test_node_children";	/* 関数名 */
	
	if ( node->flag & LEARN_FLAG_CHILDREN ) {
		topgun_error( FEC_PRG_NODE_FLAG, org_func_name );
	}
#endif /* TOPGUN_TEST */
}

void topgun_print_mes_node
(
 LEARN_NODE *node
 ){
#ifdef _DEBUG_PRT_ATPG_
	LEARN_BRANCH *branch;
	
	topgun_print_mes_char ("2-L-I Node infomation \n");
	topgun_print_mes_char ("2-L-I ID ");
	topgun_print_mes_ulong_2( node->node_id );
	topgun_print_mes_n();
	topgun_print_mes_char ("2-L-I n_forward ");
	topgun_print_mes_ulong_2( node->n_for_org );
	topgun_print_mes_n();

	branch = node->forward;

	while ( branch ) {
		topgun_print_mes_char ("2-L-I forward  ");
		topgun_print_mes_ulong_2( branch->node->node_id );
		topgun_print_mes_n();
		branch = branch->next;
	}
	branch = node->backward;

	while ( branch ) {
		topgun_print_mes_char ("2-L-I backward " );
		topgun_print_mes_ulong_2( branch->node->node_id );
		topgun_print_mes_n();
		branch = branch->next;
	}

#endif /* _DEBUG_PRT_ATPG_ */
}

void topgun_print_mes_Topo_top
(
 void
 ){
#ifdef _DEBUG_PRT_ATPG_
	Ulong i;
	Ulong cnt = 0;
	LEARN_NODE *current_node;
	
	current_node = Topo_top.top_node;
	while ( current_node ) {

		cnt++;
		printf("Dump node level %2ld pi level %3ld id %4ld imp %2ld\n"
			   ,current_node->level, current_node->line->lv_pi, current_node->node_id, current_node->imp_count);
		current_node = current_node->next;

	}
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {
		current_node = Node_head[ LEARN_STATE_0 ][ i ];

		if ( ( current_node->level == 0 ) &&
			 ( current_node->n_for_org != 0 ) && 
			 ( current_node->node_id != 0 ) ){
			if ( current_node->flag & LEARN_FLAG_CHILDREN ) {
				printf("Dump child %1ld id %4ld imp %2ld\n"
					   ,current_node->level, current_node->node_id, current_node->imp_count);
			}
			else {
				cnt++;
				printf("Dump more  %1ld id %4ld imp %2ld\n"
					   ,current_node->level, current_node->node_id, current_node->imp_count);
			}
		}
		
		current_node = Node_head[ LEARN_STATE_1 ][ i ];
		
		if ( ( current_node->level == 0 ) &&
			 ( current_node->n_for_org != 0 ) && 
			 ( current_node->node_id != 0 ) ){
			if ( current_node->flag & LEARN_FLAG_CHILDREN ) {
				printf("Dump child %1ld id %4ld imp %2ld\n"
					   ,current_node->level, current_node->node_id, current_node->imp_count);
			}
			else {
				cnt++;
				printf("Dump more  %1ld id %4ld imp %2ld\n"
					   ,current_node->level, current_node->node_id, current_node->imp_count);
			}
		}
	}
	printf("Dump total %5ld\n",cnt);
#endif /* _DEBUG_PRT_ATPG_ */
}


void topgun_print_mes_node_connect_search
(
 LEARN_NODE *start_node
 ){
#ifdef _DEBUG_PRT_ATPG_
	
	LEARN_BRANCH *forward;
	LEARN_S_LIST *start;
	LEARN_S_LIST *search_list = NULL;
	LEARN_S_LIST *add_list = NULL;
	LEARN_S_LIST *new_s_list = NULL;

	char *func_name = "topgun_print_mes_node_connect_search"; /* 関数名 */
	
	topgun_print_mes_node ( start_node );

	add_list = NULL;
	forward = start_node->forward;
	
	while  ( forward ) {

		if ( ! ( forward->node->flag & LEARN_FLAG_DEBUG ) ){
			forward->node->flag |= LEARN_FLAG_DEBUG;
			
			new_s_list = ( LEARN_S_LIST * )topgunMalloc
				( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
	
			new_s_list->node = forward->node;
			new_s_list->next = add_list;
			add_list         = new_s_list;
		}
		forward = forward->next;
	}

	start = add_list;
	search_list = add_list;
	
	while ( search_list ) {
		
		add_list = NULL;
		forward = search_list->node->forward;
		while  ( forward ) {

			if ( ! ( forward->node->flag & LEARN_FLAG_DEBUG ) ){
				forward->node->flag |= LEARN_FLAG_DEBUG;
			
				new_s_list = ( LEARN_S_LIST * )topgunMalloc
					( FMT_LEARN_S_LIST, sizeof( LEARN_S_LIST ), 1, func_name );
	
				new_s_list->node = forward->node;
				new_s_list->next = add_list;
				add_list      = new_s_list;
			}
			forward = forward->next;
		}
		static_learn_dag_make_connect( search_list, add_list );
		search_list = search_list->next;
	}

	search_list = start;

	while ( search_list ){

		topgun_print_mes_node ( search_list->node );

		search_list->node->flag &= ~LEARN_FLAG_DEBUG;

		new_s_list  = search_list;
		search_list = search_list->next;

		topgunFree ( new_s_list, FMT_LEARN_S_LIST, 1, func_name );
	}

#endif /* _DEBUG_PRT_ATPG_ */
}

void topgun_print_mes_search_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
	topgun_print_mes( 12, 3 );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( current_node->line->line_id );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( current_node->node_id );			
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_LEARN_ */
 }
/* End of File */

void topgun_print_mes_dont_search_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
	topgun_print_mes( 12, 4 );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( current_node->node_id );
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_LEARN_ */
 }
/* End of File */

void topgun_print_mes_already_search_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
	topgun_print_mes( 12, 5 );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( current_node->node_id );
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_redo_sort
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
	topgun_print_mes( 12, 10 );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( current_node->node_id );
	topgun_print_mes_n();
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_iteration
(
 Ulong loop_countxo
 ){
#ifdef _DEBUG_PRT_LEARN_
	topgun_print_mes( 12, 9 );
	topgun_print_mes_sp( 1 );
	topgun_print_mes_ulong_2( loop_count );
	topgun_print_mes_n();

	
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_new_fix_line
(
 LINE *line
 ){
#ifdef _DEBUG_PRT_LEARN_
	switch ( line->state9 ){
	case STATE9_0X:
		//printf("New FIX : %8ld state0\n",line->line_id);
		break;
	case STATE9_1X:
		//printf("New FIX : %8ld state1\n",line->line_id);
		break;
	default:
		break;
	}	
#endif /* _DEBUG_PRT_LEARN_ */
 }


void topgun_print_mes_dag_next_entry
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_ATPG_

	LEARN_BRANCH *backward;
	LEARN_NODE *back_node;
	
	backward = current_node->backward;
	while ( backward ) {
		
		printf("2-L-3 next %4ld -> ",current_node->node_id);
		
		back_node = backward->node;
		
		if ( ( back_node->n_forward == 0 ) &&
			 ( ! ( back_node->flag & LEARN_FLAG_ONLIST ) ) &&
			 ( ! ( back_node->flag & LEARN_FLAG_FINISH ) ) ){
			
			printf("%4ld Entry\n",back_node->node_id);

		}
		else {
			printf("%4ld Noentry ",back_node->node_id);
			if ( back_node->n_forward != 0 ) {
				printf(" n_forward == %2ld",back_node->n_forward);
			}
			else {
				printf("                " );
			}
			if ( back_node->flag & LEARN_FLAG_ONLIST ) {
				printf(" onlist" );
			}
			else {
				printf("       " );
			}
			if ( back_node->flag & LEARN_FLAG_FINISH ) {
				printf(" finish" );
			}
			else {
				printf("       " );
			}
			printf("\n");
			
		}
		backward = backward->next;
	}
#endif /* _DEBUG_PRT_ATPG_ */
 }


void topgun_print_mes_dag_rest_check
(
 void
 ){
#ifdef _DEBUG_PRT_LEARN_
	Ulong i;
	LEARN_NODE *node;

	Ulong flag = 0;
	
	for ( i = 0 ; i < Line_info.n_line ; i++ ) {
		
		node = Node_head[ LEARN_STATE_0 ][ i ];
		
		if ( node->node_id != 0 ) {
			if ( ! ( node->flag & LEARN_FLAG_CHILDREN ) ) {
				if ( ! ( node->flag & LEARN_FLAG_FINISH ) ) {
					if ( flag == 0 ) {
						printf("2-L-3 rest check\n");
						flag++;
					}
					printf("2-L-3 rest line %5ld %5ld n_forward %2ld n_for_org %2ld\n",node->line->line_id, node->node_id, node->n_forward, node->n_for_org);
				}
			}
			node = Node_head[ LEARN_STATE_1 ][ i ];
			if ( ! ( node->flag & LEARN_FLAG_CHILDREN ) ) {
				if ( ! ( node->flag & LEARN_FLAG_FINISH ) ){
					
					if ( flag == 0 ) {
						printf("2-L-3 rest check\n");
						flag++;
					}
					
					printf("2-L-3 rest line %5ld %5ld n_forward %2ld n_for_org %2ld\n",node->line->line_id, node->node_id, node->n_forward, node->n_for_org);
				}
			}
		}
	}
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_reentry
(
 LEARN_NODE *reentry_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-3 reentry %4ld %4ld start\n",reentry_node->line->line_id, reentry_node->node_id);
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }


void topgun_print_mes_reentry_org
(
 LEARN_NODE *reentry_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("-----o add research node n%8ld %5ld\n"
		   ,reentry_node->node_id
		   ,reentry_node->line->line_id);
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
}
void topgun_print_mes_reentry_child
(
 LEARN_NODE *reentry_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("-----o add research node n%8ld %5ld\n"
		   ,reentry_node->node_id
		   ,reentry_node->line->line_id);
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
}
void topgun_print_mes_reentry_from
(
 LEARN_NODE *reentry_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("-----f add research node n%8ld %5ld\n"
		   ,reentry_node->node_id
		   ,reentry_node->line->line_id);
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
}
void topgun_print_mes_reentry
(
 LEARN_NODE *reentry_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("-----a add research node n%8ld %5ld\n"
		   ,reentry_node->node_id
		   ,reentry_node->line->line_id);
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }


void topgun_print_mes_dag_reentry_next
(
 LEARN_NODE *reentry_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-3 reentry %4ld %4ld\n",reentry_node->line->line_id, reentry_node->node_id);
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_select_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	//printf("2-L-3 select stack node ID %ld\n",current_node->node_id);
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_select_node_add
(
 LEARN_NODE *current_node,
 Ulong add
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-L select node %8ld add %3ld\n", current_node->node_id,add );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_exe_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-R exe research node %8ld\n",current_node->node_id );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_pass_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-R pass forward node %8ld\n",current_node->node_id );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_onlist_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-R pass onlist  node %8ld\n",current_node->node_id );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_finish_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-R pass finish  node %8ld\n",current_node->node_id );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_hatena_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-R pass hatena  node %8ld\n",current_node->node_id );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_illegal_add_node
(
 LEARN_S_LIST *tmp_tmp
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	if ( tmp_tmp != NULL ) {
	 
		while ( tmp_tmp ) {
			printf("search_add node %8ld\n",tmp_tmp->node->node_id);
			tmp_tmp = tmp_tmp->next;
		}
	}
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
}

void topgun_print_mes_dag_illegal_node
(
 LEARN_NODE *current_node
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-R illegal      node %8ld\n",current_node->node_id );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }

void topgun_print_mes_dag_n_add_node
(
 Ulong n_add
 ){
#ifdef _DEBUG_PRT_LEARN_
#ifdef _DEBUG_PRT_ATPG_
	printf("2-L-L add node %8ld\n", n_add );
#endif /* _DEBUG_PRT_ATPG_ */
#endif /* _DEBUG_PRT_LEARN_ */
 }


/* End of File */
