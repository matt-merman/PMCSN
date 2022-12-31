#include "start.h"
#include "event_list.h"

int	main(void)
{
	// system("clear");
	// PRINTF("**************************************\n");
	// PRINTF("**               PMCSN              **\n");
	// PRINTF("**************************************\n");
	start_simulation();
	return (0);
}
/**
 * FIXME: This doesn't work!
 *
 * Updates time-averaged integrals for node and queue populations
 *
 * @param diff time before next event
 * @param blocks
 */
void	update_area_stats(double diff, block **blocks)
{
	area	*area;
    // per ogni blocco calcola l'area
	for (int i = 0; i < BLOCKS; i++)
	{
		if (blocks[i]->jobs > 0) // FIXME: forse non gestisce bene i multiserver
		{
			// ricava l'area dal blocco
            area = blocks[i]->block_area;
            // tempo * numero di job (integrale time-averaged, numero job nel nodo e in coda)
			area->node += diff * (double) blocks[i]->jobs;
			area->queue += diff * (double) blocks[i]->queue_jobs;
            // area->service += diff; //FIXME prima non c'era!!!
            // in servizio ci sono node-queue (ma queste sono aree)
		}
	}
}

int	start_simulation(void)
{
	clock	*system_clock;
	double	previous_clock;
	block	**blocks;
	event	*event;
	system_clock = init_clock();
	if (system_clock == NULL)
	{
		PRINTF("Error on system clock\n");
		return (-1);
	}
	init_event_list(system_clock->type);
	// initializing multi-stream lehmer generator
	PlantSeeds(123456789);
	int *network_status = init_network(CONFIG_2);
	blocks = init_blocks(network_status);
	if (blocks == NULL)
	{
		PRINTF("Error on blocks");
		return (-1);
	}
	while (1)
	{
        // break if the times is finished, all events are processed and all servers are idle
        if (system_clock->last_arrival >= PERIOD && !are_there_more_events() && !are_there_busy_servers(blocks)) {
            break;
        }
		event = get_next_event();
        PRINTF("Event %ld: Time: %f - %s\n", event->event_id, event->time, to_str_event(event->event_type));
        // if (event == NULL) break;
        if (event->event_type == ARRIVAL)
			system_clock->last_arrival = event->time; // we update the last arrival time
        // event_time: tempo in cui l'evento viene processato. current: tempo dell'evento corrente.
        // La differenza è il tempo rimanente prima del prossimo evento
        update_area_stats(event->time - system_clock->current, blocks);
        // store the previous system clock
		previous_clock = system_clock->current;
        // fast-forward system clock to the current event time
        system_clock->current = event->time;
		block_type btype = event->block_type;

        switch (event->event_type) {
            case ARRIVAL:
                // we schedule its completion (if a server is idle) or add to queue and generate a new outside arrival
                process_arrival(event, system_clock, previous_clock, blocks[btype]);
                break;
            case IMMEDIATE_ARRIVAL:
                // we only schedule its completion or add to queue
                schedule_arrival_completion_or_enqueue(event, system_clock, blocks[btype]);
                break;
            case COMPLETION:
                // we schedule the next completion and generate an immediate arrival
                process_completion(event, system_clock, previous_clock, blocks[btype]);
                break;
            default:
                break;
        }
        // the event is processed and now it can be freed

		sort_list();

        debug(system_clock, blocks, event);

        free(event);
	}
    show_and_validate_stats(blocks, system_clock);
	free(system_clock);
	clear_mem(blocks);
	return (0);
}

void debug(clock *system_clock, block **blocks, event *event) {
    PRINTF("Event %ld: Time: %lf - %-18s Target Block: %-12s in server: %d jobs in blocks [%s, %s, %s, %s, %s, %s] events: %d (linked event id %ld)\n",
           event->event_id,
           system_clock->current,
           to_str_event(event->event_type),
           to_str_block(event->block_type),
           event->target_server,
           get_server_contents(blocks[PRIMO]),
           get_server_contents(blocks[SECONDO]),
           get_server_contents(blocks[DESSERT]),
           get_server_contents(blocks[CASSA_FAST]),
           get_server_contents(blocks[CASSA_STD]),
           get_server_contents(blocks[CONSUMAZIONE]), length(),
           (event->linked_arrival != NULL) ? event->linked_arrival->event_id : -1L);

}
// To process an arrival, we need to schedule or enqueue the job and then generate a new ARRIVAL event
void process_arrival(event *event, clock *c, double current, block *block) {
    schedule_arrival_completion_or_enqueue(event, c, block);
    double p = Random();
    if (p < P_PRIMO_FUORI)
        create_insert_event(PRIMO, -1, ARRIVAL, c, NULL);
    else
        create_insert_event(SECONDO, -1, ARRIVAL, c, NULL);
}

