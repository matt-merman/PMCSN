#include "events.h"

// true if Clock is terminated
int terminated = FALSE;
// this list contains all event to process
event	eventList[N];

void 	initEventList(clock* system_clock){
	for (int i = 0; i < N; i++)
	{
		eventList[i].time = -1;
	}
	//add the first ARRIVAL to the eventList
	eventList[0].blockType = system_clock->type;
	eventList[0].type = ARRIVAL;
	eventList[0].time = getArrival(START, LAMBDA);
	printf("First arrival: %lf\n", eventList[0].time);
}

// Scans the eventList to find the minimum time at which the next event occurs
// If an element is -1, will be skipped
// If all elements are -1, the first one will be returned.
int		getNextEventIndex()
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
void	insertEvent(event *elem)
{
	for (int i = 0; i < N; i++)
	{
		if (eventList[i].time == -1)
		{
			eventList[i].time = elem->time;
			eventList[i].blockType = elem->blockType;
			eventList[i].type = elem->type;
			return ;
		}
	}
	printf("WARNING: event list is full! Event is lost!");
}

char * to_str(block_type btype){
	switch (btype){
		case PRIMO:
			return "PRIMO";
		case SECONDO:
			return "SECONDO";
		case DESSERT:
			return "DESSERT";
		case CASSA_FAST:
			return "CASSA_FAST";
		case CASSA_STD:
			return "CASSA_STD";
		case CONSUMAZIONE:
			return "CONSUMAZIONE";
		default:
			return "";
	}
}

event	*createEvent(block_type target, event_type type, double time)
{
	// char* source_str = to_str(source); 
	// char* target_str = to_str(target);
	
	event *newEvent = (event*)malloc(sizeof(event));
	if (newEvent == NULL)
	{
		printf("Error while allocating event");
		return newEvent;
	}
	newEvent->type = type;
	if (type == ARRIVAL){
		newEvent->blockType = target;
		newEvent->time = getArrival(time, LAMBDA);
		//printf("New outside arrival to block %s. Time: %lf\n", target_str, newEvent->time);
	} else if(type == IMMEDIATE_ARRIVAL){
		newEvent->blockType = target;
		newEvent->time = time;
		//printf("New arrival from block %s to block %s. Time: %lf s\n", source_str, target_str, newEvent->time);
	} else{
		newEvent->blockType = target;
		int stream = target + 1; 
		newEvent->time = time + getService(target, stream);
		//printf("New completion from block %s to %s. Time: %lf s\n", source_str, target_str, newEvent->time);
	}
	return newEvent;
}

// initialize the system clock
void 	initClock(clock *system_clock, double p)
{
	system_clock->current = START;
	printf("Lambda: %lf\n", LAMBDA);
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
int tryTerminateClock(clock *c, double time)
{
	if (time > PERIODO)
	{
		c->last = c->current;
		c->arrival = INF; // the next arrival will arrive at infinity
		terminated = TRUE;
		return TRUE;
	}
	return FALSE;
}

int isClockTerminated()
{
	return terminated;
}

// creates an event, check if it's time is after the observation period and inserts it in the list.
double 	createAndInsertEvent(block_type target, event_type eventType, clock *c)
{
	event * e = createEvent(target, eventType, c->current);
	// if we have a new outside arrival but in a time after the observation period, we skip it.
	if(eventType == ARRIVAL && tryTerminateClock(c, e->time)){
		free(e);
		return 0;
	}
	insertEvent(e);
	// return value needed in multi-server blocks
	//return (e->time - c->current); 
	return e->time; 

}

event* getEvent(int eventIndex)
{
	if (!(eventIndex >= 0 && eventIndex < N))
	{
		printf("Warning: Event for index %d is null\n", eventIndex);
		return NULL;
	}
	return &eventList[eventIndex];
}

void clearEvent(int eventIndex)
{
	eventList[eventIndex].time = -1;
}

int areThereMoreEvents()
{
	
	for (int i = 0; i < BLOCKS; i++){
		if (eventList[i].time!=-1){
			return TRUE;
		}
	}

	return FALSE;
}
