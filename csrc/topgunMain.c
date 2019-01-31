/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief Flight�ᥤ��ؿ���

	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include "topgun.h"
#include "topgunTime.h"
#include "topgunError.h"
#include "topgunLine.h"

/* topgun_init.c */
void topgun_init();

/* topgun_atpg.c */
int  topgun_atpg();

/* topgun_output.c */
void output( void );


void topgun_memory_log_file_open();
void topgun_memory_log_file_close();

void topgun_print_mes_opening( void );
void topgun_print_mes_process_time_all( void );

void helloworld(void);
void n_line_out(void);
void line_point(void);
//void makeCycle(void);
void dfs(void);
void makeBench(void);
void eval(char *);
void obfuscation(char *);
void simulate2(char *);
void test(void);
//void info_line_structure();

/********************************************************************************
 * << Function >>
 *		Topgun ATPG main function
 *
 * << Function ID >>
 *	   	1-1
 *
 * << Return >>
 *  0           : Normal Terminate
 *	ERROR		: System Error
 *
 * << Argument >>
 * args			(I/O)	type	description
 *      Void
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura
 *
 * << Date >>
 *		2016/12/6
 *
 ********************************************************************************/

int main
(
 int  argc,      /* ���ޥ�ɰ����θĿ� */
 char *argv[]    /* ���ޥ�ɰ�����ʸ���� */
){
    int      result; /* atpg�¹Է�� */
	
    char *func_name = "main"; /* �ؿ�̾ */
    
    topgun_0_utility_enter_start_time( &Time_head.total );
    
    /* opening message */
    topgun_print_mes_opening();
    
    
    /* memory log file open */
    topgun_memory_log_file_open();
    
    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
    /* $$$ 1 ATPG�������                                                   $$$ */
    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
    
    topgun_init( argv[ 1 ] );
    
    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
    /* $$$ 2 ATPG                                                           $$$ */
    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
    
    /* ATPG�¹� */
    // 2016/12/18 �Ȥꤨ��atpg��̵�������ơ�result��ATPG_FINISH�ˤ���
    //result = topgun_atpg( argv[ 2 ] );
    result = ATPG_FINISH;

    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */
    /* $$$ 3 ATPG�����                                                     $$$ */
    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

    /* ��̽��� */
    output( );

    switch( result ) {
    case ATPG_FINISH:
	break;
    default:
	topgun_error( FEC_NO_ERROR_CODE ,func_name );
	break;
    }
    /* memory log file open */
    topgun_memory_log_file_close();

    topgun_0_utility_enter_end_time( &Time_head.total );


    topgun_print_mes_process_time_all();

    //helloworld();
    //n_line_out();
    //line_point();
    //getLevelandSort();
    eval( argv[ 3 ] );
    obfuscation( argv[ 2 ] );
    //simulate2( argv[ 4 ] );
    /*
    printf("*****************\n");
    info_line_structure();
	*/
    return( 0 ); /* normal finish */
}

/* End of File */
