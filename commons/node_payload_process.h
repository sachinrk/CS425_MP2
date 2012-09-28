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


extern neighbourHeartbeat savedHeartbeat[NUM_HEARBEAT_NEIGHBOURS]; 
void processHeartbeat(char *ipAddr);
#endif
