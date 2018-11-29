/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ��Ū�ؽ���DAG�ǹԤ��ؿ��˴ؤ�������
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

typedef enum learn_state{
	LEARN_STATE_0 = 0, //!< �ؽ���STATE3_0 
	LEARN_STATE_1 = 1, //!< �ؽ���STATE3_1 
	N_LEARN_STATE = 2 //!< STATE3_0 �� STATE3_1 ��ؽ�(�����ͤΤ�)
} LEARN_STATE;

typedef enum dag_way{  //!< DAG������θ���
	DAG_FORWARD   = 0, //!< ������
	DAG_BACKWARD  = 1  //!< ������
} DAG_WAY;

typedef enum dag_parent{  //!< DAG������θ���
	DAG_MADA   = 0, //!< ̤��ǧ
	DAG_SUMI   = 1  //!< �ƤϺѤ��
} DAG_PARENT;


typedef enum learn_flag{  //!< DAG������θ���
	LEARN_FLAG_NULL   = 0, 
	LEARN_FLAG_DIRECT = 1,  //1
	LEARN_FLAG_LOOP_S = 2,  //2 �롼�ץ�������
	LEARN_FLAG_LOOP_C = 4,  //3 �롼�ץ����å�
	LEARN_FLAG_MAIN   = 8,  //4 �롼�פΤ���Υᥤ��Ρ���
	LEARN_FLAG_NO_USE = 16,  //5 �롼�פΤ�����
	LEARN_FLAG_NONE    = 32,  //6 �ե���ɥΡ��ɤΥޡ���
	LEARN_FLAG_BRANCH   = 64,   //7 �Хå���ɥΡ��ɤΥޡ���
	LEARN_FLAG_PARENT   = 128,  //8 �ե���ɥΡ��ɤΥޡ���
	LEARN_FLAG_CHILDREN = 256,   //9 �Хå���ɥΡ��ɤΥޡ���
	LEARN_FLAG_PARE_CHI = 384,  //10 �ե���ɥΡ��ɤΥޡ���
	LEARN_FLAG_PARENT_F = 512,   //11 �Хå���ɥΡ��ɤΥޡ���
	LEARN_FLAG_CHILD_C  = 1024,  //12 children�Υޡ���
	LEARN_FLAG_CONTRA   = 2048,  //13
	LEARN_FLAG_CONTRA_E = 4096,  //14
	LEARN_FLAG_LOOP_R   = 8192,   //15 loop��ϩ�򤿤ɤ�Ȥ��ˤ��Ƥ�
	LEARN_FLAG_S_LIST   = 16384,  //16 l
	LEARN_FLAG_R_BRANCH = 32768,  //17 �Фλޤ�������
	LEARN_FLAG_SORT     = 65536,   //18 toposort
	LEARN_FLAG_REACH    =131072,   //19 reachable
	LEARN_FLAG_ONLIST   =262144,   //20 on cullent_list
	LEARN_FLAG_NO_REACH =524288,   //21 reachable
	LEARN_FLAG_NO_REACH_NOT =1048576,   //22 reachable
	LEARN_FLAG_FINISH   =2097152,   //23 finish
	LEARN_FLAG_RESEARCH =4194304,   //24 re search
	LEARN_FLAG_DEPTH    =8388608,   //25 
	LEARN_FLAG_DEBUG    =16777216,   //debug
} LEARN_FLAG;


typedef enum learn_mode_flag{  //!< DAG������θ���
	LEARN_MODE_SELECT  = 1,
	LEARN_MODE_REENTRY = 2,
	LEARN_MODE_RESORT  = 4, 
} LEARN_MODE_FLAG;


/*!
  @struct learn_node
  @brief dag�ѤΥΡ���
*/
typedef struct learn_node
{
    Ulong               node_id;	//!< �Ρ��ɤ�ID
    struct line          *line;		 //!< �Ρ��ɤο������Υݥ���
    enum state_3         state3;     //!< �Ρ��ɤο������ξ��֤�3��( ������ )�򼨤�
	Ulong               level;	    //!< topological level sort��
	Ulong               imp_count;	//!< �ް��������
	Ulong               flag;	    //!< �Ρ��ɤ���ã����ã�򼨤�
	Ulong              	n_forward;  //!< �Ρ��ɤ��ؤ��Ƥ����
	Ulong              	n_for_org;  //!< �Ρ��ɤ��ؤ��Ƥ�����θ�����
	struct learn_node   *reverse;   //!< �ж��ΥΡ���
	struct learn_node   *prev;      //!< �Ρ��ɤ������ꥹ��
	struct learn_node   *next;      //!< �Ρ��ɤ������ꥹ��
	struct learn_branch *forward;   //!< ľ�ܴް��ѤΥ֥���(��)
	struct learn_branch *backward;  //!< ľ�ܴް��ѤΥ֥���(��)
	struct learn_s_list *indirect;  //!< ���ܴް��ѤΥ֥���
	struct learn_node   *parent;    //!< �ƤؤΥݥ���
	struct learn_s_list *children;  //!< �ҤؤΥݥ���
} LEARN_NODE;


/*!
  @struct learn_node
  @brief �����Ρ��ɾ���
*/
typedef struct learn_eq_head
{
	struct learn_eq_info *eq_info;   //!< Ʊ�ͤο�������State�Υꥹ��

} LEARN_EQ_HEAD;

/*!
  @struct learn_branch
  @brief dag�ѤΥ֥���
*/
typedef struct learn_branch {
	Ulong               flag;   //!< Ʊ�ͤθ��ˤ���
	Ulong               count;   //!< Ʊ�ͤθ��ˤ���
	Ulong               loop;   //!< Ʊ�ͤθ��ˤ���
	struct learn_node   *node;  //!< �Ρ��ɤؤΥݥ���
	struct learn_branch *next;  //!< ���Υꥹ�ȤؤΥݥ���
} LEARN_BRANCH;

/*!
  @struct learn_s_list
  @brief node��ñ�����ꥹ�ȤΥꥹ��
*/
typedef struct learn_b_list {
	struct learn_branch  *b_list; //!< �ꥹ�ȤؤΥݥ���
	struct learn_b_list  *next;   //!< ���Υꥹ�ȤؤΥݥ���
} LEARN_B_LIST;

/*!
  @struct learn_s_list
  @brief node��ñ�����ꥹ��
*/
typedef struct learn_s_list {
	struct learn_node   *node;  //!< �Ρ��ɤؤΥݥ���
	struct learn_s_list *next;  //!< ���Υꥹ�ȤؤΥݥ���
} LEARN_S_LIST;

/*!
  @struct learn_s_list
  @brief node��ñ�����ꥹ�ȤΥꥹ��
*/
typedef struct learn_s_list2 {
	struct learn_s_list  *s_list; //!< �ꥹ�ȤؤΥݥ���
	struct learn_s_list2 *next;   //!< ���Υꥹ�ȤؤΥݥ���
} LEARN_S_LIST2;

/*!
  @struct learn_node_list
  @brief dag�Ѥ��������ꥹ��
*/
typedef struct learn_node_list {
	struct learn_node        *node;  //!< �Ρ��ɤؤΥݥ���
	struct learn_node_list   *prev;  //!< �����Υꥹ�ȤؤΥݥ���
	struct learn_node_list   *next;  //!< �����Υꥹ�ȤؤΥݥ���
} LEARN_NODE_LIST;

/*!
  @struct learn_branch
  @brief dag�ѤΥ֥���
*/
typedef struct learn_topo {
	Ulong                   level;        //!< ��٥�
	struct learn_node       *top_node;    //!< ���Υ�٥����Ƭ�Ρ��ɤؤΥݥ���
	struct learn_topo       *next_level;  //!< �����Υ�٥�ؤΥݥ���
} LEARN_TOPO;


typedef struct learn_dag_info {
	Ulong                   num_node;
	Ulong                   max_level;
	Ulong                   continue_flag;
	struct learn_s_list    *indirect_node_list;
} LEARN_DAG_INFO;

typedef struct learn_dag_return {
	Ulong                   n_add_direct_node;
	Ulong                   redo_flag;
	LEARN_MODE_FLAG         mode_flag;
	LEARN_S_LIST            *add_indirect_list;
} LEARN_DAG_RETURN;


typedef struct learn_flag_info {
	LEARN_NODE              **list;
	Ulong                   count;          /* ��Ͽ�Ѥߤο� ( = 0 �ʤ� ) */
} LEARN_FLAG_INFO;

typedef struct learn_search_info {
	LEARN_NODE              **list;
	Ulong                   enter_id;       /* ��Ͽ�Ѥߤο� ( = 0 �ʤ� ) */
	Ulong                   get_id;         /* �����Ѥߤο� ( = 0 �ʤ� ) */
	Ulong                   number_id;      /* ��Ͽ�Ѥߤο� ( = 0 �ʤ� ) */
} LEARN_SEARCH_INFO;

