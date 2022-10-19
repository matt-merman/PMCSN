#define BLOCKS 6
#define START 0
// Parameters for multiserver
#define SERVER_PRIMI 1
#define SERVER_SECONDI 1
#define SERVER_DESSERT 1
#define NUM_CASSE_FAST 1
#define NUM_CASSE_STD 1
#define POSTI_A_SEDERE 50
#define N (BLOCKS * (1 + POSTI_A_SEDERE))

#define USER 2500                                   // number of mean user in observation period
#define HOUR (60*60)
#define PERIODO (3*HOUR)                            // observation period in seconds
#define INF 100*PERIODO                             // max int is 2147483647
#define LAMBDA ((double) USER/(double) PERIODO)     // Mean outside arrivals

// TODO: probabilità di arrivo nei tre time slots 
#define P_ARRIVO_SLOT_1 0.28 // maybe (double) USER * 0.20 /(double) PERIODO
#define P_ARRIVO_SLOT_2 0.55 // maybe (double) USER * 0.55 /(double) PERIODO
#define P_ARRIVO_SLOT_3 0.17 // maybe (double) USER * 0.25 /(double) PERIODO

//notazione probabilità di routing: P_[dest]_[source]
#define P_PRIMO_FUORI 0.75      // Probabilità di prendere il primo arrivando da fuori
#define P_SECONDO_FUORI 0.25    // Probabilità di prendere il secondo arrivando da fuori (saltando il primo)

#define P_SECONDO_PRIMO 0.55    // probabilità di prednere il secondo dopo il primo
#define P_DESSERT_PRIMO 0.25    // probabilità di prendere il dessert dopo il primo (saltando il secondo)
#define P_CASSA_PRIMO 0.20      // probabilità di andare alla cassa dopo aver preso il primo (saltando secondo e dessert)

#define P_DESSERT_SECONDO 0.45  // probabilità di prendere il dessert dopo aver preso il secondo
#define P_CASSA_SECONDO 0.55    // probabilità di andare alla cassa dopo aver preso il secondo
#define P_CASSA_FAST_SECONDO (P_SECONDO_FUORI * P_CASSA_SECONDO)
#define P_CASSA_STD_SECONDO (P_PRIMO_FUORI * P_CASSA_SECONDO)

// Una volta preso il dessert, c'è il 100% di probabilità di andare alla cassa
#define P_CASSA_DESSERT = 1.0

// TODO: Probabilità di mangiare altrove
#define P_ALTROVE 0.18

// Service time for each server in each block
#define S_PRIMO 15
#define S_SECONDO 15
#define S_DESSERT 10 //Il dessert è già impiattato, quindi si impiega meno tempo.

#define S_CASSA_FAST 11
#define S_CASSA_STD 18

// Tempo (di servizio) per consumare il pasto e liberare il posto a sedere.
#define S_CONSUMAZIONE (10*60) //10 min [s]

#define TRUE 1
#define FALSE 0

#define IDLE 0
#define BUSY 1
