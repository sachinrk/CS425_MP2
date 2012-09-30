#include "admission_contact.h"

#define IP_FILE_PATH "./IPs"

/*
//Iterate over IPs file and get the topology
int main() {

	struct Head_Node *topology = NULL;
	struct Node *node = NULL;
	FILE *fp = NULL;
	struct stat file_stat;
	int version = 0;
	time_t last_mtime = 0;
	
	while(1) {
		
		if(stat(IP_FILE_PATH, &file_stat) != -1) {
			if(last_mtime <= file_stat.st_mtime) {
				fp = fopen(IP_FILE_PATH, "r");
				(void)parse_IPs(fp, &topology, &version);
				fclose(fp);
			}
		}
		sleep(1);
	}
	return 0;
}*/


struct Head_Node *server_topology;	

int main()
{
	int listenSocket, connectSocket, socketFlags, ret, clientSize;
	struct sockaddr_in myAddress, clientAddress;
	int i,j, bytes, numBytes, pid;
	struct Head_Node *server_topology;	
	
	payloadBuf *packet;
	int rc;
	

	//Create a listening socket..
	if((listenSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating server socket. Dying ...\n");
		return 0;
	}
	
	//Init the sockaddr structure..
	memset(&myAddress, 0, sizeof(myAddress));
	myAddress.sin_family	  = AF_INET;
	myAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	myAddress.sin_port	  = htons(MY_PORT);
	
	//Now bind the socket..
	if((bind(listenSocket, (struct sockaddr *)&myAddress, sizeof(myAddress))) < 0) {
		printf("Error binding socket. Dying...\n");		
		return 0;
	}
	
	//Listen on the socket for incoming connections..	
	if((listen(listenSocket, LISTEN_Q)) < 0) {
		printf("Error listening on socket. Dying...\n");
		return 0;
	}
	
	for(;;) {
		if ((connectSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &clientSize)) < 0) {
			printf("Error accepting connection. Dying...\n");
			return 0;
		} 
		
		printf("\nClient %d.%d.%d.%d connected\n", 
				(clientAddress.sin_addr.s_addr & 0xFF),  
				(clientAddress.sin_addr.s_addr & 0xFF00) >> 8,  
				(clientAddress.sin_addr.s_addr & 0xFF0000) >> 16,  
				(clientAddress.sin_addr.s_addr & 0xFF000000) >> 24 
			);

		//A client has connected. 
		//It will either request that it wants to join or, some node has left.
		
		}	
				
	}	

