/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ������(Line)�˴ؤ�������

	@note
		topgunState.h��ɬ��

	@author
		Masayoshi Yoshimura
	@date
		2006 / 02 / 10   initialize ( renew )
		2007 / 05 / 10   add implication ID for non-chronological backtrack
		2016 / 12 / 12   initialize (renew )
*/

#define TAC_LINE

#ifndef TOPGUN_STATE_H
#include "topgunState.h"
#endif //TOPGUN_STATE


/*!
  LINE->flag�Ѥ�define
*/
#define LINE_SA0 	0x00000001	//!<  1bit stack-at 0
#define LINE_SA1 	0x00000002	//!<  2bit stack-at 1 */
#define LINE_CHK 	0x00000004	//!<  3bit function flag 1
#define LINE_FLT 	0x00000008	//!<  4bit fault flag
#define IMP_FRONT 	0x00000010	//!<  5bit front implication flag
#define IMP_BACK 	0x00000020	//!<  6bit back implication flag
#define JUSTIFY 	0x00000040	//!<  7bit justify flag
#define JUSTIFY_F 	0x00000080	//!<  8bit justify finish flag
#define JUSTIFY_NO 	0x00000100	//!<  9bit justify normal state
#define JUSTIFY_FL 	0x00000200	//!< 10bit justify failure state
#define BOUND_D 	0x00000400	//!< 11bit bound D tree
#define FP_PATH 	0x00000800	//!< 12bit Fault Propagation Path
#define FP_IAP      0x00001000	//!< 13bit Fault Propagate Potential(IAP)
#define FV_ASS      0x00002000	//!< 14bit Fault Values ASSignable

#define LF_UNIQ     0x00004000	//!< 15bit uniqsence flag

#define CIR_FIX     0x00008000	//!< 16bit with fix value for circuit structure

#define PO_FLAG     0x00010000	//!< 17bit output line

#define IMP_FRONT_C	0x00000018	//!<  4 & 5 bit enter implication check
#define IMP_BACK_C	0x00000028	//!<  4 & 6 bit enter implication check


/*!
  @struct line_info
  @brief ��ϩ�ο�������٥���Ф���������
*/
typedef struct line_info
{
    Ulong 	n_line;		//!< ��������
    Ulong 	n_pi;		//!< �������Ͽ�
    Ulong 	n_po;		//!< �������Ͽ�
    Ulong 	n_bbpi;		//!< ����Ǥ��ʤ��������Ͽ� ( Black Box )
    Ulong 	n_bbpo;		//!< ����Ǥ��ʤ��������Ͽ� ( Black Box )
    Ulong 	n_fault;	//!< �ξ��
    Ulong 	n_fault_atpg; //!< ATPG�оݸξ��
    Ulong 	n_fault_atpg_count;	//!< �Ĥ�ATPG�оݸξ��
    Ulong 	max_lv_pi;	//!< ���������ʿ�
    Ulong 	max_lv_po;	//!< ���������ʿ�
    Ulong 	*n_lv_pi;	//!< ���ʿ��ο�������
} LINE_INFO;


/*!
  @enum FSIM_FLT
  line->flist[]��ź���Ȥ����Ѥ���
*/
typedef enum fsim_flt{
    FSIM_SA0 = 0,
    FSIM_SA1 = 1,
} FSIM_FLT;

/*!
  @enum LINE_TYPE
  line->type���Ѥ��뿮�����Υ����ȷ�
*/
typedef enum line_type
{
    TOPGUN_PI   = 0,	//!< ��������
    TOPGUN_PO   = 1,	//!< ��������
    TOPGUN_BR   = 2,	//!< �ե��󥢥��ȥ֥���
    TOPGUN_INV  = 3,	//!< ����С���
    TOPGUN_BUF  = 4,	//!< �Хåե�
    TOPGUN_AND  = 5,	//!< �����
    TOPGUN_NAND = 6,	//!< �ʥ��
    TOPGUN_OR   = 7,	//!< ����
    TOPGUN_NOR  = 8,	//!< �Υ�
    TOPGUN_XOR  = 9,	//!< �����롼���֥���
    TOPGUN_XNOR = 10,	//!< �����롼���֥Υ�
    TOPGUN_BLKI = 11,	//!< ��������
    TOPGUN_BLKO = 12,	//!< �������
    TOPGUN_UNK  = 13,	//!< ̤���귿
    TOPGUN_NUM_PRIM,    //!< ���ο�
} LINE_TYPE;


