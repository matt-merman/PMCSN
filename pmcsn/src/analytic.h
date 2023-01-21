//
// Created by giaco on 17/12/22.
//

#ifndef PROGETTOPMCSN_ANALYTIC_H
#define PROGETTOPMCSN_ANALYTIC_H

#include <math.h>
#include "../utils/constants.h"
#include "../utils/structs.h"


double  factorial(int n);
int to_days_rounding_up(int hours);

double  utilization(int num_servers, double lambda, double mhu);
double  get_theoretical_lambda_raw(block_type type); // arrival frequency
double get_theoretical_lambda(block_type type, int num_servers); // arrival frequency that accounts for job loss
double  get_theoretical_mhu(block_type type); // service frequency
double  get_theoretical_service(block_type type); // service time
double  get_theoretical_rho(block_type block_type, int num_servers); // this checks also the number of servers and computes rho accordingly
double get_theoretical_visits(block_type type, int num_servers);
double  erlang_c_block_probability(int m, double rho);
double  erlang_c_queue_time(double block_probability, double service_time_multi, double rho);
double  erlang_c_response_time(double queue_time, double service_time);
long double erlang_b_loss_probability(int m, double lambda, double mhu);
// void calculate_interval_estimate_for_stat(stat_type stat, const char *stat_name, replica_stats *replica_stats_ensemble, const char *block_name);
void calculate_interval_estimate_for_stat(const char *stat_name, const double *global_response_time, int num_replicas);
double get_theoretical_response_time(block_type type, int m);
double get_theoretical_global_response_time(int *network_servers);
void calculate_autocorrelation_for_stats(const char *stat_name, const double *response_time);


#endif //PROGETTOPMCSN_ANALYTIC_H
