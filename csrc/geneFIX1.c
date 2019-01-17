#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){

    FILE *fpLIST;       //argc[1] = compareInput.info
    FILE *fpIN1;        //argc[2]
    FILE *fpIN2;        //argc[3]
    FILE *fpLAST;       //不使用?
    FILE *fpOUT;        //argc[4]:出力用
    FILE *fpOUTSTART;
    FILE *fpOUTEND;

    int literal; //読み込んだリテラルの値
    int in1;     //  line1 = ( LIST + in1 - 1 )
    int in2;     //  --------------------------
    int last;    //  line2 = ( LIST + in2 - 1 )
    int line1;   //  --------------------------
    int line2; 

    char result[10];
    char firstLine[100];
    char outputFileName[100];


    // コマンド引数の確認
    if ( argv != 5 ) {
	printf("USAGE: geneFix1 Fix1ListFile in1.start in2.start OUTPUT_CNFFileName\n");
	return 0;
    }
    //BranchListFile
    // 1 2 3 4 5
    // in1.start
    // 1
    // in2.start
    // 73

    // ファイルを開く
    fpLIST= fopen( argc[1], "r");
    if ( fpLIST == NULL ) {
        printf("File Open Error %s\n", argc[1]);
        return 1;
    }
    fpIN1 = fopen( argc[2], "r");
    if ( fpIN1 == NULL ) {
        printf("File Open Error %s\n", argc[2]);
        return 1;
    }
    fpIN2 = fopen( argc[3], "r");
    if ( fpIN2 == NULL ) {
        printf("File Open Error %s\n", argc[3]);
        return 1;
    }

    fscanf( fpIN1, "%d", &in1 );
    fclose ( fpIN1 );

    fscanf( fpIN2, "%d", &in2 );
    fclose ( fpIN2 );

    fpOUT = fopen( argc[4], "w");

    //　branchのCNFを作っていく
    while ( fscanf( fpLIST, "%d", &literal ) != EOF ) {
    	line1 = literal + in1 - 1;
    	line2 = literal + in2 - 1;

    	fprintf(fpOUT, "%d 0\n", line1);
    	fprintf(fpOUT, "%d 0\n", line2);
    }


    fclose (fpLIST);
    fclose (fpOUT);
    return 0;
}

