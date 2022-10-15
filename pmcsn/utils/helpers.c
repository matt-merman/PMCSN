#include "helpers.h"

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

double	getService(block_type type, int stream)
{
	SelectStream(stream); // stream = 1,2,...,BLOCKS
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
		return (0.0);
	}
}

void calculateStatistics(long int completedJobs, clock *clock, area *area, statistics *stats){
	stats->completedJobs = completedJobs;
	stats->interarrivalTime = clock->last / completedJobs;
	stats->wait = area->node / completedJobs;
	stats->delay = area->queue / completedJobs;
	stats->serviceTime = area->service / completedJobs;
	stats->nodePopulation = area->node / clock->current;
	stats->queuePopulation = area->queue / clock->current;
	stats->utilization = area->service / clock->current;
}

void	showStatistics(statistics *stats)
{
	printf(": %ld people\n", stats->completedJobs);
	printf("\taverage interarrival time = %6.2f\ts\n", stats->interarrivalTime);
	printf("\taverage node wait ....... = %6.2f\ts\n", stats->wait);
	printf("\taverage queue delay ..... = %6.2f\ts\n", stats->delay);
	printf("\taverage service time .... = %6.2f\ts\n", stats->serviceTime);
	printf("\taverage # in the node ... = %6.2f\tpeople\n", stats->nodePopulation);
	printf("\taverage # in the queue .. = %6.2f\tpeople\n", stats->queuePopulation);
	printf("\tutilization ............. = %6.4f\t-\n", stats->utilization);
}

void validateMM1(block* block, statistics* stats){
	if (IS_NOT_EQUAL(stats->wait, stats->delay + stats->serviceTime))
	{
		printf("Response time of block %18s: %6.2lf s,\tbut it's not equal to queue delay plus service time: \t%6.2lf s\n",
			   block->name, stats->wait, stats->delay + stats->serviceTime);
	}
	if (IS_NOT_EQUAL(stats->nodePopulation, stats->queuePopulation + stats->utilization))
	{
		printf("Population of block    %18s: %6.2lf,\tbut it's not equal to queue plus service population: \t%6.2lf\n",
			   block->name, stats->nodePopulation, stats->queuePopulation + stats->utilization);
	}
}
