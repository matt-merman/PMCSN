#include "start.h"
#include <time.h>

const char *BLOCK_NAMES[BLOCKS] = {"Primi", "Secondi e Contorni", "Frutta e Dessert",
                                   "Casse Fast", "Casse standard", "Locale Mensa"};

#define NETWORK_CONFIGURATION CONFIG_1

int main(int argc, __attribute__((unused)) char **argv) {
    int c, j, i;
    char *parameter;
    char file_name_resptime[100];
    char file_name_ploss[100];
    char index_to_str[100];
    FILE *file_resp_time;
    FILE *file_ploss;

    parameter = NULL;
    c = getopt(argc, argv, "s:");
    switch (c) {
        case 's':
            parameter = optarg;
            break;
        case '?':
        default:
            start_standard_simulation(NETWORK_CONFIGURATION);
            printf("\nTo run finite or infinite horizon simulation, use:\n./start -s [finite/infinite]\n");
            return 0;
    }

    // initializing multi-stream lehmer generator
    PlantSeeds(123456789);

    if (strcmp(parameter, "finite") == 0) {
        j = i = 1;
        while (i <= MAX_REPLICAS) {
            strcpy(file_name_resptime, "");
            strcat(file_name_resptime, "./result/finite/grt_");
            sprintf(index_to_str, "%d", i);
            strcat(file_name_resptime, index_to_str);

            file_resp_time = open_file("w", file_name_resptime);
            fprintf(file_resp_time, "%s", "grt,replica\n");


            strcpy(file_name_ploss, "");
            strcat(file_name_ploss, "./result/finite/ploss_");
            sprintf(index_to_str, "%d", i);
            strcat(file_name_ploss, index_to_str);
            file_ploss = open_file("w", file_name_ploss);

            start_finite_horizon_simulation(NETWORK_CONFIGURATION, file_resp_time, file_ploss, i);

            fclose(file_resp_time);
            fclose(file_ploss);

            i = REPLICAS_STEP * j;
            j++;
        }
    } else if (strcmp(parameter, "infinite") == 0) {
        strncpy(file_name_resptime, "./result/infinite/grt", strlen("./result/infinite/grt") + 1);
        file_resp_time = open_file("w", file_name_resptime);
        strncpy(file_name_ploss, "./result/infinite/ploss", strlen("./result/infinite/ploss") + 1);
        file_ploss = open_file("w", file_name_ploss);
        fprintf(file_resp_time, "%s", "grt,batch_index\n");

        start_infinite_horizon_simulation(NETWORK_CONFIGURATION, file_resp_time, file_ploss, PERIOD);

        fclose(file_resp_time);
        fclose(file_ploss);
    } else
        printf("Usage: ./start -s [finite/infinite]\n");

    return (0);
}

int start_standard_simulation(int config) {
    char file_name[100];
    char num_to_str[100];
    FILE *file;
    long seed;
    double grt;
    long int period;

    seed = 123456789L;
    for (int seed_index = 0; seed_index < NUM_SEED; seed_index++) {

        PlantSeeds(seed);

        strcpy(file_name, "");
        strcat(file_name, "./result/standard/grt_");

        sprintf(num_to_str, "%ld", seed);
        strcat(file_name, num_to_str);

        file = open_file("w", file_name);
        fprintf(file, "%s", "grt,period\n");

        for (int remaining_periods = PERIOD_INTERVALS; remaining_periods > 0; remaining_periods--) {
            network *canteen = create_network(BLOCK_NAMES, config);

            period = PERIOD / remaining_periods;

            simulation(canteen, 0, NULL, STANDARD, period, PERIOD_INTERVALS - remaining_periods - 1, PERIOD_INTERVALS);

            grt = probe_global_simulation_response_time(canteen, period);
            write_result(file, grt, period);

            if (NUM_SEED == 1 && remaining_periods == 1) {
                show_stats(canteen, period);
                validate_stats(canteen, period);
            }

            clear_network(canteen, TRUE);
        }

        fclose(file);

        seed = get_seed();
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
int start_finite_horizon_simulation(int config, FILE *file, FILE *file_ploss, int num_replicas) {
    network *canteen;
    int replica;
    printf("Simulation with %d replicas\n", num_replicas);
    canteen = malloc(sizeof(network));
    if (canteen == NULL) {
        perror("Error in allocation of canteen queue network (finite-horizon)");
        return -1;
    }

    canteen->network_servers = init_network(config);

    for (replica = 0; replica < num_replicas; replica++) {
        canteen->system_clock = init_clock();
        if (canteen->system_clock == NULL) {
            perror("Error on system clock\n");
            return (-1);
        }
        init_event_list(canteen->system_clock->type);
        if (replica == 0) {
            canteen->blocks = init_blocks(canteen->network_servers, BLOCK_NAMES);
            if (canteen->blocks == NULL) {
                perror("Error on blocks");
                return (-1);
            }
        } else {
            restart_blocks(canteen);
        }
        // the simulation will wait for all jobs to exit the network
        simulation(canteen, 0, NULL, FINITE, PERIOD, replica, num_replicas);
        canteen->replicas_response_time[replica] = probe_global_simulation_response_time(canteen, PERIOD);
        canteen->replicas_loss_probability[replica] = probe_global_simulation_loss_probability(canteen, PERIOD);
        write_result(file, canteen->global_response_time, num_replicas);
        write_result(file_ploss, canteen->loss_probability, num_replicas);

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

/**
 *  TODO: verifica che la media di tutta la simulazione lunga sia uguale alla media delle medie dei batch.
 */
int start_infinite_horizon_simulation(int config, FILE *file_resptime, FILE *file_ploss, long int period) {

    network *canteen;
    int batch_index;
    double grt;
    double ploss;

    canteen = create_network(BLOCK_NAMES, config);

    long arrived_jobs = 0;
    // for each batch we run a simulation
    for (batch_index = 1; batch_index <= K_BATCH; batch_index++) {
        // the simulation will not wait for jobs to exit the canteen, instead the next batch will continue from where the last ended
        simulation(canteen, batch_index * B, &arrived_jobs, INFINITE, (long) ((double) (K_BATCH * B) / LAMBDA),
                   batch_index, K_BATCH);
        update_ensemble(canteen, batch_index - 1, period);

        grt = probe_global_simulation_response_time(canteen, period);
        ploss = probe_global_simulation_loss_probability(canteen, period);
        write_result(file_resptime, grt, batch_index);
        write_result(file_ploss, ploss, batch_index);
    }

    calculate_autocorrelation_for_stats("Global Response Time", canteen->batch_response_time);
    calculate_interval_estimate_for_stat("Global Response Time", canteen->batch_response_time, K_BATCH);
    calculate_autocorrelation_for_stats("Loss Probability", canteen->batch_loss_probability);
    calculate_interval_estimate_for_stat("Loss Probability", canteen->batch_loss_probability, K_BATCH);

    clear_network(canteen, TRUE);
    return (0);
}