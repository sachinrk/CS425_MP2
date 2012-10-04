#include "list.h"
#define ID_SIZE 48

struct Head_Node * init_head(struct Node* node) {
	struct Head_Node * tmp = NULL;
	
	tmp = (struct Head_Node*)my_malloc(sizeof(struct Head_Node));
	if(tmp != NULL)
		tmp->node = node;
	
	return tmp;
}

struct Node* init_node(char ID[ID_SIZE]) {
	struct Node * tmp = NULL;
	
	tmp = (struct Node*) my_malloc ( sizeof(struct Node));
	if(tmp != NULL) {
		memcpy(tmp->timestamp, ID, 32);
                tmp->IP[15] = 0;
                memcpy(tmp->timestamp, getIpAddres(ID), 32);
                tmp->timestamp = ntohl(tmp->timestamp);  
		tmp->next = tmp;
		tmp->prev = tmp;	
	}
	
	return tmp;
}

int add_to_list(struct Head_Node ** head, char ID[ID_SIZE]) {	
	struct Node * tmp;
	
	tmp = init_node(ID);
	if ( *head == NULL ) {
		*head = init_head(tmp);
		(*head)->num_of_nodes = 1;
                return 0;
	} else {

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
			if(!strcmp(tmp->IP, IP)) { //Match found	
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
		free(tmp);

		DEBUG(("Remove_from_listNode successfully removed."));
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
     struct Node *tmp = (*head)->node;
     struct tmp1;
     while(tmp && tmp != (*head)->node) {
         tmp1 = tmp->next;
         free(tmp);
         tmp1 = tmp;
     }    
     (*head)->node = NULL;
}
