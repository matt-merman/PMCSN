#include "test.h"

void analytic_testsuite(test_count *t){
    insert_ordered_test(t);
    simulation_visits_test(t);
    simulation_routing_probabilities_test(t);
    lambda_test(t);
    mhu_test(t);
    rho_test(t);
    visits_test(t);
    erlang_c_block_probability_test(t);
    erlang_c_queue_time_test(t);
    erlang_c_response_time_test(t);
    erlang_b_loss_probability_test(t);
    global_response_time_test(t);
    insert_test(t);
    delete_test(t);
    // batch_response_time_test(t);
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

    printf("-------------------------------------\nSuccessful tests: \t%d/%d\n-------------------------------------\n", t.success, t.tests); // variables defined only once in test_definitions.h
    return 0;
}