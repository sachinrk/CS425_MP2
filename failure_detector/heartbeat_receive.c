#include "heartbeat_receive.h"

extern struct Head_Node *server_topology;
extern struct Node* myself;
extern int topology_version;

#define HEARTBEAT_RECV_PORT 1103 //This is not decided yet

void* heartbeat_receive(void* t) {

	//I need to have access to the topology
	//I am expecting heartbeat from my predecessor
	//I wait for sometime to check if I received heartbeat
	//If I don't get what I want, I am supposed to tell everyone
	
	int my_version = 0;
	struct Node* recvFromNode = NULL;
	int recvFromSocket;
	struct sockaddr_in recvFromAddr, myAddr;

	struct pollfd pollfds[1];
	int rv;
		

	recvFromSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);		
	memset(&recvFromAddr, 0, sizeof(recvFromAddr));
	myAddr.sin_family	= AF_INET;
	myAddr.sin_port  	= htons(HEARTBEAT_RECV_PORT);
	myAddr.sin_addr.s_addr	= htnl(INADDR_ANY);
	
	pollfds[0].fd = recvFromSocket;
	pollfds[0].events = POLLIN | POLLPRI;
	
	
	while(1) {
		//I may have to start receiving from a new node because... 
		//1) it was just added to the topology as my predecessor
		//2) my predecessor went down and is not communicating. 
		
		if( my_version < topology_version ) {
			my_version = topology_version;
			if( (recvFromNode == NULL) || strcmp(recvFromNode->IP, myself->prev->IP) ) {
				//I need to recv from a new node now.
				if(recvFromNode != NULL) 
					free(recvFromNode);
				
				recvFromNode = init_node(myself->prev->IP);
			}
		}
		
		//We can use the poll() function here. Nice thing about it is, it will wake up 
		//either when a heartbeat arrives or when a timeout has occurred. Exactly what we need here. 
		
		rv = poll(pollfds, 1, 2000);
		
		if(rv == -1) { //timeout has occurred. I did not receive 5 consecutive heartbeats from my recvfrom node.
			//1) Tell the members that my predecessor is dead
			//TODO
	
			//2) Delete the recvFromNode from the topology I maintain. 
			//Grab mutex
			remove_from_list(&server_topology, recvFromNode->IP);
			//Relese mutex
			
			//3) Update the topology_version. my_version will catch up.
			topology_version++;

		} else if (pollfds[0].revents & (POLLIN | POLLPRI)){
			//Heartbeat received. Now do a recvfrom and check 
			//if the heartbeat is from the recvfrom node or someone else. 
			//This can happen briefly during transitioning. It shoud NOT happen continuously for a long time.
			
			//TODO
			//recvfrom();
		} else {
			LOG(ERROR, "I shouldn't be here.\n");
		}	
	}
}
