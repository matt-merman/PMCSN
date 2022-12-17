#include "helpers.h"


double get_next_arrival(double current, double lambda) {
    double arrival;

    arrival = current;
    SelectStream(0);
    arrival += Exponential(1 / lambda);
    return (arrival);
}

double get_next_service(block_type type, int stream) {
    SelectStream(stream); // stream = 1,2,...,BLOCKS
    switch (type) {
        case PRIMO:
            return (Exponential(S_PRIMO));
        case SECONDO:
            return (Exponential(S_SECONDO));
        case DESSERT:
            return (Exponential(S_DESSERT));
        case CASSA_FAST:
            return (Exponential(S_CASSA_FAST));
        case CASSA_STD:
            return (Exponential(S_CASSA_STD));
        case CONSUMAZIONE:
            return (Exponential(S_CONSUMAZIONE));
        default:
            return (0.0);
    }
}

double get_theoretical_mhu(block_type type) {
    return 1.0 / get_theoretical_service(type);
}

double get_theoretical_service(block_type type) {
    switch (type) {
        case PRIMO:
            return S_PRIMO;
        case SECONDO:
            return S_SECONDO;
        case DESSERT:
            return S_DESSERT;
        case CASSA_FAST:
            return S_CASSA_FAST;
        case CASSA_STD:
            return S_CASSA_STD;
        case CONSUMAZIONE:
            return S_CONSUMAZIONE;
        default:
            return 0.0;
    }

}

// FIXME: verificare se è giusto
double get_theoretical_lambda(block_type type) {
    switch (type) {
        case PRIMO: //you can take PRIMO only from outside
            return LAMBDA * P_PRIMO_FUORI;
        case SECONDO: // you can take SECONDO from outside or from PRIMO
            return LAMBDA * (P_SECONDO_FUORI + P_PRIMO_FUORI * P_SECONDO_PRIMO);
        case DESSERT: // you can arrive from PRIMO or SECONDO, with 2 (if skipping SECONDO from PRIMO or starting with SECONDO) or 3 plate
            return LAMBDA * (P_PRIMO_FUORI * P_DESSERT_PRIMO + P_SECONDO_FUORI + P_DESSERT_SECONDO);
        case CASSA_FAST: // you can arrive from PRIMO or SECONDO, with only one plate
            return LAMBDA * (P_PRIMO_FUORI * P_CASSA_PRIMO + P_CASSA_FAST_SECONDO);
        case CASSA_STD: // who doesn't go to the fast cashier, goes to the standard cashier
            return LAMBDA *
                   (1.0 - (P_PRIMO_FUORI * P_CASSA_PRIMO + P_CASSA_FAST_SECONDO)); // TODO: il queue time è strano!!
        case CONSUMAZIONE: // the entire arrival flow will come to CONSUMAZIONE
            return LAMBDA; //TODO: moltiplica per (1 - probabilità di perdita ERLANG B (NON C)) se invece decidiamo di togliere la coda!!!
        default:
            return (0.0);
    }
}

// Computes the theoretical utilization of the block, even if it's a multiserver block.
double get_theoretical_rho(block *block) {
    double lambda = get_theoretical_lambda(block->type);
    double mhu = get_theoretical_mhu(block->type);
    return utilization(block->num_servers, lambda, mhu);
}

// reduced number of parameters
void get_stats(block *b, clock *clock, statistics *stats) {
    double completed_jobs = (double) b->completed_jobs;
    area *area = b->block_area;
    stats->completed_jobs = b->completed_jobs;
    stats->interarrival_time = clock->last / completed_jobs;
    // FIXME ricontrollare come viene calcolata area e se questa formula è corretta
    stats->wait = area->node / completed_jobs;
    stats->delay = area->queue / completed_jobs; // FIXME idem
    stats->service_time = area->service / completed_jobs; //FIXME idem
    stats->node_pop = area->node / clock->current; // FIXME idem
    stats->queue_pop = area->queue / clock->current; // FIXME idem
    stats->utilization = area->service / (clock->current * b->num_servers); // FIXME idem
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
void show_and_validate_block_stats(block **blocks, clock *clock) {
    double total_cost = 0.0;
    statistics stats;

    for (int i = 0; i < BLOCKS; i++) {
        get_stats(blocks[i], clock, &stats);
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
    printf("\t----------------------------------------------------------\n");
    printf("\n\t\tTotal Daily Configuration Cost: %g \u20AC \n\n", total_cost);
    free(stats.multiserver_utilization);
    free(stats.multiserver_service_time);
}

// validates global population, global queue time and response time
void show_and_validate_global_stats(block **blocks, clock *clock) {
    validate_global_population(blocks);
    validate_global_queue_time(blocks, clock);
    validate_global_response_time(blocks, clock);
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

