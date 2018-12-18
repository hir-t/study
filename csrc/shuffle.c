#include<stdlib.h>
#include<time.h>
void shuffle(int ary[],int size)
{
	srand( (int)time(NULL) );	//乱数SEED設定
    for(int i=0;i<size;i++)
    {
        int j = rand()%size;
        int t = ary[i];
        ary[i] = ary[j];
        ary[j] = t;
    }
}