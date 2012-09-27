#include "list.h"


struct Head_Node * init_head(struct Node* node) {
	struct Head_Node * tmp = NULL;
	
	tmp = (struct Head_Node*)malloc(sizeof(struct Head_Node));
	if(tmp != NULL)
		tmp->node = node;
	
	return tmp;
}

struct Node* init_node(char IP[16]) {
	struct Node * tmp = NULL;
	
	tmp = (struct Node*) malloc ( sizeof(struct Node));
	if(tmp != NULL) {
		strcpy(tmp->IP, IP);
		tmp->next = tmp;
		tmp->prev = tmp;	
	}
	
	return tmp;
}

int add_to_list(struct Head_Node ** head, char IP[16]) {	
	struct Node * tmp;
	
	tmp = init_node(IP);
	if ( *head == NULL ) {
		*head = init_head(tmp);
		return 0;
	} else {

		tmp->next = (*head)->node;
		tmp->prev = (*head)->node->prev;
		(*head)->node->prev->next = tmp;
		(*head)->node->prev = tmp;
		return 0;	
	}
}

int remove_from_list(struct Head_Node **head, char IP[16]) {
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

		free(tmp);

		DEBUG(("Remove_from_listNode successfully removed."));
		return 0;
	}
}
