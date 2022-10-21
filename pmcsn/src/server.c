#include "../utils/constants.h"
#include "../utils/structs.h"
#include <stdio.h>
#include <stdlib.h>

void	initServers(block *blocks, int num)
{
	server	**multi_servers;

	multi_servers = malloc(num * sizeof(server *));
	if (multi_servers == NULL)
	{
		printf("Error allocating multi-servers\n");
		return ;
	}
	blocks->num_servers = num;
	// multi-server initialization
	for (int s = 0; s < num; s++)
	{
		multi_servers[s] = malloc(sizeof(server));
		multi_servers[s]->sum = malloc(sizeof(sum));
		if (multi_servers[s]->sum == NULL || multi_servers[s] == NULL)
		{
			printf("Error Malloc\n");
			return ;
		}
		multi_servers[s]->status = IDLE;
	}
	blocks->servers = multi_servers;
}

int	findBusyServer(server **multi_servers, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (multi_servers[i]->status == BUSY)
			return (i);
	}
	return (-1);
}

int	findIdleServer(server **multi_servers, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (multi_servers[i]->status == IDLE)
			return (i);
	}
	return (-1);
}

int getBusyServers(server **multi_servers, int num)
{
    int count = 0;
    for (int i = 0; i < num; i++)
	{
		if (multi_servers[i]->status == BUSY)
			count++;
	}
	return count;
}

int	areThereMoreServers(block **blocks)
{
	int	num;

	for (int s = 0; s < BLOCKS; s++)
	{
		num = blocks[s]->num_servers;
		if (num > 0)
			if (findBusyServer(blocks[s]->servers, num) != -1)
				return (TRUE);
	}
	return (FALSE);
}
