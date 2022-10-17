#include "start.h"

//ogni blocco è un centro del nostro modello
//i vari blocchi sono definiti nella variabile block_type
block	*blocks[BLOCKS];

// servers array in 'CONSUMAZIONE' block
server *servers[POSTI_A_SEDERE];

// Nomi dei blocchi
const char*  names[] = {"Primi", "Secondi e Contorni", "Frutta e Dessert", "Casse Fast", "Casse standard", "Locale Mensa"};

int	main(void)
{
	// system("clear");
	// printf("**************************************\n");
    // printf("**               PMCSN              **\n");
    // printf("**************************************\n");
	startSimulation();
	return (0);
}

int	startSimulation(void)
{
	clock 	*system_clock;
	int		eventIndex;
	double 	p = Random(); // 0<p<1

	//initializing the global clock (see structs.h)
	system_clock = (clock *) malloc(sizeof(clock));
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
	// initializing each block of the system
	initBlocks();
   	// TODO: ci fermiamo quando il prossimo arrivo e' maggiore del periodo
	while ((system_clock->arrival < PERIODO) || areThereMoreEvents())
	{
		area *area;
		// FIND_SEGFAULT("prima di prendere index");
		eventIndex = getNextEventIndex();
		system_clock->next = getEvent(eventIndex); // setting next 
		double time = system_clock->next->time;
		if (system_clock->next->type == ARRIVAL)
		{
			system_clock->arrival = time;
		}
		// FIND_SEGFAULT("prima di integrare");
		for (int index = 0; index < BLOCKS; index++)
		{
			if (blocks[index]->jobs > 0)
			{
				area = blocks[index]->blockArea;
				area->node += (time - system_clock->current) * blocks[index]->jobs;
				area->queue += (time - system_clock->current) * (blocks[index]->jobs - 1);
				area->service += (time - system_clock->current);
			}
		}
		// fast-forward time until next event
		system_clock->current = system_clock->next->time;
		// Managing the next event (arrival or completion)
		// FIND_SEGFAULT("prima di arrivi/completamenti");
		switch(system_clock->next->type){
			case ARRIVAL:
				outsideArrival(system_clock);
				break;
			case IMMEDIATE_ARRIVAL:
				arrival(system_clock->next->blockType, system_clock);
				break;
			case COMPLETION:
				completion(system_clock->next->blockType, system_clock);
				break;
			default:
				break;
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
	showStatistics(blocks, system_clock, servers);
	// Free the heap
	free(system_clock);
	clearMem(blocks, servers);
	return (0);
}

void	initBlocks(void)
{
	int len1 = COUNT_OF(names);
	int len2 = COUNT_OF(blocks);
	if (len1 != len2 || len1 != BLOCKS){
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
		strncpy(blocks[index]->name, names[index], strlen(names[index])+1);
	    memset(blocks[index]->blockArea, 0x0, sizeof(area));
		blocks[index]->completedJobs = 0;
		blocks[index]->jobs = 0;
	}
	// multi-server initialization
	for (int s = 0; s < POSTI_A_SEDERE; s++) {
		servers[s] = malloc(sizeof(server));
		servers[s]->sum = malloc(sizeof(sum));
		if (servers[s]->sum == NULL || servers[s] == NULL)
		{
			printf("Error Malloc\n");
			return ;
		} 
		servers[s]->status = IDLE;
	   	memset(servers[s]->sum, 0x0, sizeof(sum));
	}
}

// return the index of an idle server
int 	findServer()
{
	for (int i = 0; i < POSTI_A_SEDERE; i++){
		if (servers[i]->status == IDLE)
			return i;
	}
	return -1;
}

void 	outsideArrival(clock *c)
{
	double p = Random();
	int block_type;
	if (p < P_PRIMO_FUORI)
		block_type = PRIMO;
	else
		block_type = SECONDO;
	
	blocks[block_type]->jobs++;
	createAndInsertEvent(block_type, ARRIVAL, c);
	if (blocks[block_type]->jobs == 1) // TODO: forse qui bisogna mettere BUSY / IDLE
		createAndInsertEvent(block_type, COMPLETION, c);
}

void	arrival(block_type target_block, clock *c)
{
	// increase number of job in the block by 1
	// if (isClockTerminated() && target_block == PRIMO){
	// 	return;
	// }
	blocks[target_block]->jobs++;
	if (target_block == CONSUMAZIONE){
		int s = findServer();
		// found an idle server
		if (s != -1){
			double serviceTime = createAndInsertEvent(target_block, COMPLETION, c);
			servers[s]->sum->service += serviceTime;
			servers[s]->sum->served ++;
			servers[s]->status = BUSY;
		}else{
			// enqueue the job if there's a queue
			// or discard it
			// ...
		}
	}else{
		// IMMEDIATE_ARRIVAL starts from SECONDO. PRIMO has only external arrivals 
		if (blocks[target_block]->jobs == 1)
			createAndInsertEvent(target_block, COMPLETION, c);
	}
}

void	completion(block_type blockType, clock *c)
{
	double p;
	int s;

	// update current and completed jobs in the block which has completed service
	blocks[blockType]->completedJobs++;
	blocks[blockType]->jobs--;
	// if there is at least one more job in blockType, we complete it
	if (blocks[blockType]->jobs > 0)
		createAndInsertEvent(blockType, COMPLETION, c);
	switch (blockType)
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
		else if (p > P_DESSERT_SECONDO && p < P_DESSERT_SECONDO + P_CASSA_STD_SECONDO)
			createAndInsertEvent(CASSA_STD, IMMEDIATE_ARRIVAL, c);
		else
			createAndInsertEvent(CASSA_FAST, IMMEDIATE_ARRIVAL, c);
		break;
	case DESSERT:
		// now we must go to CASSA_STD: if a user passes by DESSERT, he/she owns at least two plates 
		createAndInsertEvent(CASSA_STD, IMMEDIATE_ARRIVAL, c);
		break;
	case CASSA_FAST: case CASSA_STD:
		// from here we go to the seats space
		createAndInsertEvent(CONSUMAZIONE, IMMEDIATE_ARRIVAL, c);
		break;
	case CONSUMAZIONE:
		s = findServer();
		// set an idle a server
		if (s > 0)
			servers[s-1]->status = IDLE;
		break;
	default:
		break;
	}
}