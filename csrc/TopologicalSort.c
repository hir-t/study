/* 取得した経路内のノードをレベル順にソートする*/
#include <topgun.h>
#include <topgunLine.h>
void TopologicalSort(int loops, int length,LINE *routeNode[loops*length]){

	/*	取得したものをレベル順にソート */
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
}