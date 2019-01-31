/* satのために複製した回路を繋ぐブランチの生成 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argv, char *argc[] ){

    FILE *fpLIST;       //argc[1]:$3 = nomalInput.info
    FILE *fpIN1;        //argc[2]:c1_obf.cnf.start
    FILE *fpIN2;        //argc[3]:c2_obf.cnf.start
    FILE *fpLAST;       //argc[4]:c2_obf.cnf.end
    FILE *fpOUT;        //argc[5]:argc[5](=br).startで出力
    FILE *fpOUTSTART;
    FILE *fpOUTEND;
    FILE *fpOUTKEY;

    int literal; //読み込んだリテラルの値
    int in1;    //             br1 = ( LIST + in1 - 1 )
    int in2;    //           +-----------------------
    int last;    //   stem = |
    int br1;     //  --------+
    int br2;     //  (last+1)| br2 = ( LIST + in2 - 1 )
    int stem;    //          +-------------------------

    char result[10];
    char firstLine[100];
    char outputFileName[100];


    // コマンド引数の確認
    if ( argv != 6 ) {
	printf("USAGE: geneBranch BranchListFile in1.start in2.start last.end OUTPUT_CNFFileName\n");
	return 0;
    }
    //BranchListFile
    // 1 2 3 4 5
    // in1.start
    // 1
    // in2.start
    // 73
    // last.end
    // 144

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

    //c1の最小ID
    fscanf( fpIN1, "%d", &in1 );
    fclose ( fpIN1 );

    //c2の最小ID
    fscanf( fpIN2, "%d", &in2 );
    fclose ( fpIN2 );

    //c2の最大ID(全体のMAX)
    fscanf( fpLAST, "%d", &last );
    fclose ( fpLAST );

    //出力ファイル
    fpOUT = fopen( argc[5], "w");

    stem = last + 1;            //c2のmax + 1

    // branchのstartファイルを作成
    sprintf( outputFileName, "%s.start", argc[5]);
    fpOUTSTART = fopen( outputFileName, "w" );
    fprintf( fpOUTSTART, "%d", stem );
    fclose ( fpOUTSTART );

    sprintf( outputFileName, "%s.brIn", argc[5]);
    fpOUTKEY = fopen( outputFileName, "w" );

    //　branchのCNFを作っていく
    while ( fscanf( fpLIST, "%d", &literal ) != EOF ) {

    	br1 = literal + in1 - 1;   //外部入力リスト + 難読化回路1の最小ID
    	br2 = literal + in2 - 1;   //外部入力リスト + 難読化回路2の最小ID(難読化回路1.end + 1)

        //cnfに変換
     	/*fprintf(fpOUT, "-%d %d 0\n", stem, br1);
        fprintf(fpOUT, "%d -%d 0\n", stem, br1);
     	fprintf(fpOUT, "-%d %d 0\n", stem, br2);
        fprintf(fpOUT, "%d -%d 0\n", stem, br2);*/

        fprintf(fpOUT, "-%d %d 0\n", br1, stem);
        fprintf(fpOUT, "%d -%d 0\n", br1, stem);
        fprintf(fpOUT, "-%d %d 0\n", br2, stem);
        fprintf(fpOUT, "%d -%d 0\n", br2, stem);

        fprintf(fpOUTKEY, "-%d %d 0\n", br2, stem);
        fprintf(fpOUTKEY, "%d -%d 0\n", br2, stem);

    	stem++;
    }


    sprintf( outputFileName, "%s.end", argc[5]);    //br.end
    fpOUTEND = fopen( outputFileName, "w" );
    fprintf(fpOUTEND,"%d\n",stem - 1);

    fclose (fpLIST);
    fclose (fpOUT);
    fclose (fpOUTEND);
    return 0;
}

