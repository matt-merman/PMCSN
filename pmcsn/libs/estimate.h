#ifndef _ESTIMATE_H
#define _ESTIMATE_H

#include <math.h>
#include <stdio.h>
#include "../libs/rvms.h"

#define LOC 0.95                       /* level of confidence,        */
/* use 0.95 for 95% confidence */

void CalculateIntervalEstimate(FILE *stdin);

#endif
