#ifndef __TOPOLOGY_UPDATE__
#define __TOPOLOGY_UPDATE__

#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../commons/list.h"
#include "../logging/log.h"
#include "../commons/message_type.h"

#define LISTEN_THREAD_PORT 1104

void* topology_update(void*);

#endif
