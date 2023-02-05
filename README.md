# Progetto di PMCSN: Gestione di una Mensa Aziendale

## Introduzione 

Il sistema considerato è una mensa aziendale composta da una zona ristorazione, dove un utente può scegliere il suo pasto e acquistarlo, e una successiva area dove mangiare costituita da diversi posti a tavola.

### Obiettivo

L’obiettivo è analizzare le metriche (e.g., tempi di attesa in coda, utilizzazioni) del modello, considerando i seguenti requisiti di qualità:

* QoS-1: il tempo di risposta del sistema deve essere inferiore a 15 min;
* QoS-2: la percentuale di utenti che non trovano un posto a sedere deve essere inferiore al 5%;

## Running

Per compilare ed eseguire il programma:

```Bash
# per eseguire una simulazione finita con durata PERIOD 
make run

# per eseguire una simulazione finita all'aumentare del numero di repliche
make finite_simulation

# per eseguire una simulazione infinita facendo variare anche il seed
make infinite_simulation
```

### Macro

La configurazione delle varie modalità operative è gestita con l'utilizzo di macro nel file _pmcsn/utils/constants.h_.

```c
// Base model with 200 seats
#define BASE_200

// Extended model with 2 CONSUMAZIONE blocks
#define EXTENDED

// if this macro is commented out, the policy for choosing CONSUMAZIONE or CONSUMAZIONE_2 is RANDOM.
#define CHOOSE_LEAST_BUSY
```

## Implementazione

Si rimanda a [report](https://github.com/matt-merman/PMCSN/blob/main/docs/report.pdf) per i dettagli.


