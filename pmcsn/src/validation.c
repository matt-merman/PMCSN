//
// Created by giaco on 17/12/22.
// Consistency checks for validating that our simulation is compliant with the real system
//

#include "validation.h"

double get_simulation_visit(network *canteen, block_type block_type) {
    block *consumazione = NULL;
    // count arrivals from each block to the target block
    long int arrivals_to_block = 0L;
    for (int i = 0; i < BLOCKS; i++) {
        arrivals_to_block += (long) canteen->blocks[i]->count_to_next[block_type];
    }

    consumazione = canteen->blocks[CONSUMAZIONE];
    long int rejected_jobs = consumazione->rejected_jobs;
    long int entered_jobs = (consumazione->completed_jobs + rejected_jobs);

    if (block_type <= SECONDO) { // PRIMO E SECONDO
        arrivals_to_block += (long) canteen->blocks[block_type]->count_to_next[6];
    } else if (block_type == CONSUMAZIONE) {
        arrivals_to_block -= rejected_jobs;
    }

    return (double) arrivals_to_block / (double) entered_jobs;
}

double get_simulation_routing_prob(network *n, block_type from, block_type to) {
    long jobs_entered_in_network = n->blocks[CASSA_FAST]->completed_jobs + n->blocks[CASSA_STD]->completed_jobs;
    if (from == ESTERNO && to == PRIMO){
        return (double) n->blocks[PRIMO]->completed_jobs / (double) jobs_entered_in_network;
    } else if (from == ESTERNO && to == SECONDO){
        long jobs_from_primo_to_secondo = n->blocks[PRIMO]->count_to_next[SECONDO];
        return (double) (n->blocks[PRIMO]->completed_jobs - jobs_from_primo_to_secondo) / (double) jobs_entered_in_network;
    } else if (to == ESTERNO) {
        block * cons = n->blocks[CONSUMAZIONE];
        return (double) jobs_entered_in_network / (double) (cons->completed_jobs + cons->rejected_jobs);
    }
    long exiting_jobs = n->blocks[from]->count_to_next[to];
    long entering_jobs = n->blocks[from]->completed_jobs; // we exclude exiting jobs in CONSUMAZIONE
    return (double) exiting_jobs / (double) entering_jobs;
}

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
    validate_theoretical_utilizazion(block, stats);
    is_ergodic(block);
}

// Erlang-C infinity queue
void validate_MMk(block *block, statistics *stats) {
    int m = block->num_servers;
//    double rho = stats->utilization;
    double rho_theoretical = get_theoretical_rho(block->type, block->num_servers);
    double service_theoretical = get_theoretical_service(block->type);
    double block_probability_theoretical = erlang_c_block_probability(m, rho_theoretical);
//    double block_probability = erlang_c_block_probability(m, rho);
    // checking erlangC queue time with theoretical formula
    double queue_time_theoretical = erlang_c_queue_time(block_probability_theoretical,
                                                        service_theoretical / m,
                                                        rho_theoretical);
//    double queue_time = erlang_c_queue_time(block_probability, stats->service_time / m,
//                                            rho); // here we need the multiserver service time (time to free any server)

    if (block->type == CONSUMAZIONE)
        queue_time_theoretical = 0;

    CHECK_DOUBLE_EQUAL(queue_time_theoretical, stats->delay, block->name, 0.001, "queue time");

    // checking erlangC response time with theoretic formula
    double response_time_theoretical = erlang_c_response_time(queue_time_theoretical,
                                                              service_theoretical);
//    double response_time = erlang_c_response_time(queue_time,
//                                                  stats->service_time); // here we need the full service time of a single server

    CHECK_DOUBLE_EQUAL(response_time_theoretical, stats->wait, block->name, 0.001, "response time");

    validate_theoretical_arrival_time(block, stats);
    validate_theoretical_service_time(block, stats);
    validate_theoretical_utilizazion(block, stats);
    is_ergodic(block);
}


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
        double lambda = get_theoretical_lambda_raw(block->type);
        double mhu = get_theoretical_mhu(block->type);
        if (lambda > mhu) {
            printf("\tWATCH OUT! Block %s is NOT ergodic. Lambda: %g > Mhu: %g\n", block->name, lambda, mhu);
        }
    } else if (block->num_servers > 1) {
        double rho = get_theoretical_rho(block->type, block->num_servers);
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
    double lambda_theoretical = get_theoretical_lambda(block->type, block->num_servers);
    double lambda = 1.0 / stats->interarrival_time;
    if (IS_NOT_EQUAL(lambda, lambda_theoretical)) {
        printf("\tBlock %s: theoretical arrival frequency (%g) doesn't match computed arrival frequency (%g)\n",
               block->name, lambda_theoretical, lambda);
    }
}

void validate_theoretical_utilizazion(block *block, statistics *stats) {
    double utilization_theoretical = get_theoretical_rho(block->type, block->num_servers);
    double utilization = stats->utilization;
    if (IS_NOT_EQUAL(utilization, utilization_theoretical)) {
        printf("\tBlock %s: theoretical utilization (%g) doesn't match computed utilization (%g)\n",
               block->name, utilization_theoretical, utilization);
    }
}

// Check if the entering population is equal to the exiting population.
void validate_global_population(block **blocks) {
    long total_population = blocks[CASSA_FAST]->completed_jobs + blocks[CASSA_STD]->completed_jobs;
    long computed_tot_pop = blocks[CONSUMAZIONE]->completed_jobs + blocks[CONSUMAZIONE]->rejected_jobs;
    if (total_population != computed_tot_pop) {
        printf("Global Population: The sum of completed jobs %ld is not equal to the starting population %ld\n",
               total_population, computed_tot_pop); // senza coda sommiamo i job perduti
    }
}

/**
 *
 * @param response_time
 * @param network_servers
 */
void validate_global_response_time(double response_time, int *network_servers) {
    double global_wait_theoretical = get_theoretical_global_response_time(network_servers);
    printf("\tThe computed global response time (%f) doesn't match with the theoretical global response time (%f)\n",
           response_time, global_wait_theoretical);
}

/**
 * Used to get partial global simulation response time up until a period
 * @param canteen partial or end state of the network
 * @param period period until get simulation time
 * @return partial global response time
 */
double probe_global_simulation_response_time(network *canteen, long int period) {
    double global_wait = 0.0;
    statistics stats;

    for (int i = 0; i<BLOCKS; i++){
        get_stats(canteen->blocks[i], canteen->system_clock, &stats, period);
        global_wait += stats.wait * get_theoretical_visits(i, canteen->blocks[i]->num_servers);
        clear_stats(&stats);
    }
    return global_wait;
}

/**
 * Used to get partial simulation loss probability up until a period
 * @param canteen
 * @param period
 * @return
 */
double probe_global_simulation_loss_probability(network *canteen, long int period) {
    statistics stats;
    get_stats(canteen->blocks[CONSUMAZIONE], canteen->system_clock, &stats, period);
    double partial_loss_probability = stats.loss_probability;
    clear_stats(&stats);
    return partial_loss_probability;
}