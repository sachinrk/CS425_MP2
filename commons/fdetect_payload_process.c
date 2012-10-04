#include "fdetect_payload_process.h"

neigbourHeartbeat savedHeartbeat[NUM_HEARTBEAT_NEIGHBOURS];
extern struct Head_Node *server_topology;
extern pthread_mutex_t node_list_mutex;
extern myIP[16];


//nodeData *head = NULL;
//nodeData *thisNode = NULL;
/*********************************************************
** This is the heartbeat process function. 
** It updates the heartbeat timestamp when the heartbeat 
** is received. 
** 
** Arguments:
** ipAddr : IP Address.
***********************************************************/

void processHeartbeatPayload(heartbeatPayload *payload) 
{
    int i; 
    pthread_mutex_lock(&timestamp_mutex);
    for (i = 0; i < NUM_HEARTBEAT_NEIGHBOURS; i++) {
              if (!(strcmp(savedHeartbeat[i].ipAddr, payload->ip_addr))) {
                  time(&savedHeartbeat[i].latestTimeStamp);
       }
    }
    pthread_mutex_unlock(&timestamp_mutex); 
}


/*********************************************************
** This is the node add process function. 
** It adds the node to the current list of nodes. 
** 
** 
** Arguments:
** ipAddr : IP Address.
***********************************************************/

void processNodeAddDeletePayload(addDeleteNodePayload *payload, int payload_size) 
{
    int i;
    thread_data *tdata;
    char *buf;
    char IP[16];
    pthread_t thread;
    pthread_mutex_lock(&node_list_mutex);   
    if ((payload->flags & ADD_PAYLOAD) && (payload->flags & COMPLETE_PAYLOAD)) {
        delete_all_nodes();
    }
    for (i = 0; i < payload->numOfNodes; i++) {
       if (payload->flags & DELETE_PAYLOAD) {
          remove_from_list(&server_topology, payload->ID[i]);
       }else if (payload->flags & ADD_PAYLOAD) {
           add_to_list(&server_topology, payload->ID[i]);   
       }
    }
    memcpy(IP, myself->next->IP, 16); 
    pthread_mutex_unlock(&node_list_mutex); 
    if (payload->ttl > 0) {
        payload->ttl--;
        buf =  my_malloc(payload_size);
        memcpy(buf, payload, payload_size);
        tdata = my_malloc(sizeof(thread_data));
        memcpy(tdata->ip, IP, 16);
        tdata->payload_size = payload_size;
        tdata->payload = buf;
        (void)pthread_create(&thread, NULL, send_node_update_payload, (void*)tdata);
         
    }
}
/*********************************************************
** This is the topology request function. 
** Can be called by both the master and the node willing
** to join the ring
** If the ADD_NODE_REQUEST bit is set, it means that this 
** node wants to join the topology. 
** Arguments:
** 
***********************************************************/

void processTopologyRequest(int socket, topologyRequest *payload) 
{
    int i;
    int offset = 0;
    struct Node* tmp;
    struct Node* found = NULL;
    char *buf;
    char *ipAddrList;
    char ID[48];
    int numNodestoSend = 0;
    buf = (char *)my_malloc(48 * (server_topology->num_of_nodes + 1)); 
    ipAddrList = (char *)my_malloc(16 * num_of_nodes);
    pthread_mutex_lock(&node_list_mutex);
    if (server_topology->node && server_topology->node->prev) {
        memcpy(ipAddrList, server_topology->node->prev, 16);
        offset1 += 16;
    }
    memcpy(ipAddrList, ADMISSION_CONTACT_IP, 16);
    offset1 += 16;
    
    for (tmp = server_topology->node; tmp && tmp->next != server_topology->node; tmp++) {
        if (!memcmp(payload->ipAddr, tmp->IP, 15)) {
            found = tmp;
            
        }
        if (tmp->next != server_topology->node) {
            memcpy(ipAddrList + offset1, tmp->IP, 16);          
        }
        memcpy(buf + offset, htonl(tmp->timestamp), 32);
        memcpy(getIpAddress(buf + offset), (tmp->IP), 16);
        offset += 48;
    }
    if (payload->flags & ADD_NODE_REQUEST) {
        if (found) {
            LOG(ERROR, " Received duplicate node add request from %s ", payload->ipAddr);
            return;
        }
        memcpy(buf + offset, htonl(payload->timestamp), 32);
        memcpy(getIpAddress(buff + offset) , payload->IpAddress, 16);
        offest += 48;
        //memcpy(ID, payload->ipAddr, 15);
        memcpy(ID, htonl(timestamp), 32); 
        memcpy(getIPAddress(ID), payload->ipAddr, 16);
        add_to_list(&server_topology, ID); 
        //strcpy(buf + offset, tmp->IP);
        //server_topology->num_of_nodes++; 
    }
    pthread_mutex_unlock(&node_list_mutex);
    sendTopologyResponse(socket, server_topology->num_of_nodes, buf);  
    if (payload->flags & ADD_NODE_REQUEST) {
        sendAddNodePayload(ipAddrList, server_topology->num_of_nodes-1 , ID);
    }
    free(buf);
}


/*********************************************************
** This is the IP address get function 
** It gets the system's IP address
** 
** 
** Arguments:
** ipAddr : IP Address.
***********************************************************/
RC_t getIpAddr()
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    char addressBuffer[INET_ADDRSTRLEN];
    RC_t rc = RC_FAILURE;
    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (!strncmp(addressBuffer, "127.0.0.1", INET_ADDRSTRLEN)) {
                printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
                strcpy(myIP, addressBuffer);        
                rc = RC_SUCCESS;
                break;
            }
        }     
   }
   if (ifAddrStruct!=NULL) 
       freeifaddrs(ifAddrStruct);
   
   
   return rc;
    
}

