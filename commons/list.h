#ifndef __LIST__
#define __LIST__

#include "debug.h"
#include <stdlib.h>
#include <string.h>

#define NUM_HEARTBEAT_NEIGHBOURS 2

struct Node {

	char IP[16];
	unsigned long timestamp;
        unsigned long num_of_nodes;
        struct Node *next;
	struct Node *prev;
};

struct Head_Node {
	struct Node* node;
};

typedef struct {
	char ipAddr[16];
	time_t lastTimeStamp;
}neighbourHeartbeat;

struct Head_Node * init_head(struct Node*);
struct Node* init_node(char [16]);
int remove_from_list(struct Head_Node **, char [16]);
int add_to_list(struct Head_Node **, char [16]);

#endif
