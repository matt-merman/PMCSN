// Verification phase: to check if all implemented formulas are equal 
// to the theoretical formulas.     
// Created by giaco on 22/10/22.
//

#include "analytic_test.h"

// used to cache the simulation results
network *n = NULL;


network *mock_network() {
    if (n != NULL) {
        return n; // return the cache if i hits
    }
    // re-run simulation otherwise.
    const char *block_names[BLOCKS] = {"PRIMO", "SECONDO", "DESSERT", "CASSA_FAST", "CASSA_STD", "CONSUMAZIONE"};
    network *canteen = create_network((const char **) block_names, CONFIG_2);
    simulation(canteen, 0, NULL, STANDARD);
    n = canteen;
    return canteen;
}

int simulation_visits_test(test_count *t) {
    mock_network();
    double visits1 = get_simulation_visit(n, PRIMO);
    double visits2 = get_simulation_visit(n, SECONDO);
    double visits3 = get_simulation_visit(n, DESSERT);
    double visits4 = get_simulation_visit(n, CASSA_FAST);
    double visits5 = get_simulation_visit(n, CASSA_STD);
    double visits6 = get_simulation_visit(n, CONSUMAZIONE);

    // don't clear the network here!! we need it for the next test.

    ASSERT_DOUBLE_APPROX_EQUAL(visits1, 0.75, "visits1");
    ASSERT_DOUBLE_APPROX_EQUAL(visits2, 0.6625, "visits2");
    ASSERT_DOUBLE_APPROX_EQUAL(visits3, 0.485625, "visits3");
    ASSERT_DOUBLE_APPROX_EQUAL(visits4, 0.24109375, "visitsF");
    ASSERT_DOUBLE_APPROX_EQUAL(visits5, 0.75890625, "visitsC");
    ASSERT_DOUBLE_APPROX_EQUAL(visits6, 0.935746984, "visitsS");

    PRINTF("%f\n%f\n%f\n%f\n%f\n%f\n", visits1, visits2, visits3, visits4, visits5, visits6);

    SUCCESS;
}

int simulation_routing_probabilities_test(test_count *t){
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

    ASSERT_DOUBLE_APPROX_EQUAL(p_01, 0.75, "p_01");
    ASSERT_DOUBLE_APPROX_EQUAL(p_02, 0.25, "p_02");
    ASSERT_DOUBLE_APPROX_EQUAL(p_12, 0.55, "p_12");
    ASSERT_DOUBLE_APPROX_EQUAL(p_13, 0.25, "p_13");
    ASSERT_DOUBLE_APPROX_EQUAL(p_1F, 0.2, "p_1F");
    ASSERT_DOUBLE_APPROX_EQUAL(p_23, 0.45, "p_23");
    ASSERT_DOUBLE_APPROX_EQUAL(p_2F, 0.1375, "p_2F");
    ASSERT_DOUBLE_APPROX_EQUAL(p_2C, 0.4125, "p_2C");
    ASSERT_DOUBLE_APPROX_EQUAL(p_3C, 1.0, "p_3C");
    ASSERT_DOUBLE_APPROX_EQUAL(p_FS, 1.0, "p_FS");
    ASSERT_DOUBLE_APPROX_EQUAL(p_CS, 1.0, "p_CS");
    ASSERT_DOUBLE_APPROX_EQUAL(p_S0, 1.0, "p_CS");

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

    // we clear the mock_network.
    clear_network(n);
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

    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(lambda1, 0.173611111, "lambda1");
    ASSERT_DOUBLE_EQUAL(lambda2, 0.153356481, "lambda2");
    ASSERT_DOUBLE_EQUAL(lambda3, 0.112413194, "lambda3");
    ASSERT_DOUBLE_EQUAL(lambdaF, 0.055808738, "lambdaF");
    ASSERT_DOUBLE_EQUAL(lambdaC, 0.175672743, "lambdaC");
    ASSERT_DOUBLE_EQUAL(lambdaS, 0.231481481, "lambdaS");

    SUCCESS;
}

int mhu_test(test_count *t) {
    double mhu1 = get_theoretical_mhu(PRIMO);
    double mhu2 = get_theoretical_mhu(SECONDO);
    double mhu3 = get_theoretical_mhu(DESSERT);
    double mhuF = get_theoretical_mhu(CASSA_FAST);
    double mhuC = get_theoretical_mhu(CASSA_STD);
    double mhuS = get_theoretical_mhu(CONSUMAZIONE);

    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(mhu1, 0.06666667, "mhu1");
    ASSERT_DOUBLE_EQUAL(mhu2, 0.06666667, "mhu2");
    ASSERT_DOUBLE_EQUAL(mhu3, 0.1, "mhu3");
    ASSERT_DOUBLE_EQUAL(mhuF, 0.09090909, "mhuF");
    ASSERT_DOUBLE_EQUAL(mhuC, 0.05555556, "mhuC");
    ASSERT_DOUBLE_EQUAL(mhuS, 0.001666667, "mhuS");

    SUCCESS;
}

int rho_test(test_count *t) {
    double rho1 = get_theoretical_rho(PRIMO, 3);
    double rho2 = get_theoretical_rho(SECONDO, 3);
    double rho3 = get_theoretical_rho(DESSERT, 2);
    double rhoF = get_theoretical_rho(CASSA_FAST, 1);
    double rhoC = get_theoretical_rho(CASSA_STD, 4);
    double rhoS = get_theoretical_rho(CONSUMAZIONE, 139);

    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(rho1, 0.868055556, "rho1");
    ASSERT_DOUBLE_EQUAL(rho2, 0.766782407, "rho2");
    ASSERT_DOUBLE_EQUAL(rho3, 0.562065972, "rho3");
    ASSERT_DOUBLE_EQUAL(rhoF, 0.613896123, "rhoF");
    ASSERT_DOUBLE_EQUAL(rhoC, 0.790527344, "rhoC");
    ASSERT_DOUBLE_EQUAL(rhoS, 0.934999, "rhoS");

    SUCCESS;
}

int visits_test(test_count *t) {
    double visits1 = get_theoretical_visits(PRIMO, 3);
    double visits2 = get_theoretical_visits(SECONDO, 3);
    double visits3 = get_theoretical_visits(DESSERT, 2);
    double visitsF = get_theoretical_visits(CASSA_FAST, 1);
    double visitsC = get_theoretical_visits(CASSA_STD, 4);
    double visitsS = get_theoretical_visits(CONSUMAZIONE, 139);

    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(visits1, 0.75, "visits1");
    ASSERT_DOUBLE_EQUAL(visits2, 0.6625, "visits2");
    ASSERT_DOUBLE_EQUAL(visits3, 0.485625, "visits3");
    ASSERT_DOUBLE_EQUAL(visitsF, 0.24109375, "visitsF");
    ASSERT_DOUBLE_EQUAL(visitsC, 0.75890625, "visitsC");
    ASSERT_DOUBLE_EQUAL(visitsS, 0.935746984, "visitsS");

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
    double erlang_b_loss_prob = erlang_b_loss_probability(m, lambda, mhu);

    ASSERT_DOUBLE_EQUAL(erlang_b_loss_prob, 0.06425302, "erlang_b_loss_probability");

    SUCCESS;
}

int global_response_time_test(test_count *t) {

    int net_servers[] = {3, 3, 2, 1, 4, 139};
    double response_time = global_respones_time(net_servers);

    ASSERT_DOUBLE_EQUAL(response_time, 649.78441, "global_response_time");

    SUCCESS;
}

int hour_to_days_test(test_count *t) {
    int one_day = to_days_rounding_up(3);

    ASSERT_INT_EQUAL(one_day, 1, "hour_to_days_test");

    int two_days = to_days_rounding_up(25);

    ASSERT_INT_EQUAL(two_days, 2, "hour_to_days_test");

    int zero_days = to_days_rounding_up(0);

    ASSERT_INT_EQUAL(zero_days, 0, "hour_to_days_test");


    SUCCESS;
}