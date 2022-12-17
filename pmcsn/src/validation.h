//
// Created by giaco on 17/12/22.
//

#ifndef PROGETTOPMCSN_VALIDATION_H
#define PROGETTOPMCSN_VALIDATION_H

#include "../utils/structs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"

void validateMM1(block* block, statistics* stats);
void validate_population(block **blocks);
void validate_global_queue_time(block **blocks, clock *clock);
void validate_global_response_time(block **blocks, clock *clock);

#endif //PROGETTOPMCSN_VALIDATION_H
