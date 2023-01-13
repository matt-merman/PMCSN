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
void get_stats(block *b, timer *clock, statistics *stats) {
    double completed_jobs = (double) b->completed_jobs;
    area *area = b->block_area;
    stats->completed_jobs = b->completed_jobs;
    stats->interarrival_time = clock->last / completed_jobs; // rejected jobs are excluded

    // PRINTF("clock last: %f last arrival: %f current: %f\n", clock->last, clock->last_arrival, clock->current);

    // FIXME ricontrollare come viene calcolata area e se questa formula è corretta.
    stats->wait = (double) (area->node / (long double) completed_jobs);
    stats->delay = (double) (area->queue / (long double) completed_jobs);
    stats->service_time = (double) (area->service / (long double) completed_jobs);
//    printf("area_node: %g\tarea_queue: %g\tarea_service: %g\t completed jobs: %f\n", area->node, area->queue, area->service, completed_jobs);
    stats->node_pop = (double) ( area->node / (long double) PERIOD);
    stats->queue_pop = (double) ( area->queue / (long double) PERIOD) ;
    stats->utilization = (double) ( area->service / ((long double) PERIOD * (long double) b->num_servers));
//    stats->block_probabiliity = / PERIOD;
    stats->daily_cost = get_costs(b);
    // multiserver statistics
    stats->multiserver_utilization = malloc(sizeof(double) * b->num_servers);
    stats->multiserver_service_time = malloc(sizeof(double) * b->num_servers);
    for (int s = 0; s < b->num_servers; s++) {
        stats->multiserver_utilization[s] = b->servers[s]->sum->service / PERIOD;
        stats->multiserver_service_time[s] = b->servers[s]->sum->service / b->servers[s]->sum->served;
    }

    stats->routing_p = malloc(sizeof(long double) * BLOCKS);
    // to calculate the routing probabilities
    for (int s = 0; s < BLOCKS; s++){
        stats->routing_p[s] = (long double) b->count_to_next[s] / (long double) b->completed_jobs;
    }


}

void write_stats_on_file(block **blocks, timer *clock, FILE **file){

    statistics stats;
    int i;

    for (i = 0; i < BLOCKS; i++) {
        get_stats(blocks[i], clock, &stats);
        // TODO: per ora solo la popolazione media viene scritta
        // bisogna dunque scrivere anche tutte le altre nello stesso file
        // oppure su più file (in questo caso 6 * 6 * REPLICAS)
        // Meglio un solo file in cui ogni ad ogni riga corrisponde un attributo
        // in questo modo estimace.c legge per riga valori consecutivi e non più a capo.  
        write_result(file[i], stats.node_pop);
    }
}

