#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"
#include "events.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void completion(int server_id, clock *c, double current, block *block);
void arrival(clock *c, double current, block *block);
int areThereMoreEvents(void);
int startSimulation(void);
void updateStats(double diff, block **blocks);
void scheduleArrive(int type, clock *c);