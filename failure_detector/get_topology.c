#include "get_topology.h"

extern struct Head_Node *server_topology;
extern struct Node *myself;
extern int topology_version;
char myIPs[10][16];
char myIP[16];
RC_t get_topology() {	
	struct sockaddr_in master;
	struct Node* node;
	int mSocket, numIPs, nodeFound, i;
	payloadBuf *packet;
	RC_t rc;

	printf("Contacting admission control to join group\n");
	
	if((mSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Unable to create socket. Dying...\n");
		exit(0);
	}
	
	memset(&master, 0, sizeof(master));
	master.sin_family 	= AF_INET;
	master.sin_addr.s_addr 	= inet_addr(ADMISSION_CONTACT_IP);
	master.sin_port		= htons(ADMISSION_CONTACT_PORT);

	//First get my IPAddress so that we can get the pointer to it
	numIPs = getMyIPAddrs(myIPs);
	
	if((connect(mSocket, (struct sockaddr *)&master, sizeof(master))) < 0) {
		printf("Unable to connect with the Master. Dying...\n");
	}
		
	//Tell the master I want to join the topology
	
	sendTopologyJoinRequest(mSocket);
	rc = message_decode(mSocket,&packet);
	processPacket(mSocket, packet);
	
	//process the incoming packet	
			
	
	//Set the topology version received from master.
	//Get a pointer to the node containing my IP. Set it to *myself.
	
	nodeFound = 0;
	node = server_topology->node;
	do {
		for(i=0; i<numIPs; i++) {
			if(!strcmp(node->IP, myIPs[i])) {
				nodeFound = 1;
				break;	
			}
		}
		node = node->next;	
	}while(node != server_topology->node);
	
	if(nodeFound)	
		myself = node;
	else { 
		LOG(ERROR, "My IP not found in server topology%s\n","");
		return RC_FAILURE;	
	}
	//By this time, topology is formed and is present in the server_topology pointer
	//Any changes in the topology will cause a change in the version number of the topology
	
	printf("Received topology from admission control\n");	
	return RC_SUCCESS;
}