void show_stats(network *canteen) {
    long double total_cost = 0.0;
    statistics stats;
//    double queue_time_sum = 0.0;
//    double queue_population_sum = 0.0;
    double network_response_time = 0.0;
    double network_mean_population = 0.0;
    for (int i = 0; i < BLOCKS; i++) {
        // at each iteration we overwrite the statistics struct
        get_stats(canteen->blocks[i], canteen->system_clock, &stats);
//        queue_time_sum += stats.delay; // TODO: bisogna tenere conto delle visite medie a ogni centro!!!
//        queue_population_sum += stats.queue_pop;
        double visits = get_theoretical_visits(canteen->blocks[i]->type, canteen->blocks[i]->num_servers);
        network_response_time += stats.wait * visits;
        network_mean_population += stats.node_pop * visits;
        printf("\t----------------------------------------------------------\n");
        printf("\t'%s' block info:\n\n", canteen->blocks[i]->name);
        printf("\t\tpeople in the block ..... = % 10ld\tpeople\n", stats.completed_jobs);

        if (canteen->blocks[i]->type == CONSUMAZIONE) {
            printf("\t\trejected people ......... = % 10ld\tpeople\n", canteen->blocks[CONSUMAZIONE]->rejected_jobs);
            printf("\t\tblock probability ....... = % 10g\tpeople\n", (double) canteen->blocks[CONSUMAZIONE]->rejected_jobs / ( (double) canteen->blocks[CONSUMAZIONE]->completed_jobs + (double) canteen->blocks[CONSUMAZIONE]->rejected_jobs));
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

        if (canteen->blocks[i]->type == PRIMO){
            printf("\n\trouting probabilities statistics:\n");
            printf("\t\tP(to SECONDO) ....... = %6.4Lf\n", stats.routing_p[SECONDO]);
            printf("\t\tP(to CASSA FAST) .... = %6.4Lf\n", stats.routing_p[CASSA_FAST]);
            printf("\t\tP(to DESSERT) ....... = %6.4Lf\n", stats.routing_p[DESSERT]);
        }
        
        else if (canteen->blocks[i]->type == SECONDO){
            printf("\n\trouting probabilities statistics:\n");
            printf("\t\tP(to CASSA FAST) .... = %6.4Lf\n", stats.routing_p[CASSA_FAST]);
            printf("\t\tP1(to CASSA STD) .... = %6.4Lf\n", stats.routing_p[CASSA_STD]);
            printf("\t\tP2(to DESSERT) ...... = %6.4Lf\n", stats.routing_p[DESSERT]);
        }
        
        else if (canteen->blocks[i]->type == DESSERT){
            printf("\n\trouting probabilities statistics:\n");
            printf("\t\tP(to CASSA STD) ..... = %6.4Lf\ts\n", stats.routing_p[4]);
        }

        printf("\n\t\tMulti-server statistics:\n");
        printf("\t\t    server     utilization     avg service\n");
        // print all utilization and average service of each server in the node
        for (int s = 0; s < canteen->blocks[i]->num_servers; s++)
            printf("\t\t%8d %14.4f %15.2f\n", s, stats.multiserver_utilization[s], stats.multiserver_service_time[s]);

        total_cost += (long double) stats.daily_cost;
        printf("\n\t\tDaily Configuration Cost for '%s': %Lg \u20AC \n\n", canteen->blocks[i]->name, stats.daily_cost);

    }
    canteen->ensemble_response_time[0] = network_response_time;
    canteen->ensemble_mean_population[0] = network_mean_population;
    printf("\t----------------------------------------------------------\n");
    printf("\n\t\tTotal Daily Configuration Cost: %Lg \u20AC \n\n", total_cost);
    free(stats.multiserver_utilization);
    free(stats.multiserver_service_time);
}

void validate_stats(network *canteen) {

    statistics stats;
    int i;

    for (i = 0; i < BLOCKS; i++) {
        get_stats(canteen->blocks[i], canteen->system_clock, &stats);
        validate_block(canteen->blocks[i], &stats);
    }

    // validates global population, global queue time and response time
    validate_global_population(canteen->blocks);
    // validate_global_queue_time(queue_time_sum, 0);
    validate_global_response_time(canteen->ensemble_response_time[0], canteen->network_servers);
}

/**
 * Adds the statistics to a replica
 * @param canteen the network
 * @param replica index of replica in 0...REPLICAS-1
 */
void update_ensemble(network *canteen, int replica_index) {
    // for (int i = 0; i < BLOCKS; i++) {
    //     replica_stats *replica = &canteen->blocks[i]->ensemble_stats[replica_index];
    //     statistics stats;
    //     get_stats(canteen->blocks[i], canteen->system_clock, &stats);
    //     replica->interarrival = stats.interarrival_time;
    //     replica->wait = stats.wait;
    //     replica->delay = stats.delay;
    //     replica->service = stats.service_time;
    //     replica->node_pop = stats.node_pop;
    //     replica->queue_pop = stats.queue_pop;
    //     replica->utilization = stats.utilization;
    // }

	int	ergodic[] = {3, 3, 2, 1, 4, 139}; // TODO: ATTENZIONE non cambiare il 139!!!!
    //canteen->global_response_time[replica_index] += global_respones_time_real(ergodic, canteen);
    printf("%lf\n", canteen->global_response_time[replica_index]);

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

