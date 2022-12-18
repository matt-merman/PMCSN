//
// Created by giaco on 17/12/22.
//

#ifndef PROGETTOPMCSN_VALIDATION_H
#define PROGETTOPMCSN_VALIDATION_H

#include "../utils/structs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"

void validate_block(block * block, statistics *stats);
void validate_MM1(block* block, statistics* stats);
void validate_MMk(block *block, statistics *stats);
void is_ergodic(block *block);
void validate_theoretical_service_time(block *block, statistics *stats);
void validate_theoretical_arrival_time(block *block, statistics *stats);
void is_wait_delay_plus_service(block *block, statistics *stats);
void is_node_population_queue_pop_plus_service_pop(block *block, statistics *stats);
void validate_global_population(block **blocks);
void validate_global_queue_time(double queue_time, double queue_pop);
void validate_global_response_time(double response_time, double mean_population);

#endif //PROGETTOPMCSN_VALIDATION_H
