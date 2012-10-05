#ifndef __HEARTBEAT_RECEIVE__
#define __HEARTBEAT_RECEIVE__

#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>
#include "../commons/list.h"
#include "../logging/log.h"
#include "../commons/message_type.h"
#include "../commons/fdetect_payload_process.h"

void* heartbeat_receive(void* t);

#endif
