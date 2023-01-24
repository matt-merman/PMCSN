#ifndef _VERIFY_H_
#define _VERIFY_H_

#include "../utils/structs.h"

#define CHECK_DOUBLE_EQUAL(x,y,block_name,prec,msg) do {if (((MAX((x),(y)) - MIN((x),(y)) <= (prec)) ? 0 : 1)) { \
            printf("\tBlock %s: %s %g doesn't match with computed %s %g\n", \
            (block_name), (msg), (x), (msg), (y)); \
        }} while(0)

double get_simulation_visit(network *canteen, block_type block);
double get_simulation_routing_prob(network *n, block_type from, block_type to);
void verify_block(block * block, statistics *stats);
void verify_MM1(block* block, statistics* stats);
void verify_MMk(block *block, statistics *stats);
void is_ergodic(block *block);
void verify_theoretical_service_time(block *block, statistics *stats);
void verify_theoretical_arrival_time(block *block, statistics *stats);
void verify_theoretical_utilizazion(block *block, statistics *stats);
void is_wait_delay_plus_service(block *block, statistics *stats);
void is_node_population_queue_pop_plus_service_pop(block *block, statistics *stats);
void verify_global_population(block **blocks);
void verify_global_response_time(double response_time, int *network_servers);
void verify_ploss(network *canteen);
double probe_global_simulation_response_time(network *canteen, long int period);
double probe_global_simulation_loss_probability(network *canteen);
// infinite-horizon
void verify_batch_means_response_time(area area[BLOCKS], const long completed_jobs[BLOCKS], const double batch_response_times[K_BATCH]);
void verify_batch_means_loss_probability(long long int rejected_jobs, long long int total_jobs, const double batch_loss_probabilities[K_BATCH]);

#endif
