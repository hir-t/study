#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){

    FILE *fpGOLD;
    FILE *fpTMP;
    FILE *fpOUT;

    int gold;   //固定するcnfのid
    int tmp; //satの結果の値の絶対値
    int end; // satの結果の値
   
    char result[10];
    char firstLine[100];
    char outputFileName[100];


    // コマンド引数の確認
    if ( argv != 4 ) {
	printf("USAGE: convertID goldLIST tmpLIST OUTPUT_CNFFileName\n");
	return 0;
    }

    // ファイルを開く
    fpGOLD= fopen( argc[1], "r");
    if ( fpGOLD == NULL ) {
        printf("File Open Error %s\n", argc[1]);
        return 1;
    }
    fpTMP = fopen( argc[2], "r");
    if ( fpTMP == NULL ) {
        printf("File Open Error %s\n", argc[2]);
        return 1;
    }

    fpOUT = fopen( argc[3], "w");


    while ( fscanf( fpGOLD, "%d", &gold ) != EOF ) { 

	if ( fscanf( fpTMP, "%d %d", &tmp, &end ) == EOF ) {
	    printf("File End %s\n", argc[2]);
	    return 1;
	}
	
	if ( tmp < 0 ) {
	    gold = -1 * gold;
	}
	fprintf(fpOUT, "%d 0\n", gold); 
    }

    fclose (fpGOLD);
    fclose (fpTMP);
    fclose (fpOUT);
    return 0;
}
    
