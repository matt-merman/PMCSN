// Verification phase: to check if all implemented formulas are equal
// to the theoretical formulas.
#include "analytic_test.h"

// used to cache the simulation results
network *n = NULL;
#ifndef EXTENDED
const char *block_names[BLOCKS] = {"PRIMO", "SECONDO", "DESSERT", "CASSA_FAST", "CASSA_STD", "CONSUMAZIONE"};
#else
const char *block_names[BLOCKS] = {"PRIMO", "SECONDO", "DESSERT", "CASSA_FAST", "CASSA_STD", "CONSUMAZIONE", "CONSUMAZIONE_2"};
#endif

network *mock_network() {
    if (n != NULL) {
        return n; // return the cache if it hits
    }
    // re-run simulation otherwise.
    network *canteen = create_network((const char **) block_names);
    simulation(canteen, 0, NULL, STANDARD, PERIOD, 0, 1);
    n = canteen;
    return canteen;
}

int simulation_visits_test(test_count *t) {
    network *canteen = mock_network();

    double visits1 = get_simulation_visit(canteen, PRIMO);
    double visits2 = get_simulation_visit(canteen, SECONDO);
    double visits3 = get_simulation_visit(canteen, DESSERT);
    double visits4 = get_simulation_visit(canteen, CASSA_FAST);
    double visits5 = get_simulation_visit(canteen, CASSA_STD);
    double visits6 = get_simulation_visit(canteen, CONSUMAZIONE);
#ifdef EXTENDED
    double visits7 = get_simulation_visit(canteen, CONSUMAZIONE_2);
#endif

    ASSERT_DOUBLE_APPROX_EQUAL(visits1, 0.75, "visits1");
    ASSERT_DOUBLE_APPROX_EQUAL(visits2, 0.6625, "visits2");
    ASSERT_DOUBLE_APPROX_EQUAL(visits3, 0.485625, "visits3");
    ASSERT_DOUBLE_APPROX_EQUAL(visits4, 0.24109375, "visitsF");
    ASSERT_DOUBLE_APPROX_EQUAL(visits5, 0.75890625, "visitsC");
#ifndef EXTENDED
    ASSERT_DOUBLE_APPROX_EQUAL(visits6, 0.935746984, "visitsS");
#else
    ASSERT_DOUBLE_APPROX_EQUAL(visits6, 0.509634, "visitsS1"); //TODO: ricalcolare per il modello esteso
    ASSERT_DOUBLE_APPROX_EQUAL(visits7, 0.5, "visitsS2");
#endif

#ifndef EXTENDED
    PRINTF("%f\n%f\n%f\n%f\n%f\n%f\n", visits1, visits2, visits3, visits4, visits5, visits6);
#else
    PRINTF("%f\n%f\n%f\n%f\n%f\n%f\n%f\n", visits1, visits2, visits3, visits4, visits5, visits6, visits7);
#endif
    // !!!!!!!!!!!! don't clear the network here!! we need it for the next test !!!!!!!!!!!!!!!!!

    SUCCESS;
}

int simulation_routing_probabilities_test(test_count *t) {
    // la formula dovrebbe essere. se i = sorgente, j = destinazione, 0 = esterno.

    // p_ij = routing probability from block i to block j
    double p_01 = get_simulation_routing_prob(n, ESTERNO, PRIMO);
    double p_02 = get_simulation_routing_prob(n, ESTERNO, SECONDO);
    double p_12 = get_simulation_routing_prob(n, PRIMO, SECONDO);
    double p_13 = get_simulation_routing_prob(n, PRIMO, DESSERT);
    double p_1F = get_simulation_routing_prob(n, PRIMO, CASSA_FAST);
    double p_23 = get_simulation_routing_prob(n, SECONDO, DESSERT);
    double p_2F = get_simulation_routing_prob(n, SECONDO, CASSA_FAST);
    double p_2C = get_simulation_routing_prob(n, SECONDO, CASSA_STD);
    double p_3C = get_simulation_routing_prob(n, DESSERT, CASSA_STD);
    double p_FS = get_simulation_routing_prob(n, CASSA_FAST, CONSUMAZIONE);
    double p_CS = get_simulation_routing_prob(n, CASSA_STD, CONSUMAZIONE);
    double p_S0 = get_simulation_routing_prob(n, CONSUMAZIONE, ESTERNO);
#ifdef EXTENDED
    double p_FS2 = get_simulation_routing_prob(n, CASSA_FAST, CONSUMAZIONE_2);
    double p_CS2 = get_simulation_routing_prob(n, CASSA_STD, CONSUMAZIONE_2);
    double p_S02 = get_simulation_routing_prob(n, CONSUMAZIONE_2, ESTERNO);
#endif

    ASSERT_DOUBLE_APPROX_EQUAL(p_01, 0.75, "p_01");
    ASSERT_DOUBLE_APPROX_EQUAL(p_02, 0.25, "p_02");
    ASSERT_DOUBLE_APPROX_EQUAL(p_12, 0.55, "p_12");
    ASSERT_DOUBLE_APPROX_EQUAL(p_13, 0.25, "p_13");
    ASSERT_DOUBLE_APPROX_EQUAL(p_1F, 0.2, "p_1F");
    ASSERT_DOUBLE_APPROX_EQUAL(p_23, 0.45, "p_23");
    ASSERT_DOUBLE_APPROX_EQUAL(p_2F, 0.1375, "p_2F");
    ASSERT_DOUBLE_APPROX_EQUAL(p_2C, 0.4125, "p_2C");
    ASSERT_DOUBLE_APPROX_EQUAL(p_3C, 1.0, "p_3C");
#ifndef EXTENDED
    ASSERT_DOUBLE_APPROX_EQUAL(p_FS, 1.0, "p_FS");
    ASSERT_DOUBLE_APPROX_EQUAL(p_CS, 1.0, "p_CS");
    ASSERT_DOUBLE_APPROX_EQUAL(p_S0, 1.0, "p_S0");
#else
    ASSERT_DOUBLE_APPROX_EQUAL(p_FS, 0.5, "p_FS");
    ASSERT_DOUBLE_APPROX_EQUAL(p_CS, 0.5, "p_CS");
    ASSERT_DOUBLE_APPROX_EQUAL(p_S0, 1.0, "p_S0");
    ASSERT_DOUBLE_APPROX_EQUAL(p_FS2, 0.5, "p_FS2");
    ASSERT_DOUBLE_APPROX_EQUAL(p_CS2, 0.5, "p_CS2");
    ASSERT_DOUBLE_APPROX_EQUAL(p_S02, 1.0, "p_S02");
#endif

    PRINTF("p_01 = %f\n", p_01);
    PRINTF("p_02 = %f\n", p_02);
    PRINTF("p_12 = %f\n", p_12);
    PRINTF("p_13 = %f\n", p_13);
    PRINTF("p_1F = %f\n", p_1F);
    PRINTF("p_23 = %f\n", p_23);
    PRINTF("p_2F = %f\n", p_2F);
    PRINTF("p_2C = %f\n", p_2C);
    PRINTF("p_3C = %f\n", p_3C);
    PRINTF("p_FS = %f\n", p_FS);
    PRINTF("p_CS = %f\n", p_CS);
    PRINTF("p_S0 = %f\n", p_S0);
#ifdef EXTENDED
    PRINTF("p_FS2 = %f\n", p_FS2);
    PRINTF("p_CS2 = %f\n", p_CS2);
    PRINTF("p_S02 = %f\n", p_S02);
#endif
    // we clear the mock_network.
    clear_network(n, TRUE);
    SUCCESS;
}

// test_count is used inside SUCCESS and ASSERT_DOUBLE_EQUAL macros!
int lambda_test(test_count *t) {
    double lambda1 = get_theoretical_lambda_raw(PRIMO);
    double lambda2 = get_theoretical_lambda_raw(SECONDO);
    double lambda3 = get_theoretical_lambda_raw(DESSERT);
    double lambdaF = get_theoretical_lambda_raw(CASSA_FAST);
    double lambdaC = get_theoretical_lambda_raw(CASSA_STD);
    double lambdaS = get_theoretical_lambda_raw(CONSUMAZIONE);
#ifdef EXTENDED
    double lambdaS2 = get_theoretical_lambda_raw(CONSUMAZIONE_2);
#endif
    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(lambda1, 0.173611111, "lambda1");
    ASSERT_DOUBLE_EQUAL(lambda2, 0.153356481, "lambda2");
    ASSERT_DOUBLE_EQUAL(lambda3, 0.112413194, "lambda3");
    ASSERT_DOUBLE_EQUAL(lambdaF, 0.055808738, "lambdaF");
    ASSERT_DOUBLE_EQUAL(lambdaC, 0.175672743, "lambdaC");
#ifndef EXTENDED
    ASSERT_DOUBLE_EQUAL(lambdaS, 0.231481481, "lambdaS");
#else
    ASSERT_DOUBLE_EQUAL(lambdaS, 0.115741, "lambdaS");
    ASSERT_DOUBLE_EQUAL(lambdaS2, 0.115741, "lambdaS2");
#endif
    SUCCESS;
}

int mhu_test(test_count *t) {
    double mhu1 = get_theoretical_mhu(PRIMO);
    double mhu2 = get_theoretical_mhu(SECONDO);
    double mhu3 = get_theoretical_mhu(DESSERT);
    double mhuF = get_theoretical_mhu(CASSA_FAST);
    double mhuC = get_theoretical_mhu(CASSA_STD);
    double mhuS = get_theoretical_mhu(CONSUMAZIONE);
#ifdef EXTENDED
    double mhuS2 = get_theoretical_mhu(CONSUMAZIONE_2);
#endif

    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(mhu1, 0.06666667, "mhu1");
    ASSERT_DOUBLE_EQUAL(mhu2, 0.06666667, "mhu2");
    ASSERT_DOUBLE_EQUAL(mhu3, 0.1, "mhu3");
    ASSERT_DOUBLE_EQUAL(mhuF, 0.09090909, "mhuF");
    ASSERT_DOUBLE_EQUAL(mhuC, 0.05555556, "mhuC");
#ifndef EXTENDED
    ASSERT_DOUBLE_EQUAL(mhuS, 0.001666667, "mhuS");
#else
    ASSERT_DOUBLE_EQUAL(mhuS, 0.001666667, "mhuS"); //TODO ricalcolare
    ASSERT_DOUBLE_EQUAL(mhuS2, 0.001666667, "mhuS");
#endif
    SUCCESS;
}

int rho_test(test_count *t) {
    double rho1 = get_theoretical_rho(PRIMO, 3);
    double rho2 = get_theoretical_rho(SECONDO, 3);
    double rho3 = get_theoretical_rho(DESSERT, 2);
    double rhoF = get_theoretical_rho(CASSA_FAST, 1);
    double rhoC = get_theoretical_rho(CASSA_STD, 4);
#ifndef EXTENDED
    double rhoS = get_theoretical_rho(CONSUMAZIONE, 139);
#else
    double rhoS = get_theoretical_rho(CONSUMAZIONE, 75);
    double rhoS2 = get_theoretical_rho(CONSUMAZIONE_2, 75);
#endif
    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(rho1, 0.868055556, "rho1");
    ASSERT_DOUBLE_EQUAL(rho2, 0.766782407, "rho2");
    ASSERT_DOUBLE_EQUAL(rho3, 0.562065972, "rho3");
    ASSERT_DOUBLE_EQUAL(rhoF, 0.613896123, "rhoF");
    ASSERT_DOUBLE_EQUAL(rhoC, 0.790527344, "rhoC");
#ifndef EXTENDED
    ASSERT_DOUBLE_EQUAL(rhoS, 0.934999, "rhoS");
#else
    ASSERT_DOUBLE_EQUAL(rhoS, 0.881319, "rhoS"); // TODO: ricalcola
    ASSERT_DOUBLE_EQUAL(rhoS2, 0.881319, "rhoS2");
#endif

    SUCCESS;
}

int visits_test(test_count *t) {
    double visits1 = get_theoretical_visits(PRIMO);
    double visits2 = get_theoretical_visits(SECONDO);
    double visits3 = get_theoretical_visits(DESSERT);
    double visitsF = get_theoretical_visits(CASSA_FAST);
    double visitsC = get_theoretical_visits(CASSA_STD);
#ifndef EXTENDED
    double visitsS = get_theoretical_visits(CONSUMAZIONE);
#else
    double visitsS = get_theoretical_visits(CONSUMAZIONE);
    double visitsS2 = get_theoretical_visits(CONSUMAZIONE_2);
#endif
    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(visits1, 0.75, "visits1");
    ASSERT_DOUBLE_EQUAL(visits2, 0.6625, "visits2");
    ASSERT_DOUBLE_EQUAL(visits3, 0.485625, "visits3");
    ASSERT_DOUBLE_EQUAL(visitsF, 0.24109375, "visitsF");
    ASSERT_DOUBLE_EQUAL(visitsC, 0.75890625, "visitsC");
#ifndef EXTENDED
    ASSERT_DOUBLE_EQUAL(visitsS, 0.974803, "visitsS");
#else
    ASSERT_DOUBLE_EQUAL(visitsS, 0.5, "visitsS");
    ASSERT_DOUBLE_EQUAL(visitsS2, 0.5, "visitsS2");
#endif
    SUCCESS;
}

int erlang_c_block_probability_test(test_count *t) {

    double pb = erlang_c_block_probability(3, 125.0 / 144.0);

    ASSERT_DOUBLE_EQUAL(pb, 0.761288, "erlang_c_block_probability_test");

    SUCCESS;
}

int erlang_c_queue_time_test(test_count *t) {
    double rho = 125.0 / 144.0;
    double queue_time = erlang_c_queue_time(erlang_c_block_probability(3, rho), 5.0, rho);

    ASSERT_DOUBLE_EQUAL(queue_time, 28.84882392, "erlang_c_queue_time_test");

    SUCCESS;
}

int erlang_c_response_time_test(test_count *t) {
    double rho = 125.0 / 144.0;
    double multi_service_time = 5.0;
    double single_service_time = 15.0;
    double queue_time = erlang_c_queue_time(erlang_c_block_probability(3, rho), multi_service_time, rho);
    double response_time = erlang_c_response_time(queue_time, single_service_time);

    ASSERT_DOUBLE_EQUAL(response_time, 43.84882392, "erlang_c_queue_time_test");

    SUCCESS;
}

int erlang_b_loss_probability_test(test_count *t) {
    int m = 139;
    double lambda = 25. / 108.;
    double mhu = 1. / 600.;
    double erlang_b_loss_prob = (double) erlang_b_loss_probability(m, lambda, mhu);

    ASSERT_DOUBLE_EQUAL(erlang_b_loss_prob, 0.06425302, "erlang_b_loss_probability");

    SUCCESS;
}

int global_response_time_test(test_count *t) {
// #ifndef EXTENDED
//     int net_servers[] = {3, 3, 2, 1, 4, 139};
// #else
//     int net_servers[] = {3, 3, 2, 1, 4, 75, 75};
// #endif
    int *net_servers = init_network();

    double response_time = get_theoretical_global_response_time(net_servers);

    ASSERT_DOUBLE_EQUAL(response_time, 688.336, "global_response_time");

    SUCCESS;
}

/*
* To check if the global response with a single simulation is equal
* to the batch's global response time mean. 
*/
int batch_response_time_test(test_count *t) {
    network *infinite_network = mock_infinite_network();
    double mean = 0;
    for (int i = 0; i < K_BATCH; i++) {
        mean += infinite_network->batch_response_time[i];
    }
    mean = mean / (double) K_BATCH;
    double actual_mean = infinite_network->batch_response_time[K_BATCH - 1];

    clear_network(infinite_network, TRUE);

    ASSERT_DOUBLE_EQUAL(mean, actual_mean, "batch_global_response_time");
    SUCCESS;
}

network *mock_infinite_network() {

    network *canteen;
    int batch_index;

    canteen = create_network(block_names);

    long arrived_jobs = 0;
    for (batch_index = 1; batch_index <= K_BATCH; batch_index++) {
        simulation(canteen, batch_index * B, &arrived_jobs, INFINITE, PERIOD, batch_index, K_BATCH);
        compute_batch_statistics(canteen, batch_index - 1, PERIOD);
    }

    return canteen;
}