//
// Created by giaco on 23/12/22.
//
// Test for VERIFICATION step: to verify that our computational model is coherent with analytic model
// that is to see if we implemented the program well
//

#include "test.h"


void analytic_testsuite(test_count *t){
    lambda_test(t);
    mhu_test(t);
    rho_test(t);
    visits_test(t);
    erlang_c_block_probability_test(t);
    erlang_c_queue_time_test(t);
    erlang_c_response_time_test(t);
    erlang_b_loss_probability_test(t);
    global_response_time(t);
    insert_test(t);
    delete_test(t);
    sort_test(t);
}

void all_tests(test_count *t){
    analytic_testsuite(t);
}

int main(){
    test_count t = {
        .success = 0,
        .failures = 0,
        .tests = 0,
    };
    all_tests(&t);

    printf("---------------------------\nSuccessful tests: \t%d/%d\n---------------------------\n", t.success, t.tests); // variables defined only once in test_definitions.h
    return 0;
}