#include "events.h"

// true if Clock is terminated
int		terminated = FALSE;
// event	*event_list;
// used to track events
long next_event_id = 0L;

/**
 * Initializes the event list
 * @param type the type of block to which the event will occur. It can be PRIMO or SECONDO.
 */
void	init_event_list(block_type type)
{
    event *e = create_event(type, -1, ARRIVAL, START, NULL);
    insert_event_first(e);
}

/**
 * Creates an event for the next-event simulation
 * @param target the block type to which the event will occur
 * @param server_id the server id of the block in which the event will be happening
 * @param type the type of event (ARRIVAL, COMPLETION or IMMEDIATE ARRIVAL)
 * @param current the current time
 * @param linked_event the linked event, i.e. the event which triggered the creation of this event
 * @return the newly created event
 */
event *create_event(block_type target, int server_id, event_type type, double current, event *linked_event)
{
	int		stream;
	event	*e;

	e = (event *)malloc(sizeof(event));
	if (e == NULL)
	{
		printf("Error while allocating event\n");
		return (e);
	}
    e->event_id = next_event_id++;
    e->linked_event_id = (linked_event != NULL ? linked_event->event_id : -1L);

	switch (type)
	{
	case ARRIVAL:
		e->time = get_next_arrival(current, LAMBDA);
		e->target_server = -1;
		//printf("New outside arrival to block %s. Time: %lf\n", target_str,
				//newEvent->time);
		break ;
	case IMMEDIATE_ARRIVAL:
		e->time = current;
		e->target_server = -1;
		//printf("New arrival from block %s to block %s. Time: %lf s\n",
				//source_str, target_str, newEvent->time);
		break ;
	case COMPLETION:
		stream = (int) target + 1;
		e->time = current + get_next_service(target, stream);
		e->target_server = server_id;

		//printf("New completion from block %s to %s. Time: %lf s\n",
				//source_str, target_str, newEvent->time);
		break ;
	default:
		break ;
	}
	e->event_type = type;
	e->block_type = target;
	e->next = NULL;
	return (e);
}

// if time of event is greater than termination period,
//we stop the arrival flow.
int	try_terminate_clock(timer *c, double time, long int period)
{	
	if (time > period)
	{
		c->last = c->current;
		c->last_arrival = INF; // the next arrival will arrive at infinity
		terminated = TRUE;
		return (TRUE);
	}
	return (FALSE);
}

// check if clock is terminated
int	is_clock_terminated(void)
{
	return terminated;
}
// creates an event and return its time (the time at which the event will occurr)
// the clock contains the time of this event
event * create_insert_event(block_type target, int server_id, event_type eventType, timer *c, event *linked_event, long int period)
{
	event	*e;

	e = create_event(target, server_id, eventType, c->current, linked_event);
	// if we have a new outside arrival but in a time after the observation period,
	//we skip it.
	if (eventType == ARRIVAL && try_terminate_clock(c, e->time, period))
	{
		free(e);
		return NULL;
	}
	insert_event_ordered(e);
	return e;
}

event	*get_next_event()
{
	return pop_first();
}

// returns true (1) if there are more events to consume
int	are_there_more_events()
{
    // if list is empty, we do not have more events
    // otherwise we have more events
    return !is_empty();
}

void	sort_list()
{
	sort_by_time();
}

 char	*to_str_event(event_type etype)
 {
 	switch (etype)
 	{
 	case ARRIVAL:
 		return ("ARRIVAL");
 	case IMMEDIATE_ARRIVAL:
 		return ("IMMEDIATE_ARRIVAL");
 	case COMPLETION:
 		return ("COMPLETION");
 	default:
 		return ("");
 	}
 }