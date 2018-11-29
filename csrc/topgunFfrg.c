/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief Fanout Free Region Graph�˴ؤ���ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <stdlib.h>

#include "topgun.h"
#include "topgunLine.h"
#include "topgunMemory.h"
#include "topgunFfrg.h"

extern LINE_INFO  Line_info;
extern LINE       *Line_head;

static void ffrg_init ( FFRG  * ); //! FFRG�ν�����ؿ�
static void ffrg_make_edge_list( LINE *, FFRG_EDGE_LIST *); //! FFRG����³�����
static void ffrg_init_edge_list( FFRG_EDGE_LIST * ); //! edge_list�ν����
static void ffrg_free_edge_list( FFRG_EDGE_LIST * );  //! edge_list�γ���
static void ffrg_eliminate_multiple_edges( FFRG * ); //! ¿���դ�õ��
static FFRG_NODE *ffrg_check_multiple_edges( FFRG_NODE * ); //! ¿���դΥڥ��Ρ��ɤ��֤�
static void ffrg_merge_multiple_edges( FFRG_NODE *, FFRG_NODE * ); //! ¿���դ��ä���

//! debug�Ѵؿ���
static void ffrg_dump_ffrg( FFRG * ); //! FFRG�ι�¤��dump����

/* topgun_print.c */
void topgun_print_imp_all( void );
void topgun_print_line_all( void );



/*!
  @brief FFRG�Ѥν�����ؿ�

  @param [in] void
  @return void

*/

void ffrg
(
 void
 ){
	FFRG  ffrg;

	ffrg.n_node = 0;
	ffrg.node = NULL;

	// ����� ( ������ݤ���³ )
	ffrg_init ( &ffrg );

	// ¿���դ��ä���
	ffrg_eliminate_multiple_edges ( &ffrg );

	topgun_print_line_all();
	ffrg_dump_ffrg ( &ffrg );

	exit(0);
}

/*!
  @brief FFRG�Ѥν�����ؿ�

  @param [in] void
  @return void

*/

void ffrg_init
(
 FFRG  *ffrg
 ){
	
	Ulong i; // ������(��������)
	Ulong j; // ������(FFRG�Ρ��ɿ�)
	LINE  *line = NULL;
	FFRG_EDGE_LIST edge_list;
	
	char *func_name = "ffrg_init"; // �ؿ�̾

	
	// FFR����դΥΡ��ɿ��Υ������
	// ������ݤʤɤ˻���
	for ( i = 0 ; i < Line_info.n_line ;  i++ ) {

		line = &Line_head[ i ];

		if ( ( line->type == TOPGUN_PI ) ||
			 ( line->type == TOPGUN_PO ) ) {
			ffrg->n_node++;
		}
		else if ( line->n_out > 1  ) {
			ffrg->n_node++;
		}
	}

	// FFR����դΥΡ��ɤΥ������
	ffrg->node = ( FFRG_NODE * )topgunMalloc
		( FMT_FFRG_NODE, sizeof( FFRG_NODE ), ffrg->n_node, func_name );

	for ( i = 0 ; i < ffrg->n_node ; i++ ) {
		// �ƥΡ��ɤν����
		ffrg->node[ i ].node_id = i;
		ffrg->node[ i ].n_edge  = 0;
		ffrg->node[ i ].flag    = FFRG_FLAG_NULL;
		ffrg->node[ i ].line    = NULL;
		ffrg->node[ i ].edge    = NULL;
	}

	// �ƥΡ��ɽ��������2
	for ( i = 0 , j = 0; i < Line_info.n_line ; i++ ) {

		line = &Line_head[ i ];
		
		if ( ( line->type == TOPGUN_PI ) ||
			 ( line->type == TOPGUN_PO ) ||
			 ( line->n_out > 1  ) ) {

			ffrg->node[ j ].line = line;
			line->ffrg            = &( ffrg->node[ j ] );
			j++;
		}
	}

	ffrg_init_edge_list( &edge_list );
	
	// �ƥΡ��ɽ����
	// �ƥΡ��ɴ֤���³��Ԥ�
	for ( i = 0 ; i < ffrg->n_node ; i++ ) {
		ffrg_make_edge_list ( ffrg->node[ i ].line, &edge_list );

		ffrg->node[ i ].edge = ( FFRG_NODE ** )topgunMalloc
			( FMT_FFRG_NODE_P, sizeof( FFRG_NODE *), edge_list.n_edge, func_name );


		for ( j = 0 ; j < edge_list.n_edge ; j++ ) {
			ffrg->node[ i ].edge[ j ] = edge_list.edge[ j ]->ffrg;
			edge_list.edge[ j ] = NULL;
		}
		ffrg->node[ i ].n_edge = edge_list.n_edge;
		edge_list.n_edge = 0;
	}

	ffrg_free_edge_list( &edge_list );
}

