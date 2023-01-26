#include "verify.h"

double get_simulation_visit(network *canteen, block_type block_type) {
    // count arrivals from each block to the target block
    long int arrivals_to_block = 0L;
    for (int i = 0; i < BLOCKS; i++) {
        arrivals_to_block += (long) canteen->blocks[i]->count_to_next[block_type];
    }

    block *consumazione = canteen->blocks[CONSUMAZIONE];
    long int rejected_jobs = consumazione->rejected_jobs;
    long int entered_jobs = (consumazione->completed_jobs + rejected_jobs);

#ifdef EXTENDED
    block *consumazione2 = canteen->blocks[CONSUMAZIONE_2];
    long int rejected_jobs2 = consumazione2->rejected_jobs;
    entered_jobs += (consumazione2->completed_jobs + rejected_jobs2);
#endif

    if (block_type <= SECONDO) { // PRIMO E SECONDO
        arrivals_to_block += (long) canteen->blocks[block_type]->count_to_next[ESTERNO];
    } else if (IS_CONSUMAZIONE(block_type)) {
        arrivals_to_block -= rejected_jobs;
    }

    return (double) arrivals_to_block / (double) entered_jobs;
}

double get_simulation_routing_prob(network *n, block_type from, block_type to) {
    long jobs_entered_in_network = n->blocks[CASSA_FAST]->completed_jobs + n->blocks[CASSA_STD]->completed_jobs;
    if (from == ESTERNO && to == PRIMO) {
        return (double) n->blocks[PRIMO]->completed_jobs / (double) jobs_entered_in_network;
    } else if (from == ESTERNO && to == SECONDO) {
        long jobs_from_primo_to_secondo = n->blocks[PRIMO]->count_to_next[SECONDO];
        return (double) (n->blocks[PRIMO]->completed_jobs - jobs_from_primo_to_secondo) /
               (double) jobs_entered_in_network;
    } else if (to == ESTERNO) {
#ifndef EXTENDED
        block *consumazione = n->blocks[CONSUMAZIONE];
        return (double) jobs_entered_in_network / (double) (consumazione->completed_jobs + consumazione->rejected_jobs);
#else
        block *consumazione = n->blocks[CONSUMAZIONE];
        block *consumazione2 = n->blocks[CONSUMAZIONE_2];
        return (double) jobs_entered_in_network / (double) (consumazione->completed_jobs +
                                                            consumazione->rejected_jobs +
                                                            consumazione2->completed_jobs +
                                                            consumazione2->rejected_jobs);
#endif
    }
    long exiting_jobs = n->blocks[from]->count_to_next[to];
    long entering_jobs = n->blocks[from]->completed_jobs; // we exclude exiting jobs in CONSUMAZIONE
    return (double) exiting_jobs / (double) entering_jobs;
}

void verify_block(block *block, statistics *stats) {
    if (block->num_servers == 1) {
        verify_MM1(block, stats);
    } else {
        verify_MMk(block, stats);
    }
}

void verify_MM1(block *block, statistics *stats) {
    is_wait_delay_plus_service(block, stats);
    is_node_population_queue_pop_plus_service_pop(block, stats);
    verify_theoretical_arrival_time(block, stats);
    verify_theoretical_service_time(block, stats);
    verify_theoretical_utilizazion(block, stats);
    is_ergodic(block);
}

