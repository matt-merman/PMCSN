//
// Created by giaco on 10/12/22.
//
#ifndef PROGETTOPMCSN_START_H
#define PROGETTOPMCSN_START_H

#include "server.h"
#include "init.h"
#include "events.h"
#include "../utils/files.h"
#include "event_list.h"
#include "../libs/estimate.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void process_completion(event *completion_event, clock *c, block *block);
void process_immediate_arrival(event *event, clock *c, block *block);
int start_standard_simulation();
int start_finite_horizon_simulation();
int start_infinite_horizon_simulation();
void update_area_stats(block **blocks, event *event, clock *clock);
void schedule_immediate_arrival(int type, clock *c, event *triggering_event);
void process_arrival(event *event, clock *c, block *block);
void simulation(clock *system_clock, block **blocks);
void calculate_interval_estimate();

#endif //PROGETTOPMCSN_START_H
