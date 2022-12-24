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
void    insert_first(block_type block_type, event_type event_type, int target_server, double time);
void    insert_event_first(event *elem);
event*  pop_first();
bool    is_empty();
int     length();
void    sort_by_time();

#endif //PROGETTOPMCSN_EVENT_LIST_H
