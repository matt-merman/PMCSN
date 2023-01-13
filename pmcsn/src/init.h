#include "../utils/constants.h"
#include "../utils/structs.h"
#include "../utils/helpers.h"

network *create_network(const char** block_names, int config);
void clear_network(network * n);
block **init_blocks(int *config, const char **block_names);
void restart_blocks(network *canteen);
void	init_servers(block *block, int num);
timer * 	init_clock(void);
int *	init_network(int config);

