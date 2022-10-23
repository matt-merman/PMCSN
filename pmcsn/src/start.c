#include "server.h"
#include "start.h"
#include <stdio.h>
#include <stdlib.h>
//ogni blocco è un centro del nostro modello
//i vari blocchi sono definiti nella variabile block_type
block		*blocks[BLOCKS];

// variable used to check if the refusing jobs percentage in 'CONSUMAZIONE'
// is equal to P_ALTROVE
double		rejected_jobs;

// variable setted to configure the network
int			network_status[] = {0, 0, 0, 0, 0, 0};

// Nomi dei blocchi
const char	*names[] = {"Primi", "Secondi e Contorni", "Frutta e Dessert",
		"Casse Fast", "Casse standard", "Locale Mensa"};

int	main(void)
{
	// system("clear");
	// printf("**************************************\n");
	// printf("**               PMCSN              **\n");
	// printf("**************************************\n");
	startSimulation();
	return (0);
}

// function to set the network configuration (i.e., multi-server)
void	setNetworkStatus(int config)
{
	int	status_one[] = {1, 1, 1, 1, 1, 50};
	int	status_two[] = {2, 1, 1, 1, 1, 50};
	int	status_three[] = {1, 1, 2, 1, 1, 50};

	switch (config)
	{
	case CONFIG_1:
		memcpy(network_status, status_one, sizeof(network_status));
		break ;
	case CONFIG_2:
		memcpy(network_status, status_two, sizeof(network_status));
		break ;
	case CONFIG_3:
		memcpy(network_status, status_three, sizeof(network_status));
		break ;
	default:
		break ;
	}
}
// updates the time-averaged statistics (number of jobs in service, queue and block)
void	updateStats(double time, double current)
{
	area	*area;

	for (int i = 0; i < BLOCKS; i++)
	{
		if (blocks[i]->jobs > 0)
		{
			area = blocks[i]->blockArea;
			area->node += (time - current) * blocks[i]->jobs;
			area->queue += (time - current) * (blocks[i]->jobs - 1);
			area->service += (time - current);
		}
	}
}

int	startSimulation(void)
{
	clock	*system_clock;
	int		eventIndex;
	double	time;

	double p = Random(); // 0<p<1
	rejected_jobs = 0;
	//initializing the global clock (see structs.h)
	system_clock = (clock *)malloc(sizeof(clock));
	if (system_clock == NULL)
	{
		printf("Error allocating system clock\n");
		return (-1);
	}
	initClock(system_clock, p);
	// adding first ARRIVAL event to eventList
	initEventList(system_clock);
	// initializing multi-stream lehmer generator
	PlantSeeds(123456789);
	// select a network configuration
	setNetworkStatus(CONFIG_2);
	// initializing each block of the system
	initBlocks();
	// We stop when next arrival time is greater than observation period and we have no more event to process
	while (system_clock->arrival < PERIODO || areThereMoreEvents()|| areThereMoreServers(blocks))
	{
		// FIND_SEGFAULT("prima di prendere index");
		eventIndex = getNextEventIndex();
		system_clock->next = getEvent(eventIndex); // setting next
		time = system_clock->next->time;
		if (system_clock->next->type == ARRIVAL)
		{
			system_clock->arrival = time;
		}
		// FIND_SEGFAULT("prima di integrare");
		updateStats(time, system_clock->current);
		// fast-forward time until next event
		system_clock->current = system_clock->next->time;
		// Managing the next event (arrival or completion)
		// FIND_SEGFAULT("prima di arrivi/completamenti");
		switch (system_clock->next->type)
		{
		case ARRIVAL:
			// qui va passato l'evento non il blockType
			arrival(system_clock->next, system_clock, ARRIVAL);
			break ;
		case IMMEDIATE_ARRIVAL:
			arrival(system_clock->next, system_clock, IMMEDIATE_ARRIVAL);
			break ;
		case COMPLETION:
			completion(system_clock->next, system_clock);
			break ;
		default:
			break ;
		}
		clearEvent(eventIndex);
		//if (system_clock->arrival <= PERIODO)
		//	printf("Next arrival time: %lf\n", system_clock->arrival);
        // use \r instead of \n to print in one line
        printf("Time: %lf Event: %-18s Target Block: %-12s in server: %d\tjobs in blocks [%s, %s, %s, %s, %s, %s]\n",
               system_clock->arrival,
               toStrEvent(system_clock->next->type),
               toStrBlock(system_clock->next->blockType),
               system_clock->next->target_server,
               getServerContents(blocks[PRIMO]),
               getServerContents(blocks[SECONDO]),
               getServerContents(blocks[DESSERT]),
               getServerContents(blocks[CASSA_FAST]),
               getServerContents(blocks[CASSA_STD]),
               getServerContents(blocks[CONSUMAZIONE]));
    }
	// Show statistics
	showStatistics(blocks, system_clock);
	printf("Percentage of rejected jobs: %lf\n", rejected_jobs / blocks[CONSUMAZIONE]->completedJobs);
	// Free the heap
	free(system_clock);
	clearMem(blocks);
	return (0);
}

