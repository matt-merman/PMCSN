#include "../utils/structs.h"
#include "../utils/helpers.h"

void    initEventList(clock* system_clock);
int	    getNextEventIndex();
void	insertEvent(event *elem);
event*	createEvent(block_type target, event_type type, double time);
double 	createAndInsertEvent(block_type target, event_type eventType, clock *c);

void    initClock(clock *system_clock, double p);
int     tryTerminateClock(clock *system_clock, double time);
int     isClockTerminated();
event*  getEvent(int eventIndex);
void    clearEvent(int eventIndex);
int     areThereMoreEvents();