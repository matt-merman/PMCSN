#ifndef SIMULATION_H
#define SIMULATION_H

#include "../utils/structs.h"

void
simulation(network *canteen, long starting_jobs, long *arrived_jobs, sim_type type, long int period, int index,
           int total);
void update_area_stats(event *event, network *canteen);
void process_completion(event *completion_event, block *block, network *canteen);
void process_immediate_arrival(event *arrival_event, timer *c, block *block);
void schedule_immediate_arrival(block *block, timer *c, event *triggering_event, network *canteen);
void process_arrival(event *current_event, timer *c, block *block, sim_type sim_type, long int period);
int termination_conditions(sim_type type, network *canteen, long jobs, const long *arrived_jobs, long int period);

#endif