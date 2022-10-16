#include "start.h"

//ogni blocco è un centro del nostro modello
//i vari blocchi sono definiti nella variabile block_type
block	blocks[BLOCKS];

// Nomi dei blocchi
const char*  names[] = {"Primi", "Secondi e Contorni", "Frutta e Dessert", "Casse Fast", "Casse standard", "Locale Mensa"};

int	main(void)
{
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
	/* ---------------------------------------------------------- 
     * Next-event Simulation of duration O_P with BLOCKS nodes FIFO M/M/1
     * ---------------------------------------------------------- 
    */
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
			if (blocks[index].jobs > 0)
			{
				area = blocks[index].blockArea;
				area->node += (time - system_clock->current) * blocks[index].jobs;
				area->queue += (time - system_clock->current) * (blocks[index].jobs - 1);
				area->service += (time - system_clock->current);
			}
		}
		// fast-forward time until next event
		system_clock->current = system_clock->next->time;
		// Managing the next event (arrival or completion)
		// FIND_SEGFAULT("prima di arrivi/completamenti");
		if (system_clock->next->type == ARRIVAL || system_clock->next->type == IMMEDIATE_ARRIVAL)
			arrival(system_clock->next->blockType, system_clock);
		else
			completion(system_clock->next->blockType, system_clock);
		clearEvent(eventIndex);
		//if (system_clock->arrival <= PERIODO)
		//	printf("Next arrival time: %lf\n", system_clock->arrival);
		// use \r instad of \n to print in one line
		printf("Next Arrival time: %lf Cut-Off Time: %d jobs in blocks [%ld, %ld, %ld, %ld, %ld, %ld]\r", system_clock->arrival, PERIODO, 
				blocks[PRIMO].jobs, 
				blocks[SECONDO].jobs, 
				blocks[DESSERT].jobs,
				blocks[CASSA_FAST].jobs,
				blocks[CASSA_STD].jobs, 
				blocks[CONSUMAZIONE].jobs);
		// printf("-------------------------\n");
		// for (int j = 0; j < n; j++){
		// 	printf("%lf\n", eventList[j].time);
		// 	printf("%d\n", eventList[j].block_type);
		// 	printf("%d\n", eventList[j].type);
		// }
		// printf("-------------------------\n");
	}
	printf("\n====================================================================================================\n");
	// Show statistics
	for (int i = 0; i < BLOCKS; i++)
	{
		printf("%s", blocks[i].name);
		statistics stats; // stack-allocated
		calculateStatistics(blocks[i].completedJobs, system_clock, blocks[i].blockArea, &stats);
		showStatistics(&stats);
		validateMM1(&blocks[i], &stats); 
		// here stats are de-allocated
	}
	
	// Free the heap
	free(system_clock);
	for (int i = 0; i < BLOCKS; i++){
		if (blocks[i].blockArea != NULL){
			free(blocks[i].blockArea);
		}
	}
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
		blocks[index].blockArea = malloc(sizeof(area));
		if (blocks[index].blockArea == NULL)
		{
			printf("Error Malloc\n");
			return ;
		}
		//struttura dati usata per per calcolare i time-averaged number rispettivamente
		//nel nodo,in coda e in servizio.Sono calcolati come integrali
		strncpy(blocks[index].name, names[index], strlen(names[index])+1);
	    memset(blocks[index].blockArea, 0x0, sizeof(area));
		blocks[index].completedJobs = 0;
		blocks[index].jobs = 0;
	}
}

