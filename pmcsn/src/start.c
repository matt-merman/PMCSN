#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "../libs/constants.h"
#include "../libs/structs.h"
#include "../libs/rvgs.h"
#include "../libs/rngs.h"


//ogni blocco è un centro del nostro modello
//i vari blocchi sono definiti nella variabile block_types
struct block blocks[BLOCKS];

//genera tempo di arrivo tramite Exp
double getArrival(double current) {
    double arrival = current;
    SelectStream(0);
    arrival += Exponential(1 / (LAMBDA * P_PRIMO_FUORI));
    return arrival;
}

int startSimulation(void){

    
    //for (int block_type = 0; block_type < BLOCKS; block_type++) {    
    
    //initializzo il solo blocco dei 'primi' 
    int blockTypeIndex = 0;
    
    blocks[blockTypeIndex].type = blockTypeIndex;
    blocks[blockTypeIndex].jobInBlock = 0;
    blocks[blockTypeIndex].jobInQueue = 0;
    blocks[blockTypeIndex].totalArrivals = 0;
    blocks[blockTypeIndex].totalCompletions = 0;
    
    switch(blockTypeIndex){

        case 0: case 1: case 2:

            server s;
            s.status = IDLE;
            s.block = &blocks[blockTypeIndex];
            s.serviceTime = S_T_PRIMO;
                
        // case 3: case 4:

        //     for (int i = 0; i < CASSA; i++) {

        //         server s;
        //         s.status = IDLE;
        //         s.block = &blocks[blockTypeIndex];
        //         s.time_online = 0.0;
        //         s.last_online = 0.0;

        //     }

        // case 5:

        //     for (int i = 0; i < POSTI_A_SEDERE; i++) {

        //         server s;
        //         s.status = IDLE;
        //         s.block = &blocks[blockTypeIndex];
        //         s.time_online = 0.0;
        //         s.last_online = 0.0;
                    
        //     }

        }

    //inizializzo il clock globale (vedi in struct.h)
    struct clock_t * clock = malloc(sizeof(struct clock_t));
    if (clock == NULL){
        printf("Error Malloc\n");
        return -1;
    }
    
    clock->current = START;
    //genero il primo tempo di arrivo al sistema
    clock->arrival = getArrival(clock->current);
    printf("First arriving: %f\n", clock->arrival);

    //eseguo simulazione ad orizzonte finito di durata 4h = O_P 
    while (clock->arrival <= O_P){

        break;



    }

    
   
    return 0;

}


int main(void){

    startSimulation();
    return 0;

}