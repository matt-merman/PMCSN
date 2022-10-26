#include "../utils/helpers.h"
#include "../utils/structs.h"

void initEventList(int type);
int getNextEventIndex();
void insertEvent(event *elem);
event *createEvent(block_type target, int server_id, event_type type,
                   double time);
double createAndInsertEvent(block_type target, int server_id,
                            event_type eventType, clock *c);

int tryTerminateClock(clock *system_clock, double time);
int isClockTerminated();
char *toStrBlock(block_type btype);
char *toStrEvent(event_type btype);
event *getEvent(int eventIndex);
void clearEvent(int eventIndex);
int areThereMoreEvents();