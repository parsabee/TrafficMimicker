//===----------------------------------------------------------------------===//
// Server.h
// ---------------------------------------------------------------------------//
// This file contains the method table of Server object
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#ifndef SERVER_H
#define SERVER_H

typedef struct server Server;

// constructor
const Server *Server_create();

struct server {
  void *self; // private data

  // calls `recvfrom' on internal socket
  // returns non-zero if successful, 0 otherwise
  int (*receivePacket)(const Server *server);

  // starts the connection with its TMAgent peer
  // if TMAgent is initializer, sends the first packet
  // otherwise, TMAgent becomes server
  // return 1 if successful, 0 otherwise
  int (*startConnection)(const Server *server);

  void (*destroy)(const Server *); // destroctor
};

#endif // SERVER_H
