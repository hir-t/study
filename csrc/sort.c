#include <stdio.h>
#include <topgun.h>
#include <topgunLine.h>
void sort(int gate ,int gateId[],LINE *data[])
{

	LINE *temp; //バブルソート用
	for(int i = 0;i<gate;i++){
		for (int j = 1; j < gate;j++){
			if(data[gateId[j-1]]->level > data[gateId[j]]->level){

/*				temp = data[gateId[j-1]]->line_id;
				data[gateId[j-1]]->line_id = data[gateId[j]]->line_id;
				data[gateId[j]]->line_id = temp;*/
				temp = data[gateId[j-1]];
				data[gateId[j-1]] = data[gateId[j]];
				data[gateId[j]] = temp;
			}
		}
	}

/*	for (int j = 0; j < gate;j++){
		printf("ID:%lu,lv:%lu\n", data[gateId[j]]->line_id,data[gateId[j]]->level);
	}*/
}