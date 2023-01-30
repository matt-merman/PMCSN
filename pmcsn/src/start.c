/*
 * The aim of this project is to model and evaluate a network,
 * based on a generic company canteen with 2500 employees and 
 * the observation period equal to 3 hour.
 * Link to GitHub: https://github.com/matt-merman/PMCSN
 * Authors: 
 * - Di Battista Mattia
 * - Rossi Giacomo Lorenzo      
 */

#include "start.h"

const char *BLOCK_NAMES[BLOCKS] = {"Primi", "Secondi e Contorni", "Frutta e Dessert",
                                   "Casse Fast", "Casse standard", "Locale Mensa"
#ifdef EXTENDED
        , "Locale Mensa 2"
#endif
};

const long int SEEDS[MAX_SEED] = {123456789, 196747722, 315255074, 987654321, 484067034};

int main(int argc, __attribute__((unused)) char **argv) {
    int c, j, i;
    char *parameter;
    char file_name_grt[100], file_name_ploss[100];
    FILE *file_resp_time, *file_ploss;

    parameter = NULL;
    c = getopt(argc, argv, "s:");
    switch (c) {
        case 's':
            parameter = optarg;
            break;
        case '?':
        default:
            start_standard_simulation();
            printf("\nTo run finite or infinite horizon simulation, use:\n./start -s [finite/infinite]\n");
            return 0;
    }

    // initializing multi-stream lehmer generator
    PlantSeeds(SEEDS[0]);

    if (strcmp(parameter, "finite") == 0) {
        j = i = 1;

#ifdef EXTENDED
        char path_grt[100] = "./result/finite/ext_grt_";
        char path_ploss[100] = "./result/finite/ext_ploss_";
#else
        char path_grt[100] = "./result/finite/grt_";
        char path_ploss[100] = "./result/finite/ploss_";
#endif

        while (i <= MAX_REPLICAS) {

            create_file_name(path_grt, i, file_name_grt);
            create_file_name(path_ploss, i, file_name_ploss);

            file_resp_time = open_file("w", file_name_grt);
            fprintf(file_resp_time, "%s", "grt,replica\n");

            file_ploss = open_file("w", file_name_ploss);
            fprintf(file_ploss, "%s", "ploss,replica\n");

            start_finite_horizon_simulation(file_resp_time, file_ploss, i);

            fclose(file_resp_time);
            fclose(file_ploss);

            i = REPLICAS_STEP * j;
            j++;
        }
    } else if (strcmp(parameter, "infinite") == 0) {

        start_infinite_horizon_simulation(PERIOD);

    } else
        printf("Usage: ./start -s [finite/infinite]\n");

    return (0);
}

int start_standard_simulation() {
    char file_name_grt[100], file_name_ploss[100];
    FILE *file_grt, *file_ploss;
    long seed;
    double grt, ploss;
    long int period;
    int seed_index;
    network *canteen;

    FILE *files_grt[NUM_SEED], *files_ploss[NUM_SEED];

    for(int i = 0; i < NUM_SEED; i++){

#ifdef EXTENDED
        char path_grt[100] = "./result/standard/ext_grt_";
        char path_ploss[100] = "./result/standard/ext_ploss_";
#else
        char path_grt[100] = "./result/standard/grt_";
        char path_ploss[100] = "./result/standard/ploss_";
#endif
        create_file_name(path_grt, SEEDS[i], file_name_grt);
        create_file_name(path_ploss, SEEDS[i], file_name_ploss);

        file_grt = open_file("w", file_name_grt);
        fprintf(file_grt, "%s", "grt,period\n");

        file_ploss = open_file("w", file_name_ploss);
        fprintf(file_ploss, "%s", "ploss,period\n");

        files_grt[i] = file_grt;
        files_ploss[i] = file_ploss;

    }
        
    for (period = PERIOD; period <= 2*PERIOD; period += PERIOD/(PERIOD_INTERVALS*2)) { 
    
        printf("Period: %ld (%2.1f h)\nSeed\t\tGlobal Wait\t\tLoss Probability\n", period, (double) period / 3600.);
        
        for (seed_index = 0; seed_index < NUM_SEED; seed_index++) {
            seed = SEEDS[seed_index];
            PlantSeeds(seed);
            
            canteen = create_network(BLOCK_NAMES);

            simulation(canteen, 0, NULL, STANDARD, period, 0, 1);

            grt = probe_global_simulation_response_time(canteen, period);
            write_result(files_grt[seed_index], grt, period);

            ploss = probe_global_simulation_loss_probability(canteen);
            write_result(files_ploss[seed_index], ploss, period);

            if (NUM_SEED == 1) {
                show_stats(canteen, period);
                validate_stats(canteen, period);
            } else {
                printf("%ld\t%f\t\t%f\n", seed, grt, ploss);
            }

            clear_network(canteen, TRUE);

            if (NUM_SEED == 1) goto end;
        }
    }

end:

    for(int i = 0; i < NUM_SEED; i++){
        fclose(files_grt[i]);
        fclose(files_ploss[i]); 
    }

    return (0);
}

