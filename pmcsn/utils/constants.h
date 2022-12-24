#ifndef _CONST_H_
#define _CONST_H_

#define BLOCKS 6
#define START 0
#define N (BLOCKS * 100)

#define USER 2500                                   // mean number of user in the observation period
#define HOUR (60*60)
#define N_HOURS 3
#define PERIOD (N_HOURS*HOUR)                      // observation period in seconds
#define INF 100*PERIOD                             // max int is 2147483647
#define LAMBDA ((double) USER/(double) PERIOD)     // Mean outside arrivals: 0.2314814815

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

#define IDLE 0
#define BUSY 1

#define MAX_GLOBAL_MEAN_RESPONSE_TIME (10*60*30) // 30 min [s] average global response time of entire canteen
#define MAX_CASHIER_RESPONSE_TIME (1*60) // 1 min average global response time for BOTH the Standard Cashier and Fast cashier

// service node configurations id.
#define CONFIG_1 1 // all single servers
#define CONFIG_2 2 // multiserver on first course and canteen
#define CONFIG_3 3 // mix

#define SALARY 8 // 8 [eur/h]
#define PER_USER_PENALTY 1 // 1 [eur/penalty] amount that must be payed PER USER if FOR HIM the response time is greater than the MAX_GLOBAL_MEAN_RESPONSE_TIME
#endif
