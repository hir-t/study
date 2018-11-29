/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief エラーメッセージに関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/



#include<stdio.h>
#include"topgun.h"
#include"topgunMemory.h"
#include"topgunError.h"

static void topgun_exit();

extern char TOPGUN_MEMORY_TYPE_NAME[FMT_NUM_MEMORY_TYPE][20];

/********************************************************************************
 * << Function >>
 *		output error message
 *
 * << Function ID >>
 *	   	???
 *
 * << Return >>
 *	0           : Terminated in normal
 *	ERROR		: System Error
 *
 * << Argument >>
 * args			(I/O)	type	description
 *
 * << extern >>
 * name			(I/O)	type	description
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/4/15
 *
 ********************************************************************************/

void topgun_error
(
 TOPGUN_ERROR_CODE error_code,
 char *error_function_name
 )
{
	fprintf( stderr, "Error Code     : %d\n", error_code );
	fprintf( stderr, "@              : %s\n", error_function_name);
	topgun_exit( ERROR );
}

void topgun_error_o
(
 int  error_code,
 char *error_function_name,
 char *org_func_name
 )
{
	fprintf( stderr, "Error Code     : %d\n", error_code );
	fprintf( stderr, "@              : %s\n", org_func_name);
	fprintf( stderr, "->             : %s\n", error_function_name);
	topgun_exit( ERROR );
}

void topgun_error_c
(
 TOPGUN_ERROR_CODE error_code,
 char *error_function_name,
 char *character
 )
{
	fprintf( stderr, "Error Code     : %d %s\n", error_code, character);
	fprintf( stderr, "@              : %s\n", error_function_name);
	topgun_exit( ERROR );
}

void topgun_error_type
(
 TOPGUN_MEMORY_TYPE_ID type_id,
 char *error_function_name
 )
{
	topgun_error ( FEC_PRG_NULL, error_function_name );
	//topgun_error_c ( FEC_PRG_NULL, TOPGUN_MEMORY_TYPE_NAME[ type_id ], error_function_name );
}

void topgun_exit(){
	void exit();
	exit(-1);
}
