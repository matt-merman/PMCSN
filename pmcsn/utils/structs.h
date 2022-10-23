#ifndef STRUCTS // used to not re-import these structs
#define STRUCTS

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
    IMMEDIATE_ARRIVAL // // Dopo un completamento, gli arrivi vanno immediatamente nel centro a cui sono diretti.
} event_type;

typedef struct event_t{
    block_type blockType;
    double time;
    event_type type; //0 = arrival; 1 = completion
    int target_server; // 1 if is M/M/1
} event;

typedef struct clock_t{
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
    long int completedJobs;
    double interarrivalTime;
    double wait; // queue + service
    double delay; // only queue
    double serviceTime; // only service
    double nodePopulation;
    double queuePopulation;
    double trafficIntensity; // can be greater than 1
    double utilization; // same as trafficIntensity for sufficiently long observation periods
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
    long completedJobs;
	long jobs;                      /* jobs currently in the node */
    area * blockArea;
    server **servers;                // pointer to eventually multi-server
    int num_servers;
    int jobsInQueue;
    int rejectedJobs;
} block;

#endif