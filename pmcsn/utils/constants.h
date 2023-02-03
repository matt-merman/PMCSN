#ifndef _CONSTANTS_H
# define _CONSTANTS_H

#define NAME_SIZE 32                                // Size of block name string

// Base model with 200 seats
// #define BASE_200

// Extended model with 2 CONSUMAZIONE blocks
// #define EXTENDED

// if this macro is commented out, the policy for choosing CONSUMAZIONE or CONSUMAZIONE_2 is RANDOM.
#define CHOOSE_LEAST_BUSY


// needed to undefine BASE_200 when using extended model and to undefine CHOOSE_LEAST_BUSY when EXTENDED is not defined
#ifdef EXTENDED
#undef BASE_200
#else
#undef CHOOSE_LEAST_BUSY
#endif

#ifndef EXTENDED
#define BLOCKS 6
#else
#define BLOCKS 7
#endif

#define START 0

#define USER 2500L                                  // mean number of user in the observation period
#define HOUR (60L*60L)
#define N_HOURS 3L
#define PERIOD (N_HOURS*HOUR)                       // observation period in seconds
#define INF (100L * PERIOD)                         // max int is 2147483647
#define LAMBDA (0.2314814815)                        // Computed from USER / 3h (in seconds)

// Nomenclature: P_[dest]_[source]
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

#ifdef EXTENDED
#define P_SCELTA_MENSA 0.5
#else
#define P_SCELTA_MENSA 1.0
#endif

#ifdef EXTENDED
#define IS_CONSUMAZIONE(block_type) (block_type == CONSUMAZIONE || block_type == CONSUMAZIONE_2)
#else
#define IS_CONSUMAZIONE(block_type) (block_type == CONSUMAZIONE)
#endif

// Probability of not eating in the dining room
// #define P_ALTROVE 0.05
// #define MAX_GLOBAL_MEAN_RESPONSE_TIME (10*60*30) // 30 min [s] average global response time of entire canteen

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

// service node configurations id.
#define CONFIG_1 1 // base
#define CONFIG_2 2 // extended

// Finite-Horizon
#define MAX_REPLICAS 500
#define REPLICAS_STEP 50

#define LOC 0.95                       /* level of confidence, for estimate intervals */

// Total jobs number in the infinite simulation.
#define INF_USER (USER * 1000L)
// Batches dimension in the infinite simulation.
#define B 2000L
#define K_BATCH (long) (INF_USER / B)

#define NUM_SEED 1
#define MAX_SEED 5
#define PERIOD_INTERVALS 1

#endif
