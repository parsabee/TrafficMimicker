#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "Connection.h"

#include <sys/socket.h>

typedef struct udp_client UDPClient;
struct udp_client {
  void *self; // private data

  // sends msg to address
  int (*sendTo)(const UDPClient *, void *msg, size_t len,
                struct sockaddr_in *addr);

  // listen for `len' bytes long message and write it to `msg'
  int (*recv)(const UDPClient *, void *msg, size_t len);

  // returns 1 if socket is binded to an address
  int (*isBound)(const UDPClient *);

  // writes the address of the binded socket in addr
  // returns 1 if successful, 0 otherwise(socket is not binded to an address)
  int (*getAddr)(const UDPClient *, struct sockaddr_in *addr);

  // destroctor
  void (*destroy)(const UDPClient *);
};

// UDPClient constructor
// takes an optional address, if it is not NULL, the client socket will bind itself to it
const UDPClient *create_UDPClient(struct sockaddr_in *addr);

#endif // UDPCLIENT_H
