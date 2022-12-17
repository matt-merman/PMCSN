#ifndef _HELP_H_
#define _HELP_H_

#include "../libs/rngs.h"
#include "../libs/rvgs.h"
#include "./constants.h"
#include "./structs.h"
#include "../src/validation.h"
#include "../src/analytic.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define EPSILON 0.001 // With 0.01 it gives the WARNING
#define BIG_EPSILON 0.99
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) > (y) ? (y) : (x))
#define IS_EQUAL(x, y) ((MAX((x),(y)) - MIN((x),(y)) <= EPSILON) ? 1 : 0)
#define IS_NOT_EQUAL(x, y) ((MAX((x),(y)) - MIN((x),(y)) <= EPSILON) ? 0 : 1)
#define IS_NOT_APPROX_EQUAL(x,y) ((MAX((x),(y)) - MIN((x),(y)) <= BIG_EPSILON) ? 0 : 1)
#define FIND_SEGFAULT(x) printf("I'm here: %s\n", (x))

double	get_next_arrival(double current, double lambda);
double	get_next_service(block_type type, int stream);
double  get_theoretical_lambda(block_type type); // arrival frequency
double  get_theoretical_mhu(block_type type); // service frequency
double  get_theoretical_service(block_type type); // service time
double  get_theoretical_rho(block *block); // this checks also the number of servers and computes rho accordingly
void    get_stats(block *b, clock *clock, statistics *stats);
void	show_and_validate_block_stats(block **blocks, clock *clock);
void	show_and_validate_global_stats(block **blocks, clock *clock);
void    clear_mem(block **blocks);


#endif


