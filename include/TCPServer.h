#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct tcp_server TCPServer;
struct tcp_server {
  void *self;

  int (*accept)(const TCPServer *);
  int (*send)(const TCPServer *, int id, void *msg, size_t nbytes);
  int (*recv)(const TCPServer *, int id, void *msg, size_t nbytes);
  int (*close)(const TCPServer *, int id);

  // writes the address of connection with id `id' to `addr'
  // returns 1 if successful, 0 otherwise
  int (*getAddr)(const TCPServer *, int id, struct sockaddr_in *addr);
  void (*destroy)(const TCPServer *);
};

const TCPServer *create_TCPServer(struct sockaddr_in *addr);

#endif
