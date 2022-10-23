#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"
#include "events.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void	completion(event *event, clock *c);
void	arrival(event *event, clock *c, event_type arrival);
void	initBlocks(void);
int     areThereMoreEvents(void);
int	    startSimulation(void);