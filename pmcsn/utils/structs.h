#ifndef _STRUCT_H_
#define _STRUCT_H_

typedef enum block_types_t {
    PRIMO,
    SECONDO,
    DESSERT,
    CASSA_FAST,
    CASSA_STD,
    CONSUMAZIONE
} block_type;

typedef enum event_type_t {
    ARRIVAL,
    COMPLETION,
    IMMEDIATE_ARRIVAL
} event_type;

typedef struct event_t {
    block_type block_type;
    double time;
    event_type event_type;
    int target_server;
    struct event_t *next;
} event;

typedef struct clock_t_n {
    double last_arrival;                 /* next arrival time                   */
    double completion;              /* next completion time                */
    double current;                 /* current time                        */
    event *next;                    /* next (most imminent) event time     */
    double last;                    /* last arrival time                   */
    int type;
} clock;

// time-averaged integral for entire block
typedef struct area_t {
    double node;                    /* time integrated number in the node  */
    double queue;                   /* time integrated number in the queue */
    double service;                 /* time integrated number in service   */
} area;

// Statistics for each service block!
typedef struct statistics_t {
    long int completed_jobs;
    double interarrival_time;
    double wait; // queue + service
    double delay; // only queue
    double service_time; // only service
    double node_pop;
    double queue_pop;
    //float trafficIntensity; // can be greater than 1
    double utilization; // same as trafficIntensity for sufficiently long observation periods
    double daily_cost;
    double *multiserver_utilization;
    double *multiserver_service_time;
} statistics;

// cumulative sum of service for a single server
typedef struct sum_t {               /* accumulated sums of                */
    double service;                  /*   service times                    */
    long served;                   /*   number served                    */
} sum;

// struct that represent a single server inside a multi-server block (MMk)
typedef struct server_t {
    int status;  // {0=IDLE, 1=BUSY}
    sum *sum;
} server;

typedef struct block_t {
    char name[32];
    block_type type;
    long completed_jobs;            // completed jobs from this block
    long jobs;                      // jobs currently in the node
    area *block_area;               // time averaged area
    server **servers;               // pointer to eventually multi-server
    int num_servers;                // number of server in this block
    int queue_jobs;                 // job in queue
    // int has_infinite_queue;      // possible values: TRUE (has infinite queue) FALSE (doesn't have any queue)
} block;

#endif