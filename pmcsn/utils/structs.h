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
} event;

typedef struct clock_t{
    double arrival;                 /* next arrival time                   */
    double completion;              /* next completion time                */
    double current;                 /* current time                        */
    event* next;             /* next (most imminent) event time     */
    double last;                    /* last arrival time                   */
    int type;
} clock;

typedef struct area_t{
    double node;                    /* time integrated number in the node  */
    double queue;                   /* time integrated number in the queue */
    double service;                 /* time integrated number in service   */
} area;

typedef struct block_t {
    char name[32];
    long completedJobs;
	long jobs;                      /* jobs currently in the node */
    area * blockArea;
} block;

typedef struct statistics_t {
    long int completedJobs;
    float interarrivalTime;
    float wait; // queue + service
    float delay; // only queue
    float serviceTime; // only service
    float nodePopulation;
    float queuePopulation;
    float utilization;
} statistics;

#endif