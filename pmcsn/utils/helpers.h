#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "./constants.h"
#include "./structs.h"
#include <stdio.h>

double	min(double a, double c)
{
	if (a < c)
		return (a);
	else
		return (c);
}

double	getArrival(double current)
{
	double	arrival;

	arrival = current;
	SelectStream(0);
	arrival += Exponential(1 / (LAMBDA * P_PRIMO_FUORI));
	return (arrival);
}

double	getService(enum block_types type)
{
	SelectStream(1);
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
	printf("   average # in the queue .. = %6.2f\n", area->queue
			/ clock->current);
	printf("   utilization ............. = %6.2f\n", area->service
			/ clock->current);
}