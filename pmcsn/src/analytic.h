//
// Created by giaco on 17/12/22.
//

#ifndef PROGETTOPMCSN_ANALYTIC_H
#define PROGETTOPMCSN_ANALYTIC_H

#include <math.h>
#include "../utils/constants.h"

double  factorial(int n);
int     get_costs(int num);
double  utilization(int num_servers, double lambda, double mhu);
double  erlang_c_block_probability(int m, double rho);
double  erlang_c_queue_time(double block_probability, double service_time_multi, double rho);
double  erlang_c_response_time(double queue_time, double service_time);

#endif //PROGETTOPMCSN_ANALYTIC_H
