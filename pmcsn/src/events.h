#include "../utils/structs.h"
#include "../utils/helpers.h"

void    initEventList(clock* system_clock, event eventList[]);
int	    getNextEventIndex(event eventList[]);
void	insertEvent(event list[], event *elem, int len);
event*	createEvent(block_type blockType, event_type type, double time);
void 	createAndInsertEvent(event list[], int len, block_type blockType, event_type eventType, clock *c);

void    initClock(clock *system_clock, double p);
int    checkClockTermination(clock *system_clock, event *e);