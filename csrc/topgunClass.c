/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 故障分類を設定する関数
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>

#include "topgun.h"
#include "topgunState.h" /* STATE_3, STATE_9 */
#include "topgunAtpg.h"
#include "topgunFlist.h" /* FLIST & FLIST->info */

extern	GENE_HEAD	Gene_head;

/********************************************************************************
 * << Function >>
 *		judge fault which redundant fault or untestable fault
 *
 * << Function ID >>
 *    	15-1
 *
 * << Return >>
 *	TOPGUN_IUN	: untestable fault for pattern of initialize
 *	TOPGUN_RED	: redundant fault
 *
 * << Argument >>
 * args			(I/O)	type	description
 *     Void
 *
 * << extern >>
 * name			(I/O)	type	description
 * ATPG_HEAD     I      Gene_head   infomation of ATPG
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/1/14
 *
 ********************************************************************************/

int atpg_untest_redun
(
 void
){
	/* ATPG_UNTのフラグが立っているか */
    if ( Gene_head.flag & ATPG_UNT ) {
		return( TOPGUN_IUT );
    }
    else {
		return( TOPGUN_RED );
    }
}

/* End of File */
