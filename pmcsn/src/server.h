#include "../utils/structs.h"

int findBusyServer(server **multi_servers, int num);
int findIdleServer(server **multi_servers, int num);
int areThereMoreServers(block **blocks);
void initServers(block *blocks, int num);
int getBusyServers(server **multi_servers, int num);
