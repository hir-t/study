/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �ѥ���������(generate)�ˤ����������
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#ifndef TOPGUN_ATPG_H
#define TOPGUN_ATPG_H

/*!
  @{
  @name Gene_head.flag�˴ؤ������
*/
#define ATPG_PROPA_E	0x00000001	//!< 1bit�� ���´�λ( ON ) ̤��λ( OFF )
#define ATPG_JUSTI_S	0x00000002	//!< 2bit�� �������»ܳ���( ON ) ̤����( OFF ) 
#define	ATPG_UNT	    0x00000004	//!< 3bit�� �ƥ���������ǽ������ ( ON ) �ʤ� ( OFF )
/*!
  @}
*/
/*!
  @{
  @name implication�ؿ��ˤ�������֤����
*/
#define ALL_FLAG_ON   0x00000000    /* implication state check flag ( for uncontrol state ) */
#define ALL_FLAG_OFF  0x00000001    /* implication state check flag ( for uncontrol state ) */

#define ONE_FLAG_ON   0x00000000    /* implication state check flag ( for control state ) */
#define ONE_FLAG_OFF  0x00000001    /* implication state check flag ( for control state ) */

#define UNK_FLAG_ON   0x00000000    /* implication state check flag ( for unknown state ) */
#define UNK_FLAG_OFF  0x00000001    /* implication state check flag ( for unknown state ) */

#define OUT_PIN_NUM   0x00000000    /* �ް�����pin_num�ˤ�������ϥԥ���ֹ� */
#define IN0_PIN_NUM   0x00000001    /* �ް�����pin_num�ˤ�����1���ܤ����ϥԥ���ֹ� */
#define IN1_PIN_NUM   0x00000002    /* �ް�����pin_num�ˤ�����2���ܤ����ϥԥ���ֹ� */
/*!
  @}
*/

/*!
  @{
  @name btree.flag�ˤ�������֤����
*/
#define	ASS_N_0		0x00000001	   //!< 1bit 1:assign normal  0 �»ܤ���, 0: ̤�»�
#define	ASS_N_1		0x00000002	   //!< 2bit 1:assign normal  1 �»ܤ���, 0: ̤�»�
#define	ASS_F_0		0x00000004	   //!< 3bit 1:assign failure 0 �»ܤ���, 0: ̤�»�
#define	ASS_F_1		0x00000008	   //!< 4bit 1:assign failure 1 �»ܤ���, 0: ̤�»� 
#define	ASS_C_N		0x00000003	   //!< 1bit and 2bit normal  state complite
#define	ASS_C_F		0x0000000c	   //!< 3bit and 4bit failure state complite
/*!
  @}
*/




/* assign result */



/* drive_dfront�ε�����(int) */

#define	DRIVE_CONT  0x00000020	// drive��³ 
#define	DRIVE_END   0x00000040	// drive��λ
#define	DRIVE_IMPOS 0x00000080	// drive�Բ�ǽ
#define	DRIVE_POINT 0x00000100	// drive����Ľ� 

/* justified�ε�����(int) */
/* justified check result */
#define	JUSTI_END	0x00000200	/* justified end (topgun_justified) */
#define	JUSTI_KEEP  0x00000400	/* not justified end */
                                /* justified ��³ */
#define	JUSTI_CONF  0x00000800	/* justified ���� */
/* propagate check result */
#define	PROPA_END	0x00001000	/* propagate end (topgun_propagate) */
/* #define	PROPA_CONT	0x00002000	/\* not propagate end *\/ */

/* #define	LIM_BACK_END 0x00004000	/\* end for limit of back track *\/ */
#define	ASS_R_CONF	0x00000010	/* STAT_YY����礭���ͤǤ���ɬ�פ��� */


/* dfrontier flag */
#define	DF_NOCHECK	0x00000001	/* 1bit �ޤ�̤��ǧ�Υ롼�� */
#define	DF_ROOT		0x00000002	/* 2bit ���߳�ǧ��Υ롼�� */
#define	DF_DEADEND	0x00000004	/* 3bit ��ǧ��(�Ԥ��ߤޤ�) */


