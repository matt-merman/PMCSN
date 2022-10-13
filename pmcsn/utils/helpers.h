#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "./constants.h"
#include "./structs.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

double	min(double a, double c)
{
	if (a < c)
		return (a);
	else
		return (c);
}

double	getArrival(double current, double lambda)
{
	double	arrival;

	arrival = current;
	SelectStream(0);
	arrival += Exponential(1 / lambda);
	return (arrival);
}

double	getService(enum block_types type, int stream)
{
	SelectStream(stream);
	switch (type)
	{
	case PRIMO:
		return (Exponential(S_PRIMO));
	case SECONDO:
		return (Exponential(S_SECONDO));
	case DESSERT:
		return (Exponential(S_DESSERT));
	case CASSA_FAST:
		return (Exponential(S_CASSA_FAST));
	case CASSA_STD:
		return (Exponential(S_CASSA_STD));
	case CONSUMAZIONE:
		return (Exponential(S_CONSUMAZIONE));
	default:
		return (0);
	}
}

void	showStatistics(long int index, struct clock_t *clock, struct area *area)
{
	printf("\nfor %ld jobs\n", index);
	printf("   average interarrival time = %6.2f\n", clock->last / index);
	printf("   average wait ............ = %6.2f\n", area->node / index);
	printf("   average delay ........... = %6.2f\n", area->queue / index);
	printf("   average service time .... = %6.2f\n", area->service / index);
	printf("   average # in the node ... = %6.2f\n", area->node
			/ clock->current);
	printf("   average # in the queue .. = %6.2f\n",
			area->queue / clock->current);
	printf("   utilization ............. = %6.4f\n",
			area->service / clock->current);
}

void	insertElement(event list[], event *elem, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (list[i].time == -1)
		{
			list[i].time = elem->time;
			list[i].blockType = elem->blockType;
			list[i].type = elem->type;
			return ;
		}
	}
}

event	*createEvent(enum block_types blockType, int type, double time)
{
	event *newEvent = (event*)malloc(sizeof(struct event_t));
	if (newEvent == NULL)
	{
		printf("Error in Malloc");
		return newEvent;
	}
	newEvent->blockType = blockType;
	newEvent->type = type;
	if (type == ARRIVAL)
		newEvent->time = getArrival(time, LAMBDA);
	else if(type == IMMEDIATE_ARRIVAL)
		newEvent->time = time;
	else
		newEvent->time = time + getService(blockType, blockType+1);
	 
	return newEvent;
}