//
/**
 * After an arrival or immediate arrival, we schedule a process_completion, if a server of the block is idle.
 * Otherwise, it adds the job to the queue.
 * @param event the event who will schedule the completion or will be enqueued.
 * @param c the time of the current arrival event
 * @param block the service node to which the job is arrived
 */
void schedule_arrival_completion_or_enqueue(event *event, clock *c, block *block)
{
	int		s_index;
	server	*s;
	double	next_completion_time;

	block->jobs++;
	// we retrieve the server id of an idle server in the block
	s_index = retrieve_idle_server(block);
    // if the server is idle, we generate the process_completion event otherwise we enqueue the job
	if (s_index != -1)
	{
		s = block->servers[s_index]; // the selected server
        // next process_completion time (for this server)
		next_completion_time = create_insert_event(block->type, s_index, COMPLETION, c, event);
//        PRINTF("%36s (id %d) Service time for process_completion after %g: %g\n",block->name, s_index, c->current, next_completion_time);
        // we increment the cumulative service time for THIS SERVER!
		s->sum->service += next_completion_time - c->current; // the service time for this (immediate) arrival
        PRINTF("Event %ld: Tempo di servizio arrivo %f - %f = %f\n",event->event_id, next_completion_time, c->current, next_completion_time - c->current);
		s->sum->served++;
		block->block_area->service += (next_completion_time - c->current);
	}
	else // all server are idle, so we increment jobs in queue
		block->queue_jobs++; // TODO: sicuro che non bisogna aumentare l'area del blocco
}

void	schedule_immediate_arrival(int type, clock *c)
{
	double	p;
	switch (type)
	{
	case PRIMO:
		p = Random();
		if (p < P_SECONDO_PRIMO)
            create_insert_event(SECONDO, -1, IMMEDIATE_ARRIVAL, c, NULL);
		else if (p > P_SECONDO_PRIMO && p < P_SECONDO_PRIMO + P_DESSERT_PRIMO)
            create_insert_event(DESSERT, -1, IMMEDIATE_ARRIVAL, c, NULL);
		else
            create_insert_event(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c, NULL);
		break ;
	case SECONDO:
		p = Random();
		if (p < P_DESSERT_SECONDO)
            create_insert_event(DESSERT, -1, IMMEDIATE_ARRIVAL, c, NULL);
		else if (p > P_DESSERT_SECONDO && p < P_DESSERT_SECONDO + P_CASSA_STD_SECONDO)
            create_insert_event(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c, NULL);
		else
            create_insert_event(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c, NULL);
		break ;
	case DESSERT:
        create_insert_event(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c, NULL);
		break ;
	case CASSA_STD:
	case CASSA_FAST:
        create_insert_event(CONSUMAZIONE, -1, IMMEDIATE_ARRIVAL, c, NULL);
		break ;
	default:
		break ;
	}
}

// A completion should schedule another completion event (if there are job in queue) and an arrival.
void	process_completion(event *event, clock *c, double current, block *block)
{
	double service_time;
	server *s;

	block->completed_jobs++;
	block->jobs--;

	if (event->target_server != -1)
		free_busy_server(block, event->target_server);
	else
	{
		PRINTF("This is bad! A completion from who knows from!\n");
		exit(-1);
	}
    // We generate a new completion if there are jobs in queue. We now for sure that a server is empty
    if (block->queue_jobs > 0) {
        int serv_id = retrieve_idle_server(block);
        if (serv_id != -1) {
            s = block->servers[event->target_server];
            service_time = create_insert_event(block->type, serv_id, COMPLETION, c, event);
            // PRINTF("%s (id %d) Service time for job between %g and %g: %g\n",block->name, serv_id, current, service_time, service_time - current);
            block->queue_jobs--;
            s->sum->service += (service_time - c->current);
            PRINTF("Event %ld: Tempo di servizio completamento %g - %g = %g\n", event->event_id, service_time, c->current, service_time - c->current);
            block->block_area->service += (service_time - c->current);
            s->sum->served++;
        }
    }

    schedule_immediate_arrival(block->type, c);
}