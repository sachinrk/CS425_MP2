#include "udp_server.h"

//This is expected to send and receive heartbeats

#define MASTER_IP "127.0.0.1"
#define MASTER_PORT 1101


struct Head_Node *server_topology;
struct Node* myself;
int topology_version;

int main() {
	pthread_t send_thread, receive_thread;
	server_topology = NULL;
	myself = NULL;
	topology_version = 0;

	//First talk to the master and get the topology info.
	
	if(form_topology() == RC_SUCCESS) {

		if ( join_topology() == RC_SUCCESS ) {
			//At this point, I've joined the topology
		
		}		
	
		//Create sending and receiving threads
		pthread_create(&send_thread, NULL, heartbeat_send, (void*)0);
		pthread_create(&receive_thread, NULL, heartbeat_receive, (void*)0);
		
		pthread_join(send_thread, NULL);
		pthread_join(receive_thread, NULL);
	
	}
	return 0;
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
