#include "../utils/structs.h"

int findBusyServer(server **multi_servers, int num);
int requestIdleServer(block *block);
int areThereMoreServers(block **blocks);
void initServers(block *block, int num);
void freeBusyServer(block *block, int server_index);
int getBusyServersNumber(block *block);
char * getServerContents(block *block);
