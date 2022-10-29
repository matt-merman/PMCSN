#include "../utils/constants.h"
#include "../utils/structs.h"
#include "../utils/helpers.h"

block	**init_blocks(int *config);
void	init_servers(block *block, int num);
clock * 	init_clock(void);
int *	init_network(int config);

