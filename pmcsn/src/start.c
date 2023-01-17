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
        j = 1;
        i = 1;
        while(i <= MAX_REPLICAS){
            strcpy(file_name, "");
            // strcat(file_name, "grt_");
            strcat(file_name, "test");

            sprintf(index_to_str, "%d", i);
            strcat(file_name, index_to_str);

            file = open_file("w", file_name);
            start_finite_horizon_simulation(NETWORK_CONFIGURATION, file, i);

            fclose(file);
            i = REPLICAS_STEP*j;
            j++;
        }
    }
	else if (strcmp(parameter, "infinite") == 0)
        start_infinite_horizon_simulation(NETWORK_CONFIGURATION);
	else
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
 *  TODO: dato un tempo n=b*k, bisogna trovare il valore migliore di b e k.
    
    All'aumentare di b, il metodo batch means produce risultati migliori.
    1 - Select a batch size b > 1 (seconds).

    1.1 - Compute number of batches k = trunc(n/b). This doesn't have an impact on the point estimate of the mean, only on the width of the interval.
    2 - Group the sequence into k batches of size b (seconds) and calculate the batch mean
    x_j = 1/b sum_(i=1)^b x_(j-1)(b+i), with batch index j = 1,2,...,k
    TODO: qui dobbiamo riutilizzare la funzione simulation per ottenere la media di ciascun batch

    3 - Compute the mean x and standard deviation s of the batch means x_j, j=1,...,k
    with estimate.c to get the interval estimate

    we are LOC% confident that the true steady-state mean is somewhere in this interval.
    if the batch size b is large, this is probably true even if the sample is autocorrelated.

    if no points are discarded, the "mean of the means" is the same as the "grand sample mean"

    TODO: verifica che la media di tutta la simulazione lunga sia uguale alla media delle medie dei batch.
 */
int start_infinite_horizon_simulation(int config)
{

    int batches = get_batch_number();
    
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

    for (batch_index = 1; batch_index <= batches; batch_index++)
	{
        simulation(canteen, batch_index*B, arrived_jobs, INFINITE);

        // TODO: definire un array apposito per il tempo di risposta globale nell'infinito,
        // soprattutto nel caso in cui batches è maggiore di MAX_REPLICAS
        update_ensemble(canteen, batch_index);

        // grt = global_simulation_respones_time(canteen);
        // write_result(file, grt, num_replicas);
    }
    calculate_interval_estimate_for_stat("Global Response Time", canteen->batch_response_time, batches);
    free(canteen->system_clock);
    free(canteen);
    return (0);
}

// int batch_simulation(network *canteen, int batch_index){


//      for(int b = 0; b < B*batch_index)
//     canteen->system_clock = init_clock();
//     if (canteen->system_clock == NULL)
//     {
//         perror("Error on system clock\n");
//         return (-1);
//     }
//     init_event_list(canteen->system_clock->type);

//     if (replica == 0) {
//         canteen->blocks = init_blocks(canteen->network_servers, BLOCK_NAMES);
//         if (canteen->blocks == NULL)
//         {
//             perror("Error on blocks");
//             return (-1);
//         }
//     } else {
//         restart_blocks(canteen);
//     }

//     simulation(canteen);
//     update_ensemble(canteen, b);

//     // grt = global_simulation_respones_time(canteen);
//     // write_result(file, grt, num_replicas);
//     // printf("%d\n", replica);

//     free(canteen->system_clock);
// }