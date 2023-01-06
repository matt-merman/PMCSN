//
// Created by giaco on 24/12/22.
//

#include "events_test.h"

// these tests are run one after another

long test_next_event_id = 0L;

// inserts one thing and checks event_list_length
int insert_test(test_count *t){
    int prev_size = event_list_length();
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, get_next_arrival(0.0, LAMBDA));

    int next_size = event_list_length();

    ASSERT_INT_EQUAL(prev_size + 1, next_size, "event_list_length of event list");

    SUCCESS;
}

// inserts some more things and then removes everything
int delete_test(test_count *t){
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.5);
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.2);
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.7);

    while(!is_empty()) {
        int prev_size = event_list_length();
        pop_first();
        int curr_size = event_list_length();
        ASSERT_INT_EQUAL(curr_size, prev_size - 1, "delete test");
    }

    SUCCESS;
}

// insert three events unsorted and sorts them. Finally, removes everything and checks ordering
int sort_test(test_count *t) {
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.5);
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.2);
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.7);

    sort_by_time();

    event *e = pop_first();
    ASSERT_DOUBLE_EQUAL(e->time, 0.2, "first is not 0.2");

    e = pop_first();
    ASSERT_DOUBLE_EQUAL(e->time, 0.5, "second is not 0.5");

    e = pop_first();
    ASSERT_DOUBLE_EQUAL(e->time, 0.7, "last is not 0.7");

    SUCCESS;
}
