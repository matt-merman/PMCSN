#include "start.h"
#include <time.h>

const char * BLOCK_NAMES[BLOCKS] = {"Primi", "Secondi e Contorni", "Frutta e Dessert", "Casse Fast", "Casse standard", "Locale Mensa"};

#define NETWORK_CONFIGURATION CONFIG_2

int	main(int argc, __attribute__((unused)) char **argv)
{
	int		c,j,i;
	char	*parameter;
    char file_name[100];
    char index_to_str[100];
    FILE *file;

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

    if (strcmp(parameter, "finite") == 0){
        j = i = 1;
        while(i <= MAX_REPLICAS){
            strcpy(file_name, "");
            strcat(file_name, "./result/finite/grt_");

            sprintf(index_to_str, "%d", i);
            strcat(file_name, index_to_str);

            file = open_file("w", file_name);
            fprintf(file, "%s", "grt,replica\n");
            
            start_finite_horizon_simulation(NETWORK_CONFIGURATION, file, i);

            fclose(file);

            i = REPLICAS_STEP*j;
            j++;
        }
    }
	else if (strcmp(parameter, "infinite") == 0){
        strcat(file_name, "./result/infinite/grt");
        file = open_file("w", file_name);
        fprintf(file, "%s", "grt,batch_index\n");
            
        start_infinite_horizon_simulation(NETWORK_CONFIGURATION, file);

        fclose(file);
    }else
		printf("Usage: ./start -s [finite/infinite]\n");

	return (0);
}

int start_standard_simulation(int config) {
    network * canteen = create_network(BLOCK_NAMES, config);

    simulation(canteen, 0, NULL, STANDARD);

    // if you run one replica, we'll have a standard execution
    show_stats(canteen);
    validate_stats(canteen);
	clear_network(canteen);

    return (0);
}

/**
 *
 * @param config the number of the configuration for servers
 * @param file file in which save the times
 * @param num_replicas number of replicas for this finite horizon simulation
 * @return
 */
int start_finite_horizon_simulation(int config, FILE *file, int num_replicas)
{
    network *canteen;
    int replica;
    double grt;

    canteen = malloc(sizeof(network));
    if (canteen == NULL){
        perror("Error in allocation of canteen queue network (finite-horizon)");
        return -1;
    }

    canteen->network_servers = init_network(config);

    for (replica = 0; replica < num_replicas; replica++)
	{
        canteen->system_clock = init_clock();
        if (canteen->system_clock == NULL)
		{
            perror("Error on system clock\n");
            return (-1);
        }
        init_event_list(canteen->system_clock->type);
        if (replica == 0) {
            canteen->blocks = init_blocks(canteen->network_servers, BLOCK_NAMES);
            if (canteen->blocks == NULL)
            {
                perror("Error on blocks");
                return (-1);
            }
        } else {
            restart_blocks(canteen);
        }

        simulation(canteen, 0, NULL, FINITE);
        canteen->global_response_time = global_simulation_response_time(canteen);

        grt = global_simulation_response_time(canteen);
        write_result(file, grt, num_replicas);

        free(canteen->system_clock);
    }
    if (num_replicas > 1)
        calculate_interval_estimate_for_stat("Global Response Time", canteen->replicas_response_time, num_replicas);

    clear_network(canteen);
    return (0);
}

/**
    TODO: verifica che la media di tutta la simulazione lunga sia uguale alla media delle medie dei batch.
 */
int start_infinite_horizon_simulation(int config, FILE *file)
{

    network *canteen;
    int batch_index;
    double grt;

    canteen = malloc(sizeof(network));
    if (canteen == NULL){
        perror("Error in allocation of canteen queue network (finite-horizon)");
        return -1;
    }

    canteen->network_servers = init_network(config);
    canteen->system_clock = init_clock();
    if (canteen->system_clock == NULL)
    {
        perror("Error on system clock\n");
        return (-1);
    }

    init_event_list(canteen->system_clock->type);

    canteen->blocks = init_blocks(canteen->network_servers, BLOCK_NAMES);
    if (canteen->blocks == NULL)
    {
        perror("Error on blocks\n");
        return (-1);
    }

    int *arrived_jobs = (int *)malloc(sizeof(int));
    if (arrived_jobs == NULL){
        printf("Error on arrived_jobs\n");
        return (-1);
    }
    *arrived_jobs = 0;
    for (batch_index = 1; batch_index <= K_BATCH; batch_index++)
    {
        simulation(canteen, batch_index*B, arrived_jobs, INFINITE);
        update_ensemble(canteen, batch_index-1);
            
        grt = global_simulation_response_time(canteen);
        write_result(file, grt, batch_index);
    }

    calculate_autocorrelation_for_stats("Global Response Time", canteen->batch_response_time);
    calculate_interval_estimate_for_stat("Global Response Time", canteen->batch_response_time, K_BATCH);
    
    free(canteen->system_clock);
    free(canteen);
    return (0);
}