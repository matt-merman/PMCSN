#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ogni blocco è un centro del nostro modello
//i vari blocchi sono definiti nella variabile block_types
struct block	blocks[BLOCKS];

//lista contenente tutti gli eventi
int				n = 1000;
event			eventList[1000];

void	complete(int blockType, struct clock_t *c)
{
	event	*newEvent;

	switch (blockType)
	{
	case PRIMO:
		//aggiorno i numeri nel centro
		blocks[PRIMO].completedJobs++;
		blocks[PRIMO].jobs--;
		if (blocks[PRIMO].jobs > 0)
		{
			newEvent = createEvent(PRIMO, COMPLETE, c->current, 0);
			if (newEvent->time > PERIODO)
			{
				c->last = c->current;
				c->arrival = INF;
			}
			insertElement(eventList, newEvent, n);
		}
		//genera arrivo ai 'secondi'
		newEvent = createEvent(SECONDO, ARRIVAL, c->current, 1);
		break ;
	case SECONDO:
		//aggiorno i numeri nel centro
		blocks[SECONDO].completedJobs++;
		blocks[SECONDO].jobs--;
		if (blocks[SECONDO].jobs > 0)
		{
			newEvent = createEvent(SECONDO, COMPLETE, c->current, 0);
			if (newEvent->time > PERIODO)
			{
				c->last = c->current;
				c->arrival = INF;
			}
			insertElement(eventList, newEvent, n);
		}
		//genera arrivo ai 'dessert'
		newEvent = createEvent(DESSERT, ARRIVAL, c->current, 1);
		break ;
	case DESSERT:
		//aggiorno i numeri nel centro
		blocks[DESSERT].completedJobs++;
		blocks[DESSERT].jobs--;
		if (blocks[DESSERT].jobs > 0)
		{
			newEvent = createEvent(DESSERT, COMPLETE, c->current, 0);
			break ;
		}
		else
			return ;
	}
	if (newEvent->time > PERIODO)
	{
		c->last = c->current;
		c->arrival = INF;
	}
	insertElement(eventList, newEvent, n);
}
void	arrival(int blockType, struct clock_t *c)
{
	event	*newEvent;

	switch (blockType)
	{
	case PRIMO:
		blocks[PRIMO].jobs++;
		newEvent = createEvent(PRIMO, ARRIVAL, c->current, LAMBDA
				* P_PRIMO_FUORI);
		insertElement(eventList, newEvent, n);
		if (newEvent->time > PERIODO)
		{
			c->last = c->current;
			c->arrival = INF;
		}
		if (blocks[PRIMO].jobs == 1)
		{
			newEvent = createEvent(PRIMO, COMPLETE, c->current, 0);
			break ;
		}
		else
			return ;
	case SECONDO:
		blocks[SECONDO].jobs++;
		if (blocks[SECONDO].jobs == 1)
		{
			newEvent = createEvent(SECONDO, COMPLETE, c->current, 0);
			break ;
		}
		else
			return ;
	case DESSERT:
		blocks[DESSERT].jobs++;
		if (blocks[DESSERT].jobs == 1)
		{
			newEvent = createEvent(DESSERT, COMPLETE, c->current, 0);
			break ;
		}
		else
			return ;
	}
	if (newEvent->time > PERIODO)
	{
		c->last = c->current;
		c->arrival = INF;
	}
	insertElement(eventList, newEvent, n);
}

void	initBlocks(void)
{
	for (int index = 0; index < BLOCKS; index++)
	{
		blocks[index].blockArea = malloc(sizeof(struct area));
		if (blocks[index].blockArea == NULL)
		{
			printf("Error Malloc\n");
			return ;
		}
		//struttura dati usata per per calcolare i time-averaged number rispettivamente
		//nel nodo,in coda e in servizio.Sono calcolati come integrali
		blocks[index].blockArea->node = 0;
		blocks[index].blockArea->queue = 0;
		blocks[index].blockArea->service = 0;
		blocks[index].type = index;
		blocks[index].completedJobs = 0;
		blocks[index].jobs = 0;
	}
}

int	gitMinEventIndex(void)
{
	int index = 0;
	double min = eventList[index].time;
	for (int i = 0; i < n; i++)
	{
		if (min < 0)
		{
			index++;
			min = eventList[index].time;
			continue ;
		}
		if (eventList[i].time < min && eventList[i].time > 0)
		{
			min = eventList[i].time;
			index = i;
		}
	}

	return index;
}

int	startSimulation(void)
{
	struct clock_t	*system_clock;
	int				indexEvent;

	//inizializzo il clock globale (vedi in struct.h)
	system_clock = malloc(sizeof(struct clock_t));
	if (system_clock == NULL)
	{
		printf("Error Malloc\n");
		return (-1);
	}
	system_clock->current = START;
	system_clock->arrival = getArrival(START, (LAMBDA * P_PRIMO_FUORI));
	system_clock->type = 0;
	system_clock->completion = INF;
	for (int i = 0; i < n; i++)
	{
		eventList[i].time = -1;
	}
	//aggiungo il primo evento di arrivo alla lista degli eventi
	eventList[0].blockType = 0;
	eventList[0].type = ARRIVAL;
	eventList[0].time = getArrival(START, (LAMBDA * P_PRIMO_FUORI));
	PlantSeeds(123456789);
	initBlocks();
	/* ---------------------------------------------------------- 
     * Next-event Simulation di durata O_P con 3 centri FIFO M/M/1
     * ---------------------------------------------------------- 
    */
	while ((system_clock->arrival < PERIODO)) // || (blocks[0].jobs > 0) || (blocks[1].jobs > 0) || (blocks[2].jobs > 0))
	{
		indexEvent = gitMinEventIndex();
		system_clock->next = &eventList[indexEvent];
		struct area * area;
		double time = system_clock->next->time;
		for (int index = 0; index < BLOCKS; index++)
		{
			if (blocks[index].jobs > 0)
			{
				area = blocks[index].blockArea;
				area->node += (time - system_clock->current) * blocks[index].jobs;
				area->queue += (time - system_clock->current) * (blocks[index].jobs - 1);
				area->service += (time - system_clock->current);
			}
		}
		//porto avanti il tempo del sistema fino al primo evento
		system_clock->current = system_clock->next->time;
		if (system_clock->next->type == ARRIVAL)
			arrival(system_clock->next->blockType, system_clock);
		else
			complete(system_clock->next->blockType, system_clock);
		eventList[indexEvent].time = -1;
		// printf("-------------------------\n");
		// for (int j = 0; j < n; j++){
		// 	printf("%lf\n", eventList[j].time);
		// 	printf("%d\n", eventList[j].blockType);
		// 	printf("%d\n", eventList[j].type);
		// }
		// printf("-------------------------\n");
	}
	for (int i = 0; i < BLOCKS; i++)
	{
		showStatistics(blocks[i].completedJobs, system_clock,
				blocks[i].blockArea);
		printf("\n");
	}
	return (0);
}

int	main(void)
{
	startSimulation();
	return (0);
}