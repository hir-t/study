#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){

    FILE *fpLIST;
    FILE *fpCNFOUT;
    FILE *fpOUT;

    int literal; //固定するcnfのid
    int value; //satの結果の値の絶対値
    int tmp;
   
    char result[10];
    char firstLine[100];
    char outputFileName[100];


    // コマンド引数の確認
    if ( argv != 4 ) {
	printf("USAGE: InputFix goldLIST tmpLIST OUTPUT_CNFFileName\n");
	return 0;
    }

    // ファイルを開く
    fpLIST= fopen( argc[1], "r");
    if ( fpLIST == NULL ) {
        printf("File Open Error %s\n", argc[1]);
        return 1;
    }
    fpCNFOUT = fopen( argc[2], "r");
    if ( fpCNFOUT == NULL ) {
        printf("File Open Error %s\n", argc[2]);
        return 1;
    }

    fpOUT = fopen( argc[3], "w");


    while ( fscanf( fpLIST, "%d", &literal ) != EOF ) { 

	//printf("literal %d\n", literal);
	do {

	    if ( fscanf( fpCNFOUT, "%d", &value ) == EOF ) {
		printf("File End %s\n", argc[2]);
		return 1;
	    }
	
	    if ( value < 0 ) {
		tmp = -1 * value;
	    }
	    else {
		tmp = value;
	    } 
	    //printf("value %d tmp %d\n", value, tmp);

	} while (literal != tmp ); 

	fprintf(fpOUT, "%d 0\n", value); 
    }

    fclose (fpLIST);
    fclose (fpCNFOUT);
    fclose (fpOUT);
    return 0;
}
    
