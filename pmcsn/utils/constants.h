#ifndef _CONST_H_
#define _CONST_H_

#define NAME_SIZE 32                                // Size of block name string
#define BLOCKS 6

#define START 0
#define N (BLOCKS * 100)

#define USER 2500                                   // mean number of user in the observation period
#define HOUR (60*60)
#define N_HOURS 3
#define PERIOD (N_HOURS*HOUR)                       // observation period in seconds
#define INF 100*PERIOD                              // max int is 2147483647
#define LAMBDA 0.2314814815                         // Computed from USER / 3h (in seconds)
//#define LAMBDA ((double) USER/(double) PERIOD)    // Mean outside arrivals: 0.2314814815 Mean inter-arrival time 4.32
#define LAMBDA_DEFAULT 0.2314814815

// TODO: probabilità di arrivo nei tre time slots 
#define P_ARRIVO_SLOT_1 0.28 // (double) USER * 0.20 /(double) PERIOD
#define P_ARRIVO_SLOT_2 0.55 // (double) USER * 0.55 /(double) PERIOD
#define P_ARRIVO_SLOT_3 0.17 // (double) USER * 0.25 /(double) PERIOD

//notazione probabilità di routing: P_[dest]_[source]
#define P_PRIMO_FUORI 0.75      // Probability that a user buys first course from outside
#define P_SECONDO_FUORI 0.25    // Probability that a user buys second course from outside, skipping first course

#define P_SECONDO_PRIMO 0.55    // Probability that a user buys second course after buying first course
#define P_DESSERT_PRIMO 0.25    // Probability that a user buys dessert after first course, skipping second course
#define P_CASSA_FAST_PRIMO 0.20      // Probability that a user pays to the (fast) cashier after buying first course, skipping second course and dessert

#define P_DESSERT_SECONDO 0.45  // Probability that a user buys a dessert after buying a second course
#define P_CASSA_SECONDO 0.55    // Probability that a user pays to the cashier after buying second course, skipping dessert (we do not know what cashier is)
#define P_CASSA_FAST_SECONDO (P_SECONDO_FUORI * P_CASSA_SECONDO) // Probability that the user has bought only second dish and goes to the fast cashier
#define P_CASSA_STD_SECONDO (P_PRIMO_FUORI * P_CASSA_SECONDO) // Probability that the user has bought first and second dish and goes to the cashier

// After buying dessert, the user always go to the cashier
#define P_CASSA_STD_DESSERT 1.0

// TODO: Probability of not eating in the dining room (not used)
#define P_ALTROVE 0.05

// Service time for each server in each block
#define S_PRIMO 15
#define S_SECONDO 15
#define S_DESSERT 10 // The dessert is already plated, so it needs less time to serve

#define S_CASSA_FAST 11
#define S_CASSA_STD 18

// Service time to eat the launch and exit from the canteen
#define S_CONSUMAZIONE (10*60) //10 min [s]

#define TRUE 1
#define FALSE 0
#define DEBUG FALSE
#define PRINTF if(DEBUG) printf
#define IDLE 0
#define BUSY 1

#define MAX_GLOBAL_MEAN_RESPONSE_TIME (10*60*30) // 30 min [s] average global response time of entire canteen
#define MAX_CASHIER_RESPONSE_TIME (1*60) // 1 min average global response time for BOTH the Standard Cashier and Fast cashier

// service node configurations id.
#define CONFIG_1 1 // all single servers
#define CONFIG_2 2 // multiserver on first course and canteen
#define CONFIG_3 3 // mix

#define SALARY 8 // 8 [eur/h] (you must count 3h for preparation and 2h for cleanup, so we have fixed 5h/day +3h/day serving)

/*
 * L'affitto MENSILE di un immobile costa circa 10€ a metro quadro
 * Diviso 30 giorni e diviso 24 ore, si ottiene il costo orario a metro quadro.
 *
 * */
// Regulation Source: CE 852 / 2004, DPR 327 / 1980, DM 12/04/1996
#define KITCHEN_SQUARE_METER_PER_SEAT 1.0 // 0.5 for preparing meal + 0.5 for washing dishes
#define SQUARE_METER_PER_SEAT 1.0 // Regolamento Comunale N 18 20/03/2006 Firenze - Igiene in materia di alimenti e bevande. The minimum is 1 square meter for each seat.
#define MONTLY_RENT_COST_MQ 10. // € cost for renting a square meter in a property
#define DAILY_RENT_COST_MQ (MONTLY_RENT_COST_MQ / 30.0)
#define HOURLY_RENT_COST_MQ (MONTLY_RENT_COST_MQ / 30. / 24.)
#define HOURLY_FIXED_COST (250.0 / 24.0) // € food, water, energy, gas cost for each hour of work
#define PER_USER_PENALTY 1 // 1 [eur/penalty] amount that must be payed PER USER if FOR HIM the response time is greater than the MAX_GLOBAL_MEAN_RESPONSE_TIME

// Finite-Horizon
#define REPLICAS 20

#endif
