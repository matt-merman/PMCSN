#ifndef _HELP_H_
#define _HELP_H_

#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "./constants.h"
#include "./structs.h"
#include "../src/events.h"
#include "../src/server.h"
#include "../src/validation.h"
#include "../src/analytic.h"
#include "../src/event_list.h"
#include "constants.h"
#include "files.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define EPSILON 0.001 // With 0.01 it gives the WARNING
#define BIG_EPSILON 0.1
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) > (y) ? (y) : (x))
#define IS_EQUAL(x, y) ((MAX((x),(y)) - MIN((x),(y)) <= EPSILON) ? 1 : 0)
#define IS_NOT_EQUAL(x, y) ((MAX((x),(y)) - MIN((x),(y)) <= EPSILON) ? 0 : 1)
#define IS_NOT_APPROX_EQUAL(x,y) ((MAX((x),(y)) - MIN((x),(y)) <= BIG_EPSILON) ? 0 : 1)

#define FIND_SEGFAULT(x) printf("I'm here: %s\n", (x))

double	get_next_arrival(double current, double lambda);
double	get_next_service(block_type type, int stream);
void    get_stats(block *b, timer *clock, statistics *stats);
void    clear_mem(block **blocks);
void debug(event *event, network *canteen);
void write_stats_on_file(block **blocks, timer *clock, FILE **file);
void show_stats(network *canteen);
void validate_stats(network *canteen);
void update_ensemble(network *canteen, int index);
int get_batch_number();

#endif


