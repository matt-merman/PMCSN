//
// Created by giaco on 10/12/22.
//
#ifndef PROGETTOPMCSN_START_H
#define PROGETTOPMCSN_START_H

#include "server.h"
#include "init.h"
#include "events.h"

void process_completion(event *event, clock *c, double current, block *block);
void schedule_arrival_completion_or_enqueue(event *event, clock *c, block *block);
int start_simulation(void);
void debug(clock *system_clock, block **blocks, event *event);
void update_area_stats(double diff, block **blocks);
void schedule_immediate_arrival(int type, clock *c);
void process_arrival(event *event, clock *c, double current, block *block);


#endif //PROGETTOPMCSN_START_H
