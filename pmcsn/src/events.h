#include "../utils/helpers.h"

void init_event_list(int type);

void insert_event(event *elem);
event *create_event(block_type target, int server_id, event_type type,
                    double time);
int try_terminate_clock(clock *system_clock, double time);
int check_clock();
event *get_event();
void remove_event();
int check_events();
int get_len();
void sort_list();
// char *to_str_event(event_type etype);
double create_insert_event(block_type target, int server_id,
                           event_type eventType, clock *c);
