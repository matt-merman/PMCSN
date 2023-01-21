#ifndef PROGETTOPMCSN_EVENT_H
#define PROGETTOPMCSN_EVENT_H

#include "../utils/helpers.h"
#include "event_list.h"

void init_event_list(block_type type);

void insert_event(event *elem);
event *create_event(block_type target, int server_id, event_type type, double current, event *linked_event);
int try_terminate_clock(timer *system_clock, double time, long int period);
int is_clock_terminated();
event *get_next_event();
void remove_event();
int are_there_more_events();

void sort_list();
char *to_str_event(event_type etype);
event *create_insert_event(block_type target, int server_id, event_type eventType, timer *c, event *linked_event);

#endif
