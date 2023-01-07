#include "start.h"

const char * BLOCK_NAMES[BLOCKS] = {"Primi", "Secondi e Contorni", "Frutta e Dessert", "Casse Fast", "Casse standard", "Locale Mensa"};

#define NETWORK_CONFIGURATION CONFIG_2

int	main(int argc, __attribute__((unused)) char **argv)
{
	int		c;
	char	*parameter;

	parameter = NULL;
	c = getopt(argc, argv, "s:");
    switch (c) {
        case 's':
            parameter = optarg;
            break;
        case '?':
        default:
            start_standard_simulation(NETWORK_CONFIGURATION);
            printf("To run finite or infinite horizon simulation, use:\n./start -s [finite/infinite]\n");
            return 0;
    }

	// initializing multi-stream lehmer generator
	PlantSeeds(123456789);

	if (strcmp(parameter, "finite") == 0)
        start_finite_horizon_simulation(NETWORK_CONFIGURATION);
	else if (strcmp(parameter, "infinite") == 0)
        start_infinite_horizon_simulation(NETWORK_CONFIGURATION);
	else
		printf("Usage: ./start -s [finite/infinite]\n");

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

int start_standard_simulation(int config) {
    network * canteen;

    canteen = (network *) malloc(sizeof(network));
    if (canteen == NULL) {
        perror("Error in allocation of canteen queue network ");
        return -1;
    }
    canteen->network_servers = init_network(config);
    canteen->system_clock = init_clock(); // also sets next arrival time
    canteen->blocks = init_blocks(canteen->network_servers, BLOCK_NAMES);
    if (canteen->system_clock == NULL) {
        perror("Error on system clock\n");
        return (-1);
    }
    if (canteen->blocks == NULL) {
        perror("Error on blocks");
        return (-1);
    }
    init_event_list(canteen->system_clock->type); // type is arrival

    simulation(canteen->system_clock, canteen->blocks);

    // if you run one replica, we'll have a standard execution
    show_stats(canteen->blocks, canteen->system_clock);
    validate_stats(canteen->blocks, canteen->system_clock);


    free(canteen->system_clock);
    clear_mem(canteen->blocks);
    free(canteen);
    return (0);
}

int start_finite_horizon_simulation(int config)
{
    network *canteen;
    int replica;

    canteen = malloc(sizeof(network));
    if (canteen == NULL){
        perror("Error in allocation of canteen queue network (finite-horizon)");
        return -1;
    }

    canteen->network_servers = init_network(config);
	for (replica = 0; replica < REPLICAS; replica++)
	{
		canteen->system_clock = init_clock();
		if (canteen->system_clock == NULL)
		{
			perror("Error on system clock\n");
			return (-1);
		}
		init_event_list(canteen->system_clock->type);
        if (replica == 0) {
            canteen->blocks = init_blocks(canteen->network_servers, BLOCK_NAMES);
            if (canteen->blocks == NULL)
            {
                perror("Error on blocks");
                return (-1);
            }
        } else {
            restart_blocks(canteen);
        }

		simulation(canteen->system_clock, canteen->blocks);
        update_ensemble(canteen, replica);

		free(canteen->system_clock);
    }
    calculate_all_interval_estimate(canteen);
    clear_mem(canteen->blocks);
    free(canteen);
    return (0);
}

//void calculate_single_stat(const char * stat_name, const char * block_name, double *stat_sample){
//    const char * stat1 = "Interarrival time";
//    printf("\n============== Ensemble %s for block %s =============", stat_name, block_name);
//    calculate_interval_estimate_for_stat(stat1, stat_sample, NULL);
//}

/**
 * Uses the program estimate.c to compute the estimation interval of the replica_stats
 */
void calculate_all_interval_estimate(network *canteen)
{
	// FILE **files;
	int i,s;
    const char *stats[18] = {
            "Interarrival time",
            "Response time",
            "Queue time",
            "Service time",
            "Node population",
            "Queue population",
            "Utilization"
    };
	// files = open_files("r", BLOCK_NAMES);
	for(i = 0; i < BLOCKS; i++){
        for (s = 0; s < 7; s++) {
            calculate_interval_estimate_for_stat(s, stats[s], canteen->blocks[i]->ensemble_stats, BLOCK_NAMES[i]);
        }
        //TODO stampare se il valore teorico è dentro o fuori l'intervallo di confidenza
	}
	// close_files(files);
}
/**
 *
 * @return
 */
int start_infinite_horizon_simulation(int config)
{
    printf("config :%d", config);
    return (0);
}
/**
 *
 * @param system_clock
 * @param blocks
 */
void	simulation(clock *system_clock, block **blocks)
{
	event		*current_event;
	block_type	btype;

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
			process_immediate_arrival(current_event, system_clock,blocks[btype]);
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
}

/**
 * To process an arrival, we need to schedule or enqueue the job and then generate a new ARRIVAL event
 * @param event
 * @param c
 * @param block
 */
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
			next_completion_time = (next_completion_event->time - c->current);
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
				next_completion_time = (next_completion_event->time - c->current);
				block->queue_jobs--;
				block->block_area->service += next_completion_time;
				s->sum->service += next_completion_time;
				s->sum->served++;
			}
		}
	}
	schedule_immediate_arrival(block->type, c, completion_event);
}

/**
 *
 * @param type
 * @param c
 * @param triggering_event
 */
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
	create_insert_event(next_type, -1, IMMEDIATE_ARRIVAL, c, triggering_event);
}
