#ifndef __FAILURE_DETECTOR__
#define __FAILURE_DETECTOR__

#include <stdio.h>
#include <stropts.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "../commons/debug.h"
#include "../logging/log.h"
#include "../commons/list.h"
#include "join_topology.h"
#include "form_topology.h"
#include "heartbeat_send.h"
#include "heartbeat_receive.h"
#include "../logging/log.h"
#include "../ui/ui.h"

#endif
