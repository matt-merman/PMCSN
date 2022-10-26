#include "helpers.h"
#include "structs.h"

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

void calculateStatistics(long int completedJobs, clock *clock, area *area, statistics *stats, int num_servers){
	stats->completedJobs = completedJobs;
	stats->interarrivalTime = clock->last / completedJobs;
	stats->wait = area->node / completedJobs;
	stats->delay = area->queue / completedJobs;
	stats->serviceTime = area->service / completedJobs;
	stats->nodePopulation = area->node / clock->current;
	stats->queuePopulation = area->queue / clock->current;
	stats->utilization = area->service / (clock->current * num_servers);
}

void showStatistics(block **blocks, clock *clock)
{
	int total_cost = 0;
	for (int i = 0; i < BLOCKS; i++)
	{
		printf("\t----------------------------------------------------------\n");
		printf("\t'%s' block info:\n\n", blocks[i]->name);
		statistics stats; // stack-allocated
		calculateStatistics(blocks[i]->completedJobs, clock, blocks[i]->blockArea, &stats, blocks[i]->num_servers);
		printf("\t\tpeople in the block ..... = %ld\tpeople\n", stats.completedJobs);
		// job averaged
		printf("\t\taverage interarrival time = %6.2f\ts\n", stats.interarrivalTime);
		printf("\t\taverage node wait ....... = %6.2f\ts\n", stats.wait);
		printf("\t\taverage queue delay ..... = %6.2f\ts\n", stats.delay);
		printf("\t\taverage service time .... = %6.2f\ts\n\n", stats.serviceTime);
		// time averaged
		printf("\t\taverage # in the node ... = %6.2f\tpeople\n", stats.nodePopulation);
		printf("\t\taverage # in the queue .. = %6.2f\tpeople\n", stats.queuePopulation);
		printf("\t\tutilization ............. = %6.4f\t-\n", stats.utilization);
		//validateMM1(blocks[i], &stats);
		printf("\n\t\tMulti-server statistics:\n");
		printf("\t\t    server     utilization     avg service\n");
		for (int s = 0; s < blocks[i]->num_servers; s++){	
			printf("\t\t%8d %14.4f %15.2f\n", 
					s, 
					blocks[i]->servers[s]->sum->service / clock->current, 
					blocks[i]->servers[s]->sum->service / blocks[i]->servers[s]->sum->served);
		}
		if (blocks[i]->type != CONSUMAZIONE){
			total_cost += calculateCosts(blocks[i]->num_servers);
			printf("\n\t\tDaily Configuration Cost for '%s': %d \u20AC \n\n", blocks[i]->name, calculateCosts(blocks[i]->num_servers));
		}
	}
	printf("\t----------------------------------------------------------\n");
	printf("\n\t\tTotal Daily Configuration Cost: %d \u20AC \n\n", total_cost);
}

void validateMM1(block* block, statistics* stats){
	if (IS_NOT_EQUAL(stats->wait, stats->delay + stats->serviceTime))
	{
		printf("\tResponse time of block %18s: %6.2lf s,\tbut it's not equal to queue delay plus service time: \t%6.2lf s\n",
			   block->name, stats->wait, stats->delay + stats->serviceTime);
	}
	if (IS_NOT_EQUAL(stats->nodePopulation, stats->queuePopulation + stats->utilization))
	{
		printf("\tPopulation of block    %18s: %6.2lf,\tbut it's not equal to queue plus service population: \t%6.2lf\n",
			   block->name, stats->nodePopulation, stats->queuePopulation + (block->num_servers* stats->utilization));
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

int calculateCosts(int num)
{
	int cost = 0;
	for(int i = 0; i < num; i++)
		cost += SALARY;
	return cost;
}
