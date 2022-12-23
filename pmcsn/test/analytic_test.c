//
// Created by giaco on 22/10/22.
//

#include "analytic_test.h"

int erlang_c_block_probability_test(test_count *t) {

    double pb = erlang_c_block_probability(3, 125.0 / 144.0);

    if (IS_NOT_EQUAL(pb, 0.761288)) {
        printf("erlang_c_block_probability_test - Expected: %g Actual: %g\n", 0.761288, pb);
        FAILURE;
    }

    SUCCESS;
}

int erlang_c_queue_time_test(test_count *t) {
    double rho = 125.0 / 144.0;
    double queue_time = erlang_c_queue_time(erlang_c_block_probability(3, rho), 5.0, rho);

    if (IS_NOT_EQUAL(queue_time, 28.84882392)){
        printf("erlang_c_queue_time_test - Expected: %g Actual: %g\n", 28.84882392, queue_time);
        FAILURE;
    }
    SUCCESS;
}

int erlang_c_response_time_test(test_count *t) {
    double rho = 125.0 / 144.0;
    double multi_service_time = 5.0;
    double single_service_time = 15.0;
    double queue_time = erlang_c_queue_time(erlang_c_block_probability(3, rho), multi_service_time, rho);
    double response_time = erlang_c_response_time(queue_time, single_service_time);
    if (IS_NOT_EQUAL(response_time, 43.84882392)){
        printf("erlang_c_queue_time_test - Expected: %g Actual: %g\n", 28.84882392, queue_time);
        FAILURE;
    }
    SUCCESS;
}