/*!
  @brief FFRG�Ѥν�����ؿ�

  @param [in] void
  @return void

*/

void ffrg_make_edge_list
(
 LINE *start,
 FFRG_EDGE_LIST *edge_list
 ){

	Ulong i, j; //������
	LINE *line = NULL;
	
	//����¦��õ������
	for ( i = 0 ; i < start->n_out ; i++ ) {
		line = start->out[ i ];

		while ( line->n_out == 1 ) {
			line = line->out[ 0 ]; 
		}
		//PO or BLKO or Fantout Stem
		edge_list->edge[ edge_list->n_edge ] = line;
		edge_list->n_edge++;

		if ( edge_list->n_edge == edge_list->n_size ) {
			printf(" size over \n" );
			exit(0);
		}
	}

	// ����¦��õ������
	// 0��Ƚ���Ѥˤ����ƶ��ˤ���
	for ( i = 0, j = 1 ; i < start->n_in ; i++ ) {
		// enter list
		edge_list->search[ j ] = start->in[ i ];
		j++;
	}

	
	while ( j > 1 ) {
		// get list
		j--;
		line = 	edge_list->search[ j ];
		
		//�ߤ�뤫�����õ���ˤ�������Ƚ��
		if ( ( line->n_out > 1 )  || // FanoutStem or PI or BLKI
			 ( line->n_in == 0 ) ) {
			// �ߤ��
			// PI or BLKI or Fantout Stem
			edge_list->edge[ edge_list->n_edge ] = line;
			edge_list->n_edge++;
		}
		else {
			// �����õ��
			for ( i = 0 ; i < line->n_in ; i++ ) {
				// enter list
				edge_list->search[ j ] = line->in[ i ];
				j++;
			}
		}
	}
}



/*!
  @brief FFRG�Ѥλ޳��ݥƥ�ݥ�������ؿ�

  @param [in] EDGE_LIST *edge_list
  @return void

*/

void ffrg_init_edge_list
(
 FFRG_EDGE_LIST *edge_list
 ){

	Ulong i; // ������
	char *func_name = "ffrg_init_edge_list"; // �ؿ�̾

	edge_list->n_edge   = 0; 	// ���å����ν����
	edge_list->n_size   = FFRG_MAX_EDGE_SIZE;	// ���å�����Ȣ�ο�
	edge_list->n_size_s = FFRG_MAX_EDGE_SIZE_S;	// ���å�����Ȣ�ο�

	edge_list->edge = ( LINE ** )topgunMalloc
		( FMT_LINE_P, sizeof( LINE *), edge_list->n_size, func_name );
	
	for ( i = 0 ; i < edge_list->n_size ; i++ ) {
		edge_list->edge[ i ] = NULL;
	}
	
	edge_list->search = ( LINE ** )topgunMalloc
		( FMT_LINE_P, sizeof( LINE *), edge_list->n_size_s, func_name );
	
	for ( i = 0 ; i < edge_list->n_size_s ; i++ ) {
		edge_list->search[ i ] = NULL;
	}
}

/*!
  @brief FFRG�Ѥλ޳��ݥƥ�ݥ���������

  @param [in] EDGE_LIST *edge_list
  @return void

*/