/**
 *
 * @param config the number of the configuration for servers
 * @param file file in which save the times
 * @param num_replicas number of replicas for this finite horizon simulation
 * @return
 */
int start_finite_horizon_simulation(FILE *file_grt, FILE *file_ploss, int num_replicas) {
    network *canteen;
    int replica;

    PRINTF("Simulation with %d replicas\n", num_replicas);

    canteen = malloc(sizeof(network));
    if (canteen == NULL) {
        perror("Error in allocation of canteen queue network (finite-horizon)");
        return -1;
    }

    canteen->network_servers = init_network();

    for (replica = 0; replica < num_replicas; replica++) {
        canteen->system_clock = init_clock();
        if (canteen->system_clock == NULL) {
            perror("Error on system clock\n");
            return (-1);
        }
        init_event_list(canteen->system_clock->type);
        // for every replica, we reset the block state
        if (replica == 0) {
            canteen->blocks = init_blocks(canteen->network_servers, BLOCK_NAMES);
            if (canteen->blocks == NULL) {
                perror("Error on blocks");
                return (-1);
            }
        } else
            restart_blocks(canteen);

        // the simulation will wait for all jobs to exit the network
        simulation(canteen, 0, NULL, FINITE, PERIOD, replica, num_replicas);

        compute_replica_statistics(canteen, replica, PERIOD); // period is not used

        write_result(file_grt, canteen->replicas_response_time[replica], num_replicas);
        write_result(file_ploss, canteen->replicas_loss_probability[replica], num_replicas);

        free(canteen->system_clock);
    }
    if (num_replicas > 1) {
        calculate_interval_estimate_for_stat("Global Response Time", canteen->replicas_response_time, num_replicas);
        calculate_interval_estimate_for_stat("Loss Probability", canteen->replicas_loss_probability, num_replicas);
    }

    // timer is freed inside the loop
    clear_network(canteen, FALSE);
    return (0);
}

