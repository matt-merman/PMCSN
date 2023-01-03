//
// Created by giaco on 17/12/22.
//

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

// Computes the theoretical utilization of the block, even if it's a multiserver block.
double get_theoretical_rho(block_type block_type, int num_servers) {
    double lambda = get_theoretical_lambda(block_type);
    double mhu = get_theoretical_mhu(block_type);
    return utilization(num_servers, lambda, mhu);
}

double get_theoretical_mhu(block_type type) {
    return 1.0 / get_theoretical_service(type);
}

double get_theoretical_service(block_type type) {
    switch (type) {
        case PRIMO:
            return S_PRIMO;
        case SECONDO:
            return S_SECONDO;
        case DESSERT:
            return S_DESSERT;
        case CASSA_FAST:
            return S_CASSA_FAST;
        case CASSA_STD:
            return S_CASSA_STD;
        case CONSUMAZIONE:
            return S_CONSUMAZIONE;
        default:
            return 0.0;
    }
}

double get_theoretical_lambda(block_type type) {
    double lambda1 = LAMBDA * P_PRIMO_FUORI;
    double lambda2 = LAMBDA * P_SECONDO_FUORI + lambda1 * P_SECONDO_PRIMO;
    double lambda3 = lambda1 * P_DESSERT_PRIMO + lambda2 * P_DESSERT_SECONDO;
    double lambdaC = lambda2 * P_CASSA_STD_SECONDO + lambda3 * P_CASSA_STD_DESSERT;
    double lambdaF = lambda1 * P_CASSA_FAST_PRIMO + lambda2 * P_CASSA_FAST_SECONDO;
    double lambdaS = lambdaC + lambdaF;
    switch (type) {
        case PRIMO: //you can take PRIMO only from outside
            return lambda1; // checked
        case SECONDO: // you can take SECONDO from outside or from PRIMO
            return lambda2;
        case DESSERT: // you can arrive from PRIMO or SECONDO, with 2 (if skipping SECONDO from PRIMO or starting with SECONDO) or 3 plate
            return lambda3;
        case CASSA_FAST: // you can arrive from PRIMO or SECONDO, with only one plate
            return lambdaF;
        case CASSA_STD: // who doesn't go to the fast cashier, goes to the standard cashier
            return lambdaC;
        case CONSUMAZIONE: // the entire arrival flow will come to CONSUMAZIONE
            return lambdaS;
        default:
            return (0.0);
    }
}

double get_theoretical_visits(block_type type) {
    return get_theoretical_lambda(type) / LAMBDA;
}

/**
 * Computes the probability that all the servers are full and that queue will start growing
 * @param m number of servers
 * @param rho utilization of block
 * @return block probability of multiserver with infinite queue
 */
double erlang_c_block_probability(int m, double rho) {
    double coefficient = pow(m * rho, m) / (factorial(m) * (1.0 - rho)); // Checked on wolfram
    double sum = 0.0;
    for (int i = 0; i < m; i++) {
        sum += pow(m * rho, i) / factorial(i);
    }
    return coefficient / (sum + coefficient);
}

double erlang_c_queue_time(double block_probability, double service_time_multi, double rho) {
    return block_probability * service_time_multi / (1.0 - rho);
}

double erlang_c_response_time(double queue_time, double service_time) {
    return queue_time + service_time;
}