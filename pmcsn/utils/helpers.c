#include "helpers.h"


double get_next_arrival(double current, double lambda) {
    double arrival;
    arrival = current;
    SelectStream(0);
    arrival += Exponential(1 / lambda);
    return (arrival);
}

/**
 * Computes next service time for a block type, with a particular stream
 * @param type the block type
 * @param stream the corresponding stream. 1 for Primi, 2 for Secondi, and so on.
 * @return next service time
 */
double get_next_service(block_type type, int stream) {
    SelectStream(stream); // stream = 1,2,...,BLOCKS
    return Exponential(get_theoretical_service(type));
}

// reduced number of parameters
void get_stats(block *b, clock *clock, statistics *stats) {
    double completed_jobs = (double) b->completed_jobs;
    area *area = b->block_area;
    stats->completed_jobs = b->completed_jobs;
    stats->interarrival_time = clock->last / completed_jobs;
    // FIXME ricontrollare come viene calcolata area e se questa formula è corretta
    stats->wait = area->node / completed_jobs;
    stats->delay = area->queue / completed_jobs;
    stats->service_time = area->service / completed_jobs;
    stats->node_pop = area->node / clock->current;
    stats->queue_pop = area->queue / clock->current;
    stats->utilization = area->service / (clock->current * b->num_servers);

    stats->daily_cost = b->type != CONSUMAZIONE ? get_costs(b->num_servers) : 0.0;
    // multiserver statistics
    stats->multiserver_utilization = malloc(sizeof(double) * b->num_servers);
    stats->multiserver_service_time = malloc(sizeof(double) * b->num_servers);
    for (int s = 0; s < b->num_servers; s++) {
        stats->multiserver_utilization[s] = b->servers[s]->sum->service / clock->current;
        stats->multiserver_service_time[s] = b->servers[s]->sum->service / b->servers[s]->sum->served;
    }
}

// computes and validate the job averaged and time averaged statistics for each block
void show_and_validate_stats(block **blocks, clock *clock) {
    double total_cost = 0.0;
    statistics stats;
//    double queue_time_sum = 0.0;
//    double queue_population_sum = 0.0;
//    double response_time_sum = 0.0;
//    double population_sum = 0.0;
    for (int i = 0; i < BLOCKS; i++) {
        // at each iteration we overwrite the statistics struct
        get_stats(blocks[i], clock, &stats);
//        queue_time_sum += stats.delay; // TODO: bisogna tenere conto delle visite medie a ogni centro!!!
//        queue_population_sum += stats.queue_pop;
//        response_time_sum += stats.wait;
//        population_sum += stats.node_pop;
        printf("\t----------------------------------------------------------\n");
        printf("\t'%s' block info:\n\n", blocks[i]->name);
        printf("\t\tpeople in the block ..... = %ld\tpeople\n", stats.completed_jobs);
        printf("\n\tjob averaged statistics:\n");
        printf("\t\taverage interarrival time = %6.2f\ts\n", stats.interarrival_time);
        printf("\t\taverage node wait ....... = %6.2f\ts\n", stats.wait);
        printf("\t\taverage queue delay ..... = %6.2f\ts\n", stats.delay);
        printf("\t\taverage service time .... = %6.2f\ts\n", stats.service_time);
        printf("\n\ttime averaged statistics:\n");
        printf("\t\taverage # in the node ... = %6.2f\tpeople\n", stats.node_pop);
        printf("\t\taverage # in the queue .. = %6.2f\tpeople\n", stats.queue_pop);
        printf("\t\tutilization ............. = %6.4f\t-\n", stats.utilization);
        printf("\n\t\tMulti-server statistics:\n");
        printf("\t\t    server     utilization     avg service\n");
        // print all utilization and average service of each server in the node
        for (int s = 0; s < blocks[i]->num_servers; s++) {
            printf("\t\t%8d %14.4f %15.2f\n", s, stats.multiserver_utilization[s], stats.multiserver_service_time[s]);
        }
        if (blocks[i]->type != CONSUMAZIONE) { // TODO: per ora il costo del locale mensa non è stato considerato
            total_cost += stats.daily_cost;
            printf("\n\t\tDaily Configuration Cost for '%s': %g \u20AC \n\n", blocks[i]->name, stats.daily_cost);
        }
        validate_block(blocks[i], &stats);
    }
    // validates global population, global queue time and response time
    validate_global_population(blocks);
    // validate_global_queue_time(queue_time_sum, 0);
    // validate_global_response_time(response_time_sum, 0);
    printf("\t----------------------------------------------------------\n");
    printf("\n\t\tTotal Daily Configuration Cost: %g \u20AC \n\n", total_cost);
    free(stats.multiserver_utilization);
    free(stats.multiserver_service_time);
}


void clear_mem(block **blocks) {
    for (int i = 0; i < BLOCKS; i++) {
        if (blocks[i]->block_area != NULL) {
            free(blocks[i]->block_area);
        }
        for (int j = 0; j < blocks[i]->num_servers; j++) {
            if (blocks[i]->servers[j]->sum != NULL) {
                free(blocks[i]->servers[j]->sum);
            }
            if (blocks[i]->servers[j] != NULL) {
                free(blocks[i]->servers[j]);
            }
        }
    }
}