/* atpg_back_track������(int) */

typedef enum gene_back {
	GENE_BACK_END   = 0,	/* back track ��λ(while�Τ���0) */
	GENE_BACK_CONT  = 1		/* ATPG��³(while�Τ���1) */
} GENE_BACK;

/* for xor */
#define IMP_XOR		0x0001

/*!
  @enum atpg_check
  @brief atpg_check_end������(int)
*/
typedef enum atpg_check {
	ATPG_CONT  = 0,    //!< ATPG��³ 
	ATPG_NOFLT = 1	   //!< �Ĥ�ξ㤬�ʤ��Τǽ�λ
} ATPG_CHECK;

/*!
  @enum gene_result
  @brief generate�ؿ���η�̤Τ���������
*/

typedef enum gene_result {
	NO_RESULT = 0,     //!< ̤�»�
	INSERT_FAIL,       //!< �ξ������˼��Ԥ���
	INSERT_PASS,       //!< �ξ���������������
	LIM_BACK_END,      //!< back track�ξ�¤�ã����
	PROPA_PASS,        //!< �ξ����¤���������
	PROPA_FAIL,        //!< �ξ����¤˼��Ԥ���
	PROPA_FAIL_DRIVE,  //!< �ξ����¤˼��Ԥ���(D-Drive or X-Path) 
	PROPA_FAIL_ENTER,  //!< �ξ����¤˼��Ԥ���(��ճ�����)
	PROPA_FAIL_IMP,    //!< �ξ����¤˼��Ԥ���(�ް����)
	PROPA_CONT,        //!< �ξ����¤��³��Ǥ���
	JUSTI_PASS,        //!< ����������������
	JUSTI_CONT,        //!< ���������³��Ǥ���
	JUSTI_FAIL,        //!< �������˼��Ԥ���
	JUSTI_FAIL_ENTER,  //!< �������˼��Ԥ���(��ճ�����) 
	JUSTI_FAIL_IMP,    //!< �������˼��Ԥ���(�ް����)
	GENE_CONT,         //!< �ѥ������������³��Ǥ���
	GENE_END,          //!< �ѥ�������������λ���� 
} GENE_RESULT;

/*!
  @enum condition
  @brief ���� ( ������ or �ξ��� )�˴ؤ�����
*/
typedef enum condition {
	COND_UNKNOWN   = 0, //!< ̤����
	COND_FAILURE   = 1, //!< �ξ����( �ξ��� )�򰷤�
	COND_NORMAL    = 4, //!< �������( ������ )�򰷤�
	COND_REVERSE   = 5, //!< ����ȿ�Фξ��֤ˤ���
	TOPGUN_NUM_COND,
} CONDITION;


/*!
  @enum imp_state
  @brief �ް����η��

  @note bit�Ǥ���ɬ�פ��ʤ��Ϥ�
*/
typedef enum imp_state {
	IMP_KEEP = 0,	//!< �ް������ͤ��ݻ����줿(��������ʤ��ä�)
	IMP_CONF = 1,	//!< �ް������ͤ����ͤ�ȯ������
	IMP_UPDA = 2,   //!< �ް������ͤ��������줿
} IMP_STATE;

/*!
  @enum ass_result
  @brief �ͳ����Ƥη��

  @note bit�Ǥ���ɬ�פ��ʤ��Ϥ�, IMP_STATE�ȶ��̲����Ƥ��ɤ�
*/
typedef enum ass_result {
	ASS_KEEP = 0,	/* keep     on assign */
	ASS_CONF = 1,	/* conflict on assign */
} ASS_RESULT;

