#include "../utils/constants.h"
#include "../utils/structs.h"
#include "../utils/helpers.h"
#include "../utils/structs.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

block	**initBlocks(int *config);
void	initServers(block *block, int num);
clock * 	initClock(void);
int *	initNetworkStatus(int config);

