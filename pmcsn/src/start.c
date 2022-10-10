#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "../utils/constants.h"
#include "../utils/helpers.h"
//#include "../utils/structs.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ogni blocco è un centro del nostro modello
//i vari blocchi sono definiti nella variabile block_types
struct block blocks[3];

int	startSimulation(void)
{
	struct clock_t	*system_clock;

	//inizializzo il clock globale (vedi in struct.h)
	system_clock = malloc(sizeof(struct clock_t));
	if (system_clock == NULL)
	{
		printf("Error Malloc\n");
		return (-1);
	}
	system_clock->current = START;
	system_clock->arrival = getArrival(START);
	system_clock->completion = INF;
	
    /* ---------------------------------------------------------- 
     * Next-event Simulation di durata O_P con FIFO M/M/1
     * ---------------------------------------------------------- 
    */
	
    PlantSeeds(123456789);
	
    long completed_jobs = 0;  //job completati
	long block_jobs = 0; //job nel centro
	
	struct area		*block_area;
    block_area = malloc(sizeof(struct area));
	if (block_area == NULL)
	{
		printf("Error Malloc\n");
		return (-1);
	}
	
    //struttura dati usata per per calcolare i time-averaged number rispettivamente
    //nel nodo, in coda e in servizio. Sono calcolati come integrali (base*altezza)
    block_area->node = 0;
	block_area->queue = 0;
	block_area->service = 0;

    //avvio simulazione 
	while ((system_clock->arrival < PERIODO) || (block_jobs > 0))
	{
		system_clock->next = min(system_clock->arrival,
									system_clock->completion);

		if (block_jobs > 0)
		{
            //aggiorna statistiche
        	block_area->node += (system_clock->next - system_clock->current) * block_jobs;
			block_area->queue += (system_clock->next - system_clock->current)
				* (block_jobs - 1);
			block_area->service += (system_clock->next - system_clock->current);
		}

        //porto avanti il tempo del sistema fino al primo evento (arrivo || completamento)
		system_clock->current = system_clock->next;

        //se l'evento da processare è un arrivo
		if (system_clock->current == system_clock->arrival)
		{ 
        	block_jobs++;
            //genero un nuovo arrivo
			system_clock->arrival = getArrival(system_clock->current);
			//ho concluso il tempo di simulazione
            if (system_clock->arrival > PERIODO)
			{
				system_clock->last = system_clock->current;
				system_clock->arrival = INF;
			}
			if (block_jobs == 1)
				system_clock->completion = system_clock->current
					+ getService(PRIMO);
		}
        //se l'evento è un completamento
		else
		{
            //aggiorno i numeri nel centro
        	completed_jobs++;
			block_jobs--;
			if (block_jobs > 0)
				system_clock->completion = system_clock->current
					+ getService(PRIMO);
			else
				system_clock->completion = INF;
		}
	}

    showStatistics(completed_jobs, system_clock, block_area);	
    return (0);
}

int	main(void)
{
	startSimulation();
	return (0);
}