/*!
  @enum just_result
  @brief �������η��

  @note bit�Ǥ���ɬ�פ��ʤ��Ϥ�
*/
typedef enum just_result {

	JUST_ERROR = 0,	 //!< all 0 �ɤ��ˤ�ɬ�פʤ�(PI�ʤ�)
	JUST_NONE  = 1,	 //!< ERROR (1)
	JUST_OWN   = 2,	 //!< ���ߤ�line����������ɬ�פ���
	JUST_ALL   = 4,	 //!< ���Ͽ��������٤���������ɬ�פ���
	JUST_1IN   = 8,	 //!< ���ϤΤɤ줫��Ĥ���������ɬ��
	JUST_1IN_N = 16, //!< �ɤ�in[n]����������ɬ�פ�����
} JUST_RESULT;

/*!
  @enum just_method
  @brief ��������������������򤹤���

*/
typedef enum just_method {

	JUST_METHOD_LV_PI_HIGH = 0,	 //!<
	JUST_METHOD_LV_PI_LOW  = 1,	 //!<
	NUM_JUST_METHOD        	     //!< 
} JUST_METHOD;


/*!
  @enum f_class
  @brief ATPG�ˤ��ξ��ʬ���̤˴ؤ�����
*/
typedef enum f_class {
	NO_DEFINE = 0,     //!< ̤�»�
	GENE_NO_FAULT,     //!< �ξ㤬����Ǥ��ʤ� 
	GENE_DET_A,        //!< ATPG�Ǹ���
	GENE_DET_S,        //!< �ξ㥷�ߥ�졼���Ǹ���
	GENE_RED,          //!< ��Ĺ�ξ�
	GENE_UNT,          //!< �ƥ���������ǽ�ξ�
	GENE_IUN,          //!< ������ƥ���������ǽ�ξ�
	GENE_ABT,          //!< ATPG�Ǥ��ڤ�ξ�
} F_CLASS;

#define INDIRECT_IMP_LIST_SIZE 1000


/*!
  @struct gene_head
  @brief �ѥ����������˴ؤ������
*/
typedef struct gene_head
{
    Ulong               flag;            //!< �ѥ����������˴ؤ���ե饰
	Ulong               limit_back;      //!< �Хå��ȥ�å����ξ��
	Ulong               back_all;        //!< ���ߤΥХå��ȥ�å���
	Ulong               back_drive;      //!< �ξ�������ΥХå��ȥ�å���
	Ulong               back_drive_just; //!< ���¤����ѥ����������Ǽ��Ԥ�����
    struct line         *fault_line;     //!< ���ߥѥ������������оݤǤ���ξ�
	struct line         *propa_line;     //!< �ξ�����¤����Ƥ��뿮����
    struct dfront       *dfront_t;       //!< D�ե��ƥ����ĥ꡼����Ƭ�Ρ���
    struct dfront       *dfront_c;	     //!< D�ե��ƥ����ĥ꡼�θ��߽������Ƥ���Ρ���
	Ulong               n_imp_info_list; //!< ���ݤ����ް����ꥹ�Ȥο�
	Ulong               n_enter_imp_list; //!< �ް����ꥹ�Ȥθ��ߤ���Ͽ��
    struct imp_info     *imp_info_list;  //!< �ް����ꥹ��
    struct btree		*btree;		     //!< �Хå��ȥ�å��ĥ꡼
	Ulong               last_n;          //!< �����������ͤ�ްդǤ����ꥹ�Ȥν���
	Ulong               cnt_n;           //!< �����������ͤ�ްդǤ����ꥹ�Ȥβ��
	Ulong               last_f;          //!< �����Ǹξ��ͤ�ްդǤ����ꥹ�Ȥν���
	Ulong               cnt_f;           //!< �����Ǹξ��ͤ�ްդǤ����ꥹ�Ȥβ��
	struct line         **imp_list_n;
	struct line         **imp_list_f;
	struct line_stack   *cir_fix;        //!<* ��ϩ�����ˤ������ͤξ���
} GENE_HEAD;


