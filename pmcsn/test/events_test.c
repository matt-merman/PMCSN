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
        event *e = pop_first();
        int curr_size = event_list_length();
        free(e);
        ASSERT_INT_EQUAL(curr_size, prev_size - 1, "delete test");
    }

    SUCCESS;
}

int insert_ordered_test(test_count *t){

    double time;

    event *e1 = new_event(test_next_event_id++, PRIMO, 2., ARRIVAL, 0,0,NULL);
    insert_event_ordered(e1);
    event *e2 = new_event(test_next_event_id++, PRIMO, 4., ARRIVAL, 0,0,NULL);
    insert_event_ordered(e2);
    event *e3 = new_event(test_next_event_id++, PRIMO, 1., ARRIVAL, 0,0,NULL);
    insert_event_ordered(e3);
    event *e4 = new_event(test_next_event_id++, PRIMO, 3., ARRIVAL, 0,0,NULL);
    insert_event_ordered(e4);

    event *e = pop_first();
    time = e->time;
    free(e);
    ASSERT_DOUBLE_EQUAL(time, 1.0, "first is not 1");

    e = pop_first();
    time = e->time;
    free(e);
    ASSERT_DOUBLE_EQUAL(time, 2.0, "second is not 2");

    e = pop_first();
    time = e->time;
    free(e);
    ASSERT_DOUBLE_EQUAL(time, 3.0, "last is not 3");

    e = pop_first();
    time = e->time;
    free(e);
    ASSERT_DOUBLE_EQUAL(time, 4.0, "last is not 4");


    SUCCESS;
}

// insert three events unsorted and sorts them. Finally, removes everything and checks ordering
int sort_test(test_count *t) {
    double time;

    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.5);
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.2);
    insert_first(test_next_event_id++, PRIMO, ARRIVAL, 0, 0.7);

    sort_by_time();

    event *e = pop_first();
    time = e->time;
    free(e);
    ASSERT_DOUBLE_EQUAL(time, 0.2, "first is not 0.2");

    e = pop_first();
    time = e->time;
    free(e);
    ASSERT_DOUBLE_EQUAL(time, 0.5, "second is not 0.5");

    e = pop_first();
    time = e->time;
    free(e);
    ASSERT_DOUBLE_EQUAL(time, 0.7, "last is not 0.7");

    SUCCESS;
}
