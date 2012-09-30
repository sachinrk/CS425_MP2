#ifndef __NODE_PROCESS_PAYLOAD_H__
#define __NODE_PROCESS_PAYLOAD_H__
#include "node_data_structures.h"
#include "message_type.h"
#include "list.h"
#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include "../node/node_send_info.h"

extern neighbourHeartbeat savedHeartbeat[NUM_HEARBEAT_NEIGHBOURS]; 
void processHeartbeat(char *ipAddr);
void sendTopologyResponse(int socket, int numOfNodes, char *buf);
void processHeartbeatPayload(heartbeatPayload *payload) 
void processNodeAddDeletePayload(addDeleteNodePayload *payload) 

#endif
