#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "Connection.h"

#include <sys/socket.h>

typedef struct tcp_client TCPClient;
struct tcp_client {
  void *self; // private data

  // sends msg in the connection
  // returns 0 if fail, >0 otherwise
  int (*send)(const TCPClient *, void *msg, size_t len);

  // receive msg in the connection
  // returns 0 if fail, >0 otherwise
  int (*recv)(const TCPClient *, void *msg, size_t len);

  // returns 1 if client is bound to an address, 0 otherwise
  int (*isBound)(const TCPClient *);

  // if client is bound, it will write it's address in `addr'
  // returns 1 if successful, 0 otherwise
  int (*getClientAddr)(const TCPClient *, struct sockaddr_in *addr);

  // writes the address of server in `addr'
  void (*getServerAddr)(const TCPClient *, struct sockaddr_in *addr);

  // destroctor
  void (*destroy)(const TCPClient *);
};

// Constructor, takes 1 mandatory argument `server' which is the address of
// tcpserver and an optional argument `client', which will cause this tcpclient
// to bind to that address upon success returns valid pointer to TCPClient,
// otherwise NULL pointer
const TCPClient *create_TCPClient(struct sockaddr_in *server,
                                  struct sockaddr_in *client);

#endif
