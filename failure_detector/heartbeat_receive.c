#include "heartbeat_receive.h"

extern struct Head_Node *server_topology;
extern pthread_mutex_t node_list_mutex;
extern struct Node* myself;
extern int topology_version;
extern neighbourHeartbeat savedHeartbeat[NUM_HEARTBEAT_NEIGHBOURS];
#define HEARTBEAT_RECV_PORT 1103 //This is not decided yet

void* heartbeat_receive(void* t) {

	//I need to have access to the topology
	//I am expecting heartbeat from my predecessor
	//I wait for sometime to check if I received heartbeat
	//If I don't get what I want, I am supposed to tell everyone
	
	int my_version = 0;
	struct Node* recvFromNode = NULL;
	struct Node* nodePtr;
	int recvFromSocket;
	struct sockaddr_in recvFromAddr, myAddr;
	unsigned ttl = 4;
	char recvFromNodeID[20];
	uint32_t ntohltimestamp;
	payloadBuf *packet = (payloadBuf*)malloc(1000);
	uint32_t packetLen;
	struct pollfd pollfds[1];
	int rv;
	int heartbeatNotReceived;
	ssize_t recvFromAddrLen;
			
	char *IPList, *ptr;
	int numNodesToSend, i, j;
		

	recvFromSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);		
	memset(&recvFromAddr, 0, sizeof(recvFromAddr));
	myAddr.sin_family	= AF_INET;
	myAddr.sin_port  	= htons(HEARTBEAT_RECV_PORT);
	myAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	
	pollfds[0].fd = recvFromSocket;
	pollfds[0].events = POLLIN | POLLPRI;
	
	
	while(1) {
		//I may have to start receiving from a new node because... 
		//1) it was just added to the topology as my predecessor
		//2) my predecessor went down and is not communicating. 
	        printf ("\n\nIn hearbeat here \n\n");	
		if( my_version < topology_version ) {
			my_version = topology_version;
			if( (recvFromNode == NULL) || strcmp(recvFromNode->IP, myself->prev->IP) ) {
				if(recvFromNode != NULL) 
					free(recvFromNode);
				
				recvFromNode = init_node(myself->prev->IP);
				ntohltimestamp = ntohl(recvFromNode->timestamp);
				memset(recvFromNodeID, 0, 20);
				ptr = &recvFromNodeID[0];
				memcpy(ptr, &ntohltimestamp,32);
				ptr = &recvFromNodeID[4];
				memcpy(ptr, recvFromNode->IP,16);
			}
		}
		
		//We can use the poll() function here. Nice thing about it is, it will wake up 
		//either when a heartbeat arrives or when a timeout has occurred. Exactly what we need here. 
		
		rv = poll(pollfds, 1, 2000);
		printf("Awake from POLL rv = %d\n", rv);
		heartbeatNotReceived = 0;
	
		if(rv == -1) { //timeout has occurred. I did not receive 5 consecutive heartbeats from my recvfrom node.
			heartbeatNotReceived = 1;
		} else if (pollfds[0].revents & (POLLIN | POLLPRI)){ //Something received. Check if it is heartbeat.
			
			if(RC_SUCCESS == message_decode_UDP(recvFromSocket, packet, &packetLen, &recvFromAddr, &recvFromAddrLen)) {
				processPacket(recvFromSocket, packet);
			}
			
			if((time(NULL) - savedHeartbeat[0].latestTimeStamp) > 2) {
				heartbeatNotReceived = 1;
			}	
		}
		
		if(heartbeatNotReceived) {
			sendDeleteNotification(NODE_FAILURE, recvFromNodeID, ttl);	
			pthread_mutex_lock(&node_list_mutex);
			remove_from_list(&server_topology, recvFromNode->IP);
			pthread_mutex_unlock(&node_list_mutex);
		}
	}
}
