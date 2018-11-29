/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief �桼�ƥ���ƥ��ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* getenv() exit()*/
#include <ctype.h> /* isspace */
#include "topgun.h"
#include "topgunLine.h"
#include "topgunAtpg.h"
#include "topgunUty.h"   /* TOPGUN_MAX_ATOLO */
#include "topgunError.h" /* FEC_XX */
#include "topgunIo.h"   /* DELIMITER_XX */


#define BIT4_0  4  //!<  2��4bitɽ���ˤ�����0�ο�("4")�򼨤�  0: 0000 -> 4
#define BIT4_1  3  //!<  2��4bitɽ���ˤ�����0�ο�("3")�򼨤�  1: 0001 -> 3
#define BIT4_2  3  //!<  2��4bitɽ���ˤ�����0�ο�("3")�򼨤�  2: 0010 -> 3
#define BIT4_3  2  //!<  2��4bitɽ���ˤ�����0�ο�("2")�򼨤�  3: 0011 -> 2
#define BIT4_4  3  //!<  2��4bitɽ���ˤ�����0�ο�("3")�򼨤�  4: 0100 -> 3
#define BIT4_5  2  //!<  2��4bitɽ���ˤ�����0�ο�("2")�򼨤�  5: 0101 -> 2
#define BIT4_6  2  //!<  2��4bitɽ���ˤ�����0�ο�("2")�򼨤�  6: 0110 -> 2
#define BIT4_7  1  //!<  2��4bitɽ���ˤ�����0�ο�("1")�򼨤�  7: 0111 -> 1
#define BIT4_8  3  //!<  2��4bitɽ���ˤ�����0�ο�("3")�򼨤�  8: 1000 -> 3
#define BIT4_9  2  //!<  2��4bitɽ���ˤ�����0�ο�("2")�򼨤�  9: 1001 -> 2
#define BIT4_10 2  //!<  2��4bitɽ���ˤ�����0�ο�("2")�򼨤� 10: 1010 -> 2
#define BIT4_11 1  //!<  2��4bitɽ���ˤ�����0�ο�("1")�򼨤� 11: 1011 -> 1
#define BIT4_12 2  //!<  2��4bitɽ���ˤ�����0�ο�("2")�򼨤� 12: 1100 -> 2
#define BIT4_13 1  //!<  2��4bitɽ���ˤ�����0�ο�("1")�򼨤� 13: 1101 -> 1
#define BIT4_14 1  //!<  2��4bitɽ���ˤ�����0�ο�("1")�򼨤� 14: 1110 -> 1
#define BIT4_15 0  //!<  2��4bitɽ���ˤ�����0�ο�("0")�򼨤� 15: 1111 -> 0

extern void utility_renew_parameter_with_env ( char *, Ulong *, Ulong );



/********************************************************************************
 * << Function >>
 *		get token in string
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 *	   Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fp            I      FILE*   file pointer
 * string        O      char*   copy string
 * delimiter     I      char*   end of string character
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/8
 *
 ********************************************************************************/

void utility_token
(
 FILE *fp,
 char *string,
 char *delimiter,
 Ulong string_size
 )
{
	int  c;
	int  i;

	int  utility_skip_char();


	char *func_name = "utility_token";

	/* ����ޤǿʤ� */
	utility_skip_char( fp, DELIMITER_SP );

	i = 0;
	/* ��ʸ����� */
	c = fgetc( fp );

	/* �Ȥä�ʸ����Delimiter��¸�ߤ��뤫Ĵ�٤� */
	while ( strchr ( delimiter, c ) == NULL){
		/* ¸�ߤ��ʤ� */
		if(c == EOF){
			/* �ե����뤬��λ */
			/* �ʤ󤫥��顼�ξ��Τۤ���¿���褦�˻פ� */
			return ;
		}
		/* ���Ĥ���ʸ����string�˳�Ǽ���� */
		string[ i++ ] = c;
		c = fgetc( fp );
	}

	/* ¸�ߤ��� */
	string[ i ] = '\0';  /* ʸ����κǸ�ΰ����դ��� */

	/* ʸ����Υ�������Ĺ����Ķ������� */
	if ( i >= string_size ) {
		topgun_error( FEC_BUFFER_OVER_FLOW, func_name );
	}
	ungetc( c, fp ); /* ���ڤ�ʸ����ե�����ݥ��󥿤��֤� */
}

/********************************************************************************
 * << Function >>
 *		skip to delimmiter file pointer in fp
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * c             O      int     (int)character except delimiter
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fp            I      FILE*   file pointer
 * delimiter     I      char*   end of string character
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/8
 *
 ********************************************************************************/

int utility_skip_char
(
 FILE *fp,
 char *delimiter
 ){
	int  c;
	int  fgetc();

	c = fgetc( fp );

	/* ���ڤ�ʸ�������ޤǷ����֤� */
	while ( strchr( delimiter, c ) != NULL ) {
		if(c == EOF){
			/* �ե����뤬��λ */
/* 			return ; */
		}
		c = fgetc( fp );
	}
	ungetc( c, fp);

	/* ���ڤ�ʸ���μ���ʸ��?���֤� */
	return( c );
}

