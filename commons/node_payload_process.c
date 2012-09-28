#include "node_payload_process.h"

neigbourHeartbeat savedHeartbeat[NUM_HEARBEAT_NEIGHBOURS];
nodeData *head = NULL;
nodeData *thisNode = NULL;
/*********************************************************
** This is the heartbeat process function. 
** It updates the heartbeat timestamp when the heartbeat 
** is received. 
** 
** Arguments:
** ipAddr : IP Address.
***********************************************************/

void processHeartbeat(char *ipAddr) 
{
    int i; 
    for (i = 0; i < NUM_HEARTBEAT_NEIGHBOURS; i++) {
       pthread_mutex_lock(&timestamp_mutex);
       if (!(strcpy(savedHeartbeat[i].ipAddr), ipAddr)) {
           time(&savedHeartbeat[i].timeStamp);
       }
       pthread_mutex_unlock(&timestamp_mutex); 
   }
 
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
    nodeData *ptr = head; 
    nodeData *tmp;
    for (i = 0; i < payload->numOfNodes; i++) {
       pthread_mutex_lock(&node_list_mutex);
       if ((payload->flags & ADD_PAYLOAD) && (payload->flags & COMPLETE_PAYLOAD)) {
           deleteAllNodes();
       }
       if (payload->flags & DELETE_PAYLOAD) {
           while (ptr != NULL && ptr->next != head) {
               if (!strcmp(ptr->ipAddr, payload->ipAddr[i])) { /*Match found. Delete this node */
                   if (ptr == head) {
                       head = head->next;
                   }
                   (ptr->prev)->next = ptr->next;
                   (ptr->next)->prev = ptr->prev;
                   free(ptr);
                   break;
               }
               ptr = ptr->next;
           } 
       } else if (payload->flags & ADD_PAYLOAD) {
           tmp = (nodeData *)malloc(sizeof(nodeData));
           strcpy(nodeData->ipAddr, payload->ipAddr[i]);
           (head->prev)->next = tmp;
           tmp->prev = head->prev;
           tmp->next =  head;
           head->prev = tmp;
       }
       pthread_mutex_unlock(&node_list_mutex); 
   }
 
}

/*********************************************************
** This is the function used to delete all nodes. 
** 
** 
** 
** Arguments:
** None.
***********************************************************/

void deleteAllNodes()
{
   nodeData *ptr = head;
   nodeData *temp;
   while (ptr && (ptr->next != head)) {
       temp = ptr->next;
       free (ptr);
       ptr = temp;
   } 
}