/*!
  @struct line
  �������˴ؤ������
*/
typedef struct line
{
    enum line_type	    type;		//!< �������Υ����ȷ�
    enum state_9        state9;		//!< �������ξ��֤�9��( ������/�ξ��� )�򼨤�
    Ulong               line_id;	//!< ��������ID
    Ulong               n_in;		//!< �����������Ͽ�
    Ulong               n_out;		//!< �������ν��Ͽ�
    struct line         **in;       //!< ���Ͽ������ؤΥݥ���
    struct line         **out;      //!< ���Ͽ������ؤΥݥ���
    struct learn_list   *imp_0;     //!< 0�˴ްդ��줿���˰��˴ްդ������
    struct learn_list   *imp_1;     //!< 1�˴ްդ��줿���˰��˴ްդ������
    Ulong               flag;		//!< �ե饰
    Ulong               lv_pi;      //!< ���Ϥ���ο������ʿ�
    Ulong               lv_po;      //!< ���Ϥ���ο������ʿ�
    Ulong               level;

    Ulong               imp_id_n;   //!< �����ͤ��ްդ��줿�ͳ������ֹ�
    Ulong               imp_id_f;   //!< �ξ��ͤ��ްդ��줿�ͳ������ֹ�

    Ulong           	cnt_propa;	//!< �ξ����²��

    Ulong           	tm_co;		//!< �ƥ����ӥ�ƥ��᥸�� 0��������
    Ulong           	tm_c0;		//!< �ƥ����ӥ�ƥ��᥸�� 1��������
    Ulong           	tm_c1;      //!< �ƥ����ӥ�ƥ��᥸�� �Ĵ�¬��
    Ulong           	tm_bb_co;   //!< �ƥ����ӥ�ƥ��᥸�� �ξ���������0��������
    Ulong           	tm_bb_c0;   //!< �ƥ����ӥ�ƥ��᥸�� �ξ���������1��������
    Ulong           	tm_bb_c1;   //!< �ƥ����ӥ�ƥ��᥸�� �ξ��������βĴ�¬��
    struct flist    	*flist[2];  //!< �ξ�ꥹ�ȤؤΥݥ���

    Ulong           	fault_set_flag;
    Ulong           	fprop_flag;
    Ulong           	mp_flag;
    Ulong           	n_val_a;
    Ulong           	n_val_b; /* 3���� */
    Ulong           	f_val_a;
    Ulong           	f_val_b; /* 3���� */
    struct line         *event_line;

    Ulong             value; //���ߥ�졼����������

    Ulong           	*out_info;	  //!< ���Ȥ������ã��ǽ��������(stem����)
    Ulong           	*reconv_info; //!< ��³���Ƥ�ޤ��Ƽ��̤��Ƥ��뤫(stem����)

    struct ffrg_node    *ffrg;

} LINE;

typedef struct logic_level{
  Ulong level;
  Ulong id;
  Ulong type;
  Ulong value;
}LOGIC_LEVEL;

/*!
  @struct line_stack
  line�򥹥��å����빽¤��
*/
typedef struct line_stack
{
    struct line   	    *line;  //!< �������ؤΥݥ���
    struct line_stack  	*next;  //!< �����ؤΥݥ���
} LINE_STACK;

/*!
  @struct line_list
  @brief ������(line)���������ꥹ�Ȥ��Ѥ��뤿��ι�¤��
*/
typedef struct line_list
{
    struct line			*line;  //!< �������ؤΥݥ���
    struct line_list	*prev;  //!< �ꥹ�������ؤΥݥ���
    struct line_list	*next;  //!< �ꥹ�ȸ����ؤΥݥ���
} LINE_LIST;