void	initBlocks(void)
{
	int		len1;
	int		len2;

	len1 = COUNT_OF(names);
	len2 = COUNT_OF(blocks);
	if (len1 != len2 || len1 != BLOCKS)
	{
		printf("Correggere: il numero di elementi nei nomi è diverso dal numero di elementi nei blocchi\n");
		exit(-1);
	}
	for (int index = 0; index < BLOCKS; index++)
	{
		blocks[index] = malloc(sizeof(block));
		blocks[index]->blockArea = malloc(sizeof(area));
        blocks[index]->type = index;
		if (blocks[index]->blockArea == NULL || blocks[index] == NULL)
		{
			printf("Error Malloc\n");
			return ;
		}
		//struttura dati usata per per calcolare i time-averaged number rispettivamente
		//nel nodo,in coda e in servizio.Sono calcolati come integrali
		strncpy(blocks[index]->name, names[index], strlen(names[index]) + 1);
		//memset(blocks[index], 0x0, sizeof(block));
		memset(blocks[index]->blockArea, 0x0, sizeof(area));
		blocks[index]->completedJobs = 0;
		blocks[index]->jobs = 0;
		// here we ALWAYS instantiate server to avoid sigsegv for mm1 queues
        initServers(blocks[index], network_status[index]);
	}
}
// at clock c, occurs an arrival or immediate arrival
void	arrival(event *event, clock *c, event_type arrival_type)
{
	int		s_index;
	server	*s;
	double	service_time;

    block_type btype = event->blockType;
	blocks[btype]->jobs++;
    blocks[btype]->jobsInQueue++; // the job goes in the queue first, but if a server is idle, will go there immediately
	// we retrieve the number of server in the block
    // TODO: quale indice devo metterci? Lo stesso del job che sta gia dentro
    s_index = requestIdleServer(blocks[btype]);
    if (s_index != -1)
    {
        s = blocks[btype]->servers[s_index];
        // plus one because index start from zero, but server index start from 1.
        service_time = createAndInsertEvent(btype, s_index, COMPLETION, c);
        s->sum->service += service_time;
        s->sum->served++;
        blocks[btype]->jobsInQueue--; // decrement because one job started service
        // the server s_index will be busy until the event is completed
    } else{
        // all servers are busy, if the node has a queue, the job should go there (we have already done jobsInQueue++)
        if (btype == CONSUMAZIONE){ // in CONSUMAZIONE we lose the job if all servers are busy
            rejected_jobs++;
            blocks[btype]->jobs--;
        }
        // then, when a completion occurrs, a job in queue should go to service
    }

	// if internal arrival occurs, create a new arrival
	if (arrival_type == ARRIVAL){
		double p = Random();
		if (p < P_PRIMO_FUORI)
			createAndInsertEvent(PRIMO, -1, ARRIVAL, c); // -1 because we do not know where the event will be scheduled (maybe)
		else
			createAndInsertEvent(SECONDO, -1, ARRIVAL, c);
	}
}

void	completion(event *event, clock *c)
{
	double p;
	int server_id;
    // we have completed a job
    block_type type = event->blockType;
	blocks[type]->completedJobs++;
	blocks[type]->jobs--;
    // shortly after we also decrement jobsInQueue, if > 0.
    // if there are more than one server

    // FIXME: a volte server_id va fuori range
    server_id = event->target_server;// findBusyServer(blocks[type]->servers, num);
    if (server_id != -1) {
        freeBusyServer(blocks[type], server_id);
    } else {
        printf("This is bad! A completion from who knows from!\n");
        exit(-1);
    }
    // After this completion, we immediately schedule a new completion if there is at least one job in queue
    if (blocks[type]->jobsInQueue > 0){
        int serv_id = requestIdleServer(blocks[type]);
        if (serv_id != -1){
            createAndInsertEvent(type, serv_id, COMPLETION, c); // TODO: maybe there is only one server (?)
            blocks[type]->jobsInQueue--; // we scheduled a service so there will be one less job in queue
        }
    }
    //	createAndInsertEvent(type, server_id + 1, COMPLETION, c);
//	}else if (blocks[type]->jobs > 0) // TODO: from which server will complete???

    // after the completion, we schedule an immediate arrival in the right block
	switch (type)
	{
	case PRIMO:
		// routing probability
		p = Random();
		// to SECONDO from PRIMO
		if (p < P_SECONDO_PRIMO)
			createAndInsertEvent(SECONDO, -1, IMMEDIATE_ARRIVAL, c);
		// to DESSERT from PRIMO
		else if (p > P_SECONDO_PRIMO && p < P_SECONDO_PRIMO + P_DESSERT_PRIMO)
			createAndInsertEvent(DESSERT, -1, IMMEDIATE_ARRIVAL, c);
		// to CASSA_FAST from PRIMO
		else
			createAndInsertEvent(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case SECONDO:
		// routing probability
		p = Random();
		// to DESSERT from SECONDO
		if (p < P_DESSERT_SECONDO)
			createAndInsertEvent(DESSERT, -1, IMMEDIATE_ARRIVAL, c);
		else if (p > P_DESSERT_SECONDO && p < P_DESSERT_SECONDO + P_CASSA_STD_SECONDO)
			createAndInsertEvent(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c);
		else
			createAndInsertEvent(CASSA_FAST, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case DESSERT:
		// now we must go to CASSA_STD: if a user passes by DESSERT, he / she owns at least two plates
		createAndInsertEvent(CASSA_STD, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	case CASSA_FAST:
	case CASSA_STD:
		createAndInsertEvent(CONSUMAZIONE, -1, IMMEDIATE_ARRIVAL, c);
		break ;
	default:
		break ;
	}
}