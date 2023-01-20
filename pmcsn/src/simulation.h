#ifndef SIMULATION_H
#define SIMULATION_H

#include "../utils/structs.h"
#include "../src/events.h"


void simulation(network *canteen, int jobs, int *arrived_jobs, sim_type, long int period);
void update_area_stats(event *event, network *canteen);
void process_completion(event *completion_event, timer *c, block *block, long int period);
void process_immediate_arrival(event *event, timer *c, block *block, long int period);
void schedule_immediate_arrival(block* block, timer *c, event *triggering_event, long int period);
void process_arrival(event *current_event, timer *c, block *block, sim_type sim_type, long int period);
int termination_conditions(sim_type type, network *canteen, int jobs, const int * arrived_jobs, long int period);

#endif