void	arrival(block_type target_block, clock *c)
{
	double p;
	// increase number of job in the block by 1
	if (isClockTerminated() && target_block == PRIMO){
		return;
	}
	blocks[target_block].jobs++;

	switch (target_block)
	{
	// ARRIVALS from outside can go in PRIMO or SECONDO
	case PRIMO: case SECONDO:
		p = Random(); // Uniform(0,1)
		// arrival from outside goes into block PRIMO
		if (p < P_PRIMO_FUORI)
			createAndInsertEvent(OUTSIDE, PRIMO, ARRIVAL, c);
		// arrival from outside goes into block SECONDO
		else
			createAndInsertEvent(OUTSIDE, SECONDO, ARRIVAL, c);
		
		// if there is one job in PRIMO or SECONDO, we create a completion event. This must be added to the event list
		if (blocks[target_block].jobs == 1) // TODO: forse qui bisogna mettere BUSY / IDLE
			createAndInsertEvent(target_block, INSIDE, COMPLETION, c);
		break ;
	case DESSERT:
		// here cannot arrive jobs from outside
		if (blocks[DESSERT].jobs == 1)
			createAndInsertEvent(DESSERT, CASSA_STD, COMPLETION, c);
		break ;
	case CASSA_FAST:
		if (blocks[CASSA_FAST].jobs == 1)
			createAndInsertEvent(CASSA_FAST, CONSUMAZIONE, COMPLETION, c);
		// here cannot arrive jobs from outside. Only from PRIMO and SECONDO
		break;
	case CASSA_STD:
		// here cannot arrive jobs from outside. Only from SECONDO and DESSERT
		if (blocks[CASSA_STD].jobs == 1)
			createAndInsertEvent(CASSA_STD, CONSUMAZIONE, COMPLETION, c);
		break;
	case CONSUMAZIONE:
		if (blocks[CONSUMAZIONE].jobs == 1)
			createAndInsertEvent(CONSUMAZIONE, OUTSIDE, COMPLETION, c);
		break;
	default:
		break;
	}
}

// When we have a completion event
void	completion(block_type blockType, clock *c)
{
	double p;

	// update current and completed jobs in the block which has completed service
	blocks[blockType].completedJobs++;
	blocks[blockType].jobs--;
	
	switch (blockType)
	{
	case PRIMO:
		// if there is at least one more job in block PRIMO, we complete it
		if (blocks[blockType].jobs > 0)
			createAndInsertEvent(PRIMO, INSIDE, COMPLETION, c);

		// we generate a routing probability
		p = Random();

		// to SECONDO from PRIMO
		if (p < P_SECONDO_PRIMO)
			createAndInsertEvent(PRIMO, SECONDO, IMMEDIATE_ARRIVAL, c);
		// to DESSERT from PRIMO
		else if (p > P_SECONDO_PRIMO && p < P_SECONDO_PRIMO + P_DESSERT_PRIMO)
			createAndInsertEvent(PRIMO, DESSERT, IMMEDIATE_ARRIVAL, c);
		// to CASSA_FAST from PRIMO
		else
			createAndInsertEvent(PRIMO, CASSA_FAST, IMMEDIATE_ARRIVAL, c);
		break ;
	case SECONDO:
		// if there is at least one job in the node SECONDO, we complete it
		if (blocks[blockType].jobs > 0)
			createAndInsertEvent(SECONDO, INSIDE, COMPLETION, c);

		// routing probability
		p = Random();

		// to DESSERT from SECONDO
		if (p < P_DESSERT_SECONDO)
			createAndInsertEvent(SECONDO, DESSERT, IMMEDIATE_ARRIVAL, c);
		else if (1) // TODO: il dipendente ha preso due piatti
			createAndInsertEvent(SECONDO, CASSA_STD, IMMEDIATE_ARRIVAL, c);
		else // TODO: il dipendente ha preso solo un piatto...
			createAndInsertEvent(SECONDO, CASSA_FAST, IMMEDIATE_ARRIVAL, c);
		break;
	case DESSERT:
		// if there is at least one job in the node DESSERT, we complete it
		if (blocks[DESSERT].jobs > 0)
			createAndInsertEvent(DESSERT, CASSA_STD, COMPLETION, c);
		// now we must go to CASSA_STD
		createAndInsertEvent(DESSERT, CASSA_STD, IMMEDIATE_ARRIVAL, c);
		break;
	case CASSA_FAST:
		// if there is at least one job in the node CASSA_FAST, we complete it
		if (blocks[CASSA_FAST].jobs > 0)
			createAndInsertEvent(CASSA_FAST, CONSUMAZIONE, COMPLETION, c);
		// from here we go to the seats space
		createAndInsertEvent(CASSA_FAST, CONSUMAZIONE, IMMEDIATE_ARRIVAL, c);
		break;
	case CASSA_STD:
		// if there is at least one job in the node CASSA_STD, we complete it
		if (blocks[CASSA_STD].jobs > 0)
			createAndInsertEvent(CASSA_STD, CONSUMAZIONE, COMPLETION, c);
		// also from here we go to the seats
		createAndInsertEvent(CASSA_STD, CONSUMAZIONE, IMMEDIATE_ARRIVAL, c);
		break;
	case CONSUMAZIONE:
		if (blocks[CONSUMAZIONE].jobs > 0)
			createAndInsertEvent(CONSUMAZIONE, OUTSIDE, COMPLETION, c);
		break;
	default:
		break;
	}
}