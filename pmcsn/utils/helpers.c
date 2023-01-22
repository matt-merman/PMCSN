#include "helpers.h"

long get_seed() {

    return Equilikely(0L, 900000000L);

}

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

/**
 *
 * @param simulation_type STANDARD, FINITE, INFINITE
 * @param canteen the network
 * @param period PERIOD or K*B/LAMBDA
 * @param prev_perc previous percentage
 * @param index index of simulation. replica or batch
 * @param total total simulations, replicas or batches
 * @return the next percentage
 */
int progress(sim_type simulation_type, network *canteen, long period, int prev_perc, int index, int total) {
    int perc;

    switch (simulation_type) {
        case STANDARD:
            perc = (int) ((long) canteen->system_clock->current / (PERIOD / 100L));
            if (perc > prev_perc || perc == 100) {
                printf("Simulation %d/%d - Progress: %d%%\r", index + 1, total, perc);
                prev_perc = perc;
                fflush(stdout);
            }
            break;
        case FINITE:
            perc = (int) ((long) canteen->system_clock->current / (period / 100L));
            if (perc > prev_perc || perc == 100) {
                if (perc <= 100) {
                    printf("Replica %d/%d - Progress: %d%%\r", index + 1, total, perc);
                    prev_perc = perc;
                } else {
                    long remaining_jobs = 0;
                    for (int i = 0; i < BLOCKS; i++) {
                        remaining_jobs += canteen->blocks[i]->jobs;
                    }
                    printf("Replica %d/%d - Remaining job in the network: %ld\r", index + 1, total, remaining_jobs);
                    prev_perc = 100;
                }
                fflush(stdout);
            }
            break;
        case INFINITE:
            perc = (int) ((long) canteen->system_clock->current / (period / 100L));
            if (perc > prev_perc || perc == 100) {
                printf("Total Progress %d%% - Current Batch: %d/%d\r", perc, index, total);
                prev_perc = perc;
                fflush(stdout);
            }
            break;
        default:
            prev_perc = 100;
    }



    return prev_perc;
}

// reduced number of parameters
void get_stats(block *b, timer *clock, statistics *stats, long int period) {
    double completed_jobs = (double) b->completed_jobs;
    area *area = b->block_area;
    stats->completed_jobs = b->completed_jobs;
    stats->interarrival_time = clock->last / completed_jobs; // rejected jobs are excluded

    stats->wait = (double) (area->node / (long double) completed_jobs);
    stats->delay = (double) (area->queue / (long double) completed_jobs);
    stats->service_time = (double) (area->service / (long double) completed_jobs);

    stats->node_pop = (double) (area->node / (long double) period);
    stats->queue_pop = (double) (area->queue / (long double) period);
    stats->utilization = (double) (area->service / ((long double) period * (long double) b->num_servers));
    stats->loss_probability = 0.0;
    if (b->type == CONSUMAZIONE) {
        stats->loss_probability = (double) b->rejected_jobs / (double) (b->completed_jobs + b->rejected_jobs);
    }

    // multiserver statistics
    stats->multiserver_utilization = malloc(sizeof(double) * b->num_servers);
    stats->multiserver_service_time = malloc(sizeof(double) * b->num_servers);
    for (int s = 0; s < b->num_servers; s++) {
        stats->multiserver_utilization[s] = (double) (b->servers[s]->sum->service / period);
        stats->multiserver_service_time[s] = (double) (b->servers[s]->sum->service / b->servers[s]->sum->served);
    }

    stats->routing_probability = malloc(sizeof(long double) * BLOCKS);
    // to calculate the routing probabilities
    for (int s = 0; s < BLOCKS; s++) {
        stats->routing_probability[s] = (long double) b->count_to_next[s] / (long double) b->completed_jobs;
    }
}

