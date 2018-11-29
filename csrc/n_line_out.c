#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>


void n_line_out(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	LINE *line;

	printf("n_line:%lu\n",Line_info.n_line);

	for (int i = 0; i < Line_info.n_line; i++)
	{
		line = &(Line_head[i]);

		printf("-------------------------\n");
		printf("line_id:%lu\n",line->line_id);
		printf("n_in:%lu\n",line->n_in);
		printf("n_out:%lu\n",line->n_out);
		printf("入力レベル:%lu\n",line->lv_pi);
		printf("出力レベル:%lu\n",line->lv_po);
	}
	printf("-------------------------\n");

}