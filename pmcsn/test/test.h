//
// Created by giaco on 23/12/22.
//

#ifndef PROGETTOPMCSN_TEST_H
#define PROGETTOPMCSN_TEST_H

typedef struct test_count_t {
    int failures;
    int success;
    int tests;
} test_count;


#define SUCCESS t->success++; t->tests++; return 0 // Important: USE THIS INSTEAD OF return SUCCESS!!
#define FAILURE t->failures++; t->tests++; return 0 // Important: USE THIS INSTEAD OF return FAILURE!!
#include "analytic_test.h"
#endif //PROGETTOPMCSN_TEST_H
