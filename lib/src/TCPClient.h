#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "Connection.h"

#include <sys/socket.h>

typedef struct tcp_client TCPClient;
struct tcp_client {
  void *self; // private data
  int (*send)(const TCPClient *, void *msg, size_t len);
  int (*recv)(const TCPClient *, void *msg, size_t len);
  int (*isBound)(const TCPClient *);
  int (*getClientAddr)(const TCPClient *, struct sockaddr_in *addr);
  void (*getServerAddr)(const TCPClient *, struct sockaddr_in *addr);
  void (*destroy)(const TCPClient *);
};

const TCPClient *create_TCPClient(struct sockaddr_in *server,
                                  struct sockaddr_in *client);

#endif
