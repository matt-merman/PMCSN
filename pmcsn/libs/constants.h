#define BLOCKS 6 //number of center
#define START 0
#define CASSA 2
#define POSTI_A_SEDERE 1000 //seat number

#define IDLE 0
#define BUSY 1

#define N 2500 //user number
#define O_P (3*60*60) //observation period [s]
#define LAMBDA 0.2314

//probabilities 
#define P_TIME_SLOT_1 0.28
#define P_TIME_SLOT_2 0.55
#define P_TIME_SLOT_3 0.17

//notation: P_[dest]_[source]
#define P_PRIMO_FUORI 0.75
#define P_SECONDO_FUORI 0.25

#define P_SECONDO_PRIMO 0.55
#define P_DESSERT_PRIMO 0.25
#define P_CASSA_PRIMO 0.20

#define P_DESSERT_SECONDO 0.45
#define P_CASSA_SECONDO 0.55

#define P_PIATTO [(P_PRIMO_FUORI * P_CASSA_PRIMO) + (P_SECONDO_FUORI * P_CASSA_SECONDO)]   
#define P_PIATTI [(P_PRIMO_FUORI * P_SECONDO_PRIMO * P_CASSA_SECONDO) + (P_PRIMO_FUORI * P_DESSERT_PRIMO) + (P_SECONDO_FUORI * P_DESSERT_SECONDO) + \
                    + (P_PRIMO_FUORI * P_SECONDO_PRIMO + P_DESSERT_SECONDO)]

#define P_ALTROVE 0.18

//service times
#define S_T_PRIMO 15
#define S_T_SECONDO 15
#define S_T_DESSERT 10

#define S_T_CASSA_FAST 11
#define S_T_CASSA_STD 18

#define S_T_MANGIARE (10*60) //10 min [s]


