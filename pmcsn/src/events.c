#include "events.h"

// true if Clock is terminated
int		terminated = FALSE;
event	*event_list;

void	init_event_list(int type)
{
	event_list = create_event(type, -1, ARRIVAL, START);
}

void	insert_event(event *elem)
{
	event	*current;

	if (event_list == NULL)
	{
		event_list = elem;
		return ;
	}
	current = event_list;
	while (current->next != NULL)
		current = current->next;
	current->next = elem;
}

event	*create_event(block_type target, int server_id, event_type type, double current)
{
	int		stream;
	event	*e;

	e = (event *)malloc(sizeof(event));
	if (e == NULL)
	{
		printf("Error while allocating event");
		return (e);
	}
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
		c->arrival = INF; // the next arrival will arrive at infinity
		terminated = TRUE;
		return (TRUE);
	}
	return (FALSE);
}

int	check_clock(void)
{
	return (terminated);
}

double	create_insert_event(block_type target, int server_id, event_type eventType, clock *c)
{
	event	*e;

	e = create_event(target, server_id, eventType, c->current);
	// if we have a new outside arrival but in a time after the observation period,
	//we skip it.
	if (eventType == ARRIVAL && try_terminate_clock(c, e->time))
	{
		free(e);
		return 0;
	}
	insert_event(e);
	return e->time;
}

void	remove_event()
{
	event	*new_first;

	new_first = event_list->next;
	free(event_list);
	event_list = new_first;
}

event	*get_event()
{
	return event_list;
}

int	check_events()
{
	if (event_list == NULL)
		return FALSE;
	return TRUE;
}

int	get_len()
{
	int		length;
	event	*current;

	length = 0;
	for (current = event_list; current != NULL; current = current->next)
	{
		length++;
	}
	return length;
}

void	sort_list()
{
	int k;
	event *current, *next;

	block_type t_block_type;
	double t_time;
	event_type t_type;
	int t_target_server;

	int size = get_len();
	k = size;

	for (int i = 0; i < size - 1; i++, k--)
	{
		current = event_list;
		next = event_list->next;

		for (int j = 1; j < k; j++)
		{
			if (current->time > next->time)
			{
				t_time = current->time;
				current->time = next->time;
				next->time = t_time;

				t_target_server = current->target_server;
				current->target_server = next->target_server;
				next->target_server = t_target_server;

				t_block_type = current->block_type;
				current->block_type = next->block_type;
				next->block_type = t_block_type;

				t_type = current->event_type;
				current->event_type = next->event_type;
				next->event_type = t_type;
			}
			current = current->next;
			next = next->next;
		}
	}
}

// char	*to_str_event(event_type etype)
// {
// 	switch (etype)
// 	{
// 	case ARRIVAL:
// 		return ("ARRIVAL");
// 	case IMMEDIATE_ARRIVAL:
// 		return ("IMMEDIATE_ARRIVAL");
// 	case COMPLETION:
// 		return ("COMPLETION");
// 	default:
// 		return ("");
// 	}
// }