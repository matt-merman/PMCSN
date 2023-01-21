//
// Created by giaco on 24/12/22.
//

#ifndef PROGETTOPMCSN_EVENT_LIST_H
#define PROGETTOPMCSN_EVENT_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/structs.h"

void    print_list();
void    print_event(event *e);
event * new_event(long id, block_type b, double time, event_type t, int target_serv, long linked_event, event *next);
void    insert_first(long event_id, block_type block_type, event_type event_type, int target_server, double time);
void    insert_event_first(event *elem);
void    insert_event_ordered(event *elem);
event*  pop_first();
bool    is_empty();
int     event_list_length();

#endif //PROGETTOPMCSN_EVENT_LIST_H
