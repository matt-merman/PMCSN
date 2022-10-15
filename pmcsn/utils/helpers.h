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

double	min(double a, double c);
double	getArrival(double current, double lambda);
double	getService(block_type type, int stream);
void    calculateStatistics(long int completedJobs, clock* clock, area *area, statistics *stats);
void	showStatistics(statistics *stats);
void 	validateMM1(block *block, statistics* stats);
