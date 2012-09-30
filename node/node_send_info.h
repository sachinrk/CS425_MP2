#ifndef __NODE_SEND_INFO_H__
#define __NODE_SEND_INFO_H__
typedef struct {
    char ip[16];
    int payload_size;
    int msg_type;
    char *payload;
}thread_data; 
#endif
