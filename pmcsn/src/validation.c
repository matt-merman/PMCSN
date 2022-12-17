//
// Created by giaco on 17/12/22.
//

#include "validation.h"

void validateMM1(block *block, statistics *stats) {
    if (IS_NOT_EQUAL(stats->wait, stats->delay + stats->service_time))
    {
        printf("\tResponse time of block %18s: %6.2lf s,\tbut it's not equal to queue delay plus service time: \t%6.2lf s\n",
               block->name, stats->wait, stats->delay + stats->service_time);
    }
    if (IS_NOT_EQUAL(stats->node_pop, stats->queue_pop + stats->utilization))
    {
        printf("\tPopulation of block    %18s: %6.2lf,\tbut it's not equal to queue plus service population: \t%6.2lf\n",
               block->name, stats->node_pop, stats->node_pop + (block->num_servers* stats->utilization));
    }
}

// Check if the entering population is equal to the exiting population.
void validate_population(block **blocks) {
    long total_population = blocks[CASSA_FAST]->completed_jobs + blocks[CASSA_STD]->completed_jobs;
    // FIXME: per ora abbiamo messo una coda anche nel locale mensa. L'idea iniziale era quella di toglierla però. Decidere se lasciare così o metterla di nuovo
    if (total_population != blocks[CONSUMAZIONE]->completed_jobs){
        printf("The sum of completed jobs %ld is not equal to the starting population %ld", total_population, blocks[CONSUMAZIONE]->completed_jobs);
    }
}

void validate_global_queue_time(block **blocks, clock *clock) {
    // TODO
    printf("The global queue time is not equal to the sum???")
}

void validate_global_response_time(block **blocks, clock *clock) {
    // TODO
    printf("The global response time is not equal to the sum???")
}
