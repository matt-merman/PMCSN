#ifndef _STRUCT_H_
#define _STRUCT_H_

#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "../libs/rvms.h"
#include "../libs/rngs.h"
#include "../libs/rvgs.h"

typedef enum block_types_t {
    PRIMO,
    SECONDO,
    DESSERT,
    CASSA_FAST,
    CASSA_STD,
    CONSUMAZIONE,
#ifdef EXTENDED
    CONSUMAZIONE_2,
#endif
    ESTERNO,
} block_type;

typedef enum event_type_t {
    ARRIVAL,
    COMPLETION,
    IMMEDIATE_ARRIVAL
} event_type;

typedef struct event_t {
    long event_id;
    block_type block_type;
    double time;
    event_type event_type;
    int target_server;
    long linked_event_id;           /* The event id linked to the current one. If it is an arrival, points to itself*/
    struct event_t *next;
} event;

typedef struct clock_t_n {
    double last_arrival;            /* last arrival time: used to break. At the end it should INF */
    double current;                 /* current time */
    double last;                    /* last arrival time: used to compute interarrival tim. It should be near the PERIOD */
    block_type type;                /* the type of block for the first event */
} timer;

// time-averaged integral for entire block
typedef struct area_t {
    long double node;                    /* time integrated number in the node  */
    long double queue;                   /* time integrated number in the queue */
    long double service;                 /* time integrated number in service   */
} area;

// Statistics for each service block!
typedef struct statistics_t {
    long int completed_jobs;
    double interarrival_time;
    double wait;                            // queue + service
    double delay;                           // only queue
    double service_time;                    // only service
    double node_pop;
    double queue_pop;
    //float trafficIntensity;               // can be greater than 1
    double utilization;                     // same as trafficIntensity for sufficiently long observation periods
    double loss_probability;               // used for M/M/k/k
    double *multiserver_utilization;
    double *multiserver_service_time;
    long double *routing_probability;
} statistics;

// cumulative sum of service for a single server
typedef struct sum_t {
    long double service;                    /* cumulative service times */
    long served;                            /* total number served */
} sum;

// struct that represent a single server inside a multiserver block (MMk)
typedef struct server_t {
    int status;                             /* 0=IDLE 1=BUSY */
    sum *sum;
} server;

typedef struct block_t {
    char name[NAME_SIZE];
    block_type type;
    long completed_jobs;                    // completed jobs from this block
    long jobs;                              // jobs currently in the node
    area *block_area;                       // time averaged area
    server **servers;                       // pointer to eventually multi-server
    int num_servers;                        // number of server in this block
    int queue_jobs;                         // job in queue
    long int rejected_jobs;
    long int count_to_next[BLOCKS+1];       // list of exiting jobs to a next block
} block;

typedef struct network_t {
    block **blocks;
    int *network_servers;
    timer *system_clock;
    // standard simulation
    double global_response_time;
    double global_loss_probability;
    // replicated simulation (finite horizon)
    double replicas_response_time[MAX_REPLICAS];
    double replicas_loss_probability[MAX_REPLICAS];
    // batch means simulation (infinite horizon)
    double batch_response_time[K_BATCH];
    double batch_loss_probability[K_BATCH];
} network;

typedef enum sim_type_t {
    STANDARD,
    FINITE,
    INFINITE
} sim_type;

#ifndef _FILES_H_
#include "files.h"
#endif

#ifndef _EVENT_H_
#include "../src/events.h"
#endif

#ifndef _SERVER_H_
#include "../src/server.h"
#endif

#ifndef _HELP_H_
#include "helpers.h"
#endif

#ifndef _ANALYTIC_H
#include "../src/analytic.h"
#endif

#ifndef _EVENT_LIST_H_
#include "../src/event_list.h"
#endif

#ifndef _INIT_H_
#include "../src/init.h"
#endif

#ifndef SIMULATION_H
#include "../src/simulation.h"
#endif

#ifndef _START_H_
#include "../src/start.h"
#endif

#ifndef _VERIFY_H_
#include "../src/verify.h"
#endif

#ifndef _TEST_H_
#include "../test/test.h"
#endif

#ifndef _ANALYTIC_TESTS_H_
#include "../test/analytic_test.h"
#endif

#ifndef _EVENTS_TEST_H_
#include "../test/events_test.h"
#endif

#endif