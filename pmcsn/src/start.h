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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void process_completion(event *completion_event, timer *c, block *block);
void process_immediate_arrival(event *event, timer *c, block *block);
int start_standard_simulation(int config);
int start_finite_horizon_simulation(int config);
int start_infinite_horizon_simulation(int config);
void update_area_stats(event *event, network *canteen);
void schedule_immediate_arrival(block* block, timer *c, event *triggering_event);
void process_arrival(event *event, timer *c, block *block);
void simulation(network *canteen);
void calculate_all_interval_estimate(network *canteen);

#endif //PROGETTOPMCSN_START_H
