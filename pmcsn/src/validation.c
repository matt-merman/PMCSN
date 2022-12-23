//
// Created by giaco on 17/12/22.
// Consistency checks for validating that our simulation is compliant with the real system
//

#include "validation.h"

void validate_block(block *block, statistics *stats) {
    if (block->num_servers == 1) {
        validate_MM1(block, stats);
    } else {
        validate_MMk(block, stats);
    }
}

void validate_MM1(block *block, statistics *stats) {
    is_wait_delay_plus_service(block, stats);
    is_node_population_queue_pop_plus_service_pop(block, stats);
    validate_theoretical_arrival_time(block, stats);
    validate_theoretical_service_time(block, stats);
    is_ergodic(block);
}

// Erlang-C infinity queue
void validate_MMk(block *block, statistics *stats) {
    int m = block->num_servers;
    double rho = stats->utilization;
    double rho_theoretical = get_theoretical_rho(block);
    double service_theoretical = get_theoretical_service(block->type);
    double block_probability_theoretical = erlang_c_block_probability(m, rho_theoretical);
    double block_probability = erlang_c_block_probability(m, rho);
    // checking erlangC queue time with theoretical formula
    double queue_time_theoretical = erlang_c_queue_time(block_probability_theoretical,
                                                        service_theoretical / m,
                                                        rho_theoretical);
    double queue_time = erlang_c_queue_time(block_probability, stats->service_time / m, rho); // here we need the multiserver service time (time to free any server)

    if (IS_NOT_EQUAL(queue_time_theoretical, queue_time)) {
        printf("\tBlock %s: Theoretical queue time %g doesn't match with computed queue time %g\n",
               block->name, queue_time_theoretical, queue_time);
    }

    // checking erlangC response time with theoretic formula
    double response_time_theoretical = erlang_c_response_time(queue_time_theoretical,
                                                              service_theoretical);
    double response_time = erlang_c_response_time(queue_time, stats->service_time); // here we need the full service time of a single server

    if (IS_NOT_EQUAL(response_time_theoretical, response_time)) {
        printf("\tBlock %s: Theoretical response time %g doesn't match with computed response time %g\n",
               block->name, response_time_theoretical, response_time);
    }
    validate_theoretical_arrival_time(block, stats);
    validate_theoretical_service_time(block, stats);
    is_ergodic(block);
}

/* Solo se aggiungiamo la coda a CONSUMAZIONE
void validate_MMkk(block *block, statistics *stats){

}
*/


void is_wait_delay_plus_service(block *block, statistics *stats) {
    if (IS_NOT_EQUAL(stats->wait, stats->delay + stats->service_time)) {
        printf("\tBlock %s: Computed response time (%lf s) it's not equal to queue delay plus service time (%lf s)\n",
               block->name, stats->wait, stats->delay + stats->service_time);
    }
}

void is_node_population_queue_pop_plus_service_pop(block *block, statistics *stats) {
    if (IS_NOT_APPROX_EQUAL(stats->node_pop, stats->queue_pop + stats->utilization)) {
        printf("\tBlock %s: Population (%4.2lf) it's not equal to queue plus service population (%4.2lf) \n",
               block->name, stats->node_pop, stats->queue_pop + stats->utilization);
    }
}

// checks that rho < 1
void is_ergodic(block *block) {
    if (block->num_servers == 1) {
        double lambda = get_theoretical_lambda(block->type);
        double mhu = get_theoretical_mhu(block->type);
        if (lambda > mhu) {
            printf("\tWATCH OUT! Block %s is NOT ergodic. Lambda: %g > Mhu: %g\n", block->name, lambda, mhu);
        }
    } else if (block->num_servers > 1 /*TODO: && ha coda infinita*/) {
        double rho = get_theoretical_rho(block);
        if (rho >= 1) {
            printf("\tWATCH OUT! Multiserver Block %s is NOT ergodic. Rho: %g >= 1\n", block->name, rho);
        }
    }
}

// Checks that computed service time is approximately equal to theorical service time defined in constants.h
void validate_theoretical_service_time(block *block, statistics *stats) {
    double service_theoretical = get_theoretical_service(block->type);
    if (IS_NOT_APPROX_EQUAL(stats->service_time, service_theoretical)) {
        printf("\tBlock %s: theoretical service time (%g) doesn't match computed service time (%g)\n",
               block->name, service_theoretical, stats->service_time);
    }
}

void validate_theoretical_arrival_time(block *block, statistics *stats) {
    double lambda_theoretical = get_theoretical_lambda(block->type);
    double lambda = 1.0 / stats->interarrival_time;
    if (IS_NOT_APPROX_EQUAL(lambda, lambda_theoretical)){
        printf("\tBlock %s: theoretical arrival freuency (%g) doesn't match computed arrival frequency (%g)\n",
               block->name, lambda_theoretical, lambda);
    }
}

// Check if the entering population is equal to the exiting population.
void validate_global_population(block **blocks) {
    long total_population = blocks[CASSA_FAST]->completed_jobs + blocks[CASSA_STD]->completed_jobs;
    if (total_population != blocks[CONSUMAZIONE]->completed_jobs) {
        printf("Global Population: The sum of completed jobs %ld is not equal to the starting population %ld",
               total_population, blocks[CONSUMAZIONE]->completed_jobs); // se togli la coda sottrai i job perduti
    }
}

// TODO: valida il tempo in coda usando la legge di Little
/**
 * Checks that sum of mean queue times is equal to the global queue time from Little's Law.
 * E[Tq] = E[Nq]/lambda
 * This works only if the system is ergodic
 * @param queue_time sum of time spent in queues in all block
 * @param queue_pop sum of queue population in all blocks
 */
//void validate_global_queue_time(double erlang_c_queue_time_test, double queue_pop) {
//    // TODO: ricavare la formula per calcolare i tempi in coda
//    // TODO: verificare che la somma dei tempi in coda sia pari al valore teorico
//    double global_queue_time_theoretic = queue_pop / LAMBDA;
//    if (IS_NOT_APPROX_EQUAL(erlang_c_queue_time_test, global_queue_time_theoretic) ){
//        printf("TODO: The computed global queue time doesn't match with the one from Little's Law\n");
//    }
//}
/**
 *
 * @param response_time
 * @param mean_population
 */
//void validate_global_response_time(double response_time, double mean_population) {
//    // TODO: verificare che la somma dei tempo di risposta dei singoli blocchi sia pari al valore teorico
//    // TODO: ricavare la formula per calcolare il tempo di risposta globale
//    // printf("TODO: The computed global response time doesn't match with the one from Little's Law\n");
//}
