#ifndef _ANALYTIC_TESTS_H_
#define _ANALYTIC_TESTS_H_

#include "../utils/structs.h"

int simulation_visits_test(test_count *t);
int simulation_routing_probabilities_test(test_count *t);
int lambda_test(test_count *t);
int mhu_test(test_count *t);
int rho_test(test_count *t);
int visits_test(test_count *t);
int erlang_c_block_probability_test(test_count *t);
int erlang_c_queue_time_test(test_count *t);
int erlang_c_response_time_test(test_count *t);
int erlang_b_loss_probability_test(test_count *t);
int global_response_time_test(test_count *t);
int hour_to_days_test(test_count *t);
network *mock_infinite_network();
int batch_response_time_test(test_count *t);

#endif