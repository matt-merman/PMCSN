//
// Created by giaco on 17/12/22.
//

#include <stdio.h>
#include "analytic.h"
#include "../libs/rvms.h"


/**
 * Uses the gamma function to efficiently calculate factorial, knowing that Gamma(n) = (n-1)!
 * @param n integer
 * @return factorial of n
 */
double factorial(int n) {
    return tgamma((double) n + 1.0);
}

int to_days_rounding_up(int hours) {
    return (int) ceil((double) hours / 24.);
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
    double lambda = get_theoretical_lambda(block_type, num_servers);
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

double get_theoretical_lambda_raw(block_type type) {
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

double get_theoretical_lambda(block_type type, int num_servers){
    double lambda_raw = get_theoretical_lambda_raw(type);
    if (type != CONSUMAZIONE) {
        return lambda_raw;
    }
    return lambda_raw * (1-erlang_b_loss_probability(num_servers, lambda_raw, get_theoretical_mhu(type)));
}

double get_theoretical_visits(block_type type, int num_servers) {
    return get_theoretical_lambda(type, num_servers) / LAMBDA;
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

double erlang_b_loss_probability(int m, double lambda, double mhu) {
    double pi_0 = 0.0, rho, pi_m;
    int i;

    for (i = 0; i <= m; i++) {
        rho = lambda / mhu;
        pi_0 += pow(rho, i) / factorial(i);
    }
    pi_0 = pow(pi_0, -1);

    rho = lambda / mhu;
    pi_m = pow(rho, m) / factorial(m);
    pi_m = pi_m * pi_0;

    return pi_m;
}

double get_response_time(block_type type, int m) {

    double service_time = get_theoretical_service(type);
    if (type == CONSUMAZIONE) {
        return service_time;
    }
    double rho = get_theoretical_rho(type, m);
    double block_probability = erlang_c_block_probability(m, rho);
    double service_time_multi = service_time / m;
    double queue_time = erlang_c_queue_time(block_probability, service_time_multi, rho);
    return erlang_c_response_time(queue_time, service_time);
}

double global_respones_time(int *network_servers) {
    double global_wait = 0.0;
    for (int i = 0; i<BLOCKS; i++){
        global_wait += get_response_time(i, network_servers[i]) * get_theoretical_visits(i, network_servers[i]);
    }
    return global_wait;
}

// the erlang b queue time is 0, the erlang b response time is equal to service time.
void calculate_interval_estimate_for_stat(const char *stat_name, const double *global_response_time, int num_replicas) {
    long n = 0;                     /* counts data points */
    double sum = 0.0;
    double mean = 0.0;
    // double data;
    double stdev;
    double u, t, w;
    double diff;


    /* use Welford's one-pass method to compute sample mean and standard deviation */
    for (int rep = 0; rep < num_replicas; rep++) {
        n++;
        diff = global_response_time[rep] - mean;
        sum += diff * diff * ((double)n - 1.0) / (double) n;
        mean += diff / (double) n;
    }

    stdev = sqrt(sum / (double) n);

    if (n > 1) {
        u = 1.0 - 0.5 * (1.0 - LOC);              /* interval parameter  */
        t = idfStudent(n - 1, u);                 /* critical value of t */
        w = t * stdev / sqrt((double) n - 1);              /* interval half width */
        printf("\n============== Ensemble %s for block %s =============", stat_name, stat_name);
        printf("\nbased upon %ld data points", n);
        printf(" and with %d%% confidence\n", (int) (100.0 * LOC + 0.5));
        printf("the expected value is in the interval");
        printf("%10.2f +/- %6.2f\n", mean, w);
    } else
        printf("ERROR - insufficient data\n");
}