/********************************************************************************
 * << Function >>
 *		jump delimmiter file pointer in fp
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * 0             O      (int)   terminate normal
 * EOF           O      (int)   EOF
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fp            I      FILE*   file pointer
 * delimiter     I      char*   end of string character
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/8
 *
 ********************************************************************************/

void utility_jump_to_char
(
 FILE *fp,
 char delimiter
 ){
	int  c;

	char *func_name = "utility_jump_to_char";

	c = fgetc( fp );
	while ( c != delimiter ) {
		if ( c == EOF ) {
			topgun_error( FEC_FILE_FORMAT_ERROR_NOT_CHAR, func_name );
		}
		c = fgetc( fp );
	}
}

/********************************************************************************
 * << Function >>
 *		jump delimmiter file pointer in fp
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * 0             O      (int)   terminate normal
 * EOF           O      (int)   EOF
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fp            I      FILE*   file pointer
 * delimiter     I      char*   end of string character
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/8
 *
 ********************************************************************************/

int utility_skip_sentence( FILE *fp )
{
	int	c;
	int fgetc();

	c = fgetc( fp );

	while ( c != TOPGUN_LF ) {
		if( c == EOF ) {
			return( EOF );
		}
		c = fgetc( fp );
	}
	return( 0 );
}

/********************************************************************************
 * << Function >>
 *		calculate hash table value with stirng
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * 0             O      (int)   terminate normal
 * EOF           O      (int)   EOF
 *
 * << Argument >>
 * args			(I/O)	type	description
 * fp            I      FILE*   file pointer
 * delimiter     I      char*   end of string character
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2005/7/8
 *
 ********************************************************************************/

Ulong utility_calc_string_hash_value
(
 char *string,
 Ulong hash_size
 ){

	Ulong value;
	int   i;

	for ( value = 0, i = 0; string[ i ] != TOPGUN_NC; i++){
		value += string[ i ];
	}

	value %= hash_size;

	return( value );
}

/********************************************************************************
 * << Function >>
 *		"Ulong�ѿ�"��0bit�ο������
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * num_0_bit     I      Ulong   0bit�ο�
 *
 * << Argument >>
 * args			(I/O)	type	description
 * test_bit      I      Ulong   �������оݤ��ѿ�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/1/14
 *
 ********************************************************************************/

Ulong utility_count_0_bit
(
 Ulong test_bit
 ){
	static Ulong bit_valid  = 8 /* 32bit / 4 */;
	Ulong num_x_bit = 0;
	Ulong bit4 = 0;
	Ulong i; /* ������ */

	char *func_name = "utility_count_0_bit";

	for ( i = 0 ; i < bit_valid ; i++ ) {

		bit4 = test_bit % 16;
		test_bit /= 16;

		switch ( bit4 ) {
		case 0:
			num_x_bit += 4;
			break;
		case 1:
		case 2:
		case 4:
		case 8:
			num_x_bit += 3;
			break;
		case 3:
		case 5:
		case 6:
		case 9:
		case 10:
		case 12:
			num_x_bit += 2;
			break;
		case 7:
		case 11:
		case 13:
		case 14:
			num_x_bit += 1;
			break;
		case 15:
			break;
		default:
			topgun_error ( FEC_NO_ERROR_CODE, func_name );
		}
	}

	return( num_x_bit );
}

/********************************************************************************
 * << Function >>
 *		"Ulong�ѿ�"��1bit�ο������
 *
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * num_0_bit     I      Ulong   0bit�ο�
 *
 * << Argument >>
 * args			(I/O)	type	description
 * test_bit      I      Ulong   �������оݤ��ѿ�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/1/14
 *
 ********************************************************************************/

Ulong utility_count_1_bit
(
 Ulong test_bit
 ){
	static Ulong bit_valid  = 8 /* 32bit / 4 */;
	Ulong num_x_bit = 0;
	Ulong bit4 = 0;
	Ulong i; /* ������ */

	char *func_name = "utility_count_1_bit";

	for ( i = 0 ; i < bit_valid ; i++ ) {

		bit4 = test_bit % 16;
		test_bit /= 16;

		switch ( bit4 ) {
		case 0:
			num_x_bit += 0;
			break;
		case 1:
		case 2:
		case 4:
		case 8:
			num_x_bit += 1;
			break;
		case 3:
		case 5:
		case 6:
		case 9:
		case 10:
		case 12:
			num_x_bit += 2;
			break;
		case 7:
		case 11:
		case 13:
		case 14:
			num_x_bit += 3;
			break;
		case 15:
			num_x_bit += 4;
			break;
		default:
			topgun_error ( FEC_NO_ERROR_CODE, func_name );
		}
	}

	return( num_x_bit );
}

/********************************************************************************
 * << Function >>
 *		ASCII ( char * )��Ulong���Ѵ�����
 *      ����������ζ�����б�
 *    
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * num_0_bit     I      Ulong   0bit�ο�
 *
 * << Argument >>
 * args			(I/O)	type	description
 * test_bit      I      Ulong   �������оݤ��ѿ�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/1/14
 *
 ********************************************************************************/

