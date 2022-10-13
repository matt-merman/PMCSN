#define BLOCKS 3
#define START 0
#define NUM_CASSE 2
#define POSTI_A_SEDERE 1000
#define N (BLOCKS * (1 + POSTI_A_SEDERE))

#define ARRIVAL 0
#define COMPLETE 1
#define IMMEDIATE_ARRIVAL 2

#define USER 2500                  // numero di utenti
#define PERIODO (60*60)       // periodo di osservazione in [s]
#define INF 100*PERIODO
#define LAMBDA 0.2314      // Arrivi da fuori

//probabilità di arrivo nei tre time slots 
#define P_ARRIVO_SLOT_1 0.28
#define P_ARRIVO_SLOT_2 0.55
#define P_ARRIVO_SLOT_3 0.17

//notazione probabilità di routing: P_[dest]_[source]
#define P_PRIMO_FUORI 0.75      // Probabilità di prendere il primo arrivando da fuori
#define P_SECONDO_FUORI 0.25    // Probabilità di prendere il secondo arrivando da fuori (saltando il primo)

#define P_SECONDO_PRIMO 0.55    // probabilità di prednere il secondo dopo il primo
#define P_DESSERT_PRIMO 0.25    // probabilità di prendere il dessert dopo il primo (saltando il secondo)
#define P_CASSA_PRIMO 0.20      // probabilità di andare alla cassa dopo aver preso il primo (saltando secondo e dessert)

#define P_DESSERT_SECONDO 0.45  // probabilità di prendere il dessert dopo aver preso il secondo
#define P_CASSA_SECONDO 0.55    // probabilità di andare alla cassa dopo aver preso il secondo

// Una volta preso il dessert, c'è il 100% di probabilità di andare alla cassa
#define P_CASSA_DESSERT = 1.0

// Probabilità di aver preso un solo piatto (primo o secondo)
#define P_PIATTO [(P_PRIMO_FUORI * P_CASSA_PRIMO) + (P_SECONDO_FUORI * P_CASSA_SECONDO)]   
// Probabilità di aver preso più di un piatto
#define P_PIATTI [(P_PRIMO_FUORI * P_SECONDO_PRIMO * P_CASSA_SECONDO) + (P_PRIMO_FUORI * P_DESSERT_PRIMO) + (P_SECONDO_FUORI * P_DESSERT_SECONDO) + \
                    + (P_PRIMO_FUORI * P_SECONDO_PRIMO + P_DESSERT_SECONDO)]
// Probabilità di mangiare altrove
#define P_ALTROVE 0.18

// Tempi di servizio [s]
#define S_PRIMO 15
#define S_SECONDO 15
#define S_DESSERT 10 //Il dessert è già impiattato, quindi si impiega meno tempo.

#define S_CASSA_FAST 11
#define S_CASSA_STD 18

// Tempo (di servizio) per consumare il pasto e liberare il posto a sedere.
#define S_CONSUMAZIONE (10*60) //10 min [s]


