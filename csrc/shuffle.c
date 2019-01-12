#include<stdlib.h>
#include<time.h>
void shuffle(int gateId[],int randId[],int gate)
{

    for(int i=0;i<gate;i++)
    {
    	srand( (int)time(NULL) );	//乱数SEED設定
        int j = rand()%gate;
        int t = gateId[i];
        gateId[i] = gateId[j];
        gateId[j] = t;
    }

    for(int i=0;i<gate;i++)
    {
        int j = rand()%gate;
        int t = randId[i];
        randId[i] = randId[j];
        randId[j] = t;
    }
}