//===----------------------------------------------------------------------===//
// Connection.h
// ---------------------------------------------------------------------------//
// This file Connection abstract type to be derived by sub-connection types
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#ifndef CONNECTION_H
#define CONNECTION_H

#include "Packet.h"

typedef struct connection Connection;
typedef struct connection UDPConnection;
typedef struct connection TCPConnection;
struct connection {
  void *self; // private data

  // if connection type is a UDP,
  // sets the flag for becoming receiver, allowing
  // you to call `listenToTraffic' method without failing
  // and preventing you from executing `sendTraffic' successfully
  // returns 1 if connection is UDP, 0 otherwise
  int (*becomeReceiver)(const Connection *con);
  
  // binds src-address and port to socket
  // 1 if success, 0 otherwise
  int (*bindClient)(const Connection *);

  // binds dst-address and port to socket
  // 1 if success, 0 otherwise
  int (*bindServer)(const Connection *);

  // listens for traffic on `dst_port' of InfoPacket,
  // for `nbytes' of data
  // fails if connection is not in receiver state
  // returns 1 if successful, 0 otherwise
  int (*listenToTraffic)(const Connection *, size_t nbytes);

  // produces the traffic over connection
  // if connection is in receiver state, it fails;
  // otherwise it send `nbytes' traffic to `dst_ip' and `dst_port'
  // returns 1 if successful, 0 otherwise
  int (*sendTraffic)(const Connection *, size_t nbytes) ;
  
  // destroctor
  void (*destroy)(const Connection *);
};

// Connection constructor, Creates an underlying UDP(DGRAM) socket
const UDPConnection *create_UDPConnection(struct in_addr srcAddr, int srcPort,
                                       struct in_addr dstAddr, int dstPort);

const TCPConnection *create_TCPConnection(InfoPacket *packet);

#endif //CONNECTION_H
