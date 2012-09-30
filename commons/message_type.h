#ifndef __MESSAGE_TYPES__
#define __MESSAGE_TYPES__

#include <stdint.h>
#include "debug.h"
#include "time.h"
#define FILE_PATH_LENGTH 200
#define MAX_ELEMENTS_PER_PAYLOAD 8

/*Macros used for Marshalling */
#define marshal_field_info(data_structure, field) \
    (int)(&(((data_structure *)0)->field)), sizeof(((data_structure *)0)->field)

typedef enum {
   MSG_HEARTBEAT = 0,
   MSG_ADD_DELETE_NODE,
   MSG_TOPOLOGY_REQUEST,
   MSG_FILE_TRANSFER,
   MSG_EXECUTE_SCRIPT,
   NUM_OF_PAYLOADS
}messageType;

typedef struct 
{
   uint16_t length;
   uint16_t type;
   char payload[0]; 
}payloadBuf;

typedef struct 
{
    long   heartbeatTimeStamp;
    char   ip_addr[16];    
}heartbeatPayload;

typedef struct
{   
    uint16_t transferId;
    uint16_t statusFlag;
    #define FTP_REQUEST  0x01
    #define FTP_RESPONSE 0x02
    #define FTP_START    0x04
    #define FTP_STOP     0x08
    #define FTP_SUCCESS  0x10
    char fileName[FILE_PATH_LENGTH];
    char filePayload[0];
}fileTransferPayload;

typedef struct
{
    char scriptName[FILE_PATH_LENGTH];
}executeScriptPayload;

typedef struct
{
  uint8_t numOfNodes;
  uint8_t flags;
  uint8_t ttl;                                            //Indicates the number of nodes this message has to be forwarded to
  #define ADD_PAYLOAD           0x01
  #define DELETE_PAYLOAD        0x02
  #define LEAVE_NOTIFICATION    0x04
  #define NODE_FAILURE          0x08 
  #define DELTA_PAYLOAD         0x10                      //This bit is set if only the new node in the system is getting added  
  #define COMPLETE_PAYLOAD      0x20                      //This bit is set if set of all nodes is being sent. 
  char    ipAddr[0][16];                             
}addDeleteNodePayload;

typedef struct
{
  uint8_t flags;
  #define ADD_NODE_REQUEST      0x01
  long timestamp;
  char ipAddr[16]; 
}topologyRequest;

typedef enum {
    RC_FAILURE = 0,
    RC_SUCCESS,
    RC_FILE_NOT_FOUND,
    RC_SOCKET_WRITE_FAILURE,
    RC_SOCKET_READ_FAILURE
}returnCode;

#endif
