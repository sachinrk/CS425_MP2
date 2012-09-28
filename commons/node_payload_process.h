#ifndef __NODE_PROCESS_PAYLOAD_H__
#define __NODE_PROCESS_PAYLOAD_H__
#include "node_data_structures.h"
#include "message_type.h"
extern neighbourHeartbeat savedHeartbeat[NUM_HEARBEAT_NEIGHBOURS]; 
void processHeartbeat(char *ipAddr);
#endif
