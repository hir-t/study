#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){
                        // ./copyCNF = argc[0]
    FILE *fpCNF;        //argc[1]、コピーされるファイル(c1_obf.cnf)
    //FILE *fpCNFSTART;
    FILE *fpCNFEND;     //argc[2]、変数baseに与える (c1_obf.cnf.end)
    FILE *fpCOPYCNF;    //argc[3](c2_obf.cnf)

    int literal; //読み込んだリテラルの値
    int base;    //
    int max = 0; // 書き込んだリテラルの最大値   .endに書き込む
    int value;

    char result[10];
    //char firstLine[100];
    //char outputCNFStartFileName[100];
    char outputCNFEndFileName[100];

    /* ファイルの読み込み */
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

    /* ファイルの書き込み */
    fpCOPYCNF = fopen( argc[3], "w");   //argc[3]を書き込みモードで開く

   /* sprintf(outputCNFStartFileName, "%s.start", argc[3]);   //出力するファイル名の決定(argc[3].start)
    fpCNFSTART = fopen( outputCNFStartFileName, "w" );      //出力ファイルを書き込みモードで開く
    fprintf(fpCNFSTART, "%d\n", base+1);                    //開始のcnfのIDを記す


    sprintf(outputCNFEndFileName, "%s.end", argc[3]);       //出力するファイル名の決定(argc[3].end)
    fpCNFEND = fopen( outputCNFEndFileName, "w" );          //出力ファイルを書き込みモードで開く*/



    //fgets(firstLine, 100, fpCNF);       //1行目は読み飛ばす

    while ( fscanf(fpCNF, "%d", &literal ) != EOF ) {
    	if ( literal > 0 ) {
    	    value = literal+base;
    	    /*if ( max < value ) {
    		  max = value;
    	    }*/
    	    fprintf(fpCOPYCNF, "%d\n", literal+base); //正の値の時はbaseの値を足した値を出力する
    	}
    	else if ( literal <  0 ) {
    	       value = - (literal -  base);    //正の値に変換(絶対値?)
    	       if ( max < value ) {
    		      max = value;
    	       }
    	       fprintf(fpCOPYCNF, "%d\n", literal-base); //負の値の時はbaseの値を引いた値を出力する(負-負で絶対値が大きくなる)
    	}
    	else {
    	   fprintf(fpCOPYCNF, "%d\n", literal); //0の時はそのまま0を出力して改行する
    	}
    }

    //fprintf(fpCNFEND,"%d\n",max);

    fclose (fpCNF);
    fclose (fpCOPYCNF);
    //fclose (fpCNFSTART);
    //fclose (fpCNFEND);
    return 0;
}

