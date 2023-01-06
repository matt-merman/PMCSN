//
// Created by giaco on 22/10/22.
//

#ifndef PROGETTOPMCSN_TESTS_H
#define PROGETTOPMCSN_TESTS_H
#include <stdio.h>
#include "../src/analytic.h"
#include "../src/start.h"
#include "test.h"

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

#endif //PROGETTOPMCSN_TESTS_H
