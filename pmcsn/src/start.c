#include "start.h"

int	main(void)
{
	// system("clear");
	// printf("**************************************\n");
	// printf("**               PMCSN              **\n");
	// printf("**************************************\n");
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
	double p;
	event	*event;
	system_clock = init_clock();
	if (system_clock == NULL)
	{
		printf("Error on system clock\n");
		return (-1);
	}
	init_event_list(system_clock->type);
	// initializing multi-stream lehmer generator
	PlantSeeds(123456789);
	int *network_status = init_network(CONFIG_2);
	blocks = init_blocks(network_status);
	if (blocks == NULL)
	{
		printf("Error on blocks");
		return (-1);
	}
	while (1)
	{
        if (!(system_clock->arrival < PERIOD || check_events() || check_servers(blocks))) {
            break;
        }
        // FIXME: Segmentation fault: la lista di eventi diventa NULL!!!
		event = get_event();
        if (event == NULL) break;
		system_clock->next = event;
		if (event->event_type == ARRIVAL)
			system_clock->arrival = event->time;
        // event_time: tempo in cui l'evento viene processato. current: tempo dell'evento corrente.
        // La differenza è il tempo rimanente prima del prossimo evento
        update_area_stats(event->time - system_clock->current, blocks);
		previous_clock = system_clock->current;
		system_clock->current = event->time;
		block_type btype = event->block_type;
		remove_event();

        switch (event->event_type) {
            case ARRIVAL:
                arrival(system_clock, previous_clock, blocks[btype]);
                p = Random();
                if (p < P_PRIMO_FUORI)
                    create_insert_event(PRIMO, -1, ARRIVAL, system_clock);
                else
                    create_insert_event(SECONDO, -1, ARRIVAL, system_clock);
                break;
            case IMMEDIATE_ARRIVAL:
                arrival(system_clock, previous_clock, blocks[btype]);
                break;
            case COMPLETION:
                completion(event->target_server, system_clock, previous_clock,blocks[btype]);
                break;
            default:
                break;
        }

		sort_list();
		//if (system_clock->arrival <= PERIOD)
		//	printf("Next arrival time: %lf\n", system_clock->arrival);
		// use \r instead of \n to print in one line
		//     printf("Time: %lf Event: %-18s Target Block: %-12s in server: %d\tjobs in blocks [%s, %s, %s, %s, %s, %s]\n",
		//            system_clock->arrival,
		//            toStrEvent(event->type),
		//            toStrBlock(event->blockType),
		//            event->target_server,
		//            getServerContents(blocks[PRIMO]),
		//            getServerContents(blocks[SECONDO]),
		//            getServerContents(blocks[DESSERT]),
		//            getServerContents(blocks[CASSA_FAST]),
		//            getServerContents(blocks[CASSA_STD]),
		//            getServerContents(blocks[CONSUMAZIONE]));
	}
    show_and_validate_stats(blocks, system_clock);
	free(system_clock);
	clear_mem(blocks);
	return (0);
}

void	arrival(clock *c, double current, block *block)
{
	int		s_index;
	server	*s;
	double	next_completion_time;

	block->jobs++;
	// we retrieve the server id of an idle server in the block
	s_index = get_idle_server(block);
    // if the server is idle, we generate the completion event
	if (s_index != -1)
	{
		s = block->servers[s_index]; // the selected server
        // next completion time (for this server)
		next_completion_time = create_insert_event(block->type, s_index, COMPLETION, c);
        // we increment the cumulative service time for THIS SERVER!
		s->sum->service += next_completion_time - current; // the service time for this arrival
		s->sum->served++;
		block->block_area->service += (next_completion_time - current);
	}
	else //otherwise all server are idle, so we increment jobs in queue
		block->queue_jobs++;
}

void	schedule_arrive(int type, clock *c)
{
	double	p;
	switch (type)
	{
	case PRIMO:
		p = Random();
		if (p < P_SECONDO_PRIMO)
			create_insert_event(SECONDO, -1, IMMEDIATE_ARRIVAL, c);
		else if (p > P_SECONDO_PRIMO && p < P_SECONDO_PRIMO + P_DESSERT_PRIMO)
			create_insert_event(DESSERT, -1, IMMEDIATE_ARRIVAL, c);
		else
			create_insert_event(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case SECONDO:
		p = Random();
		if (p < P_DESSERT_SECONDO)
			create_insert_event(DESSERT, -1, IMMEDIATE_ARRIVAL, c);
		else if (p > P_DESSERT_SECONDO && p < P_DESSERT_SECONDO + P_CASSA_STD_SECONDO)
			create_insert_event(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c);
		else
			create_insert_event(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case DESSERT:
		create_insert_event(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case CASSA_STD:
	case CASSA_FAST:
		create_insert_event(CONSUMAZIONE, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	default:
		break ;
	}
}
void	completion(int server_id, clock *c, double current, block *block)
{
	double service_time;
	server *s;

	block->completed_jobs++;
	block->jobs--;

	if (server_id != -1)
		free_busy_server(block, server_id);
	else
	{
		printf("This is bad! A completion from who knows from!\n");
		exit(-1);
	}
	if (block->queue_jobs > 0)
	{
		int serv_id = get_idle_server(block);
		s = block->servers[server_id];
		service_time = create_insert_event(block->type, serv_id, COMPLETION, c);
		block->queue_jobs--;
		s->sum->service += (service_time - current);
		block->block_area->service += (service_time - current);
		s->sum->served++;
	}


	schedule_arrive(block->type, c);
}