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
	// TODO: ci fermiamo quando il prossimo arrivo e' maggiore del periodo
	while ((system_clock->arrival < PERIODO) || areThereMoreEvents()
		|| areThereMoreServers(blocks))
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
			arrival(system_clock->next->blockType, system_clock, ARRIVAL);
			break ;
		case IMMEDIATE_ARRIVAL:
			arrival(system_clock->next->blockType, system_clock, IMMEDIATE_ARRIVAL);
			break ;
		case COMPLETION:
			completion(system_clock->next->blockType, system_clock);
			break ;
		default:
			break ;
		}
		clearEvent(eventIndex);
		//if (system_clock->arrival <= PERIODO)
		//	printf("Next arrival time: %lf\n", system_clock->arrival);
		// use \r instead of \n to print in one line
		printf("Next Arrival time: %lf Cut-Off Time: %d jobs in blocks [%ld, %ld, %ld, %ld, %ld, %ld]\r",
				system_clock->arrival,
				PERIODO,
				blocks[PRIMO]->jobs,
				blocks[SECONDO]->jobs,
				blocks[DESSERT]->jobs,
				blocks[CASSA_FAST]->jobs,
				blocks[CASSA_STD]->jobs,
				blocks[CONSUMAZIONE]->jobs);
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
		if (network_status[index] == 1)
		{
			blocks[index]->servers = NULL;
			blocks[index]->num_servers = 0;
		}
		else
			initServers(blocks[index], network_status[index]);
	}
}

void	arrival(block_type type, clock *c, int arrival)
{
	int		s_index;
	server	*s;
	double	service_time;
	int num;

	// if (server_id < 1 || server_id > ?) {
	// TODO:  nel limite superiore fare la distinzione tra i vari blocchi. Ad esempio prendere il MAX tra i vari numeri di serventi.
	// 		printf("Error: server_id out of bound for block_type %s", to_str(target_block));
					// 		exit(-1);
					// }
	blocks[type]->jobs++;
	num = blocks[type]->num_servers;
	if (num == 0 && blocks[type]->jobs == 1)
			createAndInsertEvent(type, COMPLETION, c);
	else if (num > 0){
		s_index = findIdleServer(blocks[type]->servers, num);
		if (s_index != -1)
		{
			s = blocks[type]->servers[s_index];
			service_time = createAndInsertEvent(type,
			COMPLETION, c);
			s->sum->service += service_time;
			s->sum->served++;
			s->status = BUSY;
		} else{
			blocks[type]->jobs--;
			if (type == CONSUMAZIONE)
				rejected_jobs++;
		}
	}
	// if external arriving occurs create a new one
	if (arrival == ARRIVAL){
		double p = Random();
		if (p < P_PRIMO_FUORI)
			createAndInsertEvent(PRIMO, ARRIVAL, c);
		else
			createAndInsertEvent(SECONDO, ARRIVAL, c);
	}
}

void	completion(block_type type, clock *c)
{
	double p;
	int s_index;
	int num; 

	blocks[type]->completedJobs++;
	blocks[type]->jobs--;
	num = blocks[type]->num_servers;
	if (num > 0){
		s_index = findBusyServer(blocks[type]->servers, num);
		if (s_index != -1)
			blocks[type]->servers[s_index]->status = IDLE;
		// TO-FIX: if (getBusyServers(blocks[type]->servers, num) does not work
		if (getBusyServers(blocks[type]->servers, num>0))
			createAndInsertEvent(type, COMPLETION, c);
	}else if (blocks[type]->jobs > 0)
		createAndInsertEvent(type, COMPLETION, c);
	switch (type)
	{
	case PRIMO:
		// routing probability
		p = Random();
		// to SECONDO from PRIMO
		if (p < P_SECONDO_PRIMO)
			createAndInsertEvent(SECONDO, IMMEDIATE_ARRIVAL, c);
		// to DESSERT from PRIMO
		else if (p > P_SECONDO_PRIMO && p < P_SECONDO_PRIMO + P_DESSERT_PRIMO)
			createAndInsertEvent(DESSERT, IMMEDIATE_ARRIVAL, c);
		// to CASSA_FAST from PRIMO
		else
			createAndInsertEvent(CASSA_FAST, IMMEDIATE_ARRIVAL, c);
		break ;
	case SECONDO:
		// routing probability
		p = Random();
		// to DESSERT from SECONDO
		if (p < P_DESSERT_SECONDO)
			createAndInsertEvent(DESSERT, IMMEDIATE_ARRIVAL, c);
		else if (p > P_DESSERT_SECONDO && p < P_DESSERT_SECONDO
				+ P_CASSA_STD_SECONDO)
			createAndInsertEvent(CASSA_STD, IMMEDIATE_ARRIVAL, c);
		else
			createAndInsertEvent(CASSA_FAST, IMMEDIATE_ARRIVAL, c);
		break ;
	case DESSERT:
		// now we must go to CASSA_STD: if a user passes by DESSERT, he / she owns at least two plates 
		createAndInsertEvent(CASSA_STD, IMMEDIATE_ARRIVAL, c);
		break ;
	case CASSA_FAST:
	case CASSA_STD:
		createAndInsertEvent(CONSUMAZIONE, IMMEDIATE_ARRIVAL, c);
		break ;
	default:
		break ;
	}
}