#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"
#include "events.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double  completePrimoSecondo(event *newEvent, clock *c, int blockType);
void	completion(block_type blockType, clock *c);
void	arrival(block_type blockType, clock *c);
void 	outsideArrival(clock *c);
void	initBlocks(void);
int     areThereMoreEvents();
int	    startSimulation(void);