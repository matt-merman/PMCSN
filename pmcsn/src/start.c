#include "server.h"
#include "start.h"
#include "init.h"
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	// system("clear");
	// printf("**************************************\n");
	// printf("**               PMCSN              **\n");
	// printf("**************************************\n");
	startSimulation();
	return (0);
}

void	updateStats(double diff, block **blocks)
{
	area	*area;

	for (int i = 0; i < BLOCKS; i++)
	{
		if (blocks[i]->jobs > 0)
		{
			area = blocks[i]->blockArea;
			area->node += diff * blocks[i]->jobs;
			area->queue += diff * blocks[i]->jobsInQueue;
		}
	}
}

int	startSimulation(void)
{
	clock	*system_clock;
	double	previous_clock;
	int		eventIndex;
	block	**blocks;
	event	*event;
	double p;

	system_clock = initClock();
	if (system_clock == NULL)
	{
		printf("Error on system clock\n");
		return (-1);
	}
	initEventList(system_clock->type);
	// initializing multi-stream lehmer generator
	PlantSeeds(123456789);
	int *network_status = initNetworkStatus(CONFIG_2);
	blocks = initBlocks(network_status);
	if (blocks == NULL)
	{
		printf("Error on blocks");
		return (-1);
	}
	while (1)
	{
		// We stop when next arrival time is greater than observation period and we have no more event to process
		if (!(system_clock->arrival < PERIODO || areThereMoreEvents()
				|| areThereMoreServers(blocks)))
			break ;
		eventIndex = getNextEventIndex();
		event = getEvent(eventIndex);
		system_clock->next = event;
		if (event->type == ARRIVAL)
			system_clock->arrival = event->time;
		updateStats(event->time - system_clock->current, blocks);
		previous_clock = system_clock->current;
		system_clock->current = event->time;
		switch (event->type)
		{
		case ARRIVAL:
			arrival(system_clock, previous_clock, blocks[event->blockType]);
			p = Random();
			if (p < P_PRIMO_FUORI)
				createAndInsertEvent(PRIMO, -1, ARRIVAL, system_clock);
			else
				createAndInsertEvent(SECONDO, -1, ARRIVAL, system_clock);
			break ;
		case IMMEDIATE_ARRIVAL:
			arrival(system_clock, previous_clock, blocks[event->blockType]);
			break ;
		case COMPLETION:
			completion(event->target_server, system_clock, previous_clock,
					blocks[event->blockType]);
			break ;
		default:
			break ;
		}
		clearEvent(eventIndex);
		//if (system_clock->arrival <= PERIODO)
		//	printf("Next arrival time: %lf\n", system_clock->arrival);
		// use \r instead of \n to print in one line
		//     printf("Time: %lf Event: %-18s Target Block: %-12s in server: %d\tjobs in blocks [%s, %s, %s, %s, %s, %s]\n",
		//            system_clock->arrival,
		//            toStrEvent(event->type),
		//            toStrBlock(event->blockType),
		//            event->target_server,
		//            getServerContents(blocks[PRIMO]),
		//            getServerContents(blocks[SECONDO]),
		//            getServerContents(blocks[DESSERT]),
		//            getServerContents(blocks[CASSA_FAST]),
		//            getServerContents(blocks[CASSA_STD]),
		//            getServerContents(blocks[CONSUMAZIONE]));
	}
	showStatistics(blocks, system_clock);
	free(system_clock);
	clearMem(blocks);
	return (0);
}

void	arrival(clock *c, double current, block *block)
{
	int		s_index;
	server	*s;
	double	service_time;

	block->jobs++;
	// we retrieve the number of server in the block
	// TODO: quale indice devo metterci? Lo stesso del job che sta gia dentro
	s_index = requestIdleServer(block);
	if (s_index != -1)
	{
		s = block->servers[s_index];
		// plus one because index start from zero,
		// but server index start from 1.
		service_time = createAndInsertEvent(block->type, s_index, COMPLETION,
				c);
		s->sum->service += service_time - current;
		s->sum->served++;
		block->blockArea->service += (service_time - current);
	}
	else
		block->jobsInQueue++;
}

void	scheduleArrive(int type, clock *c)
{
	double	p;

	switch (type)
	{
	case PRIMO:
		p = Random();
		if (p < P_SECONDO_PRIMO)
			createAndInsertEvent(SECONDO, -1, IMMEDIATE_ARRIVAL, c);
		else if (p > P_SECONDO_PRIMO && p < P_SECONDO_PRIMO + P_DESSERT_PRIMO)
			createAndInsertEvent(DESSERT, -1, IMMEDIATE_ARRIVAL, c);
		else
			createAndInsertEvent(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case SECONDO:
		p = Random();
		if (p < P_DESSERT_SECONDO)
			createAndInsertEvent(DESSERT, -1, IMMEDIATE_ARRIVAL, c);
		else if (p > P_DESSERT_SECONDO && p < P_DESSERT_SECONDO
				+ P_CASSA_STD_SECONDO)
			createAndInsertEvent(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c);
		else
			createAndInsertEvent(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case DESSERT:
		createAndInsertEvent(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case CASSA_FAST:
	case CASSA_STD:
		createAndInsertEvent(CONSUMAZIONE, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	default:
		break ;
	}
}
void	completion(int server_id, clock *c, double current, block *block)
{
	double service_time;
	server *s;

	block->completedJobs++;
	block->jobs--;

	// FIXME: a volte server_id va fuori range
	// findBusyServer(blocks[type]->servers, num);
	if (server_id != -1)
		freeBusyServer(block, server_id);
	else
	{
		printf("This is bad! A completion from who knows from!\n");
		exit(-1);
	}

	// After this completion,
	// we immediately schedule a new completion if there is at least one job in queue
	if (block->jobsInQueue > 0)
	{
		int serv_id = requestIdleServer(block);
		s = block->servers[server_id];
		service_time = createAndInsertEvent(block->type, serv_id, COMPLETION,
				c);
		// TODO: maybe there is only one server (?)
		block->jobsInQueue--;
		s->sum->service += (service_time - current);
		block->blockArea->service += (service_time - current);
		s->sum->served++;
	}
	//	createAndInsertEvent(type, server_id + 1, COMPLETION, c);
	//	}else if (blocks[type]->jobs > 0)
	// TODO: from which server will complete???

	scheduleArrive(block->type, c);
}