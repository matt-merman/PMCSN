//
// Created by giaco on 10/12/22.
//
#ifndef PROGETTOPMCSN_START_H
#define PROGETTOPMCSN_START_H

#include "server.h"
#include "init.h"
#include "events.h"

void completion(int server_id, clock *c, double current, block *block);
void arrival(clock *c, double current, block *block);
int start_simulation(void);
void update_area_stats(double diff, block **blocks);
void schedule_arrive(int type, clock *c);


#endif //PROGETTOPMCSN_START_H
