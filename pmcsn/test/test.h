#ifndef _TEST_H_
#define _TEST_H_

typedef struct test_count_t {
    int failures;
    int success;
    int tests;
} test_count;


#define SUCCESS t->success++; t->tests++; return 1 // Important: USE THIS INSTEAD OF return SUCCESS!!
#define FAILURE t->failures++; t->tests++; return 0 // Important: USE THIS INSTEAD OF return FAILURE!!
// x actual, y expected, msg message
#define ASSERT_DOUBLE_EQUAL(x, y, msg) do { \
            if (IS_NOT_EQUAL(x, y)) { \
                printf("%s - Expected: %g Actual: %g\n", msg, y, x); \
                FAILURE; \
            } \
        } while (0)
// i used do {} while(0) to use ASSERT_DOUBLE_EQUAL() like a function (with ; at the end)!


#define ASSERT_DOUBLE_APPROX_EQUAL(x, y, p, msg) do { \
            if (IS_NOT_APPROX_EQUAL_PRECISION(x, y, p)) { \
                printf("%s - Expected: %g Actual: %g\n", msg, y, x); \
                FAILURE; \
            } \
        } while (0)

#define ASSERT_INT_EQUAL(x, y, msg) do { \
            if (x != y) { \
                printf("%s - Expected: %d Actual: %d\n", msg, y, x); \
                FAILURE; \
            } \
        } while (0)

#include "../utils/structs.h"

#endif