FL_BOOL utility_atolo
(
 char  *ascii,
 Ulong *ulong_number
 )
{
	char   *data;
	Ulong	number;
	Ulong   i;

	number = 0;
	data   = ascii;

	//! space���ʤ��ʤ�ޤǿʤ�
	while ( isspace( *data ) ) {
		data++;
	}

	if (*data == '\0')	{
		//! NULL( = \\0)�Ǥʤ��Ϥ���
		return ( TOPGUN_NG );
	}

	
	for ( i = 0 ; i < TOPGUN_ATOLO_LEN ; i++, data++ )	{
		
		if ( ( '0' <= *data ) && ( *data <= '9' ) ) {
			//! ʸ���������ξ��
			number = number * 10 + (*data - '0');
		}
		else
		{
			//! ʸ���������Ǥʤ����
			while ( isspace( *data ) ) {
				data++;
			}
			
			if ( *data == '\0')
			{
				//! NULL( = \\0 )�ʤ齪λ
				*ulong_number = number;
				return ( TOPGUN_OK );
			}
			else
			{
				//! NULL( = \\0 )�Ǥʤ�������
				return ( TOPGUN_OK );
			}
		}
	}
	
	//! 10��(10���ʾ�)������
	//! ���ξ��Ϻǽ��'0'�����äƤ������
	//! ���ξ��Ϻǽ��'0'�����äƤ��ʤ����
	if ( ( ('0' <= *data && *data <= '9') &&
		   ( number < TOPGUN_MAX_ATOLO ) )       ||
		 ( ('0' <= *data && *data <= '5') &&
		   (number == TOPGUN_MAX_ATOLO ) ) )	{
		number = number * 10 + (*data - '0');
		data++;
	}
	while ( isspace( *data ) ) {
		data++;
	}
	
	if (*data == '\0')
	{
		//! NULL( = \\0 )�ʤ齪λ
		*ulong_number = number;
		return ( TOPGUN_OK );
	}
	else
	{
		//! NULL( = \\0 )�Ǥʤ�������
		return ( TOPGUN_NG );
	}
}

/********************************************************************************
 * << Function >>
 *		�Ķ��ѿ��˱����ƥѥ�᡼�����ѹ�����
 *    
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 * num_0_bit     I      Ulong   0bit�ο�
 *
 * << Argument >>
 * args			(I/O)	type	description
 * test_bit      I      Ulong   �������оݤ��ѿ�
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/1/14
 *
 ********************************************************************************/

void utility_renew_parameter_with_env
(
 char  *env,
 Ulong *parameter,
 Ulong default_value
 )
{
	char *size;
	Ulong data;
	FL_BOOL result;
	
	size = ( char * ) getenv( env );

	*parameter = default_value;
	
	if ( size != NULL ) {
		result = utility_atolo ( size, &data );

		if ( result == TOPGUN_OK ) {

			*parameter = data;
		}
	}

	printf("ENV : %s %lu\n", env, *parameter);
}

/********************************************************************************
 * << Function >>
 *		�ꥹ�Ȥ򤯤äĤ���
 *    
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org_list      I/O    LEARN_LIST *
 * add_list      I      LEARN_LIST *
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/3/13
 *
 ********************************************************************************/

LEARN_LIST *utility_combine_learn_list
(
 LEARN_LIST *old_list,
 LEARN_LIST *add_list
 )
{
	
	LEARN_LIST *old_list_last = NULL;

	if ( old_list == NULL ) {
		old_list = add_list;
		add_list = NULL;
	}
	else {
		
		old_list_last = old_list;

		while ( old_list_last->next ) {

			old_list_last = old_list_last->next;
			
		}
		/* Ϣ�뤹�� */
		old_list_last->next = add_list;

		add_list = NULL;
	}
	return ( old_list );
}

/********************************************************************************
 * << Function >>
 *		ass_list�򤯤äĤ���
 *    
 * << Function ID >>
 *	   	?-?
 *
 * << Return >>
 * args			(I/O)	type	description
 *     Void
 *
 * << Argument >>
 * args			(I/O)	type	description
 * org_list      I/O    ASS_LIST *
 * add_list      I      ASS_LIST *
 *
 * << extern >>
 * name			(I/O)	type	description
 *      None
 *
 * << author >>
 * 		Masayoshi Yoshimura (FLEETS)
 *
 * << Date >>
 *		2006/05/09
 *
 ********************************************************************************/

ASS_LIST *utility_combine_ass_list
(
 ASS_LIST *old_list,
 ASS_LIST *add_list
 )
{
	
	ASS_LIST *old_list_last = NULL;

	if ( old_list == NULL ) {
		old_list = add_list;
		add_list = NULL;
	}
	else {
		
		old_list_last = old_list;

		while ( old_list_last->next ) {

			old_list_last = old_list_last->next;
			
		}
		/* Ϣ�뤹�� */
		old_list_last->next = add_list;

		add_list = NULL;
	}
	return ( old_list );
}



/* End of File */
