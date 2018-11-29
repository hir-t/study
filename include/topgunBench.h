/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief .bench I/F�˴ؤ�������
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/*!
  @{
  @name .bench�ե����ޥåȤ˴ؤ��륭�����
*/
#define BENCH_STR_INPUT       "INPUT"    //!< input
#define BENCH_STR_OUTPUT      "OUTPUT"   //!< output
#define BENCH_STR_GATE_CONECT '='        //!< �����Ȥ���³
#define BENCH_STR_COMMENT     '#'        //!< �����ȹ�
/*!
  @}
*/

/*!
  @{
  @name .bench�ե����ޥåȤˤ����륲����̾ɽ��
 */
#define BENCH_GATE_AND        "AND"     //!< AND������
#define BENCH_GATE_NAND       "NAND"    //!< NAND������
#define BENCH_GATE_OR         "OR"      //!< OR������
#define BENCH_GATE_NOR        "NOR"     //!< NOR������
#define BENCH_GATE_NOT        "NOT"     //!< NOT������ ( = INV������ )
#define BENCH_GATE_BUF        "BUF"     //!< BUFFER������
#define BENCH_GATE_INV        "INV"     //!< INV������
#define BENCH_GATE_XOR        "XOR"     //!< EX-OR������
#define BENCH_GATE_XNOR       "XNOR"    //!< EX-NOR������
/*!
  @}
*/

#define BENCH_NAME_SIZE 1024            //!< 1�Ĥ�̾������κ���ʸ����

/*!
  @enum BENCH_CELL_DEFINE
  @brief .bench ������cell�η�
*/
typedef enum bench_cell_define{
	BENCH_CELL_INPUT = 0,       //!< input
	BENCH_CELL_OUTPUT,          //!< output
	BENCH_CELL_UNKNOWN,         //!< ����¾
} BENCH_CELL_DEFINE;

/*!
  @enum BENCH_ANALYZE_HEADER
  @brief .bench ������file�إå��η�
*/
typedef enum bench_analyze_header{
	BENCH_COMMENT = 1,         //!< �����ȹ�
	BENCH_END,                 //!< �ե�����ν�ü
	BENCH_INPUT,               //!<  �������ϥԥ�
	BENCH_OUTPUT,              //!<  �������ϥԥ�
	BENCH_GATE,                //!<  ������
} BENCH_ANALYZE_HEADER;

/*!
  @enum BENCH_ANALYZE_LINE_END
  @breif .bench �����ι�����Ƚ��
*/
typedef enum bench_analyze_line_end{
	BENCH_LINE_END = 1,        //!< ����
	BENCH_LINE_CONTINUE,       //!< �����Ǥʤ�
} BENCH_ANALYZE_LINE_END;

