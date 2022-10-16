#include "events.h"

void 	initEventList(clock* system_clock, event eventList[]){
	for (int i = 0; i < N; i++)
	{
		eventList[i].time = -1;
	}
	//add the first ARRIVAL to the eventList
	eventList[0].blockType = system_clock->type;
	eventList[0].type = ARRIVAL;
	eventList[0].time = getArrival(START, LAMBDA);
}

// Scans the eventList to find the minimum time at which the next event occurs
// If an element is -1, will be skipped
// If all elements are -1, the first one will be returned.
int		getNextEventIndex(event eventList[])
{
	int index = 0;
	double min = eventList[index].time;
	for (int i = 0; i < N; i++)
	{
		if (min < 0) // found -1
		{
			index++; // go ahead and get next element time
			min = eventList[index].time;
			continue ;
		}
		if (eventList[i].time < min && eventList[i].time > 0) // we have a new minimum time
		{
			min = eventList[i].time; // update minimum and index
			index = i;
		}
	}

	return index; // the index of the minimum time in the list.
}

// adds an event in the first free spot in the list
// if the list is full, nothing will be inserted
void	insertEvent(event list[], event *elem, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (list[i].time == -1)
		{
			list[i].time = elem->time;
			list[i].blockType = elem->blockType;
			list[i].type = elem->type;
			return ;
		}
	}
	printf("WARNING: event list is full! Event is lost!");
}

event	*createEvent(block_type blockType, event_type type, double time)
{
	event *newEvent = (event*)malloc(sizeof(event));
	if (newEvent == NULL)
	{
		printf("Error while allocating event");
		return newEvent;
	}
	newEvent->blockType = blockType;
	newEvent->type = type;
	if (type == ARRIVAL)
		newEvent->time = getArrival(time, LAMBDA);
	else if(type == IMMEDIATE_ARRIVAL)
		newEvent->time = time;
	else
		newEvent->time = time + getService(blockType, blockType+1);
	 
	return newEvent;
}

// initialize the system clock
void 	initClock(clock *system_clock, double p)
{
	system_clock->current = START;
	system_clock->arrival = getArrival(START, LAMBDA); // generate the first arrival from outside
	// printf("First arrival: %lf\n", system_clock->arrival);
	// The first job arrived goes into "first course"
	if (p < P_PRIMO_FUORI)
		system_clock->type = 0;
	// Otherwise first job arrived goes into "second course"
	else
		system_clock->type = 1;

	system_clock->completion = INF;
}

// if time of event is greater than termination period, we stop the arrival flow.
int checkClockTermination(clock *c, event *e)
{
	if (e->time > PERIODO)
	{
		c->last = c->current;
		c->arrival = INF; // the next arrival will arrive at infinity
		return TRUE;
	}
	return FALSE;
}
// creates an event, check if it's time is after the observation period and inserts it in the list.
void 	createAndInsertEvent(event list[], int len, block_type blockType, event_type eventType, clock *c)
{
	event * e = createEvent(blockType, eventType, c->current);
	if(checkClockTermination(c, e)){
		free(e);
		return;
	}
	insertEvent(list, e, len);
}

