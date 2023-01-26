#ifndef _START_H_
#define _START_H_

#include "../utils/structs.h"

int start_standard_simulation();
int start_finite_horizon_simulation(FILE *file_grt, FILE *file_ploss, int num_replicas);
int start_infinite_horizon_simulation(long int period);
void calculate_all_interval_estimate(network *canteen, int num_replicas);

#endif