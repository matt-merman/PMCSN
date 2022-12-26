//
// Created by giaco on 24/12/22.
//
#include "event_list.h"

event *head = NULL;
int size = 0;

//display the list
void print_list() {
    event *e = head;
    printf("\n[ ");

    //start from the beginning
    while(e != NULL) {
        print_event(e);
        e = e->next;
    }

    printf("]\n");
}

void print_event(event *e){
    printf("E%ld(%d,%d,%d,%f) ",e->event_id, e->block_type, e->target_server, e->event_type, e->time);
}

// insert event at the first location. The head goes in 2nd position.
void insert_first(long event_id, block_type block_type, event_type event_type, int target_server, double time) {
    // create a link
    event *link = (event*) malloc(sizeof(event));

    link->event_id = event_id;
    link->event_type = event_type;
    link->block_type = block_type;
    link->target_server = target_server;
    link->time = time;

    // point it to old first node
    link->next = head;

    // point first to new first node
    head = link;

    // increment size
    size++;
}

// inserts an event in the 1st position
void insert_event_first(event *elem){
    if (is_empty()) {
        head = elem;
        size++;
        return;
    }

    elem->next = head;
    head = elem;
    size++;
}

// delete first item and returns it to the caller.
event* pop_first() {

    // save reference to first link
    event *tempLink = head;

    // mark next to first link as first
    head = head->next;
    // decrement size
    size--;

    // return the deleted link
    return tempLink;
}

// is list empty
bool is_empty() {
    return size == 0;
}

int length() {
   return size;
}

// first has minimum time, i.e. is the next event
void sort_by_time() {

    int i, j, k, tempTargetServer;
    long tempEventId;
    block_type tempBlockType;
    event_type tempEventType;
    double tempTime;
    event *current_event;
    event *next;

    k = size ;
    for ( i = 0 ; i < size - 1 ; i++, k-- ) {
        current_event = head;
        next = head->next;

        for ( j = 1 ; j < k ; j++ ) {

            if (current_event->time > next->time ) {
                // Swap all data
                tempEventId = current_event->event_id;
                current_event->event_id = next->event_id;
                next->event_id = tempEventId;

                tempTime = current_event->time;
                current_event->time = next->time;
                next->time = tempTime;

                tempTargetServer = current_event->target_server;
                current_event->target_server = next->target_server;
                next->target_server = tempTargetServer;

                tempBlockType = current_event->block_type;
                current_event->block_type = next->block_type;
                next->block_type = tempBlockType;

                tempEventType = current_event->event_type;
                current_event->event_type = next->event_type;
                next->event_type = tempEventType;

            }

            current_event = current_event->next;
            next = next->next;
        }
    }
}
