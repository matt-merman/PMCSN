//
// Created by giaco on 22/10/22.
//

#include "analytic_test.h"
int lambda_test(test_count *t){
    double lambda1 = get_theoretical_lambda(PRIMO);
    double lambda2 = get_theoretical_lambda(SECONDO);
    double lambda3 = get_theoretical_lambda(DESSERT);
    double lambdaF = get_theoretical_lambda(CASSA_FAST);
    double lambdaC = get_theoretical_lambda(CASSA_STD);
    double lambdaS = get_theoretical_lambda(CONSUMAZIONE);

    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(lambda1, 0.173611111, "lambda1");
    ASSERT_DOUBLE_EQUAL(lambda2, 0.153356481, "lambda2");
    ASSERT_DOUBLE_EQUAL(lambda3, 0.112413194, "lambda3");
    ASSERT_DOUBLE_EQUAL(lambdaF, 0.055808738, "lambdaF");
    ASSERT_DOUBLE_EQUAL(lambdaC, 0.175672743, "lambdaC");
    ASSERT_DOUBLE_EQUAL(lambdaS, 0.231481481, "lambdaS");

    SUCCESS;
}

int mhu_test(test_count *t){
    double mhu1 = get_theoretical_mhu(PRIMO);
    double mhu2 = get_theoretical_mhu(SECONDO);
    double mhu3 = get_theoretical_mhu(DESSERT);
    double mhuF = get_theoretical_mhu(CASSA_FAST);
    double mhuC = get_theoretical_mhu(CASSA_STD);
    double mhuS = get_theoretical_mhu(CONSUMAZIONE);

    // expected values computed from analytic model
    ASSERT_DOUBLE_EQUAL(mhu1, 1.0 / 15.0, "mhu1");
    ASSERT_DOUBLE_EQUAL(mhu2, 1.0 / 15.0, "mhu2");
    ASSERT_DOUBLE_EQUAL(mhu3, 1.0 / 10.0, "mhu3");
    ASSERT_DOUBLE_EQUAL(mhuF, 1.0 / 11.0, "mhuF");
    ASSERT_DOUBLE_EQUAL(mhuC, 1.0 / 18.0, "mhuC");
    ASSERT_DOUBLE_EQUAL(mhuS, 1.0 / 600.0, "mhuS");

    SUCCESS;
}

int rho_test(test_count *t){
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