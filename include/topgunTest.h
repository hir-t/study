/* test関数に対するマクロ */

#ifndef TOPGUN_TEST

#define topgun_test_condition( condition, func_name ) test_condition ( conditon, func_name );
#define topgun_test_state3( state_3, func_name ) test_state3 ( state_3, func_name );
#define topgun_test_state_normal( stat, func_name ) test_state_normal( stat, func_name );
#define topgun_test_state9_n_f_x( state9, func_name ) test_state9_n_f_x( state9, func_name );
#define topgun_test_line_null( line, func_name ) test_line_null( line, func_name );
#define topgun_test_line_value( ulong, func_name ) test_line_value( ulong, func_name );
#define topgun_test_node_children ( learn_node , char ) test_node_children ( learn_node , char );

#else

#define topgun_test_condition( condition, func_name ) {;}
#define topgun_test_state3( state_3, func_name ) {;}
#define topgun_test_state_normal( stat, func_name ) {;}
#define topgun_test_state9_n_f_x( state9, func_name ) {;}
#define topgun_test_line_null( line, func_name ) {;}
#define topgun_test_line_value( ulong, func_name ) {;}
#define topgun_test_node_children( learn_node , char ) {;}

#endif /* TOPGUN_TEST */
