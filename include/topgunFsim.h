/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ξ㥷�ߥ�졼���˴ؤ�������
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#ifndef TOPGUN_FSIM_H
#define TOPGUN_FSIM_H

/* TOPGUN Global Define */
#define BYTE 8          /* 8 bit = 1 byte */

/* line->fault_set_flag @ Fsim */
#define ALL_BIT_OFF     0x00000000

/* line->mp_flag @ Fsim */
#define FSIM_EVENT_OFF  0
#define FSIM_EVENT_ON   1


#define STATE_X_CODE 0x00
#define STATE_0_CODE 0x01
#define STATE_1_CODE 0x02
#define STATE_U_CODE 0x03


#define STATE_0 0
#define STATE_1 1

/* for 32bit 2���������� */
#define PARA_V0_BIT 0x00000000
#define PARA_V1_BIT 0xffffffff

#define BIT0_ON     0x00000001

#define RAND_BIT    0x00100000 /* �ʤ�٤���̤�����Ȥ� */

#define FSIM_ALL_ON 0xffffffff


#define NO_PROP_FAULT 0

#define FSIM_VAL3   1 /* 1 : val3��Ȥ�, 1�ʳ� : val3��Ȥ�ʤ� */
#define FSIM_S_COMP 1 /* 1 : staic compaction��Ȥ�, 1�ʳ� : �Ȥ�ʤ� */

/*!
  @enum fsim_mode
  @brief �ξ㥷�ߥ�졼���Υ⡼���ֹ�
*/
typedef enum fsim_mode {
	FSIM_NO_INFO = 0,  //!< ̤���(�ǥե����ư��)
	FSIM_PPSFP,        //!< �ѥ���ѥ����󥷥󥰥�ե�����ȥץ�ѥ��������
	FSIM_SPPFP,        //!< ���󥰥�ѥ�����ѥ���ե�����ȥץ�ѥ��������
	FSIM_SPSFP,        //!< ���󥰥�ѥ����󥷥󥰥�ե�����ȥץ�ѥ��������
	FSIM_PPSFP_VAL3,   //!< ���󥰥�ѥ����󥷥󥰥�ե�����ȥץ�ѥ��������
	FSIM_RANDOM,       //!< ���󥰥�ѥ����󥷥󥰥�ե�����ȥץ�ѥ��������
	FSIM_OUTPUT,       //!< generate��λ��Ĥ�ѥ�����򥷥ߥ�졼����󤹤�
	FSIM_STOCK         //!< ��������Ū���̤򤹤�
} FSIM_MODE;

/*!
  @struct fsim_head
  @brief �ξ㥷�ߥ�졼���إå�

  @note ���Τ����ξ㥷�ߥ�졼���Ѥξ���(info)�Ȳ�ϩ(head)��ʬ�䤹��
*/
typedef struct fsim_head
{
    Ulong 	bit_size;	      //!< bit�������ѥ���⡼�ɻ���������Ȥʤ롣ư����륳��ԥ塼����CPU�������ƥ�����˰�¸����
	Ulong   detect_mask;      //!< �ѥ���⡼�ɻ��˵���ο���­��ʤ����Ρ��ޥ��������뤿����ѿ�( global ��ɬ�פ��ʤ�����)
} FSIM_HEAD;

/*!
  @struct event_list
  @brief �ͤ����¤����뤿��Υ��٥�Ȥ˴ؤ��빽¤��
*/
typedef struct event_list {
	Ulong			n_event; //!< ���٥�ȿ�
    struct line		**event; //!< ���٥��(�������ؤΥݥ���)
} EVENT_LIST;

/*!
  @struct fsim_info
  @brief flight_atpg�Ȥ�I/F�ѥǡ���

  @note
  ���٤Ƹξ㥷�ߥ�졼����ư����뤿����������
  n_detect�ϸġ����Ѥ�붲��⾯�ʤ������ѿ���Ƚ�Ǥ��뤿�ᡢfsim_info�Ǥʤ�fsim_head�ˤ���٤�����
*/
typedef struct fsim_info {
	enum fsim_mode      mode;           //!< �ξ㥷�ߥ�졼����ư��⡼��
	struct cc_pattern   *cc_pat;        //!< 3��ɽ�����ȹ礻�ѥ�����(1�Ĥ����ξ��)
    struct cc_pattern   **cc_pat_wodge; //!< 3��ɽ�����ȹ礻�ѥ�����(N�ĤΥѥ����󡢥ѥ���ѥ�����⡼����)
	Ulong				n_cc_pat;       //!< �ξ�򸡽Ф����Ȥ����(N detection��)
	Ulong				n_detect;       //!< �ξ�򸡽Ф����Ȥ����(N detection��)
	Ulong				num_pat;        //!< �ξ�򸡽Ф����ѥ����
	Ulong				num_waste_pat;  //!< �ξ�򸡽Ф��ʤ��ѥ����(������פˤʤä�)
	Ulong				num_rand_pat;   //!< ������ѥ���ξ㥷�ߥ�졼�����Υѥ����
	Ulong				fsim3;          //!< 3�ͤ�¹Ԥ��뤫
	Ulong				static_comp;    //!< �ݥ��Ƚ����Ǥ���Ū���̤�¹Ԥ��뤫
} FSIM_INFO;


#endif
