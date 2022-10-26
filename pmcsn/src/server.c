#include "../utils/constants.h"
#include "../utils/structs.h"
#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int busy_servers[BLOCKS] = {0,0,0,0,0,0};

int	findBusyServer(server **multi_servers, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (multi_servers[i]->status == BUSY)
			return (i);
	}
	return (-1);
}
// returns the index (starting from 1) of the first idle server and makes it busy
// if are all busy returns -1
int requestIdleServer(block *block)
{
    server** multi_servers = block->servers;
    int i;
    // FIXME: da qualche parte viene deallocato multi_server[2] o forse non viene mai allocato
    for (i = 0; i < block->num_servers; i++) {
        if (multi_servers[i]->status == IDLE) {
            multi_servers[i]->status = BUSY;
            busy_servers[block->type]++;
            return i;
        }
	}
	return -1;
}

void freeBusyServer(block *block, int server_index){
    // Set status to idle!!
    block->servers[server_index]->status = IDLE;
    // Decrement busy servers
    busy_servers[block->type]--;
}

int getBusyServersNumber(block *block)
{
	return busy_servers[block->type];
}

int	areThereMoreServers(block **blocks)
{
	int s;
    for (s = 0; s < BLOCKS; s++)
	{
        if(getBusyServersNumber(blocks[s]) > 0){
            return TRUE;
        }
	}
	return FALSE;
}

char * getServerContents(block *block){
    char *string = (char *) malloc(sizeof(char) * 100);
    int in_service = busy_servers[block->type];
    long queue_minus_service = block->jobs - (long) in_service;
    long in_queue = 0L;
    if (queue_minus_service > 0L){
        in_queue = queue_minus_service;
    }

    sprintf(string, "%s |%ld|(%d)",toStrBlock(block->type), in_queue, busy_servers[block->type]);
    return string;
}
