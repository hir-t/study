/* 元の回路(Eval)をcnfに変換する */
#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>
int countClauses(void);

/* topgun_output.c */
FILE *topgun_open ( char *, char *, char * ); /* fileを開く */

/* topgunCell.c */
void topgun_close( FILE *, char * );


void eval( char *benchName ){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	//LINE *Logic_level[Line_info.n_line];
	LINE *line;

	/* 出力ファイル設定 */
	FILE *out_fp; //出力ファイル・ポインタ
	FILE *fpCNF;
    FILE *fpCNFSTART;	//1を入れる
    FILE *fpCNFEND;		//
    FILE *fpPIINFO;		//外部入力をまとめる
    FILE *fpPOINFO;		//外部出力をまとめる

    char outputCNFFileName[100];
    char outputCNFStartFileName[100];
    char outputCNFEndFileName[100];
    char outputPIInfoFileName[100];
    char outputPOInfoFileName[100];

    char *funcName = "eval"; // 関数名 

    //printf("Open file %s\n", benchName);

    sprintf(outputCNFFileName, "%s.cnf", benchName);
    sprintf(outputCNFStartFileName, "%s.cnf.start", benchName);
    sprintf(outputCNFEndFileName, "%s.cnf.end", benchName);
    sprintf(outputPIInfoFileName, "%s.piCnfInfo", benchName);
    sprintf(outputPOInfoFileName, "%s.poCnfInfo", benchName);

    out_fp = fpCNF = topgun_open( outputCNFFileName, "w", funcName );
    fpCNFSTART = topgun_open( outputCNFStartFileName, "w", funcName );
    fpCNFEND = topgun_open( outputCNFEndFileName, "w", funcName );
    fpPIINFO = topgun_open( outputPIInfoFileName, "w", funcName );
    fpPOINFO = topgun_open( outputPOInfoFileName, "w", funcName );

	/*char out_fn[128]; //出力ファイル名を入れるchar型配列
	char filename[128];
	FILE *out_fp; //出力ファイル・ポインタ
	printf("Input the name of Eval output file name 'filename'_ev.cnf: ");
	scanf("%s",out_fn);

	sprintf(filename,"%s_ev.cnf",out_fn);
	  out_fp = fopen(filename,"w");
	  if(out_fp == NULL){
	    printf("ERROR\n");
	    exit(1);
	  }*/
	int gate = 0;								//回路内のゲート数
	LINE *data[Line_info.n_line];
	 	/*	ID順にレベルやタイプを取得 */
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		if(2<line->type && line->type<11){		//回路内のゲートの数を数える
			gate++;
		}
		data[i] = line;
		data[i]->level  = line->lv_pi; //ここをポインタに
		data[i]->line_id  = line->line_id;
		data[i]->type = line->type;
		//printf("ID:%lu,入力レベル:%lu,TYPE:%u\n", data[i]->line_id,data[i]->level,data[i]->type);
	}

	Ulong variables = Line_info.n_line;			//命題変数の数(信号線数)
	Ulong clauses = countClauses();				//節数(cnfファイルの行数)
	fprintf(out_fp,"p cnf %lu %lu\n",variables,clauses);
	fprintf(fpCNFSTART,"1\n");
    topgun_close(fpCNFSTART, funcName);

	/*	ID順にレベルやタイプを取得 */
	for(int cnt1 = 0;cnt1<Line_info.n_line;cnt1++){
		//printf("%d.ID:%lu,入力レベル:%lu,TYPE:%u,n_in:%lu,n_out:%lu\n", i,data[i]->line_id,data[i]->level,data[i]->type,data[i]->n_in,data[i]->n_out);
		//入力数が1のとき
		//input:a output:z
		if(data[cnt1]->n_in == 1){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;

			if(data[cnt1]->type==1){						//外部出力の時
				//printf("1-po\n");
				fprintf(out_fp,"-%lu %lu 0\n",z,a);			//-z + a
				fprintf(out_fp,"%lu -%lu 0\n",z,a);			//z+ -a
			}
			else if(data[cnt1]->type==2){						//Branchの時
				//printf("1-buf\n");
				fprintf(out_fp,"-%lu %lu 0\n",z,a);			//-z + a
				fprintf(out_fp,"%lu -%lu 0\n",z,a);			//z+ -a
			}
			else if(data[cnt1]->type==3){							//INVゲートの時
				//printf("1-inv\n");
				fprintf(out_fp,"%lu %lu 0\n"  ,z,a);		//z + a
				fprintf(out_fp,"-%lu -%lu 0\n",z,a);		//-z+ -a
			}
			else if(data[cnt1]->type==4){						//BUFゲートの時
				//printf("1-buf\n");
				fprintf(out_fp,"-%lu %lu 0\n",z,a);			//-z + a
				fprintf(out_fp,"%lu -%lu 0\n",z,a);			//z+ -a
			}
		}

		//入力数が2のとき
		//intput1:a,input2:b,output:z
		else if(data[cnt1]->n_in == 2){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;
			//入力にMUXの出力がないか(直前にMUXを追加していないか)確認
			//あった場合、元々の入力はMUXの入力に使用されている
			//そのため、その入力線をMUXの出力線に更新

			if(data[cnt1]->type==5){										//ANDゲートの時
				//printf("2-and\n");
				fprintf(out_fp,"-%lu %lu 0\n", 		z,a); 					//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", 		z,b); 					//-z + b
				fprintf(out_fp,"%lu -%lu -%lu 0\n", z,a,b);					//z + -a + -b
			}

			else if(data[cnt1]->type==6){									//NANDゲートの時
				//printf("2-nand\n");
				fprintf(out_fp,"%lu %lu 0\n", 		z,a);					//z + a
				fprintf(out_fp,"%lu %lu 0\n", 		z,b);					//z + b
				fprintf(out_fp,"-%lu -%lu -%lu 0\n",z,a ,b);				//-z + -a + -b
			}

			else if(data[cnt1]->type==7){									//ORゲートの時
				//printf("2-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", 		z,a);					//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", 		z,b);					//z + -b
				fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,b);					//-z + a + b
			}

			else if(data[cnt1]->type==8){									//NORゲートの時
				//printf("2-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", 	z,a);					//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n",		z,b);					//-z + -b
				fprintf(out_fp,"%lu %lu %lu 0\n", 	z,a,b);					//z + a + b
			}

			else if(data[cnt1]->type==9){									//XORゲートの時
				//printf("2-xor\n");
				fprintf(out_fp,"%lu -%lu %lu 0\n", 	z,a,b);					//z + -a + b
				fprintf(out_fp,"%lu %lu -%lu 0\n", 	z,a,b); 				//z + a + -b
				fprintf(out_fp,"-%lu -%lu -%lu 0\n",z,a,b);					//-z + -a + -b
				fprintf(out_fp,"-%lu %lu %lu 0\n", 	z,a,b);					//-z + a + b
			}

			else if(data[cnt1]->type==10){									//XNORゲートの時
				//printf("2-xnor\n");
				fprintf(out_fp,"-%lu -%lu %lu 0\n", z,a,b);					//-z + -a + b
				fprintf(out_fp,"-%lu %lu -%lu 0\n", z,a,b);				   	//z + a + -b
				fprintf(out_fp,"%lu -%lu -%lu 0\n", z,a,b);					//z + -a + -b
				fprintf(out_fp,"%lu %lu %lu 0\n", 	z,a,b);   				//z + a + b
			}
		}

		//入力数が3のとき
		//intput1:a,input2:b,input3:c,output:z
		else if(data[cnt1]->n_in == 3){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong c = data[cnt1]->in[2]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;

			if(data[cnt1]->type==5){													//ANDゲートの時
				//printf("3-and\n");
				fprintf(out_fp,"-%lu %lu 0\n", z,a); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", z,b); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", z,c); 								//-z + c
				fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", z,a,b,c);					//z + -a + -b + -c
			}

			else if(data[cnt1]->type==6){												//NANDゲートの時
				//printf("3-nand\n");
				fprintf(out_fp,"%lu %lu 0\n", z,a);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", z,b);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", z,c); 								//z + c
				fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", z,a,b,c);					//-z + -a + -b + -c
			}

			else if(data[cnt1]->type==7){												//ORゲートの時
				//printf("3-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", z,a);								//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", z,b);								//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", z,c);								//z + -c
				fprintf(out_fp,"-%lu %lu %lu %lu 0\n", z,a,b,c);					//-z + a + b + +c
			}

			else if(data[cnt1]->type==8){												//NORゲートの時
				//printf("3-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", z,a);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", z,b);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", z,c);								//-z + -c
				fprintf(out_fp,"%lu %lu %lu %lu 0\n", z,a,b,c);						//z + a + b + c
			}

			else if(data[cnt1]->type==9){												//XORゲートの時
				//printf("3-xor\n");
				fprintf(out_fp,"%lu -%lu %lu %lu 0\n", z,a,b,c);   					//z + -a + b + c
				fprintf(out_fp,"%lu %lu -%lu %lu 0\n", z,a,b,c);   					//z + a + -b + c
				fprintf(out_fp,"%lu %lu %lu -%lu 0\n", z,a,b,c);   					//z + a + b + -c
				fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", z,a,b,c); 					//z + -a + -b + -c
				fprintf(out_fp,"-%lu %lu %lu %lu 0\n", z,a,b,c);   					//-z + a + b + c
			}

			else if(data[cnt1]->type==10){												//XNORゲートの時
				//printf("3-xnor\n");
				fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", z,a,b,c);   				//-z + -a + b + c
				fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", z,a,b,c);   				//-z + a + -b + c
				fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", z,a,b,c);   				//-z + a + b + -c
				fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", z,a,b,c); 				//-z + -a + -b + -c
				fprintf(out_fp,"%lu %lu %lu %lu 0\n", z,a,b,c);   					//z + a + b + c
			}
		}
		//入力数が4のとき
		//intput:a,b,c,d
		//output:z
		else if(data[cnt1]->n_in == 4){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong c = data[cnt1]->in[2]->line_id+1;
			Ulong d = data[cnt1]->in[3]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;

			if(data[cnt1]->type==5){													//ANDゲートの時
				//printf("4-and\n");
				fprintf(out_fp,"-%lu %lu 0\n", z,a); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", z,b); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", z,c); 								//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", z,d); 								//-z + d
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d);			//z + -a + -b + -c + -d
			}

			else if(data[cnt1]->type==6){												//NANDゲートの時
				//printf("4-nand\n");
				fprintf(out_fp,"%lu %lu 0\n", z,a);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", z,b);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", z,c); 								//z + c
				fprintf(out_fp,"%lu %lu 0\n", z,d); 								//z + d
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d);			//-z + -a + -b + -c + -d
			}

			else if(data[cnt1]->type==7){												//ORゲートの時
				//printf("4-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", z,a);								//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", z,b);								//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", z,c);								//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", z,d);								//z + -d
				fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", z,a,b,c,d);				//-z + a + b + c + d
			}

			else if(data[cnt1]->type==8){												//NORゲートの時
				//printf("4-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", z,a);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", z,b);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", z,c);								//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", z,d);								//-z + -d
				fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n",z,a,b,c,d);				//z + a + b + c + d
			}

			else if(data[cnt1]->type==9){												//XORゲートの時
				//printf("4-xor\n");
				fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n",z,a,b,c,d);   			//z + -a + b + c + d
				fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n",z,a,b,c,d); 				//z + a + -b + c + d
				fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n",z,a,b,c,d);    			//z + a + b + -c + d
				fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n",z,a,b,c,d);    			//z + a + b + c + -d
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d);			//-z + -a + -b + -c + -d
				fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n",z,a,b,c,d); 				//-z + a + b + c + d
			}

			else if(data[cnt1]->type==10){													//XNORゲートの時
				//printf("4-xnor\n");
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n",z,a,b,c,d);   				//-z + -a + b + c + d
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n",z,a,b,c,d);				  	//-z + a + -b + c + d
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n",z,a,b,c,d);   				//-z + a + b + -c + d
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n",z,a,b,c,d);   				//-z + a + b + c + -d
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d);				//z + -a + -b + -c + -d
				fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", z,a,b,c,d);  					//z + a + b + c + d
			}
		}

		//入力数が5のとき
		//intput:a,b,c,d,e
		//output:z
		else if(data[cnt1]->n_in == 5){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong c = data[cnt1]->in[2]->line_id+1;
			Ulong d = data[cnt1]->in[3]->line_id+1;
			Ulong e = data[cnt1]->in[4]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;

			if(data[cnt1]->type==5){														//ANDゲートの時
				//printf("5-and\n");
				fprintf(out_fp,"-%lu %lu 0\n", z,a); 									//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", z,b); 									//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", z,c); 									//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", z,d); 									//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", z,e); 									//-z + e
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e);		//z + -a + -b + -c + -d + -e
			}

			else if(data[cnt1]->type==6){													//NANDゲートの時
				//printf("5-nand\n");
				fprintf(out_fp,"%lu %lu 0\n", z,a);										//z + a
				fprintf(out_fp,"%lu %lu 0\n", z,b);										//z + b
				fprintf(out_fp,"%lu %lu 0\n", z,c); 									//z + c
				fprintf(out_fp,"%lu %lu 0\n", z,d); 									//z + d
				fprintf(out_fp,"%lu %lu 0\n", z,e); 									//z + e
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e);		//-z + -a + -b + -d
			}

			else if(data[cnt1]->type==7){													//ORゲートの時
				//printf("5-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", z,a);									//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", z,b);									//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", z,c);									//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", z,d);									//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", z,e);									//z + -e
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e);			//-z + a + b + c + d + e
			}

			else if(data[cnt1]->type==8){													//NORゲートの時
				//printf("5-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", z,a);									//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", z,b);									//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", z,c);									//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", z,d);									//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", z,e);									//-z + -e
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e);				//-z + a + b + c + d + e
			}

			else if(data[cnt1]->type==9){													//XORゲートの時
				//printf("5-xor\n");
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e);   			//z + -a + b + c + d + e
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", z,a,b,c,d,e);   			//z + a + -b + c + d + e
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e);   			//z + a + b + -c + d + e
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", z,a,b,c,d,e);   			//z + a + b + c + -d + e
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", z,a,b,c,d,e);   			//z + a + b + c + d + -e
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e);		//z + -a + -b + -c + -d + -e
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e);   			//-z + a + b + c + d + e
			}

			else if(data[cnt1]->type==10){													//XNORゲートの時
				//printf("5-xnor\n");
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e);   		//-z + -a + b + c + d + e
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", z,a,b,c,d,e);   		//-z + a + -b + c + d + e
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e);   		//-z + a + b + -c + d + e
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", z,a,b,c,d,e);   		//-z + a + b + c + -d + e
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", z,a,b,c,d,e);   		//-z + a + b + c + d + -e
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e);		//-z + -a + -b + -c + -d + -e
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e);   			//z + a + b + c + d + e
			}
		}

		//入力数が6のとき
		//intput:a,b,c,d,e,f
		//output:z
		else if(data[cnt1]->n_in == 6){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong c = data[cnt1]->in[2]->line_id+1;
			Ulong d = data[cnt1]->in[3]->line_id+1;
			Ulong e = data[cnt1]->in[4]->line_id+1;
			Ulong f = data[cnt1]->in[5]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;

			if(data[cnt1]->type==5){														//ANDゲートの時
				//printf("6-and\n");
				fprintf(out_fp,"-%lu %lu 0\n",z,a); 									//-z + a
				fprintf(out_fp,"-%lu %lu 0\n",z,b); 									//-z + b
				fprintf(out_fp,"-%lu %lu 0\n",z,c); 									//-z + c
				fprintf(out_fp,"-%lu %lu 0\n",z,d); 									//-z + d
				fprintf(out_fp,"-%lu %lu 0\n",z,e); 									//-z + e
				fprintf(out_fp,"-%lu %lu 0\n",z,f); 									//-z + f
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f);	//z + -a + -b + -c + -d + -e + -f
			}

			else if(data[cnt1]->type==6){													//NANDゲートの時
				//printf("6-nand\n");
				fprintf(out_fp,"%lu %lu 0\n",z,a);										//z + a
				fprintf(out_fp,"%lu %lu 0\n",z,b);										//z + b
				fprintf(out_fp,"%lu %lu 0\n",z,c); 										//z + c
				fprintf(out_fp,"%lu %lu 0\n",z,d); 										//z + d
				fprintf(out_fp,"%lu %lu 0\n",z,e); 										//z + e
				fprintf(out_fp,"%lu %lu 0\n", z,f); 									//z + f
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f);	//-z + -a + -b + -d + -e + -f
			}

			else if(data[cnt1]->type==7){													//ORゲートの時
				//printf("6-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", z,a);							//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", z,b);							//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", z,c);							//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", z,d);							//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", z,e);							//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", z,f);							//z + -f
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f); 		//-z + a + b + c + d + e + f
			}

			else if(data[cnt1]->type==8){													//NORゲートの時
				//printf("6-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", z,a);							//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", z,b);							//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", z,c);							//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", z,d);							//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", z,e);							//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", z,f);							//-z + -f
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);		//z + a + b + c + d + e + f
			}

			else if(data[cnt1]->type==9){																		//XORゲートの時
				//printf("6-xor\n");
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);   						//z + -a + b + c + d + e + f
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);					   		//z + a + -b + c + d + e + f
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);   						//z + a + b + -c + d + e + f
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e,f);   						//z + a + b + c + -d + e + f
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", z,a,b,c,d,e,f);   						//z + a + b + c + d + -e + f
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", z,a,b,c,d,e,f);   						//z + a + b + c + d + e + -f
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f);				 	//-z + -a + -b + -c + -d + -e + -f
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);   						//-z + a + b + c + d + e + f
			}

			else if(data[cnt1]->type==10){																												//XNORゲートの時
				//printf("6-xnor\n");
				fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);   						//-z + -a + b + c + d + e + f
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);					   		//-z + a + -b + c + d + e + f
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);   						//-z + a + b + -c + d + e + f
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e,f);  						//-z + a + b + c + -d + e + f
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", z,a,b,c,d,e,f);   						//-z + a + b + c + d + -e + f
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", z,a,b,c,d,e,f);   						//-z + a + b + c + d + e + -f
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f);					 	//z + -a + -b + -c + -d + -e + -f
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f);  							//z + a + b + c + d + e + f
			}
		}

		//入力数が7のとき
		//intput:a,b,c,d,e,f,g
		//output:z
		else if(data[cnt1]->n_in == 7){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong c = data[cnt1]->in[2]->line_id+1;
			Ulong d = data[cnt1]->in[3]->line_id+1;
			Ulong e = data[cnt1]->in[4]->line_id+1;
			Ulong f = data[cnt1]->in[5]->line_id+1;
			Ulong g = data[cnt1]->in[6]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;

			if(data[cnt1]->type==5){													//ANDゲートの時
			//	printf("7-and\n");
				fprintf(out_fp,"-%lu %lu 0\n", z,a); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", z,b); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", z,c); 								//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", z,d); 								//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", z,e); 								//-z + e
				fprintf(out_fp,"-%lu %lu 0\n", z,f); 								//-z + f
				fprintf(out_fp,"-%lu %lu 0\n", z,g); 								//-z + g
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f,g);		//z + -a + -b + -c + -d + -e + -f + -g
			}

			else if(data[cnt1]->type==6){												//NANDゲートの時
				//printf("7-nand\n");
				fprintf(out_fp,"%lu %lu 0\n", z,a);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", z,b);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", z,c); 								//z + c
				fprintf(out_fp,"%lu %lu 0\n", z,d); 								//z + d
				fprintf(out_fp,"%lu %lu 0\n", z,e); 								//z + e
				fprintf(out_fp,"%lu %lu 0\n", z,f); 								//z + f
				fprintf(out_fp,"%lu %lu 0\n", z,g); 								//z + g
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f,g);		//-z + -a + -b + -d + -e + -f + -g
			}

			else if(data[cnt1]->type==7){												//ORゲートの時
				//printf("7-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", z,a);								//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", z,b);								//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", z,c);								//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", z,d);								//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", z,e);								//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", z,f);								//z + -f
				fprintf(out_fp,"%lu -%lu 0\n", z,g);								//z + -g
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);		//-z + a + b + c + d + e + f + g
			}

			else if(data[cnt1]->type==8){												//NORゲートの時
				//printf("7-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", z,a);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", z,b);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", z,c);								//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", z,d);								//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", z,e);								//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", z,f);								//-z + -f
				fprintf(out_fp,"-%lu -%lu 0\n", z,g);								//-z + -g
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);			//z + a + b + c + d + e + f + g
			}

			else if(data[cnt1]->type==9){																		//XORゲートの時
				//printf("7-xor\n");
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);					   	//z + -a + b + c + d + e + f + g
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);						//z + a + -b + c + d + e + f + g
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g);  					//z + a + b + -c + d + e + f + g
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);   					//z + a + b + c + -d + e + f + g
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e,f,g);  					//z + a + b + c + d + -e + f + g
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",z,a,b,c,d,e,f,g);					   	//z + a + b + c + d + e + -f + g
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",z,a,b,c,d,e,f,g);   					//z + a + b + c + d + e + f + -g
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f,g);				//z + -a + -b + -c + -d + -e + -f + -g
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g);  					//-z + a + b + c + d + e + f + g
			}

			else if(data[cnt1]->type==10){																		//XNORゲートの時
				//printf("7-xnor\n");
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);					//-z + -a + b + c + d + e + f + g
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);					//-z + a + -b + c + d + e + f + g
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g);  					//-z + a + b + -c + d + e + f + g
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);   					//-z + a + b + c + -d + e + f + g
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",z,a,b,c,d,e,f,g);   					//-z + a + b + c + d + -e + f + g
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",z,a,b,c,d,e,f,g);				 	//-z + a + b + c + d + e + -f + g
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",z,a,b,c,d,e,f,g);   					//-z + a + b + c + d + e + f + -g
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f,g); 				//-z + -a + -b + -c + -d + -e + -f + -g
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g);   					//z + a + b + c + d + e + f + g
			}
		}

		//入力数が8のとき
		//intput:a,b,c,d,e,f,g,h
		//output:z
		else if(data[cnt1]->n_in == 8){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong c = data[cnt1]->in[2]->line_id+1;
			Ulong d = data[cnt1]->in[3]->line_id+1;
			Ulong e = data[cnt1]->in[4]->line_id+1;
			Ulong f = data[cnt1]->in[5]->line_id+1;
			Ulong g = data[cnt1]->in[6]->line_id+1;
			Ulong h = data[cnt1]->in[7]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;
			if(data[cnt1]->type==5){																		//ANDゲートの時
				//printf("8-and\n");
				fprintf(out_fp,"-%lu %lu 0\n", z,a); 													//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", z,b); 													//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", z,c); 													//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", z,d); 													//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", z,e); 													//-z + e
				fprintf(out_fp,"-%lu %lu 0\n", z,f); 													//-z + f
				fprintf(out_fp,"-%lu %lu 0\n", z,g); 													//-z + g
				fprintf(out_fp,"-%lu %lu 0\n", z,h); 													//-z + h
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f,g,h); 	//z + -a + -b + -c + -d + -e + -f + -g + -h
			}

			else if(data[cnt1]->type==6){																	//NANDゲートの時
				//printf("8-nand\n");
				fprintf(out_fp,"%lu %lu 0\n", z,a);														//z + a
				fprintf(out_fp,"%lu %lu 0\n", z,b);														//z + b
				fprintf(out_fp,"%lu %lu 0\n", z,c); 													//z + c
				fprintf(out_fp,"%lu %lu 0\n", z,d); 													//z + d
				fprintf(out_fp,"%lu %lu 0\n", z,e); 													//z + e
				fprintf(out_fp,"%lu %lu 0\n", z,f); 													//z + f
				fprintf(out_fp,"%lu %lu 0\n", z,g); 													//z + g
				fprintf(out_fp,"%lu %lu 0\n", z,h); 													//z + h
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f,g,h);	//-z + -a + -b + -d + -e + -f + -g + -h
			}

			else if(data[cnt1]->type==7){																	//ORゲートの時
				//printf("8-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", z,a);													//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", z,b);													//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", z,c);													//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", z,d);													//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", z,e);													//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", z,f);													//z + -f
				fprintf(out_fp,"%lu -%lu 0\n", z,g);													//z + -g
				fprintf(out_fp,"%lu -%lu 0\n", z,h);													//z + -h
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h);			//-z + a + b + c + d + e + f + g + h
			}

			else if(data[cnt1]->type==8){																	//NORゲートの時
				//printf("8-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", z,a);													//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", z,b);													//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", z,c);													//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", z,d);													//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", z,e);													//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", z,f);													//-z + -f
				fprintf(out_fp,"-%lu -%lu 0\n", z,g);													//-z + -g
				fprintf(out_fp,"-%lu -%lu 0\n", z,h);													//-z + -h
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h);			//z + a + b + c + d + e + f + g + h
			}

			else if(data[cnt1]->type==9){																	//XORゲートの時
				//printf("8-xor\n");
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h);   		//z + -a + b + c + d + e + f + g + h
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h);			//z + a + -b + c + d + e + f + g + h
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h); 			//z + a + b + -c + d + e + f + g + h
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h);   		//z + a + b + c + -d + e + f + g + h
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h);			//z + a + b + c + d + -e + f + g + h
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h); 			//z + a + b + c + d + e + -f + g + h
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", z,a,b,c,d,e,f,g,h); 			//z + a + b + c + d + e + f + -g + h
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n", z,a,b,c,d,e,f,g,h);   		//z + a + b + c + d + e + f + g + -h
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f,g,h);	//-z + -a + -b + -c + -d + -e + -f + -g + -h
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h);			//-z + a + b + c + d + e + f + g + h
			}

			else if(data[cnt1]->type==10){																	//XNORゲートの時
				//printf("8-xnor\n");
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h);   		//-z + -a + b + c + d + e + f + g + h
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h);			//-z + a + -b + c + d + e + f + g + h
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h); 			//-z + a + b + -c + d + e + f + g + h
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h);   		//-z + a + b + c + -d + e + f + g + h
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h);			//-z + a + b + c + d + -e + f + g + h
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h); 			//-z + a + b + c + d + e + -f + g + h
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",z,a,b,c,d,e,f,g,h); 			//-z + a + b + c + d + e + f + -g + h
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",z,a,b,c,d,e,f,g,h);   		//-z + a + b + c + d + e + f + g + -h
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f,g,h); 	//z + -a + -b + -c + -d + -e + -f + -g + -h
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h);			//z + a + b + c + d + e + f + g + h
			}
		}

		//入力数が9のとき
		//intput:a,b,c,d,e,f,g,h,i
		//output:z
		else if(data[cnt1]->n_in == 9){
			Ulong a = data[cnt1]->in[0]->line_id+1;
			Ulong b = data[cnt1]->in[1]->line_id+1;
			Ulong c = data[cnt1]->in[2]->line_id+1;
			Ulong d = data[cnt1]->in[3]->line_id+1;
			Ulong e = data[cnt1]->in[4]->line_id+1;
			Ulong f = data[cnt1]->in[5]->line_id+1;
			Ulong g = data[cnt1]->in[6]->line_id+1;
			Ulong h = data[cnt1]->in[7]->line_id+1;
			Ulong i = data[cnt1]->in[8]->line_id+1;
			Ulong z = data[cnt1]->line_id+1;

			if(data[cnt1]->type==5){																				//ANDゲートの時
				//printf("9-and\n");
				fprintf(out_fp,"-%lu %lu 0\n", z,a); 															//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", z,b); 															//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", z,c); 															//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", z,d); 															//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", z,e); 															//-z + e
				fprintf(out_fp,"-%lu %lu 0\n", z,f); 															//-z + f
				fprintf(out_fp,"-%lu %lu 0\n", z,g); 															//-z + g
				fprintf(out_fp,"-%lu %lu 0\n", z,h); 															//-z + h
				fprintf(out_fp,"-%lu %lu 0\n", z,i); 															//-z + i
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f,g,h,i); 	//z + -a + -b + -c + -d + -e + -f + -g + -h + i
			}

			else if(data[cnt1]->type==6){																			//NANDゲートの時
				//printf("9-nand\n");
				fprintf(out_fp,"%lu %lu 0\n", z,a);																//z + a
				fprintf(out_fp,"%lu %lu 0\n", z,b);																//z + b
				fprintf(out_fp,"%lu %lu 0\n", z,c); 															//z + c
				fprintf(out_fp,"%lu %lu 0\n", z,d); 															//z + d
				fprintf(out_fp,"%lu %lu 0\n", z,e); 															//z + e
				fprintf(out_fp,"%lu %lu 0\n", z,f); 															//z + f
				fprintf(out_fp,"%lu %lu 0\n", z,g); 															//z + g
				fprintf(out_fp,"%lu %lu 0\n", z,h); 															//z + h
				fprintf(out_fp,"%lu %lu 0\n", z,i); 															//z + i
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",z,a,b,c,d,e,f,g,h,i);	//-z + -a + -b + -d + -e + -f + -g + -h + -i
			}

			else if(data[cnt1]->type==7){																			//ORゲートの時
				//printf("9-or\n");
				fprintf(out_fp,"%lu -%lu 0\n", z,a);															//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", z,b);															//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", z,c);															//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", z,d);															//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", z,e);															//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", z,f);															//z + -f
				fprintf(out_fp,"%lu -%lu 0\n", z,g);															//z + -g
				fprintf(out_fp,"%lu -%lu 0\n", z,h);															//z + -h
				fprintf(out_fp,"%lu -%lu 0\n", z,i);															//z + -i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h,i); 			//-z + a + b + c + d + e + f + g + h + i
			}

			else if(data[cnt1]->type==8){																			//NORゲートの時
				//printf("9-nor\n");
				fprintf(out_fp,"-%lu -%lu 0\n", z,a);															//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", z,b);															//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", z,c);															//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", z,d);															//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", z,e);															//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", z,f);															//-z + -f
				fprintf(out_fp,"-%lu -%lu 0\n", z,g);															//-z + -g
				fprintf(out_fp,"-%lu -%lu 0\n", z,h);															//-z + -h
				fprintf(out_fp,"-%lu -%lu 0\n", z,i);															//-z + -i
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",z,a,b,c,d,e,f,g,h,i);				//z + a + b + c + d + e + f + g + h + i
			}

			else if(data[cnt1]->type==9){																			//XORゲートの時
				//printf("9-xor\n");
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);			//z + -a + b + c + d + e + f + g + h + i
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);			//z + a + -b + c + d + e + f + g + h + i
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i); 			//z + a + b + -c + d + e + f + g + h + i
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i); 			//z + a + b + c + -d + e + f + g + h + i
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i); 			//z + a + b + c + d + -e + f + g + h + i
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);			//z + a + b + c + d + e + -f + g + h + i
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i); 			//z + a + b + c + d + e + f + -g + h + i
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", z,a,b,c,d,e,f,g,h,i); 			//z + a + b + c + d + e + f + g + -h + i
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n", z,a,b,c,d,e,f,g,h,i); 			//z + a + b + c + d + e + f + g + h + -i
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f,g,h,i);	//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//-z + a + b + c + d + e + f + g + h + i
			}

			else if(data[cnt1]->type==10){																			//XNORゲートの時
				//printf("9-xnor\n");
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",	z,a,b,c,d,e,f,g,h,i);			//-z + -a + b + c + d + e + f + g + h + i
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",	z,a,b,c,d,e,f,g,h,i);			//-z + a + -b + c + d + e + f + g + h + i
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//-z + a + b + -c + d + e + f + g + h + i
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//-z + a + b + c + -d + e + f + g + h + i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//-z + a + b + c + d + -e + f + g + h + i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",	z,a,b,c,d,e,f,g,h,i);			//-z + a + b + c + d + e + -f + g + h + i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//-z + a + b + c + d + e + f + -g + h + i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//-z + a + b + c + d + e + f + g + -h + i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//-z + a + b + c + d + e + f + g + h + -i
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", z,a,b,c,d,e,f,g,h,i);	//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", z,a,b,c,d,e,f,g,h,i);  			//z + a + b + c + d + e + f + g + h + i
			}
		}

	}

	topgun_close(fpCNF, funcName);

    //fprintf(fpCNFEND,"%lu\n",variables-1); //信号線数-1を入力
    fprintf(fpCNFEND,"%lu\n",variables); //信号線数+1を入力
    topgun_close(fpCNFEND, funcName);



    for(Ulong i = 0; i < Line_info.n_line; i++){ 
		line = &Line_head[i];
		switch ( line->type )
		{
			case TOPGUN_PI:
		    	fprintf(fpPIINFO,"%lu\n", line->line_id + 1);	//信号線のtypeが外部入力のものを書き込む
		    	break;
			case TOPGUN_PO:
		    	fprintf(fpPOINFO,"%lu\n", line->line_id + 1);	//信号線のtypeが外部出力のものを書き込む
		    	break;
			default:
		    	break;
		}
    }


    topgun_close(fpPIINFO, funcName);
    topgun_close(fpPOINFO, funcName);
}

/**********************************************
**************TYPE一覧*************************
*
*    TOPGUN_PI   = 0,	//!< 外部入力
*    TOPGUN_PO   = 1,	//!< 外部出力
*    TOPGUN_BR   = 2,	//!< ファンアウトブランチ
*    TOPGUN_INV  = 3,	//!< インバータ
*    TOPGUN_BUF  = 4,	//!< バッファ
*    TOPGUN_AND  = 5,	//!< アンド
*    TOPGUN_NAND = 6,	//!< ナンド
*    TOPGUN_OR   = 7,	//!< オア
*    TOPGUN_NOR  = 8,	//!< ノア
*    TOPGUN_XOR  = 9,	//!< イクルーシブオア
*    TOPGUN_XNOR = 10,	//!< イクルーシブノア
*    TOPGUN_BLKI = 11,	//!< 不定入力
*    TOPGUN_BLKO = 12,	//!< 不定出力
*    TOPGUN_UNK  = 13,	//!< 未確定型
*    TOPGUN_NUM_PRIM,    //!< 型の数
*
************************************************/