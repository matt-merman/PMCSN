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
void	completion(int blockType, clock *c);
void	arrival(int blockType, clock *c);
void	initBlocks(void);

int	    startSimulation(void);