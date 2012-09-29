#include "udp_server.h"

//This is expected to send and receive heartbeats

#define MASTER_IP "127.0.0.1"
#define MASTER_PORT 1101

void* heartbeat_send(void* t);
void* heartbeat_receive(void* t);
int getMyIPAddrs(char[10][16]);
int form_topology();

struct Head_Node *server_topology;
struct Node* myself;
int topology_version;
pthread_mutex_t top_mutex;

int main() {
	pthread_t send_thread, receive_thread;
	server_topology = NULL;
	myself = NULL;
	topology_version = 0;

	//First talk to the master and get the topology info.
	
	if(form_topology() == RC_SUCCESS) {
		//Create sending and receiving threads
		
		pthread_create(&send_thread, NULL, heartbeat_send, (void*)0);
		pthread_create(&receive_thread, NULL, heartbeat_receive, (void*)0);
		
		pthread_join(send_thread, NULL);
		pthread_join(receive_thread, NULL);
	
	}
	return 0;
}

RC_t form_topology() {	
	struct sockaddr_in master;
	struct Node* node;
	int mSocket, numIPs;
	char myIPs[10][16];

	if((mSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Unable to create socket. Dying...\n");
		exit(0);
	}
	
	memset(&master, 0, sizeof(master));
	master.sin_family 	= AF_INET;
	master.sin_addr.s_addr 	= inet_addr(MASTER_IP);
	master.sin_port		= htons(MASTER_PORT);

	//First get my IPAddress so that we can get the pointer to it
	numIPs = getMyIPAddrs(myIPs);
	
	if((connect(mSocket, (struct sockaddr *)&master, sizeof(master))) < 0) {
		printf("Unable to connect with the Master. Dying...\n");
	}
	
	//TODO	
	//Tell the master I want to join the topology
	
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
		LOG(ERROR, "My IP not found in server topology\n");
		return RC_FAILURE;	
	}
	//By this time, topology is formed and is present in the server_topology pointer
	//Any changes in the topology will cause a change in the version number of the topology
	
	return RC_SUCCESS;
}

void heartbeat_send(void* t) {
	//I need to have access to the topology
	//I need to select the person I am supposed to send the heartbeats to
	//I need to send the heartbeat

	int my_version = 0;
	struct Node* sendToNode = NULL;
	int sendToSocket;
	struct sockaddr_in sendToAddr;
	
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
				memset((char*)sendToAddr, 0, sizeof(sendToAddr));
				sendToAddr.sin_family 		= AF_INET;
				sendToAddr.sin_port   		= htons(HEARTBEAT_SEND_PORT);
				sendToAddr.sin_addr.s_addr	= inet_addr(sendToNode->IP);
			}		
		}
		
		//send the heartbeat from here every 400 msec
		usleep(400 * 1000); 	
	}
	
}

void heartbeat_receive(void* t) {

	//I need to have access to the topology
	//I am expecting heartbeat from my predecessor
	//I wait for sometime to check if I received heartbeat
	//If I don't get what I want, I am supposed to tell everyone
	
	int my_version = 0;
	struct Node* recvFromNode = NULL;
	int recvFromSocket;
	struct sockaddr_in recvFromAddr, myAddr;

	struct pollfds[1];
	int rv;
		

	recvFromSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);		
	memset((char*)recvFromAddr, 0, sizeof(recvFromAddr));
	myAddr.sin_family	= AF_INET;
	myAddr.sin_port  	= htons(HEARTBEAT_RECV_PORT);
	myAddr.sin_addr.s_addr	= htnl(INADDR_ANY);
	
	pollfd[0].fd = recvFromSocket;
	pollfd[0].events = POLLIN | POLLPRI;
	
	
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
			recvfrom();
		} else {
			LOG(ERROR, "I shouldn't be here.\n");
		}	
	}
}

int getMyIPAddrs(char myIPs[10][16]) {

	int s;
	struct ifconf ifconf;
	struct ifreq ifr[50];
	int ifs;
	int i;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		return 0;
	}

	ifconf.ifc_buf = (char *) ifr;
	ifconf.ifc_len = sizeof(ifr);

	if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) {
		perror("ioctl");
		return 0;
	}

	ifs = ifconf.ifc_len / sizeof(ifr[0]);
	//printf("interfaces = %d:\n", ifs);
	for (i = 0; i < ifs; i++) {
		char ip[INET_ADDRSTRLEN];
		struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;

 		if (!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip))) {
 			perror("inet_ntop");
      			return 0;
    		}
		
		strcpy(myIPs[i], ip);
  	}

	close(s);

	return ifs;
}
