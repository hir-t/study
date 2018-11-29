/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ξ�ꥹ�Ȥ˴ؤ�������
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#ifndef TOPGUN_FLIST_H
#define TOPGUN_FLIST_H

/*!
  �ξ�ξ��֤򼨤� �� FLIST->info
*/
#define TOPGUN_SA0	0x00000001	//!< stack-at 0
#define TOPGUN_SA1	0x00000002	//!< stack-at 1 (state only) 
#define TOPGUN_DCD	0x00000004	//!< �ξ�ʬ�ब̤����( OFF ) or ����( ON )
#define TOPGUN_EXE	0x00000008	//!< ATPG̤�»� ( OFF ) or ATPG�»ܤ���( ON )
#define TOPGUN_DTA	0x00000010	//!< ATPG�ˤ�ä��󸡽� ( OFF ) or ���иξ�( ON ) 16
#define TOPGUN_DTS	0x00000020	//!< Fsim�ˤ�ä��󸡽� ( OFF ) or ���иξ�( ON ) 32
#define TOPGUN_RED	0x00000040	//!< �ƥ���������ǽ������ ( OFF) or ��ϩ��Ĺ������ ( ON ) 64
#define TOPGUN_IUT	0x00000080	//!< ���������ˤ��ƶ��ʤ� ( OFF) or �ƶ����� ( ON ) 128
#define TOPGUN_ABT	0x00000100	//!< �Ǥ��ڤ�ξ�Ǥʤ� ( OFF ) or �Ǥ��ڤ�ξ� ( ON ) 256
#define TOPGUN_DTX	0x00000200	//!< Fsim3(X����)�Ǹ��в�ǽ ( ON ) 512
#define TOPGUN_DTR	0x00000400	//!< ������ѥ���Ǹ��в�ǽ ( ON ) 1024

/*!
  ���ߥ�졼��������Ǥ���ξ�
 */
#define SELECT_DEFAULT 0x00000000	//!< initial state
#define SELECT_ON_SA0  0x00000001	//!< stack-at 0
#define SELECT_ON_SA1  0x00000010	//!< stack-at 1
#define SELECT_ON_SA01 0x00000011	//!< stack-at 0 and 1
#define SELECT_OFF     0x00000100	//!< no selectable fault


/*!
  @struct flist
  �ξ�˴ؤ������
*/
typedef struct flist
{
    Ulong           f_id;		//!< �ξ�ID
    Ulong           info;		//!< �ɤ��ͤʸξ㤫�ξ���(sa0 or sa1 / detect or redun etc )
    Ulong           n_eqf;	    //!< �����ξ��
    Ulong           *eqf;	    //!< �����ξ�ؤΥݥ���
    struct line	    *line;	    //!< �ξ㤬���ꤵ��뿮�����ؤΥݥ���
    struct flist    *next;      //!< Ʊ�������������ꤹ��ξ�Υꥹ��
} FLIST;

typedef enum flist_type
{
    FLIST_COMMENT,               //!< �����ȹ�
    FLIST_ORG,                   //!< �ѥ����������٤��ξ�
    FLIST_CON,                   //!< �����ξ�
    FLIST_END,                   //!< �ե�����ν�ü
} FLIST_TYPE;

#endif
