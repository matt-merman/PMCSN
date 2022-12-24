//
// Created by giaco on 17/12/22.
//

#ifndef PROGETTOPMCSN_ANALYTIC_H
#define PROGETTOPMCSN_ANALYTIC_H

#include <math.h>
#include "../utils/constants.h"
#include "../utils/structs.h"


double  factorial(int n);
int     get_costs(int num);
double  utilization(int num_servers, double lambda, double mhu);
double  get_theoretical_lambda(block_type type); // arrival frequency
double  get_theoretical_mhu(block_type type); // service frequency
double  get_theoretical_service(block_type type); // service time
double  get_theoretical_rho(block_type block_type, int num_servers); // this checks also the number of servers and computes rho accordingly
double  get_theoretical_visits(block_type type);
double  erlang_c_block_probability(int m, double rho);
double  erlang_c_queue_time(double block_probability, double service_time_multi, double rho);
double  erlang_c_response_time(double queue_time, double service_time);

#endif //PROGETTOPMCSN_ANALYTIC_H
