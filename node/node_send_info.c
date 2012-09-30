#include "node_info.h"

void send_node_update_payload(void *tdata)
{
   
    
        struct thread_data *my_data = (struct thread_data *)tdata;
        struct sockaddr_in nodeAddress;
        char *IP = my_data->ip;
        int sock;
        int rc = FAILURE;
        int retryCount = -1;
        int size = my_data->payload_size;
        int msg_type = my_data->msg_type;
 
        payloadBuf *payload = my_data->payload;
        memset(&nodeAddress, 0, sizeof(nodeAddress));
        serverAddress.sin_family        = AF_INET;
        serverAddress.sin_addr.s_addr   = inet_addr(IP);
        serverAddress.sin_port          = htons(TCP_LISTEN_PORT);

        if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                LOG("IP : %s Unable to create TCP Socket. Dying...\n", IP);
                my_data->statusFlag = (FAILURE | SOCKET_CREATION_FAILURE);
                free(my_data->payload);
                free(tdata);
                pthread_exit(NULL);
        }
        if((connect(sock, (struct sockaddr *) &nodeAddress,   sizeof(nodeAddress))) < 0) { 
                LOG("IP : %s Unable to connect with server %s . Dying ...\n", IP);
                my_data->statusFlag = (FAILURE | SOCKET_CONNECT_FAILURE);
                free(my_data->payload);
                free(tdata);
                pthread_exit(NULL);
        }
        DEBUG(("IP :  %s  Connection Established\n\n", IP));
        //We are now connected. Let the send receive begin...
        sendPayload(sock, msg_type, payload, size);
        //free(my_data->payload);
        free(tdata);
        close(sock);     
}