void show_stats(network *canteen, long int period) {
    statistics stats;
    double network_response_time = 0.0;
    double loss_probability = 0.0;
    for (int i = 0; i < BLOCKS; i++) {
        // at each iteration we overwrite the statistics struct
        get_stats(canteen->blocks[i], canteen->system_clock, &stats, period);
        double visits = get_theoretical_visits(canteen->blocks[i]->type, canteen->blocks[i]->num_servers);
        network_response_time += stats.wait * visits;
        printf("\t----------------------------------------------------------\n");
        printf("\t'%s' block info:\n\n", canteen->blocks[i]->name);
        printf("\t\tpeople in the block ..... = % 10ld\tpeople\n", stats.completed_jobs);

        if (canteen->blocks[i]->type == CONSUMAZIONE) {
            loss_probability = (double) canteen->blocks[CONSUMAZIONE]->rejected_jobs /
                               ((double) canteen->blocks[CONSUMAZIONE]->completed_jobs +
                                (double) canteen->blocks[CONSUMAZIONE]->rejected_jobs);
            printf("\t\trejected people ......... = % 10ld\tpeople\n", canteen->blocks[CONSUMAZIONE]->rejected_jobs);
            printf("\t\tloss probability ....... = % 10g\tpeople\n",
                   loss_probability);
        }

        printf("\n\tjob averaged statistics:\n");
        printf("\t\taverage interarrival time = %6.2f\ts\n", stats.interarrival_time);
        printf("\t\taverage node wait ....... = %6.2f\ts\n", stats.wait);
        printf("\t\taverage queue delay ..... = %6.2f\ts\n", stats.delay);
        printf("\t\taverage service time .... = %6.2f\ts\n", stats.service_time);

        printf("\n\ttime averaged statistics:\n");
        printf("\t\taverage # in the node ... = %6.2f\tpeople\n", stats.node_pop);
        printf("\t\taverage # in the queue .. = %6.2f\tpeople\n", stats.queue_pop);
        printf("\t\tutilization ............. = %6.4f\t-\n", stats.utilization);

        // printf("\t\tarea node %Lf\n", canteen->blocks[i]->block_area->node);
        // printf("\t\tarea queue %Lf\n", canteen->blocks[i]->block_area->queue);
        // printf("\t\tarea service %Lf\n", canteen->blocks[i]->block_area->service);

        if (canteen->blocks[i]->type == PRIMO) {
            printf("\n\trouting probabilities statistics:\n");
            printf("\t\tP(to SECONDO) ....... = %6.4Lf\n", stats.routing_probability[SECONDO]);
            printf("\t\tP(to CASSA FAST) .... = %6.4Lf\n", stats.routing_probability[CASSA_FAST]);
            printf("\t\tP(to DESSERT) ....... = %6.4Lf\n", stats.routing_probability[DESSERT]);
        } else if (canteen->blocks[i]->type == SECONDO) {
            printf("\n\trouting probabilities statistics:\n");
            printf("\t\tP(to CASSA FAST) .... = %6.4Lf\n", stats.routing_probability[CASSA_FAST]);
            printf("\t\tP1(to CASSA STD) .... = %6.4Lf\n", stats.routing_probability[CASSA_STD]);
            printf("\t\tP2(to DESSERT) ...... = %6.4Lf\n", stats.routing_probability[DESSERT]);
        } else if (canteen->blocks[i]->type == DESSERT) {
            printf("\n\trouting probabilities statistics:\n");
            printf("\t\tP(to CASSA STD) ..... = %6.4Lf\ts\n", stats.routing_probability[4]);
        }

        printf("\n\t\tMulti-server statistics:\n");
        printf("\t\t    server     utilization     avg service\n");
        // print all utilization and average service of each server in the node
        for (int s = 0; s < canteen->blocks[i]->num_servers; s++)
            printf("\t\t%8d %14.4f %15.2f\n", s, stats.multiserver_utilization[s], stats.multiserver_service_time[s]);

        clear_stats(&stats);
    }
    canteen->global_response_time = network_response_time;
    canteen->loss_probability = loss_probability;
    printf("\t----------------------------------------------------------\n");
}

void validate_stats(network *canteen, long int period) {

    statistics stats;
    int i;

    for (i = 0; i < BLOCKS; i++) {
        get_stats(canteen->blocks[i], canteen->system_clock, &stats, period);
        validate_block(canteen->blocks[i], &stats);
        clear_stats(&stats);
    }

    // validates global population, global queue time and response time
    validate_global_population(canteen->blocks);
    // validate_global_queue_time(queue_time_sum, 0);
    validate_global_response_time(canteen->global_response_time, canteen->network_servers);
    validate_ploss(canteen->loss_probability, canteen->blocks[CONSUMAZIONE]->num_servers);
}

/**
 * Adds the statistics to a replica
 * @param canteen the network
 * @param batch batch of batch in 0...K_BATCH-1
 * @param period duration of batch
 */
void compute_batch_statistics(network *canteen, long batch, long int period) {
    canteen->batch_response_time[batch] = probe_global_simulation_response_time(canteen, period);
    canteen->batch_loss_probability[batch] = probe_global_simulation_loss_probability(canteen, period);
}

void compute_replica_statistics(network *canteen, long replica, long int period) {
    canteen->replicas_response_time[replica] = probe_global_simulation_response_time(canteen, period);
    canteen->replicas_loss_probability[replica] = probe_global_simulation_loss_probability(canteen, period);
}

void clear_stats(statistics *stats) {
    free(stats->multiserver_utilization);
    free(stats->multiserver_service_time);
    free(stats->routing_probability);
}

void debug(event *event, network *canteen) {
    PRINTF("Event %ld(linked event id %ld): Time: %lf - %-18s Target Block: %-12s in server: %d jobs in blocks [%s, %s, %s, %s, %s, %s] events: %d\n",
           event->event_id,
           event->linked_event_id,
           canteen->system_clock->current,
           to_str_event(event->event_type),
           to_str_block(event->block_type),
           event->target_server,
           get_server_contents(canteen->blocks[PRIMO]),
           get_server_contents(canteen->blocks[SECONDO]),
           get_server_contents(canteen->blocks[DESSERT]),
           get_server_contents(canteen->blocks[CASSA_FAST]),
           get_server_contents(canteen->blocks[CASSA_STD]),
           get_server_contents(canteen->blocks[CONSUMAZIONE]),
           event_list_length());
}

