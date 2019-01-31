#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){
// ../exp/convertID c1_obf.poCnfInfo eval.$cnt.pofix.cnf.tmp eval.$cnt.pofix.cnf
    FILE *fpGOLD; //c1_obf.poCnfInfo
    FILE *fpTMP;  //eval.$cnt.pofix.cnf.tmp
    FILE *fpOUT;  //出力用:eval.$cnt.pofix.cnf

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

    //gold:c1_obf.poCnfInfo
    while ( fscanf( fpGOLD, "%d", &gold ) != EOF ) {
        //printf("gold:%d\n", gold);
        //eval.$cnt.pofix.cnf.tmp temp:１つ目=出力線, end:0
    	if ( fscanf( fpTMP, "%d %d", &tmp, &end ) == EOF ) {
    	    printf("File End %s\n", argc[2]);
    	    return 1;
    	}
        //tmpが0以下のとき、goldを反転させる
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

