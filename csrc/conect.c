#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>
#include <time.h>

#include <topgun.h>
#include <topgunLine.h>

/* topgun_output.c */
FILE *topgun_open ( char *, char *, char * ); /* fileを開く */
/* topgunCell.c */
void topgun_close( FILE *, char * );

void conect(char *benchName)
{
	extern LINE_INFO Line_info;

	FILE *fpMUXOUT;		//MUXの出力ID
	FILE *fpCNF;		//Evalのcnf
	FILE *fpCONECT;		//出力用

	int literal = 0; //読み込んだリテラルの値(CNF)
	int tmp = 0; //読み込んだリテラルの値(MUX)

	char firstLine[100];
	char line[100];
	char inputMUXOUTFileName[100];
   	char inputCNFFileName[100];
   	char outputConectFileName[100];

   	sprintf(inputMUXOUTFileName, "%smuxOut.info", benchName);
	sprintf(inputCNFFileName, "%s.cnf", benchName);
	sprintf(outputConectFileName, "%s_co.cnf", benchName);

	char *funcName = "obfuscation"; // 関数名 

	fpMUXOUT = topgun_open( inputMUXOUTFileName, "r" , funcName);
	fpCNF = topgun_open( inputCNFFileName, "r", funcName );
	fpCONECT = topgun_open( outputConectFileName , "w" , funcName);

	fgets(firstLine, 100, fpCNF);       //1行目は読み飛ばす

/*	while(fgets(line, 100, fpCNF))
	{
		printf("%s", line);
		char i = line
	}*/

	int lines[Line_info.n_line];
	int i = 0;
	while ( fscanf(fpCNF, "%d", &literal ) != EOF )
	{
		printf("%d\n", literal);
		//lines[i] = literal;
		//i++;
/*		while ( fscanf(fpMUXOUT, "%d", &tmp ) != EOF )
		{
			printf("%d\n", tmp);
			if (literal == tmp) fprintf(fpCONECT, "%d ", tmp);
			else fprintf(fpCONECT, "%d \n", literal);

		}*/

    }
    topgun_close(fpMUXOUT, funcName);
    topgun_close(fpCNF, funcName);
    topgun_close(fpCONECT, funcName);
}