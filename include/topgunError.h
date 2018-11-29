/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief ���顼�����ȥ��顼��å������˴ؤ�������
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/*!
  @enum flight_errorcode
  @brief ���顼�������ֹ�
*/
typedef enum topgunErrorcode
{
    FEC_NORMAL_END,                      //!< ����
    
    FEC_FILE_OPEN_ERROR = 101,           //!< �ե����륪���ץ󥨥顼
    FEC_FILE_CLOSE_ERROR,                //!< �ե����륯�������顼
    FEC_FILE_REMOVE_ERROR,               //!< �ե�����õ�顼
    FEC_FILE_FORMAT_ERROR_NOT_CHAR,      //!< �ե�����ե����ޥå� ����ʸ����¸�ߤ��ʤ�
    
    FEC_CANT_ALLOCATE_MEMORY,            //!<  ���ꥢ��������󥨥顼
    FEC_BUFFER_OVER_FLOW,                //!<  �Хåե������С��ե�
    
    FEC_COMMAND_EXE_ERROR,				 //!<  ���ޥ�ɼ¹ԥ��顼
    
    FEC_HASH_ERROR_CODE,                 //!<  �ϥå���ؿ���Ϣ���顼
    FEC_SYSTEM_ERROR_CODE = 198,         //!<  �����ƥ२�顼
    FEC_ERROR_CODE = 199,                //!<  ̤������顼
    
    FEC_CIRCUIT              = 201,       //!< ��ϩ�ǡ������顼
    FEC_CIRCUIT_NO_PI,                    //!< ��ϩ�ǡ������顼 �������Ϥ�0
    FEC_CIRCUIT_NO_PO,                    //!< ��ϩ�ǡ������顼 �������Ϥ�0
    FEC_CIRCUIT_NO_LINE,                  //!< ��ϩ�ǡ������顼 ���������ʤ�
    FEC_CIRCUIT_NO_FAULT,                 //!< ��ϩ�ǡ������顼 ATPG�оݸξ㤬�ʤ�
    FEC_CIRCUIT_GATE_IO,                  //!< ��ϩ�ǡ������顼 �����Ȥμ���������Ϥ����פ��ʤ�
    FEC_CIRCUIT_GATE_TYPE,                //!< ��ϩ�ǡ������顼 �����Ȥμ��ब¸�ߤ��ʤ�
    FEC_FAULT_LIST,                       //!< �ξ�ꥹ�ȥ��顼
    
    FEC_PRG_ERROR            = 601,   //!< �ץ���२�顼
    FEC_PRG_ATPG_HEAD,                //!< �ץ���२�顼 Atpg_head
    FEC_PRG_LV_HEAD,                  //!< �ץ���२�顼 Lv_head
    FEC_PRG_LV_PI_HEAD,               //!< �ץ���२�顼 Lv_pi_head
    FEC_PRG_JUDGE_GENE_END,           //!< �ץ���२�顼 judgement of generate end
    FEC_PRG_JUDGE_IMP,                //!< �ץ���२�顼 Judge_imp
    FEC_PRG_LINE_STAT,                //!< �ץ���२�顼 line->stat
    FEC_PRG_LINE_TYPE,                //!< �ץ���२�顼 line->type
    FEC_PRG_LINE_ID,                  //!< �ץ���२�顼 line->line_id
    FEC_PRG_LINE_FLAG,                //!< �ץ���२�顼 line->flag
    FEC_PRG_LINE_COND,                //!< �ץ���२�顼 line->condition
    FEC_PRG_LINE_IN,                  //!< �ץ���२�顼 line->n_in
    FEC_PRG_LINE_OUT,                 //!< �ץ���२�顼 line->n_out
    FEC_PRG_CELL,                     //!< �ץ���२�顼 cell
    FEC_PRG_CELL_IO,                  //!< �ץ���२�顼 cell->in/out
    FEC_PRG_JUSTIFIED,                //!< �ץ���२�顼 justified function
    FEC_PRG_JUST_INFO,                //!< �ץ���२�顼 justified infomation
    FEC_PRG_IMPLICATION,              //!< �ץ���२�顼 dfrontier
    FEC_PRG_DFRONT,                   //!< �ץ���२�顼 dfrontier
    FEC_PRG_BTREE,                    //!< �ץ���२�顼 btree
    FEC_PRG_BACK_TRACK,               //!< �ץ���२�顼 back trace
    FEC_PRG_FLIST,                    //!< �ץ���२�顼 Flist_head
    FEC_PRG_ASS_LIST,                 //!< �ץ���२�顼 Flist_head
    FEC_PRG_LEARN_DAG,                //!< �ץ���२�顼 static learn with dag
    FEC_PRG_NODE_FLAG,                //!< �ץ���२�顼 static learn with dag
    FEC_PRG_NULL,                     //!< �ץ���२�顼 null poitner
    
    FEC_FSIM_VAL2_BLKI = 701,          //!< Fsim���¥��顼(2��Sim��BLKI���Բ�)
    FEC_FSIM_VAL3_BLKI = 701,          //!< Fsim���¥��顼(3��Sim��BLKI�ϲ�)
    FEC_FSIM_FAULT_FLAG,               //!< Fsim flag���顼
    
    FEC_NO_ERROR_CODE = -1,            //!<  ���顼�����ɤʤ�
}TOPGUN_ERROR_CODE;

/* function define */
void  topgun_error( TOPGUN_ERROR_CODE, char * );           //!< ���顼��å���������Ϥ��� */
void  topgun_error_o( TOPGUN_ERROR_CODE, char *, char * ); //!< ���顼��å�������ʸ����Ĥ��ǽ��Ϥ��� */
