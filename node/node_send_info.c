#include "node_send_info.h"
extern char myIP[16];

void* send_node_update_payload(void *tdata)
{
   
    
        thread_data *my_data = (thread_data *)tdata;
        struct sockaddr_in nodeAddress;
        char *IP = my_data->ip;
        int sock;
        int rc = RC_FAILURE;
        int retryCount = -1;
        int size = my_data->payload_size;
        int msg_type = my_data->msg_type;
 
        char *payload = my_data->payload;
        memset(&nodeAddress, 0, sizeof(nodeAddress));
        nodeAddress.sin_family        = AF_INET;
        nodeAddress.sin_addr.s_addr   = inet_addr(IP);
        nodeAddress.sin_port          = htons(TCP_LISTEN_PORT);

        if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                //LOG(ERROR, "IP : %s Unable to create TCP Socket. Dying...\n", IP);
                printf("IP : %s Unable to create TCP Socket. Dying...\n", IP);
                free(my_data->payload);
                free(my_data);
                pthread_exit(NULL);
        }
        printf("IP : %s Unable to create TCP Socket. Dying...\n", IP);
        if((connect(sock, (struct sockaddr *) &nodeAddress,   sizeof(nodeAddress))) < 0) { 
                //LOG(ERROR, "IP : %s Unable to connect with server %s . Dying ...\n", IP);
                free(my_data->payload);
                free(my_data);
                pthread_exit(NULL);
        }
        DEBUG(("IP :  %s  Connection Established\n\n", IP));
        //We are now connected. Let the send receive begin...
        sendPayload(sock, msg_type, payload, size);
        free(my_data->payload);
        free(my_data);
        close(sock);     
}
