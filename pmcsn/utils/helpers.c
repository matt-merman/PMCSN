#include "helpers.h"

double	get_arrival(double current, double lambda)
{
	double	arrival;

	arrival = current;
	SelectStream(0);
	arrival += Exponential(1/lambda);
	return (arrival);
}

double	get_service(block_type type, int stream)
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

void get_stats(long int completedJobs, clock *clock, area *area, statistics *stats, int num_servers){
	stats->completed_jobs = completedJobs;
	stats->interarrival_time = clock->last / completedJobs;
	stats->wait = area->node / completedJobs;
	stats->delay = area->queue / completedJobs;
	stats->service_time = area->service / completedJobs;
	stats->node_pop = area->node / clock->current;
	stats->queue_pop = area->queue / clock->current;
	stats->utilization = area->service / (clock->current * num_servers);
}

void show_stats(block **blocks, clock *clock)
{
	int total_cost = 0;
	statistics stats;
	
	for (int i = 0; i < BLOCKS; i++)
	{
		printf("\t----------------------------------------------------------\n");
		printf("\t'%s' block info:\n\n", blocks[i]->name);
		get_stats(blocks[i]->completed_jobs, clock, blocks[i]->block_area, &stats, blocks[i]->num_servers);
		printf("\t\tpeople in the block ..... = %ld\tpeople\n", stats.completed_jobs);
		// job averaged
		printf("\t\taverage interarrival time = %6.2f\ts\n", stats.interarrival_time);
		printf("\t\taverage node wait ....... = %6.2f\ts\n", stats.wait);
		printf("\t\taverage queue delay ..... = %6.2f\ts\n", stats.delay);
		printf("\t\taverage service time .... = %6.2f\ts\n\n", stats.service_time);
		// time averaged
		printf("\t\taverage # in the node ... = %6.2f\tpeople\n", stats.node_pop);
		printf("\t\taverage # in the queue .. = %6.2f\tpeople\n", stats.queue_pop);
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
			total_cost += get_costs(blocks[i]->num_servers);
			printf("\n\t\tDaily Configuration Cost for '%s': %d \u20AC \n\n", blocks[i]->name, get_costs(blocks[i]->num_servers));
		}
	}
	printf("\t----------------------------------------------------------\n");
	printf("\n\t\tTotal Daily Configuration Cost: %d \u20AC \n\n", total_cost);
}

void validateMM1(block* block, statistics* stats){
	if (IS_NOT_EQUAL(stats->wait, stats->delay + stats->service_time))
	{
		printf("\tResponse time of block %18s: %6.2lf s,\tbut it's not equal to queue delay plus service time: \t%6.2lf s\n",
			   block->name, stats->wait, stats->delay + stats->service_time);
	}
	if (IS_NOT_EQUAL(stats->node_pop, stats->queue_pop + stats->utilization))
	{
		printf("\tPopulation of block    %18s: %6.2lf,\tbut it's not equal to queue plus service population: \t%6.2lf\n",
			   block->name, stats->node_pop, stats->node_pop + (block->num_servers* stats->utilization));
	}
}

void clear_mem(block **blocks){
	for (int i = 0; i < BLOCKS; i++){
		if (blocks[i]->block_area != NULL){
			free(blocks[i]->block_area);
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

int get_costs(int num)
{
	int cost = 0;
	for(int i = 0; i < num; i++)
		cost += SALARY;
	return cost;
}
