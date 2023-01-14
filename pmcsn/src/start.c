#include "start.h"
#include <time.h>

const char * BLOCK_NAMES[BLOCKS] = {"Primi", "Secondi e Contorni", "Frutta e Dessert", "Casse Fast", "Casse standard", "Locale Mensa"};

#define NETWORK_CONFIGURATION CONFIG_2

int	main(int argc, __attribute__((unused)) char **argv)
{
	int		c;
	char	*parameter;

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

	if (strcmp(parameter, "finite") == 0)
        start_finite_horizon_simulation(NETWORK_CONFIGURATION);
	else if (strcmp(parameter, "infinite") == 0)
        start_infinite_horizon_simulation(NETWORK_CONFIGURATION);
	else
		printf("Usage: ./start -s [finite/infinite]\n");

	return (0);
}

int start_standard_simulation(int config) {
    network * canteen = create_network(BLOCK_NAMES, config);

    simulation(canteen);

    // if you run one replica, we'll have a standard execution
    show_stats(canteen);
    validate_stats(canteen);

	clear_network(canteen);
    // free(canteen->system_clock);
    // clear_mem(canteen->blocks);
    // free(canteen);
    return (0);
}

int start_finite_horizon_simulation(int config)
{
    network *canteen;
    int replica;

    canteen = malloc(sizeof(network));
    if (canteen == NULL){
        perror("Error in allocation of canteen queue network (finite-horizon)");
        return -1;
    }

    canteen->network_servers = init_network(config);
    for (replica = 0; replica < REPLICAS; replica++)
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

        FIND_SEGFAULT("tre");
        simulation(canteen);
        update_ensemble(canteen, replica);

        free(canteen->system_clock);
    }
    calculate_all_interval_estimate(canteen);
    clear_mem(canteen->blocks);
    free(canteen);
    return (0);
}

/**
 * Uses the program estimate.c to compute the estimation interval of the replica_stats
 */
void calculate_all_interval_estimate(network *canteen)
{
	// FILE **files;
	// int i,s;
//     const char *stats[18] = {
//             "Interarrival time",
//             "Response time",
//             "Queue time",
//             "Service time",
//             "Node population",
//             "Queue population",
//             "Utilization"
//     };
	// for(i = 0; i < BLOCKS; i++){
     //    for (s = 0; s < STAT_NUMBER; s++) {
     //        calculate_interval_estimate_for_stat(s, stats[s], canteen->blocks[i]->ensemble_stats, BLOCK_NAMES[i]);
     //    }
     //    //TODO stampare se il valore teorico è dentro o fuori l'intervallo di confidenza
	// }

     calculate_interval_estimate_for_stat("Global Response Time", canteen->global_response_time);

}

/**
 *
 * @return
 */
int start_infinite_horizon_simulation(int config)
{
    printf("config :%d\n", config);
    // TODO: dato un tempo n=b*k, bisogna trovare il valore migliore di b e k.
    // All'aumentare di b, il metodo batch means produce risultati migliori.
    // 1 - Select a batch size b > 1 (seconds).

    // 1.1 - Compute number of batches k = trunc(n/b). This doesn't have an impact on the point estimate of the mean, only on the width of the interval.

    // 2 - Group the sequence into k batches of size b (seconds) and calculate the batch mean
    // x_j = 1/b sum_(i=1)^b x_(j-1)(b+i), with batch index j = 1,2,...,k
    // TODO: qui dobbiamo riutilizzare la funzione simulation per ottenere la media di ciascun batch

    // 3 - Compute the mean x and standard deviation s of the batch means x_j, j=1,...,k
    // with estimate.c to get the interval estimate

    // we are LOC% confident that the true steady-state mean is somewhere in this interval.
    // if the batch size b is large, this is probably true even if the sample is autocorrelated.

    // if no points are discarded, the "mean of the means" is the same as the "grand sample mean"

    //TODO: verifica che la media di tutta la simulazione lunga sia uguale alla media delle medie dei batch.
    return (0);
}