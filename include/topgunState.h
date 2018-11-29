/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 信号線の状態(state)に関する設定
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

#ifndef TOPGUN_STATE_H
#define TOPGUN_STATE_H

#define TOPGUN_STATE

/*!
  @enum STATE_3
  信号線の状態( 0 or 1 or X or U )を示す 
  line->stateなどで用いる
*/
typedef enum state_3
{
	STATE3_X = 0,            //!< 00000000 state  X 
	STATE3_0 = 1,            //!< 00000001 state  0
	STATE3_1 = 2,            //!< 00000010 state  1 
	STATE3_U = 3,            //!< 00000011 state  U 
	STATE3_C = 16            //!< 00010000 state  Conflict 
} STATE_3;

/*!
  @enum STATE_9
  信号線の状態を9値( 正常値/故障値 )を示す 
  line->stateなどで用いる
*/
typedef enum state_9
{
	STATE9_XX = 0,           //!< 00000000 state  X / X
	STATE9_0X = 1,           //!< 00000001 state  0 / X
	STATE9_1X = 2,           //!< 00000010 state  1 / X
	STATE9_UX = 3,           //!< 00000011 state  U / X
	STATE9_X0 = 4,           //!< 00000100 state  X / 0
	STATE9_00 = 5,           //!< 00000101 state  0 / 0
	STATE9_10 = 6,           //!< 00000110 state  1 / 0 = ND
	STATE9_U0 = 7,           //!< 00000111 state  U / 0
	STATE9_X1 = 8,           //!< 00001000 state  X / 1
	STATE9_01 = 9,           //!< 00001001 state  0 / 1 = D
	STATE9_11 = 10,          //!< 00001010 state  1 / 1
	STATE9_U1 = 11,          //!< 00001011 state  U / 1
	STATE9_XU = 12,          //!< 00001100 state  X / U
	STATE9_0U = 13,          //!< 00001101 state  0 / U
	STATE9_1U = 14,          //!< 00001110 state  1 / U
	STATE9_UU = 15,          //!< 00001111 state  U / U
	STATE9_CF = 16,          //!< 00000000 state  conflict

	STATE9_D  =  9,          //!< 00000000 state  D  == 0/1
	STATE9_ND =  6,          //!< 00000000 state  ND == 1/0
	STATE9_NO =  3,          //!< 00000011 state  on normal */
	STATE9_FL = 12,          //!< 00001100 state  on failure */
} STATE_9;


/* あまりに回数が多いのでマクロとして対応 */
#define atpg_get_state9_with_condition_2_state3( State9, Condition ) ( ( ( State9 * Condition ) / 4 ) & STATE9_NO ); 

#endif
