/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief エラー処理とエラーメッセージに関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/*!
  @enum flight_errorcode
  @brief エラーコード番号
*/
typedef enum topgunErrorcode
{
    FEC_NORMAL_END,                      //!< 正常
    
    FEC_FILE_OPEN_ERROR = 101,           //!< ファイルオープンエラー
    FEC_FILE_CLOSE_ERROR,                //!< ファイルクローズエラー
    FEC_FILE_REMOVE_ERROR,               //!< ファイル消去エラー
    FEC_FILE_FORMAT_ERROR_NOT_CHAR,      //!< ファイルフォーマット ある文字が存在しない
    
    FEC_CANT_ALLOCATE_MEMORY,            //!<  メモリアロケーションエラー
    FEC_BUFFER_OVER_FLOW,                //!<  バッファオーバーフロー
    
    FEC_COMMAND_EXE_ERROR,				 //!<  コマンド実行エラー
    
    FEC_HASH_ERROR_CODE,                 //!<  ハッシュ関数関連エラー
    FEC_SYSTEM_ERROR_CODE = 198,         //!<  システムエラー
    FEC_ERROR_CODE = 199,                //!<  未定義エラー
    
    FEC_CIRCUIT              = 201,       //!< 回路データエラー
    FEC_CIRCUIT_NO_PI,                    //!< 回路データエラー 外部入力が0
    FEC_CIRCUIT_NO_PO,                    //!< 回路データエラー 外部出力が0
    FEC_CIRCUIT_NO_LINE,                  //!< 回路データエラー 信号線がない
    FEC_CIRCUIT_NO_FAULT,                 //!< 回路データエラー ATPG対象故障がない
    FEC_CIRCUIT_GATE_IO,                  //!< 回路データエラー ゲートの種類と入出力が一致しない
    FEC_CIRCUIT_GATE_TYPE,                //!< 回路データエラー ゲートの種類が存在しない
    FEC_FAULT_LIST,                       //!< 故障リストエラー
    
    FEC_PRG_ERROR            = 601,   //!< プログラムエラー
    FEC_PRG_ATPG_HEAD,                //!< プログラムエラー Atpg_head
    FEC_PRG_LV_HEAD,                  //!< プログラムエラー Lv_head
    FEC_PRG_LV_PI_HEAD,               //!< プログラムエラー Lv_pi_head
    FEC_PRG_JUDGE_GENE_END,           //!< プログラムエラー judgement of generate end
    FEC_PRG_JUDGE_IMP,                //!< プログラムエラー Judge_imp
    FEC_PRG_LINE_STAT,                //!< プログラムエラー line->stat
    FEC_PRG_LINE_TYPE,                //!< プログラムエラー line->type
    FEC_PRG_LINE_ID,                  //!< プログラムエラー line->line_id
    FEC_PRG_LINE_FLAG,                //!< プログラムエラー line->flag
    FEC_PRG_LINE_COND,                //!< プログラムエラー line->condition
    FEC_PRG_LINE_IN,                  //!< プログラムエラー line->n_in
    FEC_PRG_LINE_OUT,                 //!< プログラムエラー line->n_out
    FEC_PRG_CELL,                     //!< プログラムエラー cell
    FEC_PRG_CELL_IO,                  //!< プログラムエラー cell->in/out
    FEC_PRG_JUSTIFIED,                //!< プログラムエラー justified function
    FEC_PRG_JUST_INFO,                //!< プログラムエラー justified infomation
    FEC_PRG_IMPLICATION,              //!< プログラムエラー dfrontier
    FEC_PRG_DFRONT,                   //!< プログラムエラー dfrontier
    FEC_PRG_BTREE,                    //!< プログラムエラー btree
    FEC_PRG_BACK_TRACK,               //!< プログラムエラー back trace
    FEC_PRG_FLIST,                    //!< プログラムエラー Flist_head
    FEC_PRG_ASS_LIST,                 //!< プログラムエラー Flist_head
    FEC_PRG_LEARN_DAG,                //!< プログラムエラー static learn with dag
    FEC_PRG_NODE_FLAG,                //!< プログラムエラー static learn with dag
    FEC_PRG_NULL,                     //!< プログラムエラー null poitner
    
    FEC_FSIM_VAL2_BLKI = 701,          //!< Fsim制限エラー(2値SimにBLKIは不可)
    FEC_FSIM_VAL3_BLKI = 701,          //!< Fsim制限エラー(3値SimにBLKIは可)
    FEC_FSIM_FAULT_FLAG,               //!< Fsim flagエラー
    
    FEC_NO_ERROR_CODE = -1,            //!<  エラーコードなし
}TOPGUN_ERROR_CODE;

/* function define */
void  topgun_error( TOPGUN_ERROR_CODE, char * );           //!< エラーメッセージを出力する */
void  topgun_error_o( TOPGUN_ERROR_CODE, char *, char * ); //!< エラーメッセージを文字列つきで出力する */
