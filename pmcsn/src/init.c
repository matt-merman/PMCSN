#include "init.h"

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
		memset(b[i], 0x0, sizeof(block));
		b[i]->block_area = malloc(sizeof(area));
		b[i]->type = i;
		b[i]->num_servers = config[i];
		b[i]->rejected_jobs = 0;
		if (b[i]->block_area == NULL || b[i] == NULL)
		{
			printf("Error Malloc\n");
			return (NULL);
		}
		strncpy(b[i]->name, block_names[i], NAME_SIZE - 1);
		memset(b[i]->block_area, 0x0, sizeof(area));
		init_servers(b[i], config[i]);
	}
	return (b);
}

// Initializes the servers inside a block
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
        // Nel debugger CLION puoi vedere gli elementi block->servers con il watch: (server *[2]) *blocks->servers
        //  sostituisci il 2 con il valore di num
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

// Initializes the clock, and generates the first arrival
clock * 	init_clock(void)
{
	double p = Random();
	clock *c = (clock *)malloc(sizeof(clock));
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

int	network_status[] = {0, 0, 0, 0, 0, 0};

// Initializes the network configuration
int *	init_network(int config)
{
	int	status_one[] = {1, 1, 1, 1, 1, 50};
	int	ergodic[] = {3, 3, 2, 1, 4, 139}; // TODO: ATTENZIONE non cambiare il 139!!!!
	int	status_three[] = {2, 1, 1, 1, 2, 50};

	switch (config)
	{
	case CONFIG_1:
		memcpy(network_status, status_one, sizeof(network_status));
		break ;
	case CONFIG_2:
		memcpy(network_status, ergodic, sizeof(network_status));
		break ;
	case CONFIG_3:
		memcpy(network_status, status_three, sizeof(network_status));
		break ;
	default:
		break ;
	}
    return network_status;
}