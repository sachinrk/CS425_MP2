#include "fdetect_payload_process.h"

neigbourHeartbeat savedHeartbeat[NUM_HEARTBEAT_NEIGHBOURS];
extern struct Head_Node *server_topology;
extern pthread_mutex_t node_list_mutex;
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
        buf =  malloc(payload_size);
        memcpy(buf, payload, payload_size);
        tdata = malloc(sizeof(thread_data));
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
    buf = (char *)malloc(48 * (server_topology->num_of_nodes + 1)); 
    pthread_mutex_lock(&node_list_mutex);
    for (tmp = server_topology->node; tmp && tmp->next != server_topology->node; tmp++) {
        if (!strcmp(payload->ipAddr, tmp->IP)) {
            found = tmp;
            
        }
        strcpy(buf + offset, tmp->IP);
        memcpy(buf + offset + 16, htonl(tmp->timestamp));
        offset += 48;
    }
    if (payload->flags & ADD_NODE_REQUEST) {
        if (found) {
            LOG(ERROR, " Received duplicate node add request from %s ", payload->ipAddr);
            return;
        }
        add_to_list(&server_topology, payload->ID[0]); 
        strcpy(buf + offset, tmp->IP);
        server_topology->num_of_nodes++; 
    }
    pthread_mutex_unlock(&node_list_mutex);
    sendTopologyResponse(socket, server_topology->num_of_nodes, buf);  
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
/*RC_t getIpAddr(char *IP)
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
                strcpy(IP, addressBuffer);        
                rc = RC_SUCCESS;
                break;
            }
        }     
   }
   if (ifAddrStruct!=NULL) 
       freeifaddrs(ifAddrStruct);
   
   
   return rc;
    
}*/

/*********************************************************
** This is payload used to send request for topology 
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
 
    addDeleteNodePayload *payloadBuf = malloc(size);
    
    payloadBuf->numOfNodes = numOfNodes;
    payloadBuf->flags = ADD_PAYLOAD | COMPLETE_PAYLOAD;
    payloadBuf->ttl = 0;          //No need to propogate
    memcpy(payloadBuf->ipAddr, buf, numOfNodes * 16);
    sendPayload(socket, MSG_ADD_DELETE_NODE, payloadBuf, size);
    free(payloadBuf); 
}


