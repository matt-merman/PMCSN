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

event * new_event(long id, block_type b, double time, event_type t, int target_serv, long linked_event, event *next){
    event* e = malloc(sizeof(event));

    e->event_id = id;
    e->block_type = b;
    e->time = time;
    e->event_type = t;
    e->target_server = target_serv;
    e->linked_event_id = linked_event;
    e->next = next;

    return e;
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

void insert_event_ordered(event *elem){
    if (is_empty()){
        // adding as first element
        head = elem;
        size++;
        return;
    }
    event *current = head;
    int len = event_list_length();
    event *prev_node = NULL;
    for (int i = 0; i < len; i++) {
        if (i == 0 && elem->time <= current->time){
            // adding as first element
            prev_node = head;
            head = elem;
            elem->next = prev_node;
            size++;
            return;
        }

        if (current->next == NULL){
            // adding as last element
            current->next = elem;
            size++;
            return;
        }

        if (elem->time > current->time && elem->time <= current->next->time){
            // adding in the middle
            prev_node = current;
            elem->next = prev_node->next;
            prev_node->next = elem;
            size++;
            return;
        }


        current = current->next;
    }
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

int event_list_length() {
   return size;
}

