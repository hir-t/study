/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief Flight•·•§•Û¥ÿøÙ∑≤
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/* TOPGUN Global Define */
#ifndef TOPGUN_H
#define TOPGUN_H


#define	ERROR		-1

typedef unsigned char Uchar;     /*  …‰πÊ§ §∑char∑ø¿Î∏¿                  */
typedef unsigned short Ushort;   /*  …‰πÊ§ §∑short∑ø¿Î∏¿                 */
typedef unsigned int Uint;       /*  …‰πÊ§ §∑int∑ø¿Î∏¿                   */
typedef unsigned long Ulong;     /*  …‰πÊ§ §∑long∑ø¿Î∏¿                  */

#define	FL_ULMAX	0xffffffff
#define INV_1BIT	0x00000001		/* for invert last bit*/

#define	NULL_LINE	0x00000001	/* null line flag */

#define	ATPG_FINISH	0x01

/*!
  @enum FL_BOOL
  Good §» No Good§ŒƒÍµ¡
*/
typedef enum {
    TOPGUN_OK = 0,
    TOPGUN_NG,
} FL_BOOL;

#endif
