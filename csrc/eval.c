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

	 for(int i = 0;i<Line_info.n_line;i++){
	 	line = &(Line_head[i]);
	 	if(2<line->type && line->type<11){		//回路内のゲートの数を数える
			gate++;
		}
	 }

	Ulong variables = Line_info.n_line;			//命題変数の数(信号線数)
	Ulong clauses = countClauses();				//節数(cnfファイルの行数)
	fprintf(out_fp,"p cnf %lu %lu\n",variables,clauses);
	fprintf(fpCNFSTART,"1\n");
    topgun_close(fpCNFSTART, funcName);

	/*	ID順にレベルやタイプを取得 */
	for(int i = 0;i<Line_info.n_line;i++){
		line = &(Line_head[i]);
		//入力数が1のとき
		//input:a output:z
		if(line->n_in == 1){
			if(line->type==3){																		//INVゲートの時
				fprintf(out_fp,"%lu %lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//a + z
				fprintf(out_fp,"-%lu -%lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//-a+ -z
			}
			if(line->type==4){																		//BUFゲートの時
				fprintf(out_fp,"-%lu %lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//-a + z
				fprintf(out_fp,"%lu -%lu 0\n",line->in[0]->line_id+1,line->out[0]->line_id+1);		//a+ -z
			}
		}
		//入力数が2のとき
		//intput1:a,input2:b,output:z
		if(line->n_in == 2){
			if(line->type==5){																									//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
				fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//z + -a + -b
			}

			if(line->type==6){																									//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
				fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);	//-z + -a + -b
			}

			if(line->type==7){																									//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
				fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//-z + a + b
			}

			if(line->type==8){																									//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
				fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);		//z + a + b
			}

			if(line->type==9){																									//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + -a + b
				fprintf(out_fp,"%lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + -b
				fprintf(out_fp,"-%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1); 	//-z + -a + -b
				fprintf(out_fp,"-%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//-z + a + b
			}

			if(line->type==10){																									//XNORゲートの時
				fprintf(out_fp,"-%lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//-z + -a + b
				fprintf(out_fp,"-%lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + -b
				fprintf(out_fp,"%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1); 	//z + -a + -b
				fprintf(out_fp,"%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1);   	//z + a + b
			}
		}

		//入力数が3のとき
		//intput1:a,input2:b,input3:c,output:z
		if(line->n_in == 3){
			if(line->type==5){																									//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
				fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//z + -a + -b + -c
													line->in[1]->line_id+1,line->in[2]->line_id+1);
			}

			if(line->type==6){																									//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
				fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//-z + -a + -b + -c
														line->in[1]->line_id+1,line->in[2]->line_id+1);
			}

			if(line->type==7){																									//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
				fprintf(out_fp,"-%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,							//-z + a + b + +c
														line->in[1]->line_id+1,line->in[2]->line_id+1);
			}

			if(line->type==8){																									//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
				fprintf(out_fp,"%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,							//z + a + b + c
														line->in[1]->line_id+1,line->in[2]->line_id+1);
			}

			if(line->type==9){																									//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + -a + b + c
				fprintf(out_fp,"%lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + -b + c
				fprintf(out_fp,"%lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + b + -c
				fprintf(out_fp,"%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1); 						//z + -a + -b + -c
				fprintf(out_fp,"-%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + b + c
			}

			if(line->type==10){																									//XNORゲートの時
				fprintf(out_fp,"-%lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + -a + b + c
				fprintf(out_fp,"-%lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + -b + c
				fprintf(out_fp,"-%lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//-z + a + b + -c
				fprintf(out_fp,"-%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1); 						//-z + -a + -b + -c
				fprintf(out_fp,"%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
														line->in[1]->line_id+1,line->in[2]->line_id+1);   						//z + a + b + c
			}
		}

		//入力数が4のとき
		//intput:a,b,c,d
		//output:z
		if(line->n_in == 4){
			if(line->type==5){																									//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//z + -a + -b + -c + -d
													line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
			}

			if(line->type==6){																									//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + -a + -b + -d
													line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
			}

			if(line->type==7){																									//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
				fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//-z + a + b + c + d
												line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
			}

			if(line->type==8){																									//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
				fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,						//z + a + b + c + d
												line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
			}

			if(line->type==9){																									//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + -a + b + c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"%lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   	//z + a + -b + c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + -c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + c + -d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
			}

			if(line->type==10){																									//XNORゲートの時
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + -a + b + c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   	//-z + a + -b + c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + -c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//-z + a + b + c + -d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//z + -a + -b + -c + -d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   					//z + a + b + c + d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1);
			}
		}

		//入力数が5のとき
		//intput:a,b,c,d,e
		//output:z
		if(line->n_in == 5){
			if(line->type==5){																									//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//z + -a + -b + -c + -d + -e
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
			}

			if(line->type==6){																									//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,				//-z + -a + -b + -d
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
			}

			if(line->type==7){																									//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + a + b + c + d + e
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
			}

			if(line->type==8){																									//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					//-z + a + b + c + d + e
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);
			}

			if(line->type==9){																										//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + -a + b + c + d + e
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + -b + c + d + e
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + -c + d + e
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + -d + e
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + d + -e
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1); 		//z + -a + -b + -c + -d + -e
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + d + e
			}

			if(line->type==10){																										//XNORゲートの時
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + -a + b + c + d + e
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + -b + c + d + e
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + -c + d + e
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + -d + e
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//-z + a + b + c + d + -e
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1); 		//-z + -a + -b + -c + -d + -e
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1);   	//z + a + b + c + d + e
			}
		}

		//入力数が6のとき
		//intput:a,b,c,d,e,f
		//output:z
		if(line->n_in == 6){
			if(line->type==5){																									//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//-z + f
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id,line->in[0]->line_id,				//z + -a + -b + -c + -d + -e + -f
								line->in[1]->line_id,line->in[2]->line_id,line->in[3]->line_id,line->in[4]->line_id,
								line->in[5]->line_id);
			}

			if(line->type==6){																									//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 									//z + f
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",line->out[0]->line_id+1,line->in[0]->line_id+1,			//-z + -a + -b + -d + -e + -f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
			}

			if(line->type==7){																									//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);									//z + -f
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//-z + a + b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
			}

			if(line->type==8){																									//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//-z + -f
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				//z + a + b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
			}

			if(line->type==9){																												//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + -a + b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   		//z + a + -b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + -c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + -d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + -e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + e + -f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,				 	//-z + -a + -b + -c + -d + -e + -f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
			}

			if(line->type==10){																												//XNORゲートの時
				fprintf(out_fp,"%-lu -%lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + -a + b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					   		//-z + a + -b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + -c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + -d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + -e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//-z + a + b + c + d + e + -f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,					 	//z + -a + -b + -c + -d + -e + -f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1,   						//z + a + b + c + d + e + f
								line->in[1]->line_id+1,line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1);
			}
		}

		//入力数が7のとき
		//intput:a,b,c,d,e,f,g
		//output:z
		if(line->n_in == 7){
			if(line->type==5){																									//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 								//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 								//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//-z + e
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//-z + f
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//-z + g
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//z + -a + -b + -c + -d + -e + -f + -g
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
			}

			if(line->type==6){																									//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + b
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 									//z + c
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 									//z + d
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 									//z + e
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 									//z + f
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 									//z + g
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",													//-z + -a + -b + -d + -e + -f + -g
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
			}

			if(line->type==7){																									//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);									//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);									//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);									//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);									//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);									//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);									//z + -f
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);									//z + -g
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//-z + a + b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
			}

			if(line->type==8){																									//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//-z + -f
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//-z + -g
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",															//z + a + b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
			}

			if(line->type==9){																								//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//z + -a + b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//z + a + -b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//z + a + b + -c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//z + a + b + c + -d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//z + a + b + c + d + -e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//z + a + b + c + d + e + -f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//z + a + b + c + d + e + f + -g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//z + -a + -b + -c + -d + -e + -f + -g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//-z + a + b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
			}

			if(line->type==10){																								//XNORゲートの時
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu 0\n",													   	//-z + -a + b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu 0\n",														//-z + a + -b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   													//-z + a + b + -c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu 0\n",   													//-z + a + b + c + -d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   													//-z + a + b + c + d + -e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu 0\n",													   	//-z + a + b + c + d + e + -f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   													//-z + a + b + c + d + e + f + -g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 												//-z + -a + -b + -c + -d + -e + -f + -g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   													//z + a + b + c + d + e + f + g
								line->out[0]->line_id+1,line->in[0]->line_id+1,
								line->in[1]->line_id+1,line->in[2]->line_id+1,
								line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1);
			}
		}

		//入力数が8のとき
		//intput:a,b,c,d,e,f,g,h
		//output:z
		if(line->n_in == 8){
			if(line->type==5){																								//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 							//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 							//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 							//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 							//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 							//-z + e
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 							//-z + f
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 							//-z + g
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 							//-z + h
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 											//z + -a + -b + -c + -d + -e + -f + -g + -h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
			}

			if(line->type==6){																								//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + b
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//z + c
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//z + d
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//z + e
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//z + f
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//z + g
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 								//z + h
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",											//-z + -a + -b + -d + -e + -f + -g + -h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
			}

			if(line->type==7){																								//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//z + -f
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//z + -g
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);								//z + -h
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//-z + a + b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
			}

			if(line->type==8){																								//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);							//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);							//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);							//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);							//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);							//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);							//-z + -f
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);							//-z + -g
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);							//-z + -h
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",													//z + a + b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
			}

			if(line->type==9){																								//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//z + -a + b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//z + a + -b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 													//z + a + b + -c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//z + a + b + c + -d + e + f + g + h
								line->out[0]->line_id,line->in[0]->line_id,line->in[1]->line_id,
								line->in[2]->line_id,line->in[3]->line_id,line->in[4]->line_id,
								line->in[5]->line_id,line->in[6]->line_id,line->in[7]->line_id);
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//z + a + b + c + d + -e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  						//z + a + b + c + d + e + -f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  						//z + a + b + c + d + e + f + -g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//z + a + b + c + d + e + f + g + -h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//-z + -a + -b + -c + -d + -e + -f + -g + -h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//-z + a + b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
			}

			if(line->type==10){																								//XNORゲートの時
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",   												//-z + -a + b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",					   								//-z + a + -b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + -c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   												//-z + a + b + c + -d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",													//-z + a + b + c + d + -e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n", 							  					//-z + a + b + c + d + e + -f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n", 							  					//-z + a + b + c + d + e + f + -g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   												//-z + a + b + c + d + e + f + g + -h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										 	//z + -a + -b + -c + -d + -e + -f + -g + -h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",							   						//z + a + b + c + d + e + f + g + h
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1);
			}
		}

		//入力数が9のとき
		//intput:a,b,c,d,e,f,g,h,i
		//output:z
		if(line->n_in == 9){
			if(line->type==5){																								//ANDゲートの時
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1); 							//-z + a
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1); 							//-z + b
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 							//-z + c
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 							//-z + d
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 							//-z + e
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 							//-z + f
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 							//-z + g
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 							//-z + h
				fprintf(out_fp,"-%lu %lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1); 							//-z + i
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
			}

			if(line->type==6){																								//NANDゲートの時
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + a
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + b
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1); 								//z + c
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1); 								//z + d
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1); 								//z + e
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1); 								//z + f
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1); 								//z + g
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1); 								//z + h
				fprintf(out_fp,"%lu %lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1); 								//z + i
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n",										//-z + -a + -b + -d + -e + -f + -g + -h + -i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
			}

			if(line->type==7){																								//ORゲートの時
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);								//z + -a
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);								//z + -b
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);								//z + -c
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);								//z + -d
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);								//z + -e
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);								//z + -f
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);								//z + -g
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);								//z + -h
				fprintf(out_fp,"%lu -%lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1);								//z + -i
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n", 												//-z + a + b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
			}

			if(line->type==8){																								//NORゲートの時
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[0]->line_id+1);							//-z + -a
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[1]->line_id+1);							//-z + -b
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[2]->line_id+1);							//-z + -c
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[3]->line_id+1);							//-z + -d
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[4]->line_id+1);							//-z + -e
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[5]->line_id+1);							//-z + -f
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[6]->line_id+1);							//-z + -g
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[7]->line_id+1);							//-z + -h
				fprintf(out_fp,"-%lu -%lu 0\n", line->out[0]->line_id+1,line->in[8]->line_id+1);							//-z + -i
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
			}

			if(line->type==9){																								//XORゲートの時
				fprintf(out_fp,"%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",											   	//z + -a + b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//z + a + -b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + -c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + -d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id);
				fprintf(out_fp,"%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + -e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//z + a + b + c + d + e + -f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + -g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//z + a + b + c + d + e + f + g + -h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//z + a + b + c + d + e + f + g + h + -i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 										//z + -a + -b + -c + -d + -e + -f + -g + -h + -i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
			}

			if(line->type==10){																								//XNORゲートの時
				fprintf(out_fp,"-%lu -%lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + -a + b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu -%lu %lu %lu %lu %lu %lu %lu %lu 0\n",												//-z + a + -b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu -%lu %lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + -c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu -%lu %lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + -d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu -%lu %lu %lu %lu %lu 0\n",   											//-z + a + b + c + d + -e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu -%lu %lu %lu %lu 0\n",												//-z + a + b + c + d + e + -f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu -%lu %lu %lu 0\n",   											//-z + a + b + c + d + e + f + -g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu -%lu %lu 0\n",   											//-z + a + b + c + d + e + f + g + -h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu %lu %lu %lu %lu %lu %lu %lu %lu -%lu 0\n",   											//-z + a + b + c + d + e + f + g + h + -i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"-%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu -%lu 0\n", 									//-z + -a + -b + -c + -d + -e + -f + -g + -h + -i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
				fprintf(out_fp,"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu 0\n",   											//z + a + b + c + d + e + f + g + h + i
								line->out[0]->line_id+1,line->in[0]->line_id+1,line->in[1]->line_id+1,
								line->in[2]->line_id+1,line->in[3]->line_id+1,line->in[4]->line_id+1,
								line->in[5]->line_id+1,line->in[6]->line_id+1,line->in[7]->line_id+1,
								line->in[8]->line_id+1);
			}
		}
	}
	topgun_close(fpCNF, funcName);

    //fprintf(fpCNFEND,"%lu\n",variables-1); //信号線数-1を入力
    fprintf(fpCNFEND,"%lu\n",variables+1); //信号線数+1を入力(これが正しい_)
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