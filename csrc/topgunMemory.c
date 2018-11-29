/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief メモリ確保/解放/記録に関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include<stdio.h>
#include<stdlib.h>

#include"topgun.h"
#include"topgunMemory.h" /* Topgun Memory Type ID */
#include"topgunError.h"  /* Topgun Error Code ID */

#ifdef TOPGUN_MEMORY_LOG
extern FILE *Memory_log_fp;
extern char TOPGUN_MEMORY_TYPE_NAME[ FMT_NUM_MEMORY_TYPE ][ 20 ];
extern int  TOPGUN_MEMORY_TYPE_SIZE[ FMT_NUM_MEMORY_TYPE ];
#endif /* TOPGUN_MEMORY_LOG */

extern void topgun_memory_log_file_open ( void );
extern void topgun_memory_log_file_close ( void );

/********************************************************************************
 * << Function >>
 *		original "malloc" function for TOPGUN
 *
 * << Function ID >>
 *	   	20-1
 *
 * << Return >>
 *	   pointer		: allocate memory pointer
 *
 * << Argument >>
 * args			(I/O)	type	description
 * type_id       I      int     malloc type ID
 * num           I      int     number of allocated memory
 * size          I      int     size by allocated memory
 * org_func_name I      char *  name of function calling "feets malloc"
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/16
 *
 ********************************************************************************/

void *topgunMalloc
(
 TOPGUN_MEMORY_TYPE_ID type_id,          /*  メモリの型ID                     */
 int size,                               /*  メモリ確保サイズ                 */
 int num,                                /*  メモリ確保数                     */
 char *org_func_name                     /*  呼び出し元関数名                 */
 ){
    char *func_name = "topgunMalloc";   /*  関数名                           */
    
    void *pointer;                       /*  メモリ確保用ポインタ             */

#ifdef TOPGUN_MEMORY_LOG
    fprintf( Memory_log_fp, "M TypeID %2d Type %-12s Num %8d Size %2d Func %-30s\n",
	     type_id, TOPGUN_MEMORY_TYPE_NAME[ type_id ], num, size, org_func_name);
#endif /* TOPGUN_MEMORY_LOG */
    
    /* メモリを確保 */
    pointer = ( void * )malloc( num * size );
    
    /* メモリを確保出来なかった場合はエラー終了 */
    if ( pointer == NULL ){
        topgun_error_o( FEC_CANT_ALLOCATE_MEMORY, func_name, org_func_name );
    }
    return( pointer );
}

/********************************************************************************
 * << Function >>
 *		original "free" function for TOPGUN
 *
 * << Function ID >>
 *	   	20-2
 *
 * << Return >>
 *	    Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * type_id       I      int     malloc type ID
 * num           I      int     number of allocated memory
 * size          I      int     size by allocated memory
 * org_func_name I      char *  name of function calling "feets malloc"
 *
 * << extern >>
 * name			(I/O)	type	description
 *     None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/16
 *
 ********************************************************************************/

void topgunFree
(
 void *ptr,                                /*  解放するメモリのポインタ         */
 TOPGUN_MEMORY_TYPE_ID type_id,            /*  メモリの型ID                     */
 int num,                                  /*  メモリ確保数                     */
 char *org_func_name                       /*  呼び出し元関数名                 */
)
{
	free ( ptr );

#ifdef TOPGUN_MEMORY_LOG
	fprintf( Memory_log_fp, "F TypeID %2d Type %-12s Num %8d Size %2d Func %-30s\n",
			 type_id, TOPGUN_MEMORY_TYPE_NAME[ type_id ], num,
			 TOPGUN_MEMORY_TYPE_SIZE[ type_id], org_func_name );
#endif /* TOPGUN_MEMORY_LOG */

}

/********************************************************************************
 * << Function >>
 *		memory management log file open
 *
 * << Function ID >>
 *	   	20-3
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Memory_log_fp I      FLIE *  memory log file pointer
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/16
 *
 ********************************************************************************/

void topgun_memory_log_file_open
(
 void
){

#ifdef TOPGUN_MEMORY_LOG
	char    *func_name = "topgun_memory_log_file_open";	/* 関数名               */

	if ( ( Memory_log_fp = fopen ( FLINE_MEMORY_LOG_FILE_NAME, "w" ) ) == NULL ) {
		topgun_error( FEC_FILE_OPEN_ERROR, func_name );
	}
#endif /* TOPGUN_MEMORY_LOG */
}

/********************************************************************************
 * << Function >>
 *		memory management log file close
 *
 * << Function ID >>
 *	   	20-4
 *
 * << Return >>
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * Memory_log_fp I      FLIE *  memory log file pointer
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/6/16
 *
 ********************************************************************************/

void topgun_memory_log_file_close
(
 void
){

#ifdef TOPGUN_MEMORY_LOG
	fclose( Memory_log_fp );
#endif /* TOPGUN_MEMORY_LOG */
}
