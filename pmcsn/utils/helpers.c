#include "helpers.h"
#include "structs.h"

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
	arrival += Exponential(1/lambda);
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
	stats->trafficIntensity = area->service / clock->current;
	// number of jobs * mean service time / last completion time 
	stats->utilization = completedJobs * stats->serviceTime / clock->current;
}

void showStatistics(block **blocks, clock *clock)
{

	printf("\n====================================================================================================\n");
	for (int i = 0; i < BLOCKS; i++)
	{
		printf("%s", blocks[i]->name);
		statistics stats; // stack-allocated
		calculateStatistics(blocks[i]->completedJobs, clock, blocks[i]->blockArea, &stats);
		printf(": %ld people\n", stats.completedJobs);
		// job averaged
		printf("\taverage interarrival time = %6.2f\ts\n", stats.interarrivalTime);
		printf("\taverage node wait ....... = %6.2f\ts\n", stats.wait);
		printf("\taverage queue delay ..... = %6.2f\ts\n", stats.delay);
		printf("\taverage service time .... = %6.2f\ts\n", stats.serviceTime);
		// time averaged
		printf("\taverage # in the node ... = %6.2f\tpeople\n", stats.nodePopulation);
		printf("\taverage # in the queue .. = %6.2f\tpeople\n", stats.queuePopulation);
		printf("\ttraffic intensity ....... = %6.4f\t-\n", stats.trafficIntensity);
		printf("\tutilization ............. = %6.4f\t-\n", stats.utilization);
		validateMM1(blocks[i], &stats);
		if (blocks[i]->num_servers > 0){
			printf("\n\tMulti-server statistics:\n\n");
			printf("\t    server     trafficIntensity     avg service\n");
		}
		for (int s = 0; s < blocks[i]->num_servers; s++){	
			printf("\t%8d %14.3f %15.2f\n", 
					s, 
					blocks[i]->servers[s]->sum->service / clock->current, 
					blocks[i]->servers[s]->sum->service / blocks[i]->servers[s]->sum->served);
		}
	}
  	printf("\n");

}

void validateMM1(block* block, statistics* stats){
	if (IS_NOT_EQUAL(stats->wait, stats->delay + stats->serviceTime))
	{
		printf("\tResponse time of block %18s: %6.2lf s,\tbut it's not equal to queue delay plus service time: \t%6.2lf s\n",
			   block->name, stats->wait, stats->delay + stats->serviceTime);
	}
	if (IS_NOT_EQUAL(stats->nodePopulation, stats->queuePopulation + stats->trafficIntensity))
	{
		printf("\tPopulation of block    %18s: %6.2lf,\tbut it's not equal to queue plus service population: \t%6.2lf\n",
			   block->name, stats->nodePopulation, stats->queuePopulation + stats->trafficIntensity);
	}
}

void clearMem(block **blocks){
	for (int i = 0; i < BLOCKS; i++){
		if (blocks[i]->blockArea != NULL){
			free(blocks[i]->blockArea);
		}
		for (int j = 0; j < blocks[i]->num_servers; j++){
			if (blocks[i]->servers[j]->sum != NULL){
				free(blocks[i]->servers[j]->sum);
			}
			if (blocks[i]->servers[j] != NULL){
				free(blocks[i]->servers[j]);
			}
		}
	}
}
