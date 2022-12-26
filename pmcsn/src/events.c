#include "events.h"
#include "event_list.h"

// true if Clock is terminated
int		terminated = FALSE;
// event	*event_list;
// used to track events
long next_event_id = 0L;

void	init_event_list(int type)
{
    event *e = create_event(type, -1, ARRIVAL, START);
    insert_event_first(e);
}

event	*create_event(block_type target, int server_id, event_type type, double current)
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
int	try_terminate_clock(clock *c, double time)
{
	if (time > PERIOD)
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
double	create_insert_event(block_type target, int server_id, event_type eventType, clock *c)
{
	event	*e;

	e = create_event(target, server_id, eventType, c->current);
	// if we have a new outside arrival but in a time after the observation period,
	//we skip it.
	if (eventType == ARRIVAL && try_terminate_clock(c, e->time))
	{
		free(e);
		return 0.0;
	}
	insert_event_first(e);
	return e->time;
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