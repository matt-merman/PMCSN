#ifndef _SERVER_H_
#define _SERVER_H_

#include "../utils/structs.h"

int get_busy_server(server **multi_servers, int num);
int get_busy_server_num();
int get_busy_server_num_for_block(block * b);
int retrieve_idle_server(block *block);
int are_there_busy_servers(block **blocks);
void free_busy_server(block *block, int server_index);
char * get_server_contents(block *block);
char	*to_str_block(block_type btype);

#endif
