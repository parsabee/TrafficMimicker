//===----------------------------------------------------------------------===//
// Client.h
// ---------------------------------------------------------------------------//
// This file contains the method table of Client object
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

typedef struct client Client;

// Client constructor
const Client *Client_create();

struct client {
  void *self; // private data

  // create a traffic-info packet to send to TMAgent
  // returns 1 if successful, 0 otherwise
  int (*createPacket)(const Client *self, int gran, char *srcip, int srcprt,
                         char *dstip, int dstprt, char *protocol,
                         int vol);

  // sends the packet created by `createPacket()' 
  // to destination ip (TMAgentServer), waits for ack,
  // then sends the packet to source ip (TMAgentClient)
  // returns 1 if successful, 0 otherwise
  int (*dispatch)(const Client *self);

  // Client destructor
  void (*destroy)(const Client *self);
};
