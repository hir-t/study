#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <topgun.h>
#include <topgunLine.h>
void makeCnf(FILE *out_fp,LINE *data[],LINE *r[],Ulong c1,Ulong c2,Ulong variables,Ulong clauses){
	extern LINE_INFO Line_info;
	int count = 0;
	int g = 0;
	fprintf(out_fp,"p cnf %lu %lu\n",variables,clauses);
	for(int i = 0;i<Line_info.n_line;i++){
		//printf("%d.ID:%lu,入力レベル:%lu,TYPE:%u,n_in:%lu,n_out:%lu\n", i,data[i]->line_id,data[i]->level,data[i]->type,data[i]->n_in,data[i]->n_out);

		//if(data[i]->line_id == routeNode[NM]->line_id){	//取得したIDがループ経路のとき
		if(data[i]->rtflg == 1){	//取得したIDがループ経路のとき
			//printf("route\n");

			//入力数が1のとき
			//input:a output:z
			if(data[i]->n_in == 1){
				if(data[i]->type==3){																			//INVゲートの時
					//printf("1-inv\n");
					fprintf(out_fp,"%lu %lu 0\n"  ,data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//z + a
					fprintf(out_fp,"-%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//-z+ -a
				}
				else if(data[i]->type==4){																		//BUFゲートの時
					//printf("1-buf\n");
					fprintf(out_fp,"-%lu %lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//-z + a
					fprintf(out_fp,"%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);		//z+ -a
				}
			}

			//入力数が2のとき
			//intput1:a,input2:b,output:z
			else if(data[i]->n_in == 2){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("2-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,														//z + -a + -b
														data[i]->in[0]->line_id+1 ,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("2-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//z + a
					fprintf(out_fp,"%lu %lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,														//-z + -a + -b
														data[i]->in[0]->line_id+1 ,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("2-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", 		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,														//-z + a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("2-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", 	data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n",		data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,														//z + a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("2-xor\n");
					fprintf(out_fp,"%lu -%lu %lu 0\n", 	data[i]->out[0]->line_id+1,													   	//z + -a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu 0\n", 	data[i]->out[0]->line_id+1,  												 	//z + a + -b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,													 	//-z + -a + -b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,													 	//-z + a + b
														data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("2-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//-z + -a + b
																				   data[i]->in[1]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						   	//z + a + -b
																				   data[i]->in[1]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						 	//z + -a + -b
																				   data[i]->in[1]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu 0\n", 	data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b
																				   data[i]->in[1]->line_id+1);
				}
			}

			//入力数が3のとき
			//intput1:a,input2:b,input3:c,output:z
			else if(data[i]->n_in == 3){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("3-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + -a + -b + -c
														data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("3-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + -a + -b + -c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("3-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + a + b + +c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("3-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//z + a + b + c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("3-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + -a + b + c
					fprintf(out_fp,"%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + a + -b + c
					fprintf(out_fp,"%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + a + b + -c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//z + -a + -b + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + a + b + c
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("3-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + -a + b + c
					fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + a + -b + c
					fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//-z + a + b + -c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//-z + -a + -b + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   					//z + a + b + c
				}
			}

			//入力数が4のとき
			//intput:a,b,c,d
			//output:z
			else if(data[i]->n_in == 4){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("4-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//z + -a + -b + -c + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("4-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + -a + -b + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("4-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("4-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("4-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 //-z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("4-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   				//-z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				  	//-z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   				//-z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   				//-z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}
			}

			//入力数が5のとき
			//intput:a,b,c,d,e
			//output:z
			else if(data[i]->n_in == 5){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("5-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,			//z + -a + -b + -c + -d + -e
							data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("5-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + -a + -b + -d
							data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("5-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d + e
							data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("5-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//-z + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + a + b + c + d + e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==9){																											//XORゲートの時
					//printf("5-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + -a + b + c + d + e
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + -b + c + d + e
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + -c + d + e
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + -d + e
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + -e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//z + -a + -b + -c + -d + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + e
				}

				else if(data[i]->type==10){																											//XNORゲートの時
					//printf("5-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + -a + b + c + d + e
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + -b + c + d + e
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + -c + d + e
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + -d + e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + -e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//-z + -a + -b + -c + -d + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + e
				}
			}

			//入力数が6のとき
			//intput:a,b,c,d,e,f
			//output:z
			else if(data[i]->n_in == 6){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("6-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("6-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,			//-z + -a + -b + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("6-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("6-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==9){																												//XORゲートの時
					//printf("6-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==10){																												//XNORゲートの時
					//printf("6-xnor\n");
					fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//-z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					 	//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}
			}

			//入力数が7のとき
			//intput:a,b,c,d,e,f,g
			//output:z
			else if(data[i]->n_in == 7){
				if(data[i]->type==5){																									//ANDゲートの時
				//	printf("7-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//-z + g
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("7-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 									//z + g
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//-z + -a + -b + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("7-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);									//z + -g
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("7-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//-z + -g
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("7-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("7-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//-z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//-z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//-z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//-z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//-z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//-z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//-z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//-z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}
			}

			//入力数が8のとき
			//intput:a,b,c,d,e,f,g,h
			//output:z
			else if(data[i]->n_in == 8){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("8-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("8-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",											//-z + -a + -b + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("8-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("8-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("8-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 													//z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id,data[i]->in[0]->line_id,data[i]->in[1]->line_id,
									data[i]->in[2]->line_id,data[i]->in[3]->line_id,data[i]->in[4]->line_id,
									data[i]->in[5]->line_id,data[i]->in[6]->line_id,data[i]->in[7]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  						//z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  						//z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//-z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("8-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//-z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//-z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//-z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//-z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  					//-z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  					//-z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//-z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}
			}

			//入力数が9のとき
			//intput:a,b,c,d,e,f,g,h,i
			//output:z
			else if(data[i]->n_in == 9){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("9-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 							//-z + i
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("9-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 								//z + i
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										//-z + -a + -b + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("9-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);								//z + -i
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("9-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);							//-z + -i
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("9-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",											   	//z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("9-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//-z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//-z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 									//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}
			}

			/*** 折り返し地点から開始地点へフェードバック(muxを挟む) ***/
			//*経路の折り返し地点の時
			//*フィードバックのためのMUXを追加
			if(data[i]->endflg == 1)
			{
				//printf("mux-end\n");
				/*printf("start:%lu\n",start[N]->line_id);
				printf("end:%lu\n",end[N]->line_id);
				printf("in1:%lu",start[N]->in[0]->line_id);
				//printf("in2:%lu",data[i]->in[1]->line_id);
				printf("out1:%lu\n",end[N]->out[0]->line_id);*/
				//fprintf(out_fp,"mux\n");
				fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,data[i]->st->in[0]->line_id+1);		//-z + a + b
				fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);									//-z + a + s
				fprintf(out_fp,"-%lu %lu -%lu 0\n", c2,data[i]->st->in[0]->line_id+1,c1);								//-z + b + -s
				fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->st->in[0]->line_id+1,c1);								//z + -b + -s
				fprintf(out_fp,"%lu -%lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);									//z + -a + s
				fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->out[0]->line_id+1,data[i]->st->in[0]->line_id+1);		//z + -a + -b
				//printf("c1:%lu,c2:%lu\n", c1,c2);
				c1+=2;
				c2+=2;
			}
			//経路内の折り返し地点以外の時
			//経路を開くためのMUXを追加する
			else
			{//bはr[]
				//ファンアウト数が1のときMUXを2つ追加
				if(data[i]->n_out == 1){
					//printf("2-muxes\n");
					for (int j = 0; j < 2; j++)
					{
						//fprintf(out_fp,"mux\n");
						//printf("St.in1:%lu",start[N]->in[0]->line_id);
						//printf("in2:%lu",data[i]->in[1]->line_id);
						//printf("%d\n", j);
						//printf("rand[%d]ID:%lu,outID%lu\n",g,r[g]->line_id,r[g]->out[0]->line_id);
						//printf("rand[%d]ID:%lu,TYPE:%u\n",g,r[g]->line_id,r[g]->type);
						//printf("outID:%lu\n",r[g]->out[0]->line_id);
						fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//-z + a + b
						fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);						//-z + a + s
						fprintf(out_fp,"-%lu %lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);							//-z + b + -s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);							//z + -b + -s
						fprintf(out_fp,"%lu -%lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);						//z + -a + s
						fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);	//z + -a + -b
						//printf("c1:%lu,c2:%lu\n", c1,c2);
						g++;
						c2+=2;
					}
					c1+=2;	//追加した2つのMUXはキービットsを共有するため、ここでカウントする
				}
				//ファンアウト数が1以上の時MUXを1つ追加+1
				else if(data[i]->n_out > 1){
					//printf("1-mux\n");
					//fprintf(out_fp,"mux\n");
					//printf("rand[%d]ID:%lu,outID%lu\n",g,r[g]->line_id,r[g]->out[0]->line_id);
					//printf("rand[%d]ID:%lu,TYPE:%u\n",g,r[g]->line_id,r[g]->type);
					//printf("outID:%lu\n",r[g]->out[0]->line_id);
					fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);		//-z + a + b
					fprintf(out_fp,"-%lu %lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);							//-z + a + s
					fprintf(out_fp,"-%lu %lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);								//-z + b + -s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,r[g]->out[0]->line_id+1,c1);								//z + -b + -s
					fprintf(out_fp,"%lu -%lu %lu 0\n", c2,data[i]->out[0]->line_id+1,c1);							//z + -a + s
					fprintf(out_fp,"%lu -%lu -%lu 0\n", c2,data[i]->out[0]->line_id+1,r[g]->out[0]->line_id+1);		//z + -a + -b
					//printf("c1:%lu,c2:%lu\n", c1,c2);
					g++;
					c1+=2;
					c2+=2;
				}
			}
			count++;
				/*
				* end[N]->out[0]->line_id	:	muxの入力a
				* start[N]->in[0]->line_id	:	muxの入力b
				* c1						:	muxの入力s
				* start[N]->out[0]->line_id	:	muzの出力z(c2にするかも？)
				*/

		}
		else{																								//経路でない時
			//入力数が1のとき
			//input:a output:z
			//printf("nomal\n");
			if(data[i]->n_in == 1){
				if(data[i]->type==3){																		//INVゲートの時
					//printf("inv\n");
					fprintf(out_fp,"%lu %lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);		//a + z
					fprintf(out_fp,"-%lu -%lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);	//-a+ -z
				}
				else if(data[i]->type==4){																		//BUFゲートの時
					//printf("buf\n");
					fprintf(out_fp,"-%lu %lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);	//-a + z
					fprintf(out_fp,"%lu -%lu 0\n",data[i]->in[0]->line_id+1,data[i]->out[0]->line_id+1);	//a+ -z
				}
			}

			//入力数が2のとき
			//intput1:a,input2:b,output:z
			else if(data[i]->n_in == 2){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("2-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);		//z + -a + -b
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("2-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);	//-z + -a + -b
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("2-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);		//-z + a + b
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("2-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);		//z + a + b
				}

				else if(data[i]->type==9){																									//XORゲートの時
					//printf("2-xor\n");
					fprintf(out_fp,"%lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + -a + b
					fprintf(out_fp,"%lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"-%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1); 	//-z + -a + -b
					fprintf(out_fp,"-%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//-z + a + b
				}

				else if(data[i]->type==10){																									//XNORゲートの時
					//printf("2-xor\n");
					fprintf(out_fp,"-%lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//-z + -a + b
					fprintf(out_fp,"-%lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + a + -b
					fprintf(out_fp,"%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1); 	//z + -a + -b
					fprintf(out_fp,"%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1);   	//z + a + b
				}
			}

			//入力数が3のとき
			//intput1:a,input2:b,input3:c,output:z
			else if(data[i]->n_in == 3){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("3-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + -a + -b + -c
														data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("3-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + -a + -b + -c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("3-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//-z + a + b + +c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("3-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,							//z + a + b + c
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);
				}

				else if(data[i]->type==9){																									//XORゲートの時
					//printf("3-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + -a + b + c
					fprintf(out_fp,"%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + a + -b + c
					fprintf(out_fp,"%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + a + b + -c
					fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//z + -a + -b + -c
					fprintf(out_fp,"-%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + a + b + c
				}

				else if(data[i]->type==10){																									//XNORゲートの時
					//printf("3-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + -a + b + c
					fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + a + -b + c
					fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//-z + a + b + -c
					fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1); 						//-z + -a + -b + -c
					fprintf(out_fp,"%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
															data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1);   						//z + a + b + c
				}
			}

			//入力数が4のとき
			//intput:a,b,c,d
			//output:z
			else if(data[i]->n_in == 4){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("4-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//z + -a + -b + -c + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("4-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + -a + -b + -d
													data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("4-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//-z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("4-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,						//z + a + b + c + d
												data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==9){																									//XORゲートの時
					//printf("4-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   	//z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}

				else if(data[i]->type==10){																									//XNORゲートの時
					//printf("4-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + -a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   	//-z + a + -b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + -c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//-z + a + b + c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//z + -a + -b + -c + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   					//z + a + b + c + d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1);
				}
			}

			//入力数が5のとき
			//intput:a,b,c,d,e
			//output:z
			else if(data[i]->n_in == 5){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("5-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("5-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + -a + -b + -d
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("5-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d + e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("5-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					//-z + a + b + c + d + e
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);
				}

				else if(data[i]->type==9){																										//XORゲートの時
					//printf("5-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + -a + b + c + d + e
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + -b + c + d + e
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + -c + d + e
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + -d + e
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + -e
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//z + -a + -b + -c + -d + -e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + e
				}

				else if(data[i]->type==10){																										//XNORゲートの時
					//printf("5-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + -a + b + c + d + e
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + -b + c + d + e
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + -c + d + e
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + -d + e
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//-z + a + b + c + d + -e
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1); 		//-z + -a + -b + -c + -d + -e
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1);   	//z + a + b + c + d + e
				}
			}

			//入力数が6のとき
			//intput:a,b,c,d,e,f
			//output:z
			else if(data[i]->n_in == 6){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("6-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("6-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,			//-z + -a + -b + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("6-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("6-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==9){																												//XORゲートの時
					//printf("6-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}

				else if(data[i]->type==10){																												//XNORゲートの時
					//printf("6-xnor\n");
					fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + -a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					   		//-z + a + -b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + -c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + -d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + -e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//-z + a + b + c + d + e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,					 	//z + -a + -b + -c + -d + -e + -f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,   						//z + a + b + c + d + e + f
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1);
				}
			}

			//入力数が7のとき
			//intput:a,b,c,d,e,f,g
			//output:z
			else if(data[i]->n_in == 7){
				if(data[i]->type==5){																									//ANDゲートの時
					//printf("7-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 								//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 								//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//-z + g
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",														//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==6){																									//NANDゲートの時
					//printf("7-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 									//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 									//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 									//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 									//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 									//z + g
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//-z + -a + -b + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==7){																									//ORゲートの時
					//printf("7-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);									//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);									//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);									//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);									//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);									//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);									//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);									//z + -g
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==8){																									//NORゲートの時
					//printf("7-or\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//-z + -g
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("7-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//-z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("7-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//-z + -a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//-z + a + -b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//-z + a + b + -c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//-z + a + b + c + -d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//-z + a + b + c + d + -e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//-z + a + b + c + d + e + -f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//-z + a + b + c + d + e + f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//-z + -a + -b + -c + -d + -e + -f + -g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//z + a + b + c + d + e + f + g
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,
									data[i]->in[1]->line_id+1,data[i]->in[2]->line_id+1,
									data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1);
				}
			}

			//入力数が8のとき
			//intput:a,b,c,d,e,f,g,h
			//output:z
			else if(data[i]->n_in == 8){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("8-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("8-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",											//-z + -a + -b + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("8-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("8-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("8-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 													//z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id,data[i]->in[0]->line_id,data[i]->in[1]->line_id,
									data[i]->in[2]->line_id,data[i]->in[3]->line_id,data[i]->in[4]->line_id,
									data[i]->in[5]->line_id,data[i]->in[6]->line_id,data[i]->in[7]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  						//z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  						//z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//-z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//-z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}

				else if(data[i]->type==10){																								//XNORゲートの時
					//printf("8-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//-z + -a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//-z + a + -b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + -c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//-z + a + b + c + -d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//-z + a + b + c + d + -e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  					//-z + a + b + c + d + e + -f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  					//-z + a + b + c + d + e + f + -g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//-z + a + b + c + d + e + f + g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//z + -a + -b + -c + -d + -e + -f + -g + -h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//z + a + b + c + d + e + f + g + h
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1);
				}
			}

			//入力数が9のとき
			//intput:a,b,c,d,e,f,g,h,i
			//output:z
			else if(data[i]->n_in == 9){
				if(data[i]->type==5){																								//ANDゲートの時
					//printf("9-and\n");
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1); 							//-z + a
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1); 							//-z + b
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 							//-z + c
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 							//-z + d
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 							//-z + e
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 							//-z + f
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 							//-z + g
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 							//-z + h
					fprintf(out_fp,"-%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 							//-z + i
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==6){																								//NANDゲートの時
					//printf("9-nand\n");
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + a
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + b
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1); 								//z + c
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1); 								//z + d
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1); 								//z + e
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1); 								//z + f
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1); 								//z + g
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1); 								//z + h
					fprintf(out_fp,"%lu %lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1); 								//z + i
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										//-z + -a + -b + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==7){																								//ORゲートの時
					//printf("9-or\n");
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);								//z + -a
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);								//z + -b
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);								//z + -c
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);								//z + -d
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);								//z + -e
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);								//z + -f
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);								//z + -g
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);								//z + -h
					fprintf(out_fp,"%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);								//z + -i
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==8){																								//NORゲートの時
					//printf("9-nor\n");
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1);							//-z + -a
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[1]->line_id+1);							//-z + -b
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[2]->line_id+1);							//-z + -c
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[3]->line_id+1);							//-z + -d
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[4]->line_id+1);							//-z + -e
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[5]->line_id+1);							//-z + -f
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[6]->line_id+1);							//-z + -g
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[7]->line_id+1);							//-z + -h
					fprintf(out_fp,"-%lu -%lu 0\n", data[i]->out[0]->line_id+1,data[i]->in[8]->line_id+1);							//-z + -i
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==9){																								//XORゲートの時
					//printf("9-xor\n");
					fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",											   	//z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id);
					fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}

				else if(data[i]->type==10){																						//XNORゲートの時
					//printf("9-xnor\n");
					fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + -a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + a + -b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + -c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + -d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + -e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//-z + a + b + c + d + e + -f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + -g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + -h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//-z + a + b + c + d + e + f + g + h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 									//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
					fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + g + h + i
									data[i]->out[0]->line_id+1,data[i]->in[0]->line_id+1,data[i]->in[1]->line_id+1,
									data[i]->in[2]->line_id+1,data[i]->in[3]->line_id+1,data[i]->in[4]->line_id+1,
									data[i]->in[5]->line_id+1,data[i]->in[6]->line_id+1,data[i]->in[7]->line_id+1,
									data[i]->in[8]->line_id+1);
				}
			}
		}
	}
	printf("count:%d\n",count);
	//printf("m_node:%d\n",m_node);
	//printf("m_num:%d\n",m_num);
	printf("g:%d\n",g);
	//printf("m_edge:%lu\n",m_edge);
	printf("size:%lu\n",Line_info.n_line);
}
