/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief CELL�����˴ؤ�������
	
	@note

	CELL�����ϥ����ȥ�٥�ͥåȥꥹ�Ȥ�ATPG�ѥͥåȥꥹ�ȤȤδ֤�Ĥʤ�����Τ�ΤǤ��롣�롼������å��μ»ܤ�ѥ�����������ξ㥷�ߥ�졼���Τ���˲�ϩ�Ѵ��θ��Ȥʤ롣
	��ϩʬ�����ˤϤ�������Ÿ������Τ�����ǤϤʤ���������

	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/


/*!
  @{
  @name cell�Υϥå���ơ��֥�˴ؤ�������

  @note ̾����ʸ���������¤ϲ�����٤�����
*/
#define CELL_HASH 999991 			//!< cell_head�ѥϥå�����
#define CELL_NAME_SIZE 1024         //!< ̾����ʸ�����κ�����
/*!
  @}
*/

/*!
  @enum cell_flag
  @brief Cell�θξ㤬�������Ƥ��뤫�ݤ��򼨤��ե饰
*/
typedef enum cell_flag
{
	Fault_off       = 0,           //!< ���Ϥˤ���Ϥˤ��������Ƥ��ʤ�
	Fault_out_on    = 1,           //!< ���Ϥ����������Ƥ���
	Fault_in_on     = 2,           //!< ���Ϥ����������Ƥ���
	Fault_in_out_on = 3,           //!< ���Ϥˤ���Ϥˤ��������Ƥ���
} CELL_FLAG;


/*!
  @enum cell_type
  @brief Cell�ե����ޥåȾ�Υ����ȥ�����
*/
typedef enum cell_type
{
	CELL_PI   = 0,
	CELL_PO,
	CELL_BR,
	CELL_INV,
	CELL_BUF,
	CELL_AND,
	CELL_NAND,
	CELL_OR,
	CELL_NOR,
	CELL_XOR,
	CELL_XNOR,
	CELL_SEL,
	CELL_FF,
	CELL_BLK,   /* �ɤ߹��߻� */
	CELL_BLKI,  /* �Ѵ��� */
	CELL_BLKO,	/* �Ѵ��� */
	CELL_UNK,
	CELL_NUM_PRIM
} CELL_TYPE;


/*!
  @struct cell_head
  �����٥���Ф���������
*/
typedef struct cell_head
{
    Ulong           n_id;		//!< �����
    Ulong           n_pi;		//!< �������Ͽ�
    Ulong           n_po;		//!< �������Ͽ�
    Ulong           n_bbpi;		//!< ����Ǥ��ʤ��������Ͽ� ( Black Box )
    Ulong           n_bbpo;		//!< ����Ǥ��ʤ��������Ͽ� ( Black Box )
    struct cell     **cell;     /* cells */
} CELL_HEAD;

/*!
  @struct cell
  ����˴ؤ������
*/
typedef struct cell {
	char			*name;		//!< ������̾
	Ulong			id;			//!< ����ID
	CELL_FLAG		flag;		//!<  flag
	CELL_TYPE		type;		//!< ����Υ�����
	Ulong			n_in;		//!< ���Ͽ�
	Ulong			n_out;		//!< ���Ͽ�
	struct cell_io	*in;		//!< ���ϥ���ؤΥݥ���
	struct cell_io  *out;		//!< ���ϥ���ؤΥݥ���
    struct cell	    *next;		//!< �ϥå���ɽ�μ����ؤΥݥ���
} CELL;


/*!
  @struct cell_io
  �������³�˴ؤ������

  @note
  �����Ͽ��ν���ͤ��狼��ʤ��Τ��ɲä��Ƥ��������ˤʤäƤ���
*/
typedef struct cell_io {
	struct cell    *cell;		/*	��³������		*/
	struct cell_io *next;
} CELL_IO;
