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

typedef struct event_t{
    block_type block_type;
    double time;
    event_type event_type;
    int target_server;
    struct event_t * next;
} event;

typedef struct clock_t_n{
    double arrival;                 /* next arrival time                   */
    double completion;              /* next completion time                */
    double current;                 /* current time                        */
    event* next;                    /* next (most imminent) event time     */
    double last;                    /* last arrival time                   */
    int type;
} clock;

typedef struct area_t{
    double node;                    /* time integrated number in the node  */
    double queue;                   /* time integrated number in the queue */
    double service;                 /* time integrated number in service   */
} area;

typedef struct statistics_t {
    long int completed_jobs;
    float interarrival_time;
    float wait; // queue + service
    float delay; // only queue
    float service_time; // only service
    float node_pop;
    float queue_pop;
    //float trafficIntensity; // can be greater than 1
    float utilization; // same as trafficIntensity for sufficiently long observation periods
} statistics;

typedef struct sum_t {               /* accumulated sums of                */
    double service;                  /*   service times                    */
    long   served;                   /*   number served                    */
} sum;

typedef struct server_t {
    int status;  // {0=IDLE, 1=BUSY}
    sum *sum;
} server;

typedef struct block_t {
    char name[32];
    block_type type;
    long completed_jobs;
	long jobs;                      /* jobs currently in the node */
    area * block_area;
    server **servers;                // pointer to eventually multi-server
    int num_servers;
    int queue_jobs;
} block;

#endif