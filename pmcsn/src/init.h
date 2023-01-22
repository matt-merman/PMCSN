#ifndef _INIT_H_
#define _INIT_H_

#include "../utils/structs.h"

network *create_network(const char** block_names, int config);
void clear_network(network *n, int clear_timer);
block **init_blocks(int *config, const char **block_names);
void restart_blocks(network *canteen);
void	init_servers(block *block, int num);
timer * 	init_clock(void);
int *	init_network(int config);

#endif
