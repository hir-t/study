// LISTの要素が4の場合
// LIST_1 + in1 - 1 +---+ xor =
// -----------------+ X | last+1 +---+
//                  | O +--------+   | or =
// -----------------+ R |        | O | last+5 +---+
// LIST_1 + in2 - 1 +---+        | R +--------+   |
// LIST_2 + in1 - 1 +---+ xor =  |   |        | O |
// -----------------+ X | last+2 |   |        | R |
//                  | O +--------+   |        |   | or =
// -----------------+ R |        +---+        |   | last+7
// LIST_2 + in2 - 1 +---+                     |   +--------
// LIST_3 + in1 - 1 +---+ xor =               |   |
// -----------------+ X | last+3 +---+        |   |
//                  | O +--------+   | or =   |   |
// -----------------+ R |        | O | last+6 |   |
// LIST_3 + in2 - 1 +---+        | R +--------+   |
// LIST_4 + in1 - 1 +---+ xor =  |   |        +---+
// -----------------+ X | last+4 |   |
//                  | O +--------+   |
// -----------------+ R |        +---+
// LIST_4 + in2 - 1 +---+
// last

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){

    FILE *fpLIST;   //argc[1]:入力(外部出力リスト)
    FILE *fpIN1;    //argc[2]:入力(i.start)
    FILE *fpIN2;    //argc[3]:入力(i+1.start)
    FILE *fpLAST;   //argc[4]:入力(br.cnf.end：ここまでの最大ID)
    FILE *fpOUT;    //argc[5]:出力
    FILE *fpOUTSTART;
    FILE *fpOUTEND;

    int literal;    //外部入力を入れる変数
    int in1;        //argc[2]の内容 c1_obf.cnf.start
    int in2;        //argc[3]の内容 c2_obf.cnf.start
    int last;       //argc[4]の内容 br.cnf.end
    int xorIn1;     //xorの入力1
    int xorIn2;     //xorの入力2
    int xor;        //xorの出力
    int orIn1;      //orの入力1
    int orIn2;      //orの入力2
    int or;         //orの出力

    char result[10];
    char firstLine[100];
    char outputFileName[100];


    // コマンド引数の確認
    if ( argv != 6 ) {
	printf("USAGE: geneXORT OUTPUTListFile in1.start in2.start last.end OUTPUT_CNFFileName\n");
	return 0;
    }
    // OUTListFile
    // 13 15
    // in1.start
    // 1
    // in2.start
    // 73
    // last.end
    // 149

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
    fpLAST = fopen( argc[4], "r");
    if ( fpLAST == NULL ) {
        printf("File Open Error %s\n", argc[4]);
        return 1;
    }

    //１つ目のファイルの最小の信号線
    fscanf( fpIN1, "%d", &in1 );
    fclose ( fpIN1 );

    //2つ目のファイルの最小の信号線(=１つ目のファイルの最大の信号線+1)
    fscanf( fpIN2, "%d", &in2 );
    fclose ( fpIN2 );

    //最大の信号線ID
    fscanf( fpLAST, "%d", &last );
    fclose ( fpLAST );

    fpOUT = fopen( argc[5], "w");

    xor = last;

    // XORTのstartファイルを作成
    sprintf( outputFileName, "%s.start", argc[5]);
    fpOUTSTART = fopen( outputFileName, "w" );
    fprintf( fpOUTSTART, "%d", xor+1 );
    fclose ( fpOUTSTART );

    //　xorのCNFを作っていく
    while ( fscanf( fpLIST, "%d", &literal ) != EOF ) {

    	xor++;                          //出力:初期値はlast(=ここまでの最大の値を持つ信号線)

        xorIn1 = literal + in1 - 1;     //literal(外部出力) + c1の最小ID - 1
        xorIn2 = literal + in2 - 1;     //literal(外部出力) + c2の最小ID - 1

    	// xor in1 in2
    	//  0   0   0
    	//  1   0   1
    	//  1   1   0
    	//  0   1   1

        fprintf( fpOUT, "%d -%d %d 0\n", xor, xorIn1, xorIn2);      //z + -a + b
    	fprintf( fpOUT, "%d %d -%d 0\n", xor, xorIn1, xorIn2);      //z + a + -b
        fprintf( fpOUT, "-%d -%d -%d 0\n", xor, xorIn1, xorIn2);    //-z + -a + -b
    	fprintf( fpOUT, "-%d %d %d 0\n", xor, xorIn1, xorIn2);      //-z + a + b

    }

    or    = xor + 1;    //出力を更新
    orIn1 = last + 1;   //上で作ったXORの出力を入力にする
    orIn2 = last + 2;   //

    //　or-TreeのCNFを作っていく
    while( or != orIn2 ){

    	// or  in1 in2
    	//  0   0   0
    	//  1   0   1
    	//  1   1   0
    	//  1   1   1

    	fprintf( fpOUT, "%d -%d 0\n", or, orIn1);              //z + -a
    	fprintf( fpOUT, "%d -%d 0\n", or, orIn2);              //z + -b
    	fprintf( fpOUT, "-%d %d %d 0\n", or, orIn1, orIn2);    //-z + a + b

    	or    += 1;
    	orIn1 += 2;
    	orIn2 += 2;
    }

    last = or - 1; //最後の1つは作ってないので-1する
    // ORツリーの頂点を1にするため
    fprintf( fpOUT, "%d 0\n", last );

    sprintf( outputFileName, "%s.end", argc[5]);    //xort.end
    fpOUTEND = fopen( outputFileName, "w" );
    fprintf(fpOUTEND,"%d\n", last );

    fclose (fpLIST);
    fclose (fpOUT);
    fclose (fpOUTEND);
    return 0;
}

