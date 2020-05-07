//===----------------------------------------------------------------------===//
// TMAgent.h
// ---------------------------------------------------------------------------//
// This file contains the method table of TMAgent object
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#ifndef TMAGENT_H
#define TMAGENT_H

typedef struct tm_agent TMAgent;

// constructor
const TMAgent *TMAgent_create(char *hostname, int port);

struct tm_agent {
  void *self; // private data

  // calls `recvfrom' on internal socket
  // returns non-zero if successful, 0 otherwise
  int (*receivePacket)(const TMAgent *server);

  // starts a connection with the information received by `receivePacket()' call
  // spawns a thread which will handle the connection
  // fails if called before `receivePacket()'
  // returns 1 if successful, 0 otherwise
  int (*startConnection)(const TMAgent *server);
  
  // destructor, 
  // blocks until all threads return
  // returns 1 upon success, 0 otherwise(threads failure)
  int (*destroy)(const TMAgent *);
};

#endif // TMAGENT_H
