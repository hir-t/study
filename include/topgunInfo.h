/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief data structure for atpg 
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/*!
  @struct atpg_info

  @brief memo
*/
typedef struct atpg_info
{
	struct gene_info    gene_info;
	struct comp_info    comp_info;
	struct fsim_info    fsim_info;
	struct cc_pattern   cc_pat;     // テンポラリ用パタン
} ATPG_INFO;

