#include <stdio.h>
#include <stdlib.h> //exit()ÍÑ
#include <string.h>

#include <topgun.h>
#include <topgunLine.h>

void line_point(){
	extern LINE_INFO Line_info;
	extern LINE *Line_head;
	LINE *line;


	for(int i=0; i < Line_info.n_line; i++){
		line = &(Line_head[i]);
		printf("-------------------------\n");
		printf("line_id:%lu\n",line->line_id);
		printf("type:%u\n",line->type );

		for (int j = 0; j < line->n_in; j++){
			printf("in:%lu\n",line->in[j]->line_id);
		}

		for (int j = 0; j < line->n_out; j++){
			printf("out:%lu\n",line->out[j]->line_id);
		}

	}
	printf("-------------------------\n");
}