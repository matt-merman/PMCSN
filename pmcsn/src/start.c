#include "event_list.h"
#include "start.h"
#include <stdio.h>

void	main(void)
{
	start_simulation();
}

/**
 * FIXME: This doesn't work!
 *
 * Updates time-averaged integrals for node and queue populations
 *
 * @param diff time before next event
 * @param blocks
 */
void	update_area_stats(block **blocks, event *event, clock *clock)
{
	area	*area;
	double	diff;
	int		i;

	diff = event->time - clock->current;
	for (i = 0; i < BLOCKS; i++)
	{
		if (blocks[i]->jobs > 0)
		{
			area = blocks[i]->block_area;
			area->node += diff * (double)blocks[i]->jobs;
			area->queue += diff * (double)blocks[i]->queue_jobs;
		}
	}
}

void	update_areas_for_block(block *block, event *event, clock *clock)
{
	area	*area;
	double	diff;

	if (block->jobs > 0)
	{
		area = block->block_area;
		diff = event->time - clock->current;
		area->node += diff * (double)block->jobs;
		area->queue += diff * (double)block->queue_jobs;
	}
}

int	start_simulation(void)
{
	clock		*system_clock;
	block		**blocks;
	event		*current_event;
	int			*network_status;
	block_type	btype;

	system_clock = init_clock();
	if (system_clock == NULL)
	{
		PRINTF("Error on system clock\n");
		return (-1);
	}
	init_event_list(system_clock->type);
	// initializing multi-stream lehmer generator
	PlantSeeds(123456789);
	network_status = init_network(CONFIG_2);
	blocks = init_blocks(network_status);
	if (blocks == NULL)
	{
		PRINTF("Error on blocks");
		return (-1);
	}
	while (true)
	{
		/* break if the times is finished,
			all events are processed and all servers are idle */
		if (system_clock->last_arrival >= PERIOD && !are_there_more_events()
			&& !are_there_busy_servers(blocks))
			break ;
		current_event = get_next_event();
		if (current_event->event_type == ARRIVAL)
			// we update the last arrival time
			system_clock->last_arrival = current_event->time;
		update_area_stats(blocks, current_event, system_clock);
		system_clock->current = current_event->time;
		btype = current_event->block_type;
		switch (current_event->event_type)
		{
		case ARRIVAL:
			// schedule its completion (if a server is idle) or add to queue and generate a new outside arrival
			process_arrival(current_event, system_clock, blocks[btype]);
			break ;
		case IMMEDIATE_ARRIVAL:
			// schedule its completion or add to queue
			process_immediate_arrival(current_event, system_clock,
					blocks[btype]);
			break ;
		case COMPLETION:
			// schedule the next completion and generate an immediate arrival
			process_completion(current_event, system_clock, blocks[btype]);
			break ;
		default:
			break ;
		}
		// the current_event is processed and now it can be freed
		sort_list();
		debug(system_clock, blocks, current_event);
		free(current_event);
	}
	show_and_validate_stats(blocks, system_clock);
	printf("Rejected job = %ld\n", blocks[CONSUMAZIONE]->rejected_jobs);
	free(system_clock);
	clear_mem(blocks);
	return (0);
}

// To process an arrival, we need to schedule or enqueue the job and then generate a new ARRIVAL event
void	process_arrival(event *event, clock *c, block *block)
{
	double	p;

	process_immediate_arrival(event, c, block);
	p = Random();
	if (p < P_PRIMO_FUORI)
		create_insert_event(PRIMO, -1, ARRIVAL, c, event);
	else
		create_insert_event(SECONDO, -1, ARRIVAL, c, event);
}

/**
 * After an arrival or immediate arrival, we schedule a process_completion,
 * if a server of the block is idle.
 * Otherwise, it adds the job to the queue.
 * @param event the event who will schedule the completion or will be enqueued.
 * @param c the time of the current arrival event
 * @param block the service node to which the job is arrived
 */
void	process_immediate_arrival(event *arrival_event, clock *c, block *block)
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
		next_completion_event = create_insert_event(block->type, s_index,
				COMPLETION, c, arrival_event);
		if (next_completion_event != NULL)
		{
			next_completion_time = next_completion_event->time;
			s->sum->service += next_completion_time - c->current;
			s->sum->served++;
			block->block_area->service += (next_completion_time - c->current);
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

// A completion should schedule another completion event (if there are job in queue) and an arrival.
void	process_completion(event *completion_event, clock *c, block *block)
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
				next_completion_time = (next_completion_event->time
						- c->current);
				block->queue_jobs--;
				block->block_area->service += next_completion_time;
				s->sum->service += next_completion_time;
				s->sum->served++;
			}
		}
	}
	schedule_immediate_arrival(block->type, c, completion_event);
}

void	schedule_immediate_arrival(int type, clock *c, event *triggering_event)
{
	double	p;
	int		next_type;

	switch (type)
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
	create_insert_event(next_type, -1, IMMEDIATE_ARRIVAL, c, triggering_event);
}
