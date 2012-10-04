#ifndef __HEARTBEAT_SEND__
#define __HEARTBEAT_SEND__

#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../commons/list.h"
#include "../logging/log.h"

void* heartbeat_send(void* t);
#endif 
