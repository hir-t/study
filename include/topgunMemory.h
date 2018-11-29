/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 動的メモリ確保に関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#ifndef TOPGUN_MEMORY_H
#define TOPGUN_MEMORY_H

#define FLINE_MEMORY_LOG_FILE_NAME    "topgun_memory.log" //!< メモリの確保状況を示すログファイルのファイル名

/*!
  @enum topgun_memory_type_id
  @brief 確保するメモリ型のID番号。

  @note
  ログファイル用に存在する
*/
typedef enum topgunMemoryTypeId
{
    FMT_Char                    = 1,  //!<  Char
    FMT_Char_P,                       //!<  Char pointer
    FMT_Ulong,                        //!<  Unsigned long
    FMT_Ulong_P,                      //!<  Unsigned long pointer
    FMT_LINE,                         //!<  Line
    FMT_LINE_P,                       //!<  Line pointer
    FMT_LINE_PP,                      //!<  Line pointer's pointer
    FMT_FLIST,                        //!<  FLIST
    FMT_FLIST_P,                      //!<  FLIST pointer
    FMT_LINE_STACK,                   //!<  LINE_STACK
    FMT_LINE_STACK_P,                 //!<  LINE_STACK pointer
    FMT_BTREE,                        //!<  Back trace TREE
    FMT_DFRONT,                       //!<  D-Frontier
    FMT_DFRONT_P,                     //!<  D-Frontier pointer
    FMT_DFRONT_LIST,                  //!<  D-Frontier LIST
    FMT_ASS_LIST,                     //!<  ASsign LIST
    FMT_IMP_LIST,                     //!<  IMPlication LIST
    FMT_IMP_INFO,                     //!<  IMPlication Infomation
    FMT_IMP_TRACE,                    //!<  IMPlication Trace Infomation
    FMT_JUST_INFO,                    //!<  JUSTified Infomation
    FMT_JUST_INFO_P,                  //!<  JUSTified Infomation pointer
    FMT_CELL,                         //!<  CELL
    FMT_CELL_P,                       //!<  CELL pointer
    FMT_CELL_IO,                      //!<  CELL_IO pointer
    FMT_EVENT_LIST,                   //!<  EVENT_LIST
    FMT_CC_PATTERN,                   //!<  CC pattern
    FMT_CC_PATTERN_P,                 //!<  CC pattern pointer
    FMT_LEARN_LIST,                   //!<  Learn LIST
    FMT_LEARN_LIST_P,                 //!<  Pointer of Learn LIST
    FMT_LEARN_NODE,                   //!<  Learn Node
    FMT_LEARN_NODE_P,                 //!<  Learn Node pointer
    FMT_LEARN_NODE_PP,                //!<  Pointer of Learn Node pointer
    FMT_LEARN_BRANCH,                 //!<  Learn Branch
    FMT_LEARN_BRANCH_P,               //!<  Learn Branch pointer
    FMT_LEARN_S_LIST,                 //!<  Learn Singly linked list
    FMT_LEARN_S_LIST2,                //!<  List of Learn Singly linked list 
    FMT_LEARN_B_LIST,                 //!<  Learn branch list
    FMT_LEARN_TOPO,                   //!<  Learn Branch
    FMT_LEARN_NODE_LIST,              //!<  Learn Node List
    FMT_LEARN_EQ_INFO,                //!<  Learn EQual line INFOmation
    FMT_LEARN_EQ_HEAD,                //!<  Learn EQual line HEADer
    FMT_GENE_INFO,                    //!<  Generate infomation
    FMT_STOCK_PAT,                    //!<  Stock pattern 
    FMT_STOCK_PAT_P,                  //!<  Pointer of Stock pattern
    FMT_STOCK_FAULT,                  //!<  Target Faults of Stock pattern 
    FMT_STOCK_FAULT_P,                //!<  Pointer of Target Faults of Stock pattern 
    FMT_FFRG_NODE,                    //!<  Node of Fanout Free Region Graph
    FMT_FFRG_NODE_P,                  //!<  Pointer of Node of Fanout Free Region Graph
    FMT_NUM_MEMORY_TYPE,              //!<  nubmer of memory type
}TOPGUN_MEMORY_TYPE_ID;


/* function list */

extern void  *topgunMalloc( TOPGUN_MEMORY_TYPE_ID, int, int, char * ); /* 動的にメモリを確保する */
extern void  topgunFree( void *, TOPGUN_MEMORY_TYPE_ID, int, char * );  /* メモリを解放する */

#endif
