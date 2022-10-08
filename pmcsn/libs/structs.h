typedef struct server_t {
    int status;  // {0=IDLE, 1=BUSY}
    struct block *block;
    double serviceTime;
} server;

enum block_types {
    PRIMO,
    SECONDO,
    DESSERT,
    CASSA_FAST,
    CASSA_STD,
    MANGIARE
};

struct clock_t {
    double current;  // Tempo attuale di simulazione
    double next;     // Tempo attuale del prossimo evento, sia arrivo che completamento
    double arrival;  // Tempo attuale del prossimo arrivo
};

struct block {
    enum block_types type;
    int jobInQueue;
    int jobInBlock;
    int totalArrivals;
    int totalCompletions;
};

struct job {
    double arrival;
    struct job *next;
};
