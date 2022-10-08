typedef struct tipo_servente {
    int status;  // {0=IDLE, 1=BUSY}
    struct centro *centro;
    double tempoServizio;
} servente;

enum tipo_centro {
    PRIMO,
    SECONDO,
    DESSERT,
    CASSA_FAST,
    CASSA_STD,
    MANGIARE
};

struct clock_t {
    double corrente;  // Tempo attuale di simulazione
    double successivo;     // Tempo attuale del prossimo evento, sia arrivo che completamento
    double prossimoArrivo;  // Tempo attuale del prossimo arrivo
};

struct centro {
    enum tipo_centro tipo;
    int dipInCoda;
    int dipNeiCentri;
    int arriviTotali;
    int completamentiTotali;
};

struct dipendente {
    double prossimoArrivo; // tempo prima del prossimo arrivo in secondi.
    struct dipendente *successivo;
};
