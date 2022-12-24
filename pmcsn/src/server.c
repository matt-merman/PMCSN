#include "./server.h"

int busy_servers[BLOCKS] = {0,0,0,0,0,0};

int	get_busy_server(server **multi_servers, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (multi_servers[i]->status == BUSY)
			return (i);
	}
	return (-1);
}

int get_busy_server_num_for_block(block * b){
    return busy_servers[b->type];
}

int get_busy_server_num() {
    int sum = 0;
    for (int i = 0; i < BLOCKS; i++){
        sum += busy_servers[i];
    }
    return sum;
}

int retrieve_idle_server(block *block)
{
    server** multi_servers = block->servers;
    for (int i = 0; i < block->num_servers; i++) {
        if (multi_servers[i]->status == IDLE) {
            multi_servers[i]->status = BUSY;
            busy_servers[block->type]++;
            return i;
        }
	}
	return -1;
}

void free_busy_server(block *block, int server_index){
    block->servers[server_index]->status = IDLE;
    busy_servers[block->type]--;
}
// checks if there is at least one busy server
int	are_there_busy_servers(block **blocks)
{
	int num;
    for (int s = 0; s < BLOCKS; s++)
	{
        // check if there are busy server for block s
		num = busy_servers[blocks[s]->type];
        if(num > 0){
            return TRUE;
        }
	}
	return FALSE;
}

char *to_str_block(block_type btype) {
    switch (btype) {
        case PRIMO:
            return ("PRIMO");
        case SECONDO:
            return ("SECONDO");
        case DESSERT:
            return ("DESSERT");
        case CASSA_FAST:
            return ("CASSA_FAST");
        case CASSA_STD:
            return ("CASSA_STD");
        case CONSUMAZIONE:
            return ("CONSUMAZIONE");
        default:
            return ("");
    }
}

 char * get_server_contents(block *block){
     char *string = (char *) malloc(sizeof(char) * 100);
     int in_service = busy_servers[block->type];
     long queue_minus_service = block->jobs - (long) in_service;
     long in_queue = 0L;
     if (queue_minus_service > 0L){
         in_queue = queue_minus_service;
     }

     sprintf(string, "%s |%ld|(%d)",to_str_block(block->type), in_queue, busy_servers[block->type]);
     return string;
 }

