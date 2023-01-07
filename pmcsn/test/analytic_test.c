// Verification phase: to check if all implemented formulas are equal 
// to the theoretical formulas.     
// Created by giaco on 22/10/22.
//

#include "analytic_test.h"

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
    ASSERT_DOUBLE_EQUAL(rhoS, 0.999200639, "rhoS");

    SUCCESS;
}

int visits_test(test_count *t){
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

int erlang_b_loss_probability_test(test_count *t){
    double m = 139;
    double lambda = 25./108.;
    double mhu = 1./600.;
    double erlang_b_loss_prob = erlang_b_loss_probability(m, lambda, mhu);

    ASSERT_DOUBLE_EQUAL(erlang_b_loss_prob, 0.06425302, "erlang_b_loss_probability");

    SUCCESS;
}

int global_response_time_test(test_count *t){

    int net_servers[] = {3,3,2,1,4,139};
    double response_time = global_respones_time(net_servers);

    ASSERT_DOUBLE_EQUAL(response_time, 649.78441, "global_response_time");

    SUCCESS;
}

int hour_to_days_test(test_count *t){
    int one_day = to_days_rounding_up(3);

    ASSERT_INT_EQUAL(one_day, 1, "hour_to_days_test");

    int two_days = to_days_rounding_up(25);

    ASSERT_INT_EQUAL(two_days, 2, "hour_to_days_test");

    int zero_days = to_days_rounding_up(0);

    ASSERT_INT_EQUAL(zero_days, 0, "hour_to_days_test");


    SUCCESS;
}