#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct udp_server UDPServer;
struct udp_server {
  void *self; // private data

  // sends msg back to client
  // only successful if we have previously received a message
  int (*send)(const UDPServer *, void *msg, size_t len);

  // sends msg to a `addr'
  int (*sendTo)(const UDPServer *server, void *msg, size_t len,
                struct sockaddr_in *addr);

  // listen for `len' bytes long message and write it to `msg'
  int (*recv)(const UDPServer *, void *msg, size_t len);

  // writes address of latest client in addr
  // return 1 if successful, 0 if has no client
  int (*getClientAddr)(const UDPServer *, struct sockaddr_in *addr);

  void (*destroy)(const UDPServer *);
};

const UDPServer *create_UDPServer(struct sockaddr_in *addr);

#endif
