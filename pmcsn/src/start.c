#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "../utils/constants.h"
#include "../utils/structs.h"
#include "../libs/rvgs.h"
#include "../libs/rngs.h"

// ogni blocco è un centro del nostro modello
// i vari blocchi sono definiti nella variabile tipo_centro
struct centro centri[NUM_CENTRI];

// genera il tempo di arrivo con una Exponential di media ARRIVI_MEDI pesata per la probabilità di entrare nel primo...
double getArrival(double corrente)
{
    double prossimoArrivo = corrente;
    SelectStream(0);
    prossimoArrivo += Exponential(1 / (ARRIVI_MEDI * P_PRIMO_FUORI)); // TODO: forse qui bisogna prendere l'arrivo, poi dopo si suddivide tra primi e secondi. Quindi senza moltiplicare per P_PRIMO_FUORI
    return prossimoArrivo;
}

int avviaSimulazione(void)
{
    int i = 0;
    // for (; i < NUM_CENTRI; i++) {

    // inizializzo il solo blocco dei 'primi'

    centri[i].tipo = i;
    centri[i].dipNeiCentri = 0;
    centri[i].dipInCoda = 0;
    centri[i].arriviTotali = 0;
    centri[i].completamentiTotali = 0;

    switch (i)
    {

    case 0:
    case 1:
    case 2:

        servente s;
        s.status = IDLE;
        s.centro = &centri[i];
        s.tempoServizio = S_PRIMO;

        // case 3: case 4:

        //     for (int i = 0; i < NUM_CASSE; i++) {

        //         servente s;
        //         s.status = IDLE;
        //         s.centro = &centri[i];
        //         s.time_online = 0.0;
        //         s.last_online = 0.0;

        //     }

        // case 5:

        //     for (int i = 0; i < POSTI_A_SEDERE; i++) {

        //         servente s;
        //         s.status = IDLE;
        //         s.centro = &centri[i];
        //         s.time_online = 0.0;
        //         s.last_online = 0.0;

        //     }
    }

    // inizializzo il clock globale (vedi in struct.h)
    struct clock_t *clock = malloc(sizeof(struct clock_t));
    if (clock == NULL)
    {
        printf("Error Malloc\n");
        return -1;
    }

    clock->corrente = START;
    // genero il primo tempo di arrivo al sistema
    clock->prossimoArrivo = getArrival(clock->corrente);
    printf("First arriving: %f\n", clock->prossimoArrivo);

    // eseguo simulazione ad orizzonte finito di durata 4h = PERIODO
    while (clock->prossimoArrivo <= PERIODO)
    {
        break;
    }

    return 0;
}

int main(void)
{
    avviaSimulazione();
    return 0;
}