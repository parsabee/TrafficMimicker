#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct tcp_server TCPServer;
struct tcp_server {
  void *self;

  int (*accept)(const TCPServer *);
  int (*send)(const TCPServer *, int id, void *msg, size_t nbytes);
  int (*recv)(const TCPServer *, int id, void *msg, size_t nbytes);
  int (*close)(const TCPServer *, int id);
  void (*destroy)(const TCPServer *);
};

const TCPServer *create_TCPServer(struct sockaddr_in *addr);

#endif