/*!
  @struct ass_list
  @brief �ͳ�����Ƥ˴ؤ������
*/
typedef struct ass_list
{
    struct line		    *line;           //!< �ͤ������Ƥ뿮����
	enum   state_3      ass_state3;      //!< ������Ƥ���
	enum   condition    condition;       //!< ������Ƥ�ΤϿ������������� or �ξ���
    struct ass_list     *next;           //!< �����ؤΥݥ���(ñ�����ꥹ��)
} ASS_LIST;

/*!
  @struct btree
  @brief �Хå��ȥ�å��ĥ꡼�Υǡ�����¤

  @note ���ߤ�����������ʬ��������
*/
typedef struct btree
{
	Ulong               flag;		     //!< back track tree��flag 
	enum	condition   condition;       //!< ������ or �ξ��� 
	enum    state_3     ass_state3;      //!< �������˳�����Ƥ���
    struct	line		*line;           //!< �ͤ������Ƥ뿮����
#ifndef OLD_IMP2
	Ulong               last_n;          //!< �����������ͤ�ްդǤ����ꥹ�Ȥν���
	Ulong               cnt_n;           //!< �����������ͤ�ްդǤ����ꥹ�Ȥβ��
	Ulong               last_f;          //!< �����Ǹξ��ͤ�ްդǤ����ꥹ�Ȥν���
	Ulong               cnt_f;           //!< �����Ǹξ��ͤ�ްդǤ����ꥹ�Ȥβ��
#else
    struct	imp_trace   *imp_trace;      //!< �����Ǵް����Ǥ�������������or��
#endif /* OLD_IMP2 */
    struct	just_info	*just_e; 	     //!< ��Ͽ���줿������������ 
    struct	just_info	*just_d; 	     //!< ������줿������������
	struct	just_info	*just_flag;      //!< JUSTIFY_NO, _FL�ε�Ͽ
    struct	btree		*next;		     //!< back track tree�λҤؤΥݥ���
    struct	btree		*prev;		     //!< back track tree�οƤؤΥݥ���
} BTREE;
	
/*!
  @struct dfront
  @brief D�ե��ƥ����Υǡ�����¤

  @note ��ϩ��ʬ���ˤ��碌��nʬ�������Ƥ���
  @note ass_list��¸�ߤ�����ˤ�äƤϻ������򤫤��������ɤ� 
*/
typedef struct dfront
{
	Ulong               flag;		    //!< dfrontier��flag
	struct	line		*line;		    //!< FFR����Ƭ�������ؤΥݥ���
    struct	ass_list    *ass_list;	    //!< �������ͤȿ����� 
#ifndef OLD_IMP2
	Ulong               last_n;          //!< �����������ͤ�ްդǤ����ꥹ�Ȥν���
	Ulong               cnt_n;           //!< �����������ͤ�ްդǤ����ꥹ�Ȥβ��
	Ulong               last_f;          //!< �����Ǹξ��ͤ�ްդǤ����ꥹ�Ȥν���
	Ulong               cnt_f;           //!< �����Ǹξ��ͤ�ްդǤ����ꥹ�Ȥβ��
#else
    struct	imp_trace   *imp_trace;      //!< �����Ǵް����Ǥ�������������or��
#endif /* OLD_IMP2 */
	struct	dfront		*prev;		    //!< �ƤؤΥݥ��� �롼�Ȥ�NULL
    struct	dfront		**next;		    //!< �Ҷ��ؤΥݥ���(����line��n_out)
} DFRONT;

/*!
  @struct dfront_list
  @brief D�ե��ƥ����Υꥹ�ȹ�¤
*/
typedef struct dfront_list			
{
    struct	dfront		*dfront;	    //!< Dforntier�ؤΥݥ���
    struct	dfront_list	*next;		    //!< �����ؤΥݥ���
} DFRONT_LIST;

