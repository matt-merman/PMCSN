//
// Created by giaco on 17/12/22.
//

#include <stdio.h>
#include "analytic.h"


/**
 * Uses the gamma function to efficiently calculate factorial, knowing that Gamma(n) = (n-1)!
 * @param n integer
 * @return factorial of n
 */
double factorial(int n) {
    return tgamma((double) n + 1.0);
}

/**
 * Computes costs of center node given the number of servers in it
 * @param num number of server
 * @return
 */
int get_costs(int num) {
    return SALARY * N_HOURS * num;
    // TODO: ci sono altri costi oltre al numero di serventi?
}

/**
 * Computes utilization for a server or multiserver
 * @param num_servers number of server in the node
 * @param lambda arrival frequency
 * @param mhu departure or service frequency
 * @return utilization of server/multiserver
 */
double utilization(int num_servers, double lambda, double mhu) {
    return lambda / (num_servers * mhu);
}

/**
 * Computes the probability that all the servers are full and that queue will start growing
 * @param m number of servers
 * @param rho utilization of block
 * @return block probability of multiserver with infinite queue
 */
double erlang_c_block_probability(int m, double rho) {
    double coefficient = pow(m * rho, m) / (factorial(m) * (1.0 - rho)); // Checked
    double sum = 0.0;
    for (int i = 0; i < m; i++) {
        sum += pow(m * rho, i) / factorial(i);
    }
    return coefficient / (sum + coefficient);
}

double erlang_c_queue_time(double block_probability, double service_time_multi, double rho) {
    // printf("%g, %g, %g\n", block_probability, service_time_multi, rho);
    return block_probability * service_time_multi / (1.0 - rho);
}

double erlang_c_response_time(double queue_time, double service_time) {
    return queue_time + service_time;
}