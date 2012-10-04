#include "failure_detector.h"

#define ADMISSION_CONTACT_IP "127.0.0.1"	//This is not decided yet
#define ADMISSION_CONTACT_PORT 1101		//This is not decided yet

struct Head_Node *server_topology;
struct Node* myself;
int topology_version;
char myIP[16];

extern pthread_mutex_t state_machine_mutex;
extern state_machine current_state;

pthread_t send_thread, receive_thread, listen_thread;

int node_init() {
	server_topology = NULL;
	myself = NULL;
	topology_version = 0;
	struct Node* node;
	RC_t rc;

	//First talk to the master and get the topology info.
	
	if ( getIPAddr() != RC_SUCCESS) {
		LOG(ERROR, "Failed to get my IP address");	
	}
	
	if(get_topology() == RC_SUCCESS) {
		LOG(INFO, "Get topology successful\n");
		printf("Get topology successful\n");

		if ( join_topology() == RC_SUCCESS ) {

			//At this point, I've joined the topology
			//Create sending and receiving threads and TCP listening thread

			pthread_create(&send_thread, NULL, heartbeat_send, (void*)0);
			pthread_create(&receive_thread, NULL, heartbeat_receive, (void*)0);
			pthread_create(&listen_thread, NULL, topology_update, (void*)0);
	
			//I still need to tell others, if there are any, that I've joined. I've told to prev and next in the join_topology()
			node = myself->next->next;
			
			//TODO: This algo might require a change

			for	(node = myself->next->next ;\
				(node != myself->prev) && (node != myself) && (node != myself->next); \
				node = node->next) {

				if( tell(node) /*that I've joined*/ != RC_SUCCESS ) {
					LOG(ERROR, "Failed to convey topology to node %s", node->IP);
				}
			}
	
			//pthread_join(send_thread, NULL);
			//pthread_join(receive_thread, NULL);
			//pthread_join(listen_thread, NULL);
		}
	}
	
	if (current_state == INIT) {
		current_state = TOPOLOGY_FORMED;
	} else {
		LOG(ERROR, "State is other than INIT. Can't be\n");
	}

	return RC_SUCCESS;
}


/*
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
*/
