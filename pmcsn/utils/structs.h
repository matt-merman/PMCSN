// typedef struct server_t {
//     int status;  // {0=IDLE, 1=BUSY}
//     struct block *block;
//     double serviceTime;
// } server;

enum block_types {
    PRIMO,
    SECONDO,
    DESSERT,
    CASSA_FAST,
    CASSA_STD,
    CONSUMAZIONE
};

struct clock_t{
    double arrival;                 /* next arrival time                   */
    double completion;              /* next completion time                */
    double current;                 /* current time                        */
    double next;                    /* next (most imminent) event time     */
    double last;                    /* last arrival time                   */
} clock;

struct area{
    double node;                    /* time integrated number in the node  */
    double queue;                   /* time integrated number in the queue */
    double service;                 /* time integrated number in service   */
};

struct block {
    enum block_types type;
    long completed_jobs;  //job completati
	long jobs; //job nel centro
};

// struct job {
//     double arrival;
//     struct job *next;
// };
