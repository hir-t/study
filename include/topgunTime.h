/********************************************************************************

                            TOPGUN
     All Right Reversed, Copyright (C) Yoshimura Lab. , Kyoto Sangyo University

*********************************************************************************/
/*! @file
    @brief 
	
	@author
		Masayoshi Yoshimura
	@date
		2016 / 12 / 12   initialize
*/

/*!
  @struct get_time
  @brief
*/
typedef struct get_time
{
	double	user;                              //!<
	double	system;                            //!<
	double	real;                              //!<
} GET_TIME;

/*!
  @struct process_time
  @brief
*/
typedef struct process_time
{
	GET_TIME            start;                 //!<
	GET_TIME            total;                 //!<
	Ulong               count;                 //!<
} PROCESS_TIME;


/*!
  @struct time_head
  @brief
*/
typedef struct time_head
{
	PROCESS_TIME        total;                 //!<
	PROCESS_TIME        main_initialize;       //!<
	PROCESS_TIME        atpg;                  //!< ATPG( generate + compact + sim)
	PROCESS_TIME        sim;                   //!<
	PROCESS_TIME        select;                //!<
	PROCESS_TIME        generate;              //!<
	PROCESS_TIME        gene_det;              //!<
	PROCESS_TIME        gene_redun;            //!<
	PROCESS_TIME        gene_abort;            //!<
	PROCESS_TIME        compact;               //!<
	PROCESS_TIME        random_sim;            //!<
	PROCESS_TIME        logic_sim;             //!<
	PROCESS_TIME        fault_sim;             //!<
	PROCESS_TIME        stock_sim;             //!<
	PROCESS_TIME        sim_val3;              //!<
	PROCESS_TIME        sim_val2;              //!<
	PROCESS_TIME        learn;                 //!<
	PROCESS_TIME        insert_fault;          //!<
	PROCESS_TIME        check_gene;            //!<
	PROCESS_TIME        propagate;             //!<
	PROCESS_TIME        justified;             //!<
	PROCESS_TIME        justified_spt;         //!<
	PROCESS_TIME        back_track;            //!<
	PROCESS_TIME        output;                //!<
	PROCESS_TIME        reflesh;               //!<
	PROCESS_TIME        implication;           //!<
	PROCESS_TIME        learn_imp;             //!<
	PROCESS_TIME        learn_init;            //!<
	PROCESS_TIME        learn_direct_node;     //!<
	PROCESS_TIME        learn_loop_check;      //!<
	PROCESS_TIME        learn_loop_ope;        //!<
	PROCESS_TIME        learn_loop_mk;         //!<
	PROCESS_TIME        learn_no_reach;        //!<
	PROCESS_TIME        learn_add_br;          //!<
	PROCESS_TIME        learn_search;          //!<
	PROCESS_TIME        learn_finish;          //!<
	PROCESS_TIME        learn_redo_sort;       //!<
	PROCESS_TIME        learn_dynamic_resort;  //!<
	PROCESS_TIME        learn_upflag;  //!<
	PROCESS_TIME        learn_downflag;  //!<
	PROCESS_TIME        learn_contra;  //!<
	PROCESS_TIME        learn_select;          //!<
} TIME_HEAD;

extern TIME_HEAD Time_head;

/*!
  @{
  @name
*/
void utility_get_time ( GET_TIME *);              //!<
void utility_enter_start_time( PROCESS_TIME * );  //!<
void utility_enter_end_time( PROCESS_TIME * );    //!<
void utility_process_time_init( PROCESS_TIME * ); //!<
void topgun_print_mes_process_time ( PROCESS_TIME *, char *, Ulong ); //!<
/*!
  @}
*/

/*
  level 0
  + total time

  level 1
  + initialize
  + atpg
  + output

  level 2
  + learn
  + generate
  + comaction
  + simulation

  level 3
  other

  level 4
  + check_end
*/

#ifdef TOPGUN_TIME_3

#define topgun_0_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_0_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_1_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_1_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_2_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_2_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_2_utility_enter_end_time_gene( time, gene ) utility_enter_end_time_gene( time, gene );
#define topgun_3_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_3_utility_enter_end_time( time ) utility_enter_end_time( time );

#define topgun_4_utility_enter_start_time( time ) {;}
#define topgun_4_utility_enter_end_time( time ) {;}

#else

#ifdef TOPGUN_TIME_2

#define topgun_0_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_0_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_1_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_1_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_2_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_2_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_2_utility_enter_end_time_gene( time, gene ) utility_enter_end_time_gene( time, gene );

#define topgun_3_utility_enter_start_time( time ) {;}
#define topgun_3_utility_enter_end_time( time ) {;}
#define topgun_4_utility_enter_start_time( time ) {;}
#define topgun_4_utility_enter_end_time( time ) {;}

#else

#ifdef TOPGUN_TIME_1

#define topgun_0_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_0_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_1_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_1_utility_enter_end_time( time ) utility_enter_end_time( time );

#define topgun_2_utility_enter_start_time( time ) {;} 
#define topgun_2_utility_enter_end_time( time ) {;}
#define topgun_2_utility_enter_end_time_gene( time, gene ) {;}
#define topgun_3_utility_enter_start_time( time ) {;}
#define topgun_3_utility_enter_end_time( time ) {;}
#define topgun_4_utility_enter_start_time( time ) {;}
#define topgun_4_utility_enter_end_time( time ) {;}

#else 

#ifdef TOPGUN_TIME_0

#define topgun_0_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_0_utility_enter_end_time( time ) utility_enter_end_time( time );

#define topgun_1_utility_enter_start_time( time ) {;}  
#define topgun_1_utility_enter_end_time( time ) {;} 
#define topgun_2_utility_enter_start_time( time ) {;} 
#define topgun_2_utility_enter_end_time( time ) {;}
#define topgun_2_utility_enter_end_time_gene( time, gene ) {;}
#define topgun_3_utility_enter_start_time( time ) {;}
#define topgun_3_utility_enter_end_time( time ) {;}
#define topgun_4_utility_enter_start_time( time ) {;}
#define topgun_4_utility_enter_end_time( time ) {;}

#else 

#ifdef TOPGUN_TIME_NONE

#define topgun_0_utility_enter_start_time( time ) {;}
#define topgun_0_utility_enter_end_time( time ) {;}
#define topgun_1_utility_enter_start_time( time ) {;}  
#define topgun_1_utility_enter_end_time( time ) {;} 
#define topgun_2_utility_enter_start_time( time ) {;} 
#define topgun_2_utility_enter_end_time( time ) {;}
#define topgun_2_utility_enter_end_time_gene( time, gene ) {;}
#define topgun_3_utility_enter_start_time( time ) {;}
#define topgun_3_utility_enter_end_time( time ) {;}
#define topgun_4_utility_enter_start_time( time ) {;}
#define topgun_4_utility_enter_end_time( time ) {;}

#else

/*  */

#define topgun_0_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_0_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_1_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_1_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_2_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_2_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_2_utility_enter_end_time_gene( time, gene ) utility_enter_end_time_gene( time, gene );
#define topgun_3_utility_enter_start_time( time )  utility_enter_start_time( time );
#define topgun_3_utility_enter_end_time( time ) utility_enter_end_time( time );
#define topgun_4_utility_enter_start_time( time ) utility_enter_start_time( time );
#define topgun_4_utility_enter_end_time( time ) utility_enter_end_time( time );

#endif /* TOPGUN_TIME_NONE */
#endif /* TOPGUN_TIME_0 */
#endif /* TOPGUN_TIME_1 */
#endif /* TOPGUN_TIME_2 */
#endif /* TOPGUN_TIME_3 */