/*!
  @struct imp_info
  @brief �ް��������Ϥȷ��

  @note  pin_num��ɽ�����򤹤뤿��˥����ɲ�����Ƥ���
*/
typedef struct imp_info			    
{
    enum   state_3      new_state3;	   //!< �ް����븶������
	enum   condition    condition;	   //!< ������ or �ξ���
	Ulong	        	pin_num;       //!< �ɤ�pin��0(out),1(in0),2(in1),3, ... 
	Ulong	        	max_imp_id;    //!< �ްդε����ȤʤäƤ����ͳ�����Ƥκ����
	struct line         *line;	       //!< �ް����뿮�����ؤΥݥ��� 
	struct imp_info     *next;	       //!< ���ؤΥݥ���
} IMP_INFO;


/*!
  @struct just_info
  @brief ��������ɬ�פʿ������Ȥ��ξ��֤ξ��� 
*/
typedef struct just_info
{
	enum   condition    condition;	   //!< ������ or �ξ���
	struct line         *line;	       //!< ���������뿮�����ؤΥݥ���
	struct just_info    *next;	       //!< ���ؤΥݥ���
} JUST_INFO;


/*!
  @struct imp_result_info
  @brief �ް����ˤ����
*/
typedef struct imp_result_info
{
	enum   imp_state    i_state;       //!< �ް����μ»ܷ��
	enum   state_3      state3;       //!< �Ѳ�����state ( X -> 0 or 1 or U )
	Ulong				pin_num;      //!< �Ѳ�����pin�ֹ�
} IMP_RESULT_INFO;

/*!
  @struct imp_result
  @brief �ް����ˤ����
*/
typedef struct imp_result
{
	enum   imp_state    i_state;       //!< �ް����μ»ܷ��
	Ulong				imp_id;        //!< ���ͤ����ް�����ID
} IMP_RESULT;

/*!
  @struct imp_trace
  @brief �ް����ε�Ͽ

  @note  �Хå��ȥ�å���ƽ�������Ѥ��뤿�ᡢ�ް������ս��Ͽ����
*/
typedef struct imp_trace
{
	enum   condition    condition;   //!< �ް��������� ������(STAT_N) or �ξ���(STAT_F) */
	struct line         *line;       //!< �ް������������ؤΥݥ���
	struct imp_trace    *next;       //!< �����ؤΥݥ���
} IMP_TRACE;


/*!
  @struct gene_info
  @brief �ѥ����������˴ؤ������
*/
typedef struct gene_info
{
	enum   f_class      f_class;     //!< �ξ��ʬ����
	enum   gene_result  result;      //!< �ѥ����������¹Է��
	struct flist        *fault;      //!< �ѥ����������оݸξ�ؤΥݥ���
	struct cc_pattern   *cc_pat;     //!< �ξ���Ф������������ѥ�����(3��)
} GENE_INFO;


/*!
  @struct learn_list
  @brief ��Ū�ؽ��˴ؤ������
*/
typedef struct learn_list
{
    struct line		    *line;           //!< �ͤ������Ƥ뿮����
	enum   state_3      ass_state3;      //!< ������Ƥ���
    struct learn_list   *next;           //!< �����ؤΥݥ���(ñ�����ꥹ��)
} LEARN_LIST;

/*!
  @struct learn_list_list
  @brief ��Ū�ؽ��˴ؤ������
*/
typedef struct indirect_imp_info
{
	Ulong               list_size;       //!<
	Ulong               enter_id;        //!<
	Ulong               get_id;          //!<
    struct learn_list   **l_list;        //!< learn_list�ؤΥݥ���
	struct learn_list   *next;           //!< �����ؤΥݥ���(ñ�����ꥹ��)
} INDIRECT_IMP_INFO;

/*!
  @enum learn_gate_type
  @brief �ؽ�����/����뿮������
*/
typedef enum learn_type {
	LEARN_OFF = 0,     //!< ̤�»�
	LEARN_ON,          //!< �ؽ���»ܤ���/�»ܤ����
} LEARN_TYPE;

/*!
  @enum back_track_result
  @brief �Хå��ȥ�å�������̤��֤�
*/
typedef struct back_result {
	enum   gene_back    result;
	Ulong               back_imp_id; //!< �Хå��ȥ�å������ʿ�
} BACK_RESULT;

#endif
