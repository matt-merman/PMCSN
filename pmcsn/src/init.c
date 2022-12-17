#include "init.h"


block	**init_blocks(int *config)
{
    char	*names[] = {"Primi", "Secondi e Contorni", "Frutta e Dessert",
		"Casse Fast", "Casse standard", "Locale Mensa"};
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
		if (b[i]->block_area == NULL || b[i] == NULL)
		{
			printf("Error Malloc\n");
			return (NULL);
		}
		strncpy(b[i]->name, names[i], strlen(names[i]) + 1);
		memset(b[i]->block_area, 0x0, sizeof(area));
		init_servers(b[i], config[i]);
	}
	return (b);
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
	c->arrival = get_arrival(START, LAMBDA);
	c->completion = INF;
	if (p < P_PRIMO_FUORI)
		c->type = 0;
	else
		c->type = 1;
	return c;
}

int	network_status[] = {0, 0, 0, 0, 0, 0};

int *	init_network(int config)
{
	int	status_one[] = {2, 1, 1, 1, 2, 50};
	int	status_two[] = {2, 1, 1, 1, 2, 50};
	int	status_three[] = {2, 1, 1, 1, 2, 50};

	switch (config)
	{
	case CONFIG_1:
		memcpy(network_status, status_one, sizeof(network_status));
		break ;
	case CONFIG_2:
		memcpy(network_status, status_two, sizeof(network_status));
		break ;
	case CONFIG_3:
		memcpy(network_status, status_three, sizeof(network_status));
		break ;
	default:
		break ;
	}
    return network_status;
}