#include "join_topology.h"

extern struct Head_Node *server_topology;
extern struct Node* myself;
extern int topology_version;

void* join_topology(void* t) {

	pthread_exit(NULL);
}
