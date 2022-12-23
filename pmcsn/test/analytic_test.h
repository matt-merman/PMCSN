//
// Created by giaco on 22/10/22.
//

#ifndef PROGETTOPMCSN_TESTS_H
#define PROGETTOPMCSN_TESTS_H
#include <stdio.h>
#include "../src/analytic.h"
#include "../src/start.h"
#include "test.h"

int erlang_c_block_probability_test(test_count *t);
int erlang_c_queue_time_test(test_count *t);
int erlang_c_response_time_test(test_count *t);

#endif //PROGETTOPMCSN_TESTS_H
