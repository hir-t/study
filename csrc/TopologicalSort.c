/* 取得した経路内のノードをレベル順にソートする*/
#include <topgun.h>
#include <topgunLine.h>
void TopologicalSort(int loops, int length,LINE *routeNode[],LINE *data[]){
	extern LINE_INFO Line_info;

	/*	経路内のノードをレベル順にソート */
	LINE *temp[loops*length]; //バブルソート用
	for(int i = 0;i<loops*length;i++){
		for (int j = 1; j < loops*length;j++){
			if (routeNode[j-1]->level > routeNode[j]->level){

				temp[i] = routeNode[j-1];
				routeNode[j-1] = routeNode[j];
				routeNode[j] = temp[i];
			}
		}
	}

	/* 回路内の全信号線をレベル順にソート*/
	LINE *temp2[Line_info.n_line]; //バブルソート用
	for(int i = 0;i<Line_info.n_line;i++){
		for (int j = 1; j < Line_info.n_line;j++){
			if (data[j-1]->level > data[j]->level){

				temp2[i] = data[j-1];
				data[j-1] = data[j];
				data[j] = temp2[i];
			}
		}
	}
}