#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){

    FILE *fpLIST;   //argc[1]:eval.piCnfInfo
    FILE *fpCNFOUT; //argc[2]:f$cnt.cnf.out.dat
    FILE *fpOUT;    //argc[3]:eval.$cnt.pifix.cnf

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
    //eval.$cnt.pifix.cnf
    fpOUT = fopen( argc[3], "w");

    //literal = eval.piCnfInfo
    while ( fscanf( fpLIST, "%d", &literal ) != EOF ) { 

	//printf("literal %d\n", literal);
	   do {
                //value = f$cnt.cnf.out.dat(SATの結果)
    	    if ( fscanf( fpCNFOUT, "%d", &value ) == EOF ) {
    		  printf("File End %s\n", argc[2]);
    		  return 1;
    	    }
            //literalは全て正の数で,valueはそうではないため,比較用に正の数に変換してtmpに格納する
    	    if ( value < 0 ) {
    		  tmp = -1 * value;   //負の数(=0)のとき正の数にする
    	    }
    	    else {
    		  tmp = value;        //正の数(=1)のときは変えない
    	    }
    	    //printf("value %d tmp %d\n", value, tmp);

    	} while (literal != tmp );     //evalの入力 = |SATの出力|になったらdo-whileを抜ける
        //ループを抜けてSATの結果を書き込む
        //satの結果、evalの入力線に与える数値(負=0,正=1)を出力ファイルへ書き込んでいく。
	   fprintf(fpOUT, "%d 0\n", value); 
    }

    fclose (fpLIST);
    fclose (fpCNFOUT);
    fclose (fpOUT);
    return 0;
}