int start_infinite_horizon_simulation(long int period) {

    network *canteen;
    int batch_number, i, c, s, num_servers;
    double grt[K_BATCH], ploss[K_BATCH];
    long arrived_jobs, completed_jobs[BLOCKS], seed;
    long long rejected_jobs, total_jobs;
    char file_name_grt[100], file_name_ploss[100];
    FILE *file_g, *file_p;

#ifdef EXTENDED
    char path_grt[100] = "./result/infinite/ext_grt_";
    char path_ploss[100] = "./result/infinite/ext_ploss_";
#else
    char path_grt[100] = "./result/infinite/grt_";
    char path_ploss[100] = "./result/infinite/ploss_";
#endif

    for (int seed_index = 0; seed_index < NUM_SEED; seed_index++) {

        seed = SEEDS[seed_index];
        PlantSeeds(seed);

        create_file_name(path_grt, seed, file_name_grt);
        create_file_name(path_ploss, seed, file_name_ploss);

        file_g = open_file("w", file_name_grt);
        fprintf(file_g, "%s", "grt,batch\n");

        file_p = open_file("w", file_name_ploss);
        fprintf(file_p, "%s", "ploss,batch\n");

        rejected_jobs = total_jobs = 0LL;

        canteen = create_network(BLOCK_NAMES);
        arrived_jobs = 0;

        // those are the global area stats
        area whole_simulation_area_stats[BLOCKS];
        for (i = 0; i < BLOCKS; i++) {
            whole_simulation_area_stats[i].node = 0.0L;
            whole_simulation_area_stats[i].queue = 0.0L;
            whole_simulation_area_stats[i].service = 0.0L;
        }

        memset(completed_jobs, 0, sizeof(long) * BLOCKS);

        // for each batch we run a simulation
        for (batch_number = 1; batch_number <= K_BATCH; batch_number++) {

            // first we reset all the stats except the state (how many jobs in each block)
            for (i = 0; i < BLOCKS; i++) {
                canteen->blocks[i]->rejected_jobs = 0;
                canteen->blocks[i]->completed_jobs = 0;
                canteen->blocks[i]->block_area->node = 0.0L;
                canteen->blocks[i]->block_area->queue = 0.0L;
                canteen->blocks[i]->block_area->service = 0.0L;
                for (c = 0; c < BLOCKS + 1; c++)
                    canteen->blocks[i]->count_to_next[c] = 0L;

                num_servers = canteen->blocks[i]->num_servers;
                for (s = 0; s < num_servers; s++) {
                    canteen->blocks[i]->servers[s]->sum->service = 0.0L;
                    canteen->blocks[i]->servers[s]->sum->served = 0L;
                }
            }

            // the simulation will not wait for jobs to exit the canteen, instead the next batch will continue
            // with the STATE of the previous (but output statistics are all set to 0).
            simulation(canteen, batch_number * B, &arrived_jobs, INFINITE,
                       (long) ((double) (K_BATCH * B) / LAMBDA), batch_number, K_BATCH);

            // then, we increment the global simulation stats, from the result of the batch.
            for (i = 0; i < BLOCKS; i++) {
                whole_simulation_area_stats[i].node += canteen->blocks[i]->block_area->node;
                whole_simulation_area_stats[i].queue += canteen->blocks[i]->block_area->queue;
                whole_simulation_area_stats[i].service += canteen->blocks[i]->block_area->service;
                completed_jobs[i] += canteen->blocks[i]->completed_jobs;
            }
#ifndef EXTENDED
            rejected_jobs += canteen->blocks[CONSUMAZIONE]->rejected_jobs;
            total_jobs += canteen->blocks[CONSUMAZIONE]->rejected_jobs + canteen->blocks[CONSUMAZIONE]->completed_jobs;
#else
            rejected_jobs += canteen->blocks[CONSUMAZIONE]->rejected_jobs + canteen->blocks[CONSUMAZIONE_2]->rejected_jobs;
            total_jobs += canteen->blocks[CONSUMAZIONE]->rejected_jobs + canteen->blocks[CONSUMAZIONE]->completed_jobs +
                        canteen->blocks[CONSUMAZIONE_2]->rejected_jobs + canteen->blocks[CONSUMAZIONE_2]->completed_jobs;
#endif
            // here we compute batch response time and loss probability
            compute_batch_statistics(canteen, batch_number - 1, period); // period is not used

            grt[batch_number - 1] = canteen->batch_response_time[batch_number - 1];
            ploss[batch_number - 1] = canteen->batch_loss_probability[batch_number - 1];

            write_result(file_g, grt[batch_number - 1], batch_number);
            write_result(file_p, ploss[batch_number - 1], batch_number);
        }

        if (seed == SEEDS[0]) {
            verify_batch_means_response_time(whole_simulation_area_stats, completed_jobs, grt, canteen);
            verify_batch_means_loss_probability(rejected_jobs, total_jobs, ploss);

            // if |auto-correlation for lag 1| < 0.2,
            // we are safe to compute the interval estimate (i.e. the sample is not auto-correlated)
            calculate_autocorrelation_for_stats("Global Response Time", canteen->batch_response_time);
            calculate_interval_estimate_for_stat("Global Response Time", canteen->batch_response_time, K_BATCH);
            calculate_autocorrelation_for_stats("Loss Probability", canteen->batch_loss_probability);
            calculate_interval_estimate_for_stat("Loss Probability", canteen->batch_loss_probability, K_BATCH);
        }

        fclose(file_g);
        fclose(file_p);

        clear_network(canteen, TRUE);
    }

    return (0);
}


