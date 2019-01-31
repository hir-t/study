#include <stdio.h>
#include <string.h>

int main(int argv, char *argc[] ){

    FILE *fpSAT;        //argc[1]=f1.cnf.out(satの結果)
    FILE *fpRESULT;

    int literal;

    char result[10];
    char outputName[100];

    fpSAT = fopen( argc[1], "r");
    if ( fpSAT == NULL ) {
        printf("File Open Error %s\n", argc[1]);
        return 1;
    }

    sprintf(outputName, "%s.result", argc[1]);  //f1.result
    fpRESULT= fopen( outputName, "w");
    if ( fpRESULT == NULL ) {
        printf("File Open Error %s\n", outputName );
        return 1;
    }

    fscanf(fpSAT, "%s", result );

    fprintf( fpRESULT, "%s\n", result); //f1.cnf.out(satの結果,１行目)を書き込む

    if ( strcmp(result, "SAT") != 0 ) {
        printf("UNSAT\n");
        return 0;
    }

    //2行目以下(信号線の値)を書き込む
    // > *.datとしているため、.datに書き込まれる
    while ( fscanf(fpSAT, "%d", &literal ) != EOF ) {
        printf("%d\n", literal);
    }
    return 0;
}

