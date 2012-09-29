#include "node_payload_process.h"

neigbourHeartbeat savedHeartbeat[NUM_HEARBEAT_NEIGHBOURS];
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
              if (!(strcpy(savedHeartbeat[i].ipAddr), payload->ipAddr)) {
                  time(&savedHeartbeat[i].timeStamp);
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

void processNodeAddDeletePayload(addDeleteNodePayload *payload) 
{
    int i;
    //nodeData *ptr = head; 
    //nodeData *tmp;
    for (i = 0; i < payload->numOfNodes; i++) {
       pthread_mutex_lock(&node_list_mutex);
       if ((payload->flags & ADD_PAYLOAD) && (payload->flags & COMPLETE_PAYLOAD)) {
           delete_all_nodes();
       }
       if (payload->flags & DELETE_PAYLOAD) {
          remove_from_list(&server_topology, payload->ipAddr[i]);
          /* while (ptr != NULL && ptr->next != head) {
               if (!strcmp(ptr->ipAddr, payload->ipAddr[i])) { //Match found. Delete this node 
                   if (ptr == head) {
                       head = head->next;
                   }
                   (ptr->prev)->next = ptr->next;
                   (ptr->next)->prev = ptr->prev;
                   free(ptr);
                   break;
               }
               ptr = ptr->next;
           } */
       } else if (payload->flags & ADD_PAYLOAD) {
           add_to_list(&server_topology, payload->ipAddr[i]);   
           /*tmp = (nodeData *)malloc(sizeof(nodeData));
           strcpy(nodeData->ipAddr, payload->ipAddr[i]);
           (head->prev)->next = tmp;
           tmp->prev = head->prev;
           tmp->next =  head;
           head->prev = tmp;*/
       }
       pthread_mutex_unlock(&node_list_mutex); 
   }
 
}

/*********************************************************
** This is the IP address get function 
** It gets the system's IP address
** 
** 
** Arguments:
** ipAddr : IP Address.
***********************************************************/
RC_t getIpAddr(char *IP)
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
    
}


