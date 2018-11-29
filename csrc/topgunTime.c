/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 処理時間の計測に関する関数群
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/
#include <stdio.h>

/* for linux */
#include <sys/resource.h>
#include <sys/time.h>
/* for linux */

#include "topgun.h"
#include "topgunState.h"
#include "topgunAtpg.h"
#include "topgunError.h" /* FEC_XX */
#include "topgunIo.h"   /* DELIMITER_XX */
#include "topgunTime.h"   /* DELIMITER_XX */


#define  TIME_CONVERT( s, us ) ( (double) s + (double) us * 0.000001 )

/* 1970年から2000年からの時間(秒) (年数*365+閏年)*24時間*3600秒 */
static int TIME_OFF_2000 = ( ( 2000 - 1970 ) * 365 + ( 2000-1969 ) / 4 ) * 24 * 3600;



/********************************************************************************
 * << Function >>
 *		Getting Time
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	       description
 * operate       I      PROCESS_TIME * startする処理
 * 
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/1/4
 *
 ********************************************************************************/

void utility_get_time
(
 GET_TIME *get
 )
{
	struct rusage user_system;
	struct timeval real;
	
	getrusage( RUSAGE_SELF, &user_system );

	/* cpu time for user */
	get->user = TIME_CONVERT ( user_system.ru_utime.tv_sec,
							   user_system.ru_utime.tv_usec );

	/* cpu time for system */
	get->system = TIME_CONVERT ( user_system.ru_stime.tv_sec,
								 user_system.ru_stime.tv_usec );
	
	/* real time */
	gettimeofday ( &real, NULL );

	/* 2000年からの時刻に変換 */
	real.tv_sec -= TIME_OFF_2000;
	
	get->real = TIME_CONVERT ( real.tv_sec,
							   real.tv_usec );
}


/********************************************************************************
 * << Function >>
 *		Start CPU Time
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	       description
 * operate       I      PROCESS_TIME * startする処理
 * 
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/27
 *
 ********************************************************************************/

 void utility_enter_start_time
(
 PROCESS_TIME *operate
 ){
	 GET_TIME get;
	 
	 utility_get_time ( &get );
	 
	 operate->start.user   = get.user;
	 operate->start.system = get.system;
	 operate->start.real   = get.real;
}

/********************************************************************************
 * << Function >>
 *		Stop & Calculate CPU Time
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	       description
 * operate       I      PROCESS_TIME * startする処理
 * 
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/27
 *
 ********************************************************************************/

void utility_enter_end_time
(
 PROCESS_TIME *operate

 ){
	GET_TIME get;

	utility_get_time ( &get );

	operate->total.user   += ( get.user   - operate->start.user );
	operate->total.system += ( get.system - operate->start.system );
	operate->total.real   += ( get.real   - operate->start.real );
	
	operate->count++;

}

/********************************************************************************
 * << Function >>
 *		Stop & Calculate CPU Time
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	       description
 * operate       I      PROCESS_TIME * startする処理
 * 
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/27
 *
 ********************************************************************************/

void utility_enter_end_time_gene
(
 PROCESS_TIME *operate,
 GENE_INFO *gene_info
 ){
	GET_TIME get;

	utility_get_time ( &get );

	operate->total.user   += ( get.user   - operate->start.user );
	operate->total.system += ( get.system - operate->start.system );
	operate->total.real   += ( get.real   - operate->start.real );

	operate->count++;

	if ( gene_info->f_class == GENE_DET_A ) {
		Time_head.gene_det.total.user   += ( get.user   - operate->start.user );
		Time_head.gene_det.total.system += ( get.system - operate->start.system );
		Time_head.gene_det.total.real   += ( get.real   - operate->start.real );

		Time_head.gene_det.count++;
	}
	else if ( ( gene_info->f_class == GENE_RED ) ||
			  ( gene_info->f_class == GENE_UNT ) ) {
		Time_head.gene_redun.total.user   += ( get.user   - operate->start.user );
		Time_head.gene_redun.total.system += ( get.system - operate->start.system );
		Time_head.gene_redun.total.real   += ( get.real   - operate->start.real );

		Time_head.gene_redun.count++;
	}
	else {
		Time_head.gene_abort.total.user   += ( get.user   - operate->start.user );
		Time_head.gene_abort.total.system += ( get.system - operate->start.system );
		Time_head.gene_abort.total.real   += ( get.real   - operate->start.real );

		Time_head.gene_abort.count++;
	}
}

/********************************************************************************
 * << Function >>
 *		Initialize CPU Time
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	       description
 * operate       I      PROCESS_TIME * 処理する
 * 
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/27
 *
 ********************************************************************************/

void utility_process_time_init
(
 PROCESS_TIME *operate
 ){
	operate->count = 0;
	operate->total.user   = 0;
	operate->total.system = 0;
	operate->total.real   = 0;
}


/* これ以降の二つの関数は変なのでとりあえずコメントアウト */
#ifdef FLIGHT_TIME

void utility_get_time 
(
 double *user_time,
 double *system_time
 ){

	struct tms buf;

	time_t end_user_time;
	time_t end_system_time;

	times( &buf );

	end_user_time   = buf.tms_cutime + buf.tms_utime;
	
	*user_time      = ( end_user_time - start_system_time ) / ( HZ * 1.00 ) + 0.05;

	end_system_time = buf.tms_cutime + buf.tms_stime;

	*system_time    = ( end_system_time - start_system_time ) / ( HZ * 1.00 ) + 0.05;
	
}

/********************************************************************************
 * << Function >>
 *		Stop CPU Time  for Total CPU time
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     void
 *
 * << Argument >>
 * args			(I/O)	type	    description
 * user_time     I/O    double *    user time from start to now
 * system_time   I/O    double *    system time from start to now
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/12/19
 *
 ********************************************************************************/

void utility_get_time_start
(
 void
 ){

	struct tms buf;

	times( &buf );

	end_user_time = buf.tms_cutime + buf.tms_utime;
	
	start_usertime = buf.tms_cutime + buf.tms_utime;

	start_system_time = buf.tms_cutime + buf.tms_stime;
}
#endif 
/* End of File */
