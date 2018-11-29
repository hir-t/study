/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief stock�Хåե��˴ؤ�������
	
	@note
		�ѥ�������ξ��֤�ɽ�� 3��
		X ( a = 0 , b = 0 ), 0 ( a = 0 , b = 1 )  1 ( a = 1 , b = 0 )
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


#define STOCK_SIZE    100            //!< ��¢�Хåե��Υ����� �ǥե������

// STOCK_FAULT.no_pat_x���̤���
#define STOCK_F_N_DET     0xfffffffd   //!< Not Detect
#define STOCK_F_D_DET     0xfffffffe   //!< Double Detect
#define STOCK_F_END       0xffffffff   //!< Detect by One Deteted Patttern

// STOCK_PAT.flag�����
#define SP_NO_DEFINE      0x00000000	/* ����� */
#define SP_ONE_DET        0x00000001	/* ���Υѥ���������ФǤ���ξ㤬���� */
#define SP_REDUN          0x00000002	/* ���Υѥ��󤬤ʤ��Ƥ⸡�ФǤ��ʤ��ξ�Ϥʤ� */
#define SP_COMPLETE       0x00000004	/* �ǽ�Ū��ʬ��Ѥ� */

typedef enum stock_mode {
	STOCK_NO_INFO = 0,  //!< ̤���(�ǥե����ư��)
	STOCK_DDET,        //!< Double Detect Mode
	STOCK_ONE_DET,     //!< One Detect Pattern Simulation
	STOCK_REVERSE,     //!< Reverse Order Simulation by Redundant Pattern
} STOCK_MODE;


/*!
  @struct stock_pat_head
  @brief ��¢�ѥѥ�������(stock_sizeʬ����)
*/
typedef struct stock_pat
{
    Ulong               **buf_a;       //!< ���̥Хåե�a
	Ulong               **buf_b;       //!< ���̥Хåե�b
	Ulong               *flag;         //!< �ѥ��󤴤ȤΥե饰
	struct stock_pat    *next;         //!< ����stock_pat�ؤΥݥ���
} STOCK_PAT;

/*!
  @struct stock_pat_head
  @brief ��¢�ѥѥ���˴ؤ������
*/
typedef struct stock_pat_head
{
    struct stock_pat    *stock_pat;    //!< ��¢���Ƥ���ѥ���ؤΥݥ���
	Ulong               total_size;    //!< ���ߤΥѥ����
	Ulong               stock_size;    //!< stock��Ĥ�����Υ�����
	Ulong               width;         //!< �ѥ��󤴤Ȥ���
} STOCK_PAT_HEAD;

/*!
  @struct stock_pat_head
  @brief ��¢�ѥѥ���˴ؤ������
*/
typedef struct stock_info
{
    Ulong               *pat_order;    //!< Fsim��»ܤ������
	Ulong               total_size;    //!< ���ΤΥѥ����
	Ulong               n_fault;       //!< �оݤȤʤ�ξ��
	struct stock_fault  *stock_fault;  //!< �ξ�ꥹ�ȤؤΥݥ���
	struct stock_fault  **all_fault;    //!< �ξ�ꥹ�ȤؤΥݥ���
} STOCK_INFO;

/*!
  @struct stock_fault
  @brief ��¢�ѥѥ���θξ㸡�Ф˴ؤ������
*/
typedef struct stock_fault
{
    struct line         *line;         //!< �оݸξ㤬���뿮�����ؤΥݥ���
	Ulong               no_pat_0;      //!< ���Ф����ѥ����ֹ�
	Ulong               no_pat_1;      //!< ���Ф����ѥ����ֹ�	
	Ulong               flag;          //!< ���ФǤ���ξ�
	struct stock_fault  *next;         //!< ���ؤΥݥ���
} STOCK_FAULT;
