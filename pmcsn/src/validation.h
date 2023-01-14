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
void validate_global_queue_time(double queue_time, double queue_pop);
void validate_global_response_time(double response_time, int *network_servers);

#endif //PROGETTOPMCSN_VALIDATION_H
