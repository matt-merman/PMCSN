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
 * Computes the theoretical utilization of the block, even if it's a multiserver block.
 * @param block_type
 * @param num_servers
 * @return the theoretical utilization of a multiserver node or a single server node.
 */
double get_theoretical_rho(block_type block_type, int num_servers) {
    double lambda = get_theoretical_lambda(block_type, num_servers); // including rejected jobs
    double mhu = get_theoretical_mhu(block_type);
    return utilization(num_servers, lambda, mhu);
}

/**
 * Returns the theoretical service frequency
 * @param type the block type
 * @return the service frequency of the block
 */
double get_theoretical_mhu(block_type type) {
    return 1.0 / get_theoretical_service(type);
}
/**
 * Returns the service time
 * @param type block type
 * @return the service time of the block
 */
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
#ifdef EXTENDED
        case CONSUMAZIONE_2:
#endif
            return S_CONSUMAZIONE;
        default:
            return 0.0;
    }
}
/**
 * Returns the arrival frequency, including the rejected jobs
 * @param type block type
 * @return the raw arrival frequency
 */
double get_theoretical_lambda_raw(block_type type) {
    double lambda1 = LAMBDA * P_PRIMO_FUORI;
    double lambda2 = LAMBDA * P_SECONDO_FUORI + lambda1 * P_SECONDO_PRIMO;
    double lambda3 = lambda1 * P_DESSERT_PRIMO + lambda2 * P_DESSERT_SECONDO;
    double lambdaC = lambda2 * P_CASSA_STD_SECONDO + lambda3 * P_CASSA_STD_DESSERT;
    double lambdaF = lambda1 * P_CASSA_FAST_PRIMO + lambda2 * P_CASSA_FAST_SECONDO;
    double lambdaS = (lambdaC + lambdaF) * P_SCELTA_MENSA;
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
#ifdef EXTENDED
        case CONSUMAZIONE_2:
            return lambdaS;
#endif
        default:
            return (0.0);
    }
}
/**
 * Computes the theoretical lambda, excluding the rejected jobs
 * @param type block type
 * @param num_servers number of servers in the block
 * @return the arrival frequency to the block
 */
double get_theoretical_lambda(block_type type, int num_servers) {
    double lambda_raw = get_theoretical_lambda_raw(type);
    if (!IS_CONSUMAZIONE(type)) {
        return lambda_raw;
    }
    return lambda_raw * (1 - (double) erlang_b_loss_probability(num_servers, lambda_raw, get_theoretical_mhu(type)));
}
/**
 * Computes the mean number of visits to a server
 * @param type the block type
 * @return number of visits
 */
double get_theoretical_visits(block_type type, network *canteen) {
    // return get_theoretical_lambda(type, num_servers) / LAMBDA;

    if (IS_CONSUMAZIONE(type)){
        int m = canteen->blocks[type]->num_servers;
        long double ploss_theoretical = erlang_b_loss_probability(m, get_theoretical_lambda_raw(type),                                                          get_theoretical_mhu(CONSUMAZIONE));
        return (1 - ploss_theoretical) * get_theoretical_lambda_raw(type) / LAMBDA;
    }
    return get_theoretical_lambda_raw(type) / LAMBDA;
}

// With LAMBDA RAW
// The computed global response time (712.541879) doesn't match with the theoretical global response time (688.336224)

// WITH LAMBDA
// The computed global response time (697.250604) doesn't match with the theoretical global response time (673.217853)

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

/**
 * Computes erlang c queue time, using the theoretic formula
 * @param block_probability the probability that all servers are busy
 * @param service_time_multi the service time divided by number of server
 * @param rho the utilization (lambda/(mhu*m))
 * @return the queue time for a multiserver with infinite queue
 */
double erlang_c_queue_time(double block_probability, double service_time_multi, double rho) {
    return block_probability * service_time_multi / (1.0 - rho);
}

/**
 * Computes erlang c response time as the sum of queue time and service time
 * @param queue_time the mean time spent in queue
 * @param service_time the mean time spent in service
 * @return the response time for a multiserver with infinite queue
 */
double erlang_c_response_time(double queue_time, double service_time) {
    return queue_time + service_time;
}

/**
 * Computes erlang b loss probability, that is the probability that all servers are busy
 * and a new job will be lost.
 * @param m the number of server
 * @param lambda the raw lambda, that is all jobs that try to enter the node
 * @param mhu the serving frequency
 * @return erlang b loss probability of a multiserver without queue
 */
long double erlang_b_loss_probability(int m, double lambda, double mhu) {
    long double pi_0 = 0.0, pi_m;
    double rho;
    int i;

    for (i = 0; i <= m; i++) {
        rho = lambda / mhu;
        pi_0 += powl(rho, i) / factorial(i);
    }
    pi_0 = powl(pi_0, -1);

    rho = lambda / mhu;
    pi_m = powl(rho, m) / factorial(m);
    pi_m = pi_m * pi_0;

    return pi_m;
}

double get_theoretical_response_time(block_type type, int m) {

    double service_time = get_theoretical_service(type);
#ifndef EXTENDED
    if (type == CONSUMAZIONE) {
#else
    if(type == CONSUMAZIONE || type == CONSUMAZIONE_2) {
#endif
        return service_time;
    }
    double rho = get_theoretical_rho(type, m);
    double block_probability = erlang_c_block_probability(m, rho);
    double service_time_multi = service_time / m;
    double queue_time = erlang_c_queue_time(block_probability, service_time_multi, rho);
    return erlang_c_response_time(queue_time, service_time);
}

double get_theoretical_global_response_time(int *network_servers, network *canteen) {
    double global_wait = 0.0;
    for (int i = 0; i < BLOCKS; i++) {
        global_wait +=
                get_theoretical_response_time(i, network_servers[i]) * get_theoretical_visits(i, canteen);
    }
    return global_wait;
}

// the erlang b queue time is 0, the erlang b response time is equal to service time.
void calculate_interval_estimate_for_stat(const char *stat_name, const double *global_response_time, int num_replicas) {
    long n = 0;                     /* counts data points */
    double sum = 0.0;
    double mean = 0.0;
    double stdev;
    double u, t, w;
    double diff;


    /* use Welford's one-pass method to compute sample mean and standard deviation */
    for (int rep = 0; rep < num_replicas; rep++) {
        n++;
        diff = global_response_time[rep] - mean;
        sum += diff * diff * ((double) n - 1.0) / (double) n;
        mean += diff / (double) n;
    }

    stdev = sqrt(sum / (double) n);

    if (n > 1) {
        u = 1.0 - 0.5 * (1.0 - LOC);              /* interval parameter  */
        t = idfStudent(n - 1, u);                 /* critical value of t */
        w = t * stdev / sqrt((double) n - 1);              /* interval half width */
        printf("\n============== Ensemble %s =============", stat_name);
        printf("\nbased upon %ld data points", n);
        printf(" and with %d%% confidence\n", (int) (100.0 * LOC + 0.5));
        printf("the expected value is in the interval %10.6f +/- %10.6f\n", mean, w);
    } else
        printf("ERROR - insufficient data\n");
}

void calculate_autocorrelation_for_stats(const char *stat_name, const double *response_time) {
    long i = 0;                   /* data point index              */
    double sum = 0.0;               /* sums x[i]                     */
    long n;                       /* number of data points         */
    long j;                       /* lag index                     */
    long p = 0;                   /* points to the head of 'hold'  */
    double mean;
    double hold[K_BATCH];              /* K + 1 most recent data points */
    double cosum[K_BATCH] = {0.0};     /* cosum[j] sums x[i] * x[i+j]   */
    int k = 0;

    while (i < K_BATCH) {              /* initialize the hold array with */
        sum += response_time[k];
        hold[i] = response_time[k];
        i++;
        k++;
    }

    n = i;

    while (i < n + K_BATCH) {         /* empty the circular array */
        for (j = 0; j < K_BATCH; j++)
            cosum[j] += hold[p] * hold[(p + j) % K_BATCH];
        hold[p] = 0.0;
        p = (p + 1) % K_BATCH;
        i++;
    }

    mean = sum / (double) n;
    for (j = 0; j <= K_BATCH - 1; j++)
        cosum[j] = (cosum[j] / (double) (n - j)) - (mean * mean);

    printf("\n============== Ensemble %s =============\n", stat_name);
    printf("for %ld data points\n", n);
    printf("the mean is ... %8.2f\n", mean);
    printf("the stdev is .. %8.2f\n\n", sqrt(cosum[0]));
    printf("  j (lag)   r[j] (autocorrelation)\n");
    printf("  1  %11.3f\n", cosum[1] / cosum[0]);
}
