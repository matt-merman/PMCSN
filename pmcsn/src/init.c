#include "init.h"

network *create_network(const char** block_names, int config) {
	network* canteen = (network *) malloc(sizeof(network));
    if (canteen == NULL) {
        perror("Error in allocation of canteen queue network ");
        exit(-1);
    }
	canteen->network_servers = init_network(config);
    canteen->system_clock = init_clock(); // also sets next arrival time
	if (canteen->system_clock == NULL) {
        perror("Error on system clock\n");
        exit(-1);
    }
    canteen->blocks = init_blocks(canteen->network_servers, block_names);
    if (canteen->blocks == NULL) {
        perror("Error on blocks");
        exit(-1);
    }
    init_event_list(canteen->system_clock->type); // type is arrival

	return canteen;
}

void clear_network(network *n, int clear_timer) {
    for (int block = 0; block < BLOCKS; block++){
        free(n->blocks[block]->block_area);
        for (int server = 0; server < n->blocks[block]->num_servers; server++){
            free(n->blocks[block]->servers[server]->sum);
            free(n->blocks[block]->servers[server]);
        }
        free(n->blocks[block]->servers);
        free(n->blocks[block]);
    }
    free(n->blocks);
	if (clear_timer)
		free(n->system_clock);
    free(n);
}


// Initializes the block structs given a configuration
block **init_blocks(int *config, const char **block_names)
{
	block	**b;

	b = (block **)malloc(BLOCKS * sizeof(block *));
	if (b == NULL)
	{
		printf("Error Malloc\n");
		return (NULL);
	}
	for (int i = 0; i < BLOCKS; i++)
	{
		b[i] = malloc(sizeof(block));
		if (b[i] == NULL)
		{
			printf("Error Malloc\n");
			return (NULL);
		}

		memset(b[i], 0, sizeof(block));
		b[i]->block_area = malloc(sizeof(area));
		b[i]->type = i;
		b[i]->num_servers = config[i];	

		if (b[i]->block_area == NULL)
		{
			printf("Error Malloc\n");
			return (NULL);
		}
		memset(b[i]->count_to_next, 0, sizeof(long int) * BLOCKS);
		strncpy(b[i]->name, block_names[i], NAME_SIZE - 1);
		memset(b[i]->block_area, 0x0, sizeof(area));
		init_servers(b[i], config[i]);
	}
	return (b);
}

void restart_blocks(network *canteen){
    for (int i = 0; i < BLOCKS; i++){
        block *block = canteen->blocks[i];
        memset(block->block_area, 0, sizeof(area));
        block->queue_jobs = 0;
        block->completed_jobs = 0;
        block->rejected_jobs = 0;
        block->jobs = 0;
	    memset(block->count_to_next, 0, (BLOCKS + 1)*sizeof(long));
        for(int j = 0 ; j < block->num_servers; j++){
            block->servers[j]->sum->service = (long double) 0;
            block->servers[j]->sum->served = 0L;
            block->servers[j]->status = IDLE;
        }
    }
}

void	init_servers(block *block, int num)
{
    block->servers = (server **) malloc(num * sizeof(server *));
	if (block->servers == NULL)
	{
		printf("Error allocating multi-servers\n");
		return ;
	}
    memset(block->servers, 0x0, num * sizeof(server *));
	for (int s = 0; s < num; s++)
	{
        block->servers[s] = (server *) malloc(sizeof(server));
        memset(block->servers[s], 0x0, sizeof(server));
        block->servers[s]->sum = (sum *) malloc(sizeof(sum));
        memset(block->servers[s]->sum, 0x0, sizeof(sum));
        if (block->servers[s]->sum == NULL || block->servers[s] == NULL)
		{
			printf("Error Malloc\n");
			return ;
		}
        block->servers[s]->status = IDLE;
	}
}

// Initializes the clock and generates the first arrival
timer * 	init_clock(void)
{
	double p = Random();
	timer *c = (timer *)malloc(sizeof(timer));
	if (c == NULL)
	{
		printf("Error allocating clock\n");
		return NULL;
	}
	c->current = START;
	c->last_arrival = get_next_arrival(START, LAMBDA);
    if (p < P_PRIMO_FUORI) {
        c->type = PRIMO;
    } else {
        c->type = SECONDO;
    }
	return c;
}
#ifndef EXTENDED
int	network_status[] = {0, 0, 0, 0, 0, 0};
#else
int	network_status[] = {0, 0, 0, 0, 0, 0, 0};
#endif

int *	init_network(int config)
{
	int	base[] = {3, 3, 2, 1, 4, 150};
	int	extended[] = {3, 3, 2, 1, 4, 75, 75};

	switch (config)
	{
	case CONFIG_1:
		memcpy(network_status, base, sizeof(network_status));
		break ;
	case CONFIG_2:
		memcpy(network_status, extended, sizeof(network_status));
		break ;
	default:
		break ;
	}
    return network_status;
}