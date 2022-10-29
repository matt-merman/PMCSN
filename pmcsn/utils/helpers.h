#ifndef _HELP_H_
#define _HELP_H_

#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "./constants.h"
#include "./structs.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define EPSILON 0.001 // With 0.01 it gives the WARNING
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) > (y) ? (y) : (x))
#define IS_EQUAL(x, y) ((MAX((x),(y)) - MIN((x),(y)) <= EPSILON) ? 1 : 0)
#define IS_NOT_EQUAL(x, y) ((MAX((x),(y)) - MIN((x),(y)) <= EPSILON) ? 0 : 1)
#define FIND_SEGFAULT(x) printf("I'm here: %s\n", (x))

double	get_arrival(double current, double lambda);
double	get_service(block_type type, int stream);
void    get_stats(long int completedJobs, clock* clock, area *area, statistics *stats, int num_servers);
void 	validateMM1(block *block, statistics* stats);
void    clear_mem(block **blocks);
void	show_stats(block **blocks, clock *clock);
int get_costs(int num);

#endif