// Erlang-C infinity queue or Erlang-B for CONSUMAZIONE
void verify_MMk(block *block, statistics *stats) {
    int m = block->num_servers;
    double rho_theoretical = get_theoretical_rho(block->type, block->num_servers);
    double service_theoretical = get_theoretical_service(block->type);
    double block_probability_theoretical = erlang_c_block_probability(m, rho_theoretical);
    // checking erlangC queue time with theoretical formula
    double queue_time_theoretical = erlang_c_queue_time(block_probability_theoretical,
                                                        service_theoretical / m,
                                                        rho_theoretical);

    if (IS_CONSUMAZIONE(block->type))
        queue_time_theoretical = 0;

    CHECK_DOUBLE_EQUAL(queue_time_theoretical, stats->delay, block->name, 0.001, "queue time");

    // checking erlangC response time with theoretic formula
    double response_time_theoretical = erlang_c_response_time(queue_time_theoretical,
                                                              service_theoretical);

    CHECK_DOUBLE_EQUAL(response_time_theoretical, stats->wait, block->name, 0.001, "response time");

    verify_theoretical_arrival_time(block, stats);
    verify_theoretical_service_time(block, stats);
    verify_theoretical_utilizazion(block, stats);
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
        double lambda = get_theoretical_lambda(block->type, block->num_servers);
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
void verify_theoretical_service_time(block *block, statistics *stats) {
    double service_theoretical = get_theoretical_service(block->type);
    if (IS_NOT_APPROX_EQUAL(stats->service_time, service_theoretical)) {
        printf("\tBlock %s: theoretical service time (%g) doesn't match computed service time (%g)\n",
               block->name, service_theoretical, stats->service_time);
    }
}

void verify_theoretical_arrival_time(block *block, statistics *stats) {
    double lambda_theoretical = get_theoretical_lambda(block->type, block->num_servers);
    double lambda = 1.0 / stats->interarrival_time;
    if (IS_NOT_EQUAL(lambda, lambda_theoretical)) {
        printf("\tBlock %s: theoretical arrival frequency (%g) doesn't match computed arrival frequency (%g)\n",
               block->name, lambda_theoretical, lambda);
    }
}

void verify_theoretical_utilizazion(block *block, statistics *stats) {
    double utilization_theoretical = get_theoretical_rho(block->type, block->num_servers);
    double utilization = stats->utilization;
    if (IS_NOT_EQUAL(utilization, utilization_theoretical)) {
        printf("\tBlock %s: theoretical utilization (%g) doesn't match computed utilization (%g)\n",
               block->name, utilization_theoretical, utilization);
    }
}

// Check if the entering population is equal to the exiting population.
void verify_global_population(block **blocks) {
    long total_population = blocks[CASSA_FAST]->completed_jobs + blocks[CASSA_STD]->completed_jobs;
    long computed_tot_pop = blocks[CONSUMAZIONE]->completed_jobs + blocks[CONSUMAZIONE]->rejected_jobs;
#ifdef EXTENDED
    computed_tot_pop += blocks[CONSUMAZIONE_2]->completed_jobs + blocks[CONSUMAZIONE_2]->rejected_jobs;
#endif
    if (total_population != computed_tot_pop) {
        printf("Global Population: The sum of completed jobs %ld is not equal to the starting population %ld\n",
               total_population, computed_tot_pop);
    }
}

/**
 * Validates that response time is equal to wait
 * @param response_time
 * @param network_servers
 */
void verify_global_response_time(double response_time, int *network_servers) {
    double global_wait_theoretical = get_theoretical_global_response_time(network_servers);
    if (IS_NOT_EQUAL(global_wait_theoretical, response_time)) {
        printf("\tThe computed global response time (%f) doesn't match with the theoretical global response time (%f)\n",
               response_time, global_wait_theoretical);
    }
}

/**
 * This function validates the ploss of the canteen
 * @param canteen the network
 */
void verify_ploss(network *canteen) {
#ifndef EXTENDED

    int m = canteen->blocks[CONSUMAZIONE]->num_servers;
    long double ploss_theoretical = erlang_b_loss_probability(m, get_theoretical_lambda_raw(CONSUMAZIONE),
                                                              get_theoretical_mhu(CONSUMAZIONE));
#else
    // the loss probability is the sum for each i of (probability at block i multiplied by the visits)
    int m_1 = canteen->blocks[CONSUMAZIONE]->num_servers;
    int m_2 = canteen->blocks[CONSUMAZIONE_2]->num_servers;

    long double ploss_theoretical_1 = erlang_b_loss_probability(m_1, get_theoretical_lambda_raw(CONSUMAZIONE),
                                                                get_theoretical_mhu(CONSUMAZIONE));
    long double ploss_theoretical_2 = erlang_b_loss_probability(m_2, get_theoretical_lambda_raw(CONSUMAZIONE_2),
                                                                get_theoretical_mhu(CONSUMAZIONE_2));

    long double ploss_theoretical = ploss_theoretical_1 * get_theoretical_visits(CONSUMAZIONE)
                                     + ploss_theoretical_2 * get_theoretical_visits(CONSUMAZIONE_2);
#endif

    long double ploss = canteen->global_loss_probability;

    if (IS_NOT_EQUAL(ploss_theoretical, ploss)) {
        printf("\tThe computed loss probability (%Lf) doesn't match with the theoretical loss probability (%Lf)\n",
               (long double) ploss, ploss_theoretical);
    }
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

    for (int i = 0; i < BLOCKS; i++) {
        get_stats(canteen->blocks[i], canteen->system_clock, &stats, period);
        global_wait += stats.wait * get_simulation_visit(canteen, i);
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
double probe_global_simulation_loss_probability(network *canteen) {
    double partial_loss_probability;
    long rejected_jobs;
    long total_jobs;
    block *b = canteen->blocks[CONSUMAZIONE];

    rejected_jobs = b->rejected_jobs;
    total_jobs = b->completed_jobs + b->rejected_jobs;

#ifdef EXTENDED
    block *b2 = canteen->blocks[CONSUMAZIONE_2];
    rejected_jobs += b2->rejected_jobs;
    total_jobs += b2->completed_jobs + b2->rejected_jobs;
#endif

    partial_loss_probability = (double) rejected_jobs / (double) total_jobs;
    return partial_loss_probability;
}

void verify_batch_means_response_time(area area[BLOCKS], const long completed_jobs[BLOCKS], const double batch_response_times[K_BATCH]) {
    double total_response_time = 0.0;
    // Calculate response time from the entire simulation
    for (int i = 0; i < BLOCKS; i++) {
        double visits = get_theoretical_visits(i);
        double block_resp_time = (double) (area[i].node / (long double) completed_jobs[i]);
        total_response_time += block_resp_time * visits;
    }
    // Calculate response time from the batch means
    double batch_means_response_time = 0.0;
    for (int i = 0; i < K_BATCH; i++) {
        batch_means_response_time += batch_response_times[i] / K_BATCH;
    }
    // they must be equal
    if (IS_NOT_EQUAL(total_response_time, batch_means_response_time)) {
        printf("\tInfinite-Horizon: The TOTAL global response time (%f) doesn't match with the BATCH MEANS response time (%f)\n",
               total_response_time, batch_means_response_time);
    }
}

void verify_batch_means_loss_probability(long long int rejected_jobs, long long int total_jobs,
                                         const double batch_loss_probabilities[K_BATCH]) {

    double total_ploss = (double) ((long double) rejected_jobs / (long double) total_jobs);

    double batch_means_ploss = 0.0;
    for (int i = 0; i < K_BATCH; i++) {
        batch_means_ploss += batch_loss_probabilities[i] / (double) K_BATCH;
    }

    if (IS_NOT_EQUAL(total_ploss, batch_means_ploss)) {
        printf("\tInfinite-Horizon: The TOTAL loss probability (%f) IT IS NOT EQUAL TO mean of batch loss probabilities (%f)\n",
               total_ploss, batch_means_ploss);
    } else {
        printf("\tTotal vs batch_means: %f ?= %f\n", total_ploss, batch_means_ploss);
    }
}
