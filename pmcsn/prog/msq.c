//
// Created by giaco on 30/12/22.
//
/* -------------------------------------------------------------------------
 * This program is a next-event simulation of a multi-server, single-queue
 * service node.  The service node is assumed to be initially idle, no
 * arrivals are permitted after the terminal time STOP and the node is then
 * purged by processing any remaining jobs.
 *
 * Name              : msq.c (Multi-Server Queue)
 * Author            : Steve Park & Dave Geyer
 * Language          : ANSI C
 * Latest Revision   : 10-19-98
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "../src/analytic.h"

// #define START    0.0                    /* initial (open the door)        */
#define STOP     20000.0                /* terminal (close the door) time */
#define SERVERS  4                      /* number of servers              */
#ifdef START
#undef START
#define START 0.0
#endif


typedef struct event_list_t {          /* the next-event list    */
    double t;                          /*   next event time      */
    int x;                             /*   event status, 0 or 1 */
} event_list;


double GetArrival(double interarrival_time, int stream_arrival)
/* ---------------------------------------------
 * generate the next arrival time, with rate 1/2
 * ---------------------------------------------
 */
{
    static double arrival = START;

    SelectStream(stream_arrival);
    arrival += Exponential(interarrival_time);
    return (arrival);
}


double GetService(double service_time, int stream_service)
/* ---------------------------------------------
 * generate the next service time, with rate 1/6
 * ---------------------------------------------
 */
{
    SelectStream(stream_service);
    return (Exponential(service_time));
}


int NextEvent(event_list event[], int servers)
/* ---------------------------------------
 * return the index of the next event type
 * ---------------------------------------
 */
{
    int e;
    int i = 0;

    while (event[i].x == 0)       /* find the index of the first 'active' */
        i++;                        /* element in the event list            */
    e = i;
    while (i < servers) {         /* now, check the others to find which  */
        i++;                        /* event type is most imminent          */
        if ((event[i].x == 1) && (event[i].t < event[e].t))
            e = i;
    }
    return (e);
}


int FindOne(event_list event[], int servers)
/* -----------------------------------------------------
 * return the index of the available server idle longest
 * -----------------------------------------------------
 */
{
    int s;
    int i = 1;

    while (event[i].x == 1)       /* find the index of the first available */
        i++;                        /* (idle) server                         */
    s = i;
    while (i < servers) {         /* now, check the others to find which   */
        i++;                        /* has been idle longest                 */
        if ((event[i].x == 0) && (event[i].t < event[s].t))
            s = i;
    }
    return (s);
}


int run(long seed, int stop, int servers, double expo_arrival, double expo_service, int stream_arrival,
        int stream_service) {
    struct {
        double current;                  /* current time                       */
        double next;                     /* next (most imminent) event time    */
    } t;
    event_list event[servers + 1];
    long number = 0;             /* number in the node                 */
    int e;                      /* next event index                   */
    int s;                      /* server index                       */
    long index = 0;             /* used to count processed jobs       */
    double area = 0.0;           /* time integrated number in the node */
    struct {                           /* accumulated sums of                */
        double service;                  /*   service times                    */
        long served;                   /*   number served                    */
    } sum[servers + 1];
    PlantSeeds(seed);
    t.current = START;
    event[0].t = GetArrival(expo_arrival, stream_arrival);
    event[0].x = 1;                 // sistema busy, accetta nuovi job in arrivo.
    for (s = 1; s <= servers; s++) {
        event[s].t = START;          /* this value is arbitrary because */
        event[s].x = 0;              /* all servers are initially idle  */
        sum[s].service = 0.0;
        sum[s].served = 0;
    }

    while ((event[0].x != 0) || (number != 0)) {
        e = NextEvent(event, servers);                  /* next event index */
        t.next = event[e].t;                        /* next event time  */
        area += (t.next - t.current) * number;     /* update integral  */
        t.current = t.next;                            /* advance the clock*/

        if (e == 0) {                                  /* process an arrival*/
            number++; // incrementa il numero di job nel nodo multiserver
            event[0].t = GetArrival(expo_arrival, stream_arrival); // prende il tempo del prossimo arrivo
            if (event[0].t > stop)
                event[0].x = 0; // sistema idle. Non accetta più job
            if (number <= servers) {
                double service = GetService(expo_service, stream_service);
                s = FindOne(event, servers);
                sum[s].service += service;
                sum[s].served++;
                event[s].t = t.current + service;
                event[s].x = 1; // busy
            }
        } else {                                         /* process a departure */
            index++;                                     /* from server s       */
            number--;
            s = e;
            if (number >= servers) {
                double service = GetService(expo_service, stream_service);
                sum[s].service += service;
                sum[s].served++;
                event[s].t = t.current + service;
            } else
                event[s].x = 0; // idle il servente
        }
    }

    printf("\nfor %ld jobs the service node statistics are:\n\n", index);
    double interarrival_time = event[0].t / index;
    printf("  avg interarrivals .. = %6.2f\n", interarrival_time);

    double wait = area / index;
    double node_population = area / t.current;

    printf("  area node %f\n", area);
    printf("  avg wait ........... = %6.2f\n", wait);
    printf("  avg # in node ...... = %6.2f\n", node_population);

    for (s = 1; s <= servers; s++)            /* adjust area to calculate */
        area -= sum[s].service;              /* averages for the queue   */

    double delay = area / index;
    double queue_population = area / t.current;

    printf("  area queue %f\n", area);
    printf("  avg delay .......... = %6.2f\n", delay);
    printf("  avg # in queue ..... = %6.2f\n", queue_population);

    printf("  area service %f\n", sum[s].service);
    printf("  avg service ........ = %6.2f\n", wait - delay);
    printf("  avg # in service ... = %6.2f\n", node_population - queue_population);

    printf("\n  node utilization .. = %8.4f\n", (wait - delay) / (interarrival_time * servers));

    printf("\nthe server statistics are:\n\n");
    printf("    server     utilization     avg service        share\n");
    for (s = 1; s <= servers; s++)
        printf("%8d %14.3f %15.2f %15.3f\n", s, sum[s].service / t.current,
               sum[s].service / sum[s].served,
               (double) sum[s].served / index);
    printf("\n");

    return (0);
}

int main(void) {
    long seed = 123456789;
    int stop = 3 * 3600;
    int stream_arrival = 0;
    // stream service: 1, 2, 3 (dessert), 4 (casse fast), 5 (casse standard), 6 (consumazione)

    // primi
    // run(seed, stop, 3, 1.0 / get_theoretical_lambda_raw(PRIMO), get_theoretical_service(PRIMO), stream_arrival, 1);
    // Attenzione qui il processo degli arrivi non segue lo stream 0!!!
    // secondi
    // run(seed, stop, 3, 1.0 / get_theoretical_lambda_raw(SECONDO), get_theoretical_service(SECONDO), stream_arrival, 2);
    // dessert
    // run(seed, stop, 2, 1.0 / get_theoretical_lambda_raw(DESSERT), get_theoretical_service(DESSERT), stream_arrival, 3);
    // casse fast
    // run(seed, stop, 1, 1.0 / get_theoretical_lambda_raw(CASSA_FAST), get_theoretical_service(CASSA_FAST), stream_arrival, 4);
    // casse standard
    // run(seed, stop, 4, 1.0 / get_theoretical_lambda_raw(CASSA_STD), get_theoretical_service(CASSA_STD), stream_arrival, 5);
    // consumazione
    run(seed, stop, 139, 1.0 / get_theoretical_lambda_raw(CONSUMAZIONE), get_theoretical_service(CONSUMAZIONE), stream_arrival, 6);
}