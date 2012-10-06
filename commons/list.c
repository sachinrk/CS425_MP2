#include "list.h"

struct Head_Node * init_head(struct Node* node) {
	struct Head_Node * tmp = NULL;
	
	tmp = (struct Head_Node*)calloc(1, sizeof(struct Head_Node));
	if(tmp != NULL)
	    tmp->node = node;
	
	return tmp;
}

struct Node* init_node(char ID[ID_SIZE]) {
	struct Node * tmp = NULL;
	
	tmp = (struct Node*) calloc (1, sizeof(struct Node));
	if(tmp != NULL) {
		memcpy(&tmp->timestamp, ID, 4);
                memcpy(tmp->IP, ID + 4, 16);
                tmp->IP[15] = 0;
                tmp->timestamp = ntohl(tmp->timestamp);  
		tmp->next = tmp;
		tmp->prev = tmp;	
	}
	
	return tmp;
}

int add_to_list(struct Head_Node ** head, char ID[ID_SIZE]) {	
	struct Node * tmp;
	int i = 0;
	tmp = init_node(ID);
        long timestamp;  
        printf("\nIn adding before, head : %lu\n", (long)(*head));
        //getchar();
        if ( *head == NULL ) {
	        
	       printf("\nIn adding 1\n");
        	
                *head = init_head(tmp);
		(*head)->num_of_nodes = 1;
                     
                printf("\nReturning :%d\n", (*head)->num_of_nodes);

                return 0;
	} else {
                
                printf("\nIn adding %lu \n",(long)(*head));
                for (i = 0 , tmp = (*head)->node; i < (*head)->num_of_nodes; i++) {
                    if (!strncmp(tmp->IP, ID+4, 16)) {
                        memcpy(&timestamp, ID, 4);
                        tmp->timestamp = ntohl(timestamp);
                        return 0;
                    }
                }
		tmp->next = (*head)->node;
		tmp->prev = (*head)->node->prev;
		(*head)->node->prev->next = tmp;
		(*head)->node->prev = tmp;
		(*head)->num_of_nodes++;
                return 0;	
	}
}

int remove_from_list(struct Head_Node **head, char ID[ID_SIZE]) {
	struct Node *tmp, *next_node, *prev_node;
	int found = 0;

	tmp = next_node = prev_node = NULL;

	if((*head) == NULL) {
		DEBUG(("Remove_From_List :There is no list\n"));
		return 1;
	} else { 
		if((*head)->node == NULL) {
			DEBUG(("Remove_From_List: List is empty\n"));
			return 1;
		}
		
		tmp = (*head)->node;
		do {
			if(!memcmp(tmp->IP, ID+4, 16)) { //Match found	
				found = 1;
				break;
			}

			tmp = tmp->next;
	
		}while((tmp != (*head)->node));
		
		if(!found) return 1;
		
		next_node = tmp->next;
		prev_node = tmp->prev;
		next_node->prev = prev_node;
		prev_node->next = next_node;
		
		if(tmp == (*head)->node) 
			(*head)->node = NULL;
                (*head)->num_of_nodes--;
		free(*head);
                *head = NULL;
                free(tmp);

		DEBUG(("Remove_from_list : Node successfully removed."));
		return 0;
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

RC_t delete_all_nodes(struct Head_Node **head)
{
     struct Node *tmp = NULL;
     struct Node *tmp1 = NULL;
     int numOfNodes = 0;
     int index =0;
     
     printf("\ndelete_all_nodes *head = %lu\n", *head); 
    	
     if (*head != NULL) {
         tmp = (*head)->node;
         numOfNodes = (*head)->num_of_nodes; 
     }
     
     while(index < numOfNodes && tmp) {
         tmp1 = tmp->next;
         free(tmp);
         tmp = tmp1;
         index++;  
     }    
     (*head) = NULL;
}
