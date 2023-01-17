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
#include "simulation.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int start_standard_simulation(int config);
int start_finite_horizon_simulation(int config, FILE* file, int num_replicas);
int start_infinite_horizon_simulation(int config);
void calculate_all_interval_estimate(network *canteen, int num_replicas);
int batch_simulation(network *canteen, int batch_index);

#endif //PROGETTOPMCSN_START_H
