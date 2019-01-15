#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){

    FILE *fpCNF;
    FILE *fpCNFSTART;
    FILE *fpCNFEND;
    FILE *fpCOPYCNF;

    int literal; //読み込んだリテラルの値
    int base;    // 
    int max = 0; // 書き込んだリテラルの最大値
    int value;

    char result[10];
    char firstLine[100];
    char outputCNFStartFileName[100];
    char outputCNFEndFileName[100];


    if ( argv != 4 ) {
	printf("USAGE: copyCNF CNFFileName CNFFileName.end OUTPUT_CNFFileName\n");
	return 0;
    }

    fpCNF = fopen( argc[1], "r");
    if ( fpCNF == NULL ) {
        printf("File Open Error %s\n", argc[1]);
        return 1;
    }

    fpCNFEND = fopen( argc[2], "r");

    fscanf(fpCNFEND, "%d", &base );
    //printf("arg2 = %d\n",base);
    fclose ( fpCNFEND ); 
    
    fpCOPYCNF = fopen( argc[3], "w");

    sprintf(outputCNFStartFileName, "%s.start", argc[3]);
    fpCNFSTART = fopen( outputCNFStartFileName, "w" );
    fprintf(fpCNFSTART, "%d\n", base+1); //開始のcnfのIDを記す


    sprintf(outputCNFEndFileName, "%s.end", argc[3]);
    fpCNFEND = fopen( outputCNFEndFileName, "w" );



    fgets(firstLine, 100, fpCNF); //1行目は読み飛ばす

    while ( fscanf(fpCNF, "%d", &literal ) != EOF ) {
	if ( literal > 0 ) {
	    value = literal+base;
	    if ( max < value ) {
		max = value;
	    }
	    fprintf(fpCOPYCNF, "%d ", literal+base); //正の値の時はbaseの値を足した値を出力する
	}
	else if ( literal <  0 ) { 
	    value = - (literal -  base);
	    if ( max < value ) {
		max = value;
	    }
	    fprintf(fpCOPYCNF, "%d ", literal-base); //負の値の時はbaseの値を引いた値を出力する
	}
	else {
	    fprintf(fpCOPYCNF, "%d\n", literal); //0の時はそのまま0を出力して改行する
	}

    }
    fprintf(fpCNFEND,"%d\n",max);

    fclose (fpCNF);
    fclose (fpCOPYCNF);
    fclose (fpCNFSTART);
    fclose (fpCNFEND);
    return 0;
}
    
