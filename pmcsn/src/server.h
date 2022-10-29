#include <stdio.h>
#include <stdlib.h>
#include "../utils/structs.h"
#include "../utils/constants.h"

int get_busy_server(server **multi_servers, int num);
int get_idle_server(block *block);
int check_servers(block **blocks);
void free_busy_server(block *block, int server_index);
//char * get_server_contents(block *block);
//char	*to_str_block(block_type btype);
