#include "simulation.h"
#include <time.h>

/**
 * Updates time-averaged integrals for node and queue populations
 * @param event the current event to process
 * @param canteen the network that represents the canteen
 */
void update_area_stats(event *event, network *canteen)
{
	area	*area;
	double	diff;
	int		i;

	diff = event->time - canteen->system_clock->current;
	for (i = 0; i < BLOCKS; i++)
	{
		if (canteen->blocks[i]->jobs > 0)
		{
			area = canteen->blocks[i]->block_area;
			area->node += diff * (double) canteen->blocks[i]->jobs;
			area->queue += diff * (double) canteen->blocks[i]->queue_jobs;
		}
	}
}

/**
 * @param arrived_jobs to count the number of arriving job in batch means simulation
 * @param canteen the network of service nodes that represents a canteen
 */
void simulation(network *canteen, int jobs, int *arrived_jobs, sim_type type)
{
	event		*current_event;
	block_type	btype;
    int perc;
    int prev_perc = 0;
#if DEBUG == TRUE
    long elapsed_time;
    time_t begin, end;
    time(&begin);
#endif
    while (TRUE)
	{
        perc = (int) ((long) canteen->system_clock->current / (PERIOD / 100));
        if ( (int) perc > prev_perc || perc == 100){
            printf("%d%%\r", perc);
            fflush(stdout);
            prev_perc = perc;
        }
		if(termination_conditions(type, canteen, jobs, arrived_jobs))
			break;

		current_event = get_next_event();

		if (current_event->event_type == ARRIVAL){
			// we update the last arrival time
			canteen->system_clock->last_arrival = current_event->time;
			if (arrived_jobs != NULL)
				(*arrived_jobs)++;
		}

        	update_area_stats(current_event, canteen);
		canteen->system_clock->current = current_event->time;
		btype = current_event->block_type;
		switch (current_event->event_type)
		{
		case ARRIVAL:
			// schedule its completion (if a server is idle) or add to queue and generate a new outside arrival
			process_arrival(current_event, canteen->system_clock, canteen->blocks[btype], type);
			break ;
		case IMMEDIATE_ARRIVAL:
			// schedule its completion or add to queue
			process_immediate_arrival(current_event, canteen->system_clock,canteen->blocks[btype]);
			break ;
		case COMPLETION:
			// schedule the next completion and generate an immediate arrival
			process_completion(current_event, canteen->system_clock, canteen->blocks[btype]);
			break ;
		default:
			break ;
		}
		// the current_event is processed and now it can be freed
		sort_list();
        debug(current_event, canteen);
		free(current_event);
	}
#if DEBUG == TRUE
	if (arrived_jobs != NULL)
		printf("BATCH MEANS INFO: %d batch dimension - %d counted arrivals\n", jobs, *arrived_jobs);

    time(&end);
    // compute and print the elapsed time in millisec
    elapsed_time = end - begin;
    long elapsed_time_hours = elapsed_time / 3600L;
    long elapsed_time_minutes = (elapsed_time % 3600L) / 60L;
    long elapsed_time_seconds = elapsed_time % 60L;
    printf("Simulation time hh:mm:ss - %02li:%02li:%02li\n", elapsed_time_hours, elapsed_time_minutes, elapsed_time_seconds);

#endif
}

int termination_conditions(sim_type type, network *canteen, int jobs, const int * arrived_jobs){
	switch(type){
		case STANDARD:
		/* FINITE SIMULATION: breaks if the times is finished,
          	all events are processed and all servers are idle */
		case FINITE:
			return (canteen->system_clock->last_arrival >= PERIOD && !are_there_more_events());
		/* BATCH MEANS SIMULATION: breaks if all jobs in the batch are arrived. */
		case INFINITE:
			return (jobs == (*arrived_jobs));
	}
	return FALSE;
}

/**
 * To process an arrival, we need to schedule or enqueue the job and then generate a new ARRIVAL event
 * @param event
 * @param c
 * @param block
 */
void	process_arrival(event *current_event, timer *c, block *block, sim_type sim_type)
{
	double	p;
	event	*new_event;
	process_immediate_arrival(current_event, c, block);
	p = Random();
    if (p < P_PRIMO_FUORI) {
		new_event = create_event(PRIMO, -1, ARRIVAL, c->current, current_event);
    } else {
		new_event = create_event(SECONDO, -1, ARRIVAL, c->current, current_event);
    }
    // if we have a new outside arrival but in a time after the observation period,
	//we skip it.
	if (sim_type != INFINITE && try_terminate_clock(c, new_event->time))
	{
		free(new_event);
		return;
	}
	insert_event_first(new_event);
      block->count_to_next[ESTERNO]++;
}

/**
 * After an arrival or immediate arrival, we schedule a process_completion,
 * if a server of the block is idle.
 * Otherwise, it adds the job to the queue.
 * @param event the event who will schedule the completion or will be enqueued.
 * @param c the time of the current arrival event
 * @param block the service node to which the job is arrived
 */
void	process_immediate_arrival(event *arrival_event, timer *c, block *block)
{
	int		s_index;
	server	*s;
	double	next_completion_time;
	event	*next_completion_event;

	block->jobs++;
	s_index = retrieve_idle_server(block);
	if (s_index != -1)
	{
		s = block->servers[s_index];
		next_completion_event = create_insert_event(block->type, s_index,COMPLETION, c, arrival_event);
		if (next_completion_event != NULL)
		{
			next_completion_time = (next_completion_event->time - c->current);
            //printf("%Lf", s->sum->service);
			s->sum->service += next_completion_time;
			s->sum->served++;
            block->block_area->service += next_completion_time;
		}
	}
	else if (block->type == CONSUMAZIONE)
	{
		block->jobs--;
		block->rejected_jobs++;
	}
	else
		block->queue_jobs++;
}

/**
 * A completion should schedule another completion event (if there are job in queue) and an arrival.
 * @param completion_event
 * @param c
 * @param block
 */
void	process_completion(event *completion_event, timer *c, block *block)
{
	event	*next_completion_event;
	double	next_completion_time;
	server	*s;
	int		serv_id;

	next_completion_event = NULL;
	block->completed_jobs++;
	block->jobs--;
	if (completion_event->target_server == -1)
	{
		PRINTF("This is bad! A completion from who knows from!\n");
		exit(-1);
	}
	free_busy_server(block, completion_event->target_server);
	if (block->type == CONSUMAZIONE)
		return ;
	if (block->queue_jobs > 0)
	{
		serv_id = retrieve_idle_server(block);
		if (serv_id != -1)
		{
			s = block->servers[completion_event->target_server];
			next_completion_event = create_insert_event(block->type, serv_id,
					COMPLETION, c, completion_event);
			if (next_completion_event != NULL)
			{
				next_completion_time = (next_completion_event->time - c->current);
				block->queue_jobs--;
				block->block_area->service += next_completion_time;
				s->sum->service += next_completion_time;
				s->sum->served++;
			}
		}
	}
	schedule_immediate_arrival(block, c, completion_event);
}

/**
 *
 * @param type
 * @param c
 * @param triggering_event
 */
void	schedule_immediate_arrival(block* block, timer *c, event *triggering_event)
{
	double	p;
	int		next_type;

	switch (block->type)
	{
	case PRIMO:
		p = Random();
		if (p < P_SECONDO_PRIMO)
			next_type = SECONDO;
		else if (p < P_SECONDO_PRIMO + P_DESSERT_PRIMO)
			next_type = DESSERT;
		else
			next_type = CASSA_FAST;
		break ;
	case SECONDO:
        // TODO: se abbiamo tempo, invece di andare a random alle casse, bisogna scegliere a seconda del percorso preso dall'utente.
		p = Random();
		if (p < P_DESSERT_SECONDO)
			next_type = DESSERT;
		else if (p < P_DESSERT_SECONDO + P_CASSA_STD_SECONDO)
			next_type = CASSA_STD;
		else
			next_type = CASSA_FAST;
		break ;
	case DESSERT:
		next_type = CASSA_STD;
		break ;
	case CASSA_STD:
	case CASSA_FAST:
		next_type = CONSUMAZIONE;
		break ;
	default:
		return ;
	}

	block->count_to_next[next_type]++;
	create_insert_event(next_type, -1, IMMEDIATE_ARRIVAL, c, triggering_event);

}