/*********************************************************
** This is function used to send request for topology 
** request
** 
** 
** Arguments:
** socket     : socket on which the payload was received.
** numOfNodes : Num of nodes in topology.
** buf        : buffer having IP addresses of nodes.
***********************************************************/
void sendTopologyResponse(int socket, int numOfNodes, char *buf)
{
    int size = (sizeof(addDeleteNodePayload) + (numOfNodes * 48));
 
    addDeleteNodePayload *payloadBuf = my_malloc(size);
    
    payloadBuf->numOfNodes = numOfNodes;
    payloadBuf->flags = ADD_PAYLOAD | COMPLETE_PAYLOAD;
    payloadBuf->ttl = 0;          //No need to propogate
    memcpy(payloadBuf->ID, buf, numOfNodes * 48);
    sendPayload(socket, MSG_ADD_DELETE_NODE, payloadBuf, size);
    free(payloadBuf); 
}

/*********************************************************
** This is the function used to send request for topology 
** 
** 
** 
** Arguments:
** socket     : socket on which the payload was received.
** numOfNodes : Num of nodes in topology.
** buf        : buffer having IP addresses of nodes.
***********************************************************/
void sendTopologyJoinRequest(int socket)
{
    //int size = (sizeof(topologyRequest) + (numOfNodes * 48));
 
    topologyRequestPayload *payloadBuf = 
                        my_malloc(sizeof(topolgyRequestPayload));
    
    payloadBuf->flags = ADD_NODE_REQUEST;
    memcpy(payloadBuf->ipAddr, myIP, 16);
    sendPayload(socket, MSG_TOPOLOGY_REQUEST, payloadBuf, size);
    free(payloadBuf); 
}

/********************************************************************
** This is function used to send Add Node Payload 
** 
** 
** 
** Arguments:
** socket     : socket on which the payload was received.
** numOfNodes : Num of nodes in topology.
** buf        : buffer having IP addresses of nodes.
**********************************************************************/
void sendAddNodePayload(char *ipAddrList, int numOfNodesToSend, char ID[48] )
{
    char (*IP)[16];
    IP = ipAddrList;
    int index = 0;
    int i;
    thread_data *my_data[5];
    pthread_t   thread[5];
    int threads_created = 0;
    addDeleteNodePayload *payloadBuf = my_malloc(sizeof(addDeleteNodePayload) + ID_SIZE);  
    payloadBuf->numOfNodes = 1;
    paylodBuf->flags |= (ADD_PAYLOAD | DELTA_PAYLOAD);
    payloadBuf->ttl = 0;
    memcpy(payloadBuf->ID, ID, 47);
    payload->ID[47] = 0;
    while(index < numOfNodesToSend ) {
        threads_created = 0;
        for (i=0; i<5 && index < numOfNodesToSend; i++, index ++, threads_created++) {
            //my_data[i].ip[15] = 0;
            my_data[i] = my_malloc(sizeof(thread_data) + ID_SIZE);
            *(my_data[i]).payload = my_malloc(sizeof(ID));
            memcpy(*(my_data[i]).ip, IP, 16);
            IP++;
            *(my_data[i]).payload_size = (sizeof(addDeleteNodePayload) + ID_SIZE;  
            *(my_data[i]).msg_type = MSG_ADD_DELETE_NODE;
            memcpy(*(my_data[i]).payload, ID, 48);
            pthread_create(&thread[i], NULL, send_node_update_payload, *(my_data[i])); 
        }
        for (i=0 ; i < threads_created; i++) {
            pthread_join(&thread[i],NULL);
        } 
    }
}


/********************************************************************
** This is function used to send Add Node Payload 
** 
** 
** 
** Arguments:
** socket     : socket on which the payload was received.
** numOfNodes : Num of nodes in topology.
** buf        : buffer having IP addresses of nodes.
**********************************************************************/
void sendDeleteNodePayload(char *ipAddrList, int numOfNodesToSend, char ID[48] , int ttl)
{
    char (*IP)[16];
    IP = ipAddrList;
    int index = 0;
    int i;
    thread_data *my_data[5];
    pthread_t   thread[5];
    int threads_created = 0;
    addDeleteNodePayload *payloadBuf = my_malloc(sizeof(addDeleteNodePayload) + ID_SIZE);  
    payloadBuf->numOfNodes = 1;
    paylodBuf->flags |= (DELETE_PAYLOAD | DELTA_PAYLOAD);
    payloadBuf->ttl = ttl;
    memcpy(payloadBuf->ID, ID, 47);
    payload->ID[47] = 0;
    while(index < numOfNodesToSend ) {
        threads_created = 0;
        for (i=0; i<5 && index < numOfNodesToSend; i++, index ++, threads_created++) {
            //my_data[i].ip[15] = 0;
            my_data[i] = my_malloc(sizeof(thread_data) + ID_SIZE);
            *(my_data[i]).payload = my_malloc(sizeof(ID));  
            memcpy(*(my_data[i]).ip, IP, 16);
            IP++;
            *(my_data[i]).payload_size = (sizeof(addDeleteNodePayload) + ID_SIZE;  
            *(my_data[i]).msg_type = MSG_ADD_DELETE_NODE;
            memcpy(*(my_data[i]).payload, ID, 48);
            pthread_create(&thread[i], NULL, send_node_update_payload, *(my_data[i])); 
        }
        for (i=0 ; i < threads_created; i++) {
            pthread_join(&thread[i],NULL);
        } 
    }
}
