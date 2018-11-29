/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ���̥Хåե��˴ؤ�������

	@note
		@verbatim
		�ѥ�������ξ��֤�ɽ�� 2��
		0 ( = 0 )  1 ( = 1 )
		�ѥ�������ξ��֤�ɽ�� 3��
		X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )
		@endverbatim
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#define BIT_1_ON      0x00000001      //!< 1bit�ܤ���1�Ǥ���
#define COMP_BUF_SIZE 100             //!< ���̥Хåե��Υ����� �ǥե������
//#define STOCK_SIZE    1000            //!< ��¢�Хåե��Υ����� �ǥե������

/*!
  @{
  @name comp_info->result�˴ؤ������
*/
#define COMPACT_OK  0x00000000      /* compact enable */
#define COMPACT_NG  0xffffffff      /* compact conflict */
/*!
  @}
*/





/*!
  @{
  @name comp_info->comp_mode flag�˴ؤ������
*/
#define COMP_NO_INFO 0x00000000      /* ������� */
#define COMP_GET     0x00000001      /* ���̥Хåե�����1�ļ�äƤ��� */
#define COMP_GET_ALL 0x00000002      /* ���̥Хåե�����bit��ʬ��äƤ��� */
#define COMP_INSERT  0x00000004      /* ���̥Хåե��˥ѥ����������� */
#define COMP_FAST    0x00000008      /* ���̲�ǽ�ʲս�ˤ�������� */
#define COMP_REST_X  0x00000010      /* �Ĥ��X�ο����Ǥ�¿���褦�ˤ��� */
/*!
  @}
*/


/*!
  @enum comp_check_all_x
  @brief �ѥ����󤬤��٤�X��������å�����
*/
typedef enum comp_check_all_x {
	COMP_PAT_ALL_X = 0,     //!< ���٤�X�Ǥ���
	COMP_PAT_NOT_X = 1,     //!< ���٤�X�ǤϤʤ�
} COMP_CHECK_ALL_X;

/* ���̳��Ϥ˴ؤ���compact_on�ؿ����֤��� */
typedef enum comp_on_check {
	COMP_ON_STOP = 0,      //!< ���ʤ�
	COMP_ON_START = 1,     //!< ���̤���
} COMP_ON_CHECK;

/*!
  @struct comp_head
  @brief ���̥Хåե��˴ؤ������
*/
typedef struct comp_head
{
    Ulong               **buf_a;       //!< ���̥Хåե�a
	Ulong               **buf_b;       //!< ���̥Хåե�b 
	Ulong               *num_x_state;  //!< ���̥Хåե������X�ο�
    Ulong               flag;          //!< flag for atpg
	Ulong               width;         //!< ���̥Хåե�����
	Ulong               buf_size;      //!< ���̥Хåե��Υ�����
	Ulong               min_n_x_state; //!< ���̥Хåե������X�ο��κǾ���
	Ulong               min_x_buf_id;  //!< ���̥Хåե�����ǺǾ�X���ĥХåե���ID
} COMP_HEAD;

/*!
  @struct cc_pattern
  @brief ���̥Хåե��ˤ�����ѥ�����
*/
typedef struct cc_pattern
{
    Ulong               *pattern_a;    //!< �ѥ����� a
	Ulong               *pattern_b;    //!< �ѥ����� b
} CC_PATTERN;

/*!
  @struct comp_info
  @brief  ưŪ���̤˴ؤ������
*/
typedef struct comp_info
{
	Ulong               mode;           //!< ưŪ���̤�ư��⡼��
	Ulong               result;         //!< ưŪ���̤η��
	Ulong               n_cc_pat;       //!< ����ѥ����
	struct cc_pattern   *cc_pat;        //!< 3���ȹ礻�ѥ�����(1)
    struct cc_pattern   **cc_pat_wodge; //!< 3���ȹ礻�ѥ�����(N)
} COMP_INFO;