void ffrg_free_edge_list
(
 FFRG_EDGE_LIST *edge_list
 ){

	char *func_name = "ffrg_free_edge_list"; // �ؿ�̾

	topgunFree ( edge_list->edge, FMT_LINE_P, edge_list->n_size, func_name );
	topgunFree ( edge_list->search, FMT_LINE_P, edge_list->n_size_s, func_name );
}


/*!
  @brief FFRG���¿���դ��ä���

  @param [in] EDGE_LIST *edge_list
  @return void

*/

void ffrg_eliminate_multiple_edges
(
 FFRG *ffrg
 ){

	Ulong     i; //������
	FFRG_NODE *node;
	FFRG_NODE *pair_node;
	
	// char *func_name = "ffrg_eliminate_multiple_edges"; // �ؿ�̾

	for ( i = 0 ; i < ffrg->n_node ; i++ ) {
		node = &(ffrg->node[ i ]);

		//¿���դ����뤫���ǧ����
		pair_node = ffrg_check_multiple_edges ( node );

		if ( pair_node != NULL ) {
			//¿���դ��ä���
			ffrg_merge_multiple_edges ( node, pair_node ) ;
			pair_node = NULL;
			i--;
		}
	}
}


/*!
  @brief FFRG���¿���դ��֤�

  @param [in] FFRG_NODE *pair_node
  @return void

*/


FFRG_NODE *ffrg_check_multiple_edges
(
 FFRG_NODE *node
 ){

	Ulong i, j; // ������

	node->edge[0]->flag |= FFRG_FLAG_MULTI;
	
	for ( i = 1 ; i < node->n_edge ; i++ ) {

		if ( node->edge[ i ]->flag & FFRG_FLAG_MULTI ) {

			//! ¿����ȯ��
			for ( j = 0 ; j < i ; j++ ) {
				node->edge[ j ]->flag &= ~FFRG_FLAG_MULTI;
			}
			return ( node->edge[ i ] );
		}
	}
	//! ¿���դϤʤ�
	for ( i = 0 ; i < node->n_edge ; i++ ) {
		node->edge[ i ]->flag &= ~FFRG_FLAG_MULTI;
	}
	return ( NULL );
}

/*!
  @brief FFRG���¿���դ��ä���

  @node �Ȥꤢ�����Ρ��ɤ�ޡ������Ƥ��������
  
  @param [in] FFRG_NODE *pair_node
  @return void

*/


void ffrg_merge_multiple_edges
(
 FFRG_NODE *node,
 FFRG_NODE *pair_node
 ){

	Ulong i; // ������
	Ulong del_n_edge = 0;
	Ulong new_n_edge = 0;

	//! pair_node¦��������

	//! �ޤ�edge�ο���Ĵ�٤�

	for ( i = 0 ; i < node->n_edge ; i++ ) {
		if ( node->edge[ i ]->node_id == pair_node->node_id ) {
			del_n_edge++;
		}
	}

	//! �������Ρ��ɤΥΡ��ɿ���׻�
	new_n_edge = node->n_edge + pair_node->n_edge - ( del_n_edge * 2 );
	//! 
	
}

	


/*!
  @brief FFRG�Ѥ�debug�ؿ� FFRG��dump����

  @param [in] FFRG *ffrg
  @return void

*/

void ffrg_dump_ffrg
(
 FFRG *ffrg
 ){

	Ulong i,j; //! ������
	FFRG_NODE *node;
	
	for ( i = 0 ; i < ffrg->n_node ; i++ ) {

		node = &(ffrg->node[ i ]);

		printf("FFRG ID      %5lu\n", node->node_id );
		printf("FFRG FLAG    %5lu\n", node->flag );
		printf("FFRG Line_id %5lu\n", node->line->line_id );
		printf("FFRG N_EDGE  %5lu\n", node->n_edge );

		for ( j = 0 ; j < node->n_edge ; j++ ) {
			printf("FFRG Edge[%2lu] -> %lu\n", j, node->edge[j]->node_id );
		}
	}
}
