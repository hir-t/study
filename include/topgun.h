/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief Flight�ᥤ��ؿ���
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/* TOPGUN Global Define */
#ifndef TOPGUN_H
#define TOPGUN_H


#define	ERROR		-1

typedef unsigned char Uchar;     /*  ���ʤ�char�����                  */
typedef unsigned short Ushort;   /*  ���ʤ�short�����                 */
typedef unsigned int Uint;       /*  ���ʤ�int�����                   */
typedef unsigned long Ulong;     /*  ���ʤ�long�����                  */

#define	FL_ULMAX	0xffffffff
#define INV_1BIT	0x00000001		/* for invert last bit*/

#define	NULL_LINE	0x00000001	/* null line flag */

#define	ATPG_FINISH	0x01

/*!
  @enum FL_BOOL
  Good �� No Good�����
*/
typedef enum {
    TOPGUN_OK = 0,
    TOPGUN_NG,
} FL_BOOL;

#endif
