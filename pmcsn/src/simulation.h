#ifndef SIMULATION_H
#define SIMULATION_H

#include "../utils/structs.h"
#include "../src/events.h"


void simulation(network *canteen);
void update_area_stats(event *event, network *canteen);
void process_completion(event *completion_event, timer *c, block *block);
void process_immediate_arrival(event *event, timer *c, block *block);
void schedule_immediate_arrival(block* block, timer *c, event *triggering_event);
void process_arrival(event *event, timer *c, block *block);

#endif