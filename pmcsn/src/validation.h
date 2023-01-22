//
// Created by giaco on 17/12/22.
//

#ifndef PROGETTOPMCSN_VALIDATION_H
#define PROGETTOPMCSN_VALIDATION_H

#include "../utils/structs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"
#include "../src/analytic.h"

#define CHECK_DOUBLE_EQUAL(x,y,block_name,prec,msg) do {if (((MAX((x),(y)) - MIN((x),(y)) <= (prec)) ? 0 : 1)) { \
            printf("\tBlock %s: %s %g doesn't match with computed %s %g\n", \
            (block_name), (msg), (x), (msg), (y)); \
        }} while(0)

double get_simulation_visit(network *canteen, block_type block);
double get_simulation_routing_prob(network *n, block_type from, block_type to);
void validate_block(block * block, statistics *stats);
void validate_MM1(block* block, statistics* stats);
void validate_MMk(block *block, statistics *stats);
void is_ergodic(block *block);
void validate_theoretical_service_time(block *block, statistics *stats);
void validate_theoretical_arrival_time(block *block, statistics *stats);
void validate_theoretical_utilizazion(block *block, statistics *stats);
void is_wait_delay_plus_service(block *block, statistics *stats);
void is_node_population_queue_pop_plus_service_pop(block *block, statistics *stats);
void validate_global_population(block **blocks);
void validate_global_response_time(double response_time, int *network_servers);
void validate_ploss(double ploss, int m);
double probe_global_simulation_response_time(network *canteen, long int period);
double probe_global_simulation_loss_probability(network *canteen, long int period);
// infinite-horizon
void validate_batch_means_response_time(area area[BLOCKS], const long completed_jobs[BLOCKS], int network_servers[BLOCKS], const double batch_response_times[K_BATCH]);
void validate_batch_means_loss_probability(long long int rejected_jobs, long long int total_jobs, const double batch_loss_probabilities[K_BATCH]);
#endif //PROGETTOPMCSN_VALIDATION_H
