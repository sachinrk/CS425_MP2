#include "heartbeat_send.h"

extern struct Head_Node *server_topology;
extern struct Node* myself;
extern int topology_version;

#define HEARTBEAT_SEND_PORT 1102 //This is not decided yet

void* heartbeat_send(void* t) {
	//I need to have access to the topology
	//I need to select the person I am supposed to send the heartbeats to
	//I need to send the heartbeat

	int my_version = 0;
	struct Node* sendToNode = NULL;
	int sendToSocket;
	struct sockaddr_in sendToAddr;
	heartbeatPayload *hbPayload = (heartbeatPayload*)malloc(sizeof(heartbeatPayload));
	
	sendToSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);		

	while(1) {
		//if topology has changed, it might be that I need to now send to some other node.
		
		if( my_version < topology_version ) {
			my_version = topology_version;
			if( (sendToNode == NULL) || strcmp(sendToNode->IP, myself->next->IP) ) {
				//I need to send to a new node now.
				if(sendToNode != NULL) 
					free(sendToNode);
				
				sendToNode = init_node(myself->next->IP);
				memset(&sendToAddr, 0, sizeof(sendToAddr));
				sendToAddr.sin_family 		= AF_INET;
				sendToAddr.sin_port   		= htons(HEARTBEAT_SEND_PORT);
				sendToAddr.sin_addr.s_addr	= inet_addr(sendToNode->IP);
			}		
		}
		
		//send the heartbeat from here every 400 msec
		//TODO add a function to actually send the heartbeat here
		strcpy(hbPayload->ip_addr, myself->IP);
		sendPayloadUDP(sendToSocket,MSG_HEARTBEAT,hbPayload,sizeof(hbPayload),&sendToAddr);
		usleep(400 * 1000); 	
	}
	pthread_exit(NULL);	
}
