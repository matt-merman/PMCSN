//
// Created by giaco on 10/12/22.
//
#ifndef PROGETTOPMCSN_START_H
#define PROGETTOPMCSN_START_H

#include "server.h"
#include "init.h"
#include "events.h"

void process_completion(event *completion_event, clock *c, block *block);
void process_immediate_arrival(event *event, clock *c, block *block);
int start_simulation(void);
void debug(clock *system_clock, block **blocks, event *event);
void update_area_stats(block **blocks, event *event, clock *clock);
void schedule_immediate_arrival(int type, clock *c, event *triggering_event);
void process_arrival(event *event, clock *c, block *block);


#endif //PROGETTOPMCSN_START_H
