//===----------------------------------------------------------------------===//
// UDPConnection.c
// ---------------------------------------------------------------------------//
// This file defines the methods of Connection.h for a UDP socket
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "TMAgent/Connection.h"
#include "Packet.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  int theSocket;
  int isReceiver; // boolean, 1 if is Receiver, 0 otherwise
                  // changes after every send and receive
  struct sockaddr_in serverAddr;
  struct sockaddr_in clientAddr;
};

// private function
// initializes `addr' with the `ip' and `port' provided
static void initAddr(int port, struct in_addr ip, struct sockaddr_in *addr) {
  memset(addr, 0, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr = ip;
}

static void initServerAddr(Data *d, int port, struct in_addr ip) {
  initAddr(port, ip, &(d->serverAddr));
}

static void initClientAddr(Data *d, int port, struct in_addr ip) {
  initAddr(port, ip, &(d->clientAddr));
}

static int listenToTraffic(const Connection *con, size_t nbytes) {
  Data *data = (Data *)(con->self);
  if (!data->isReceiver)
    return 0;

  struct sockaddr_in clientAddr;
  socklen_t clientLen;
  char buf[nbytes];
  int status;
  if ((status = recvfrom(data->theSocket, (void *)buf,
                  nbytes, MSG_WAITALL,
                  (struct sockaddr *)&(clientAddr), &clientLen)))
    data->isReceiver = 0;

  return status;
}

// bind socket with address
// returns 1 if successful, 0 otherwise
static int bindSock(int sock, struct sockaddr_in *addr) {
  return !bind(sock, (struct sockaddr *)addr, (socklen_t)sizeof(struct sockaddr_in));
}

static int bindServer(const Connection *con) {
  Data *data = (Data *)(con->self);

  // bind socket with address
  return bindSock(data->theSocket, &(data->serverAddr));
}

static int bindClient(const Connection *con) {
  Data *data = (Data *)(con->self);

  // bind socket with address
  return bindSock(data->theSocket, &(data->clientAddr));
}

static int sendTraffic(const Connection *con, size_t nbytes) {
  Data *data = (Data *)(con->self);
  if (data->isReceiver)
    return 0;

  char buf[nbytes];
  int status;
  if ((status = sendto(data->theSocket, (void *)buf,
                nbytes, 0,
                (struct sockaddr *)&(data->serverAddr),
                (socklen_t)sizeof(data->serverAddr))))
    data->isReceiver = 1;

  return status;
}

static int becomeReceiver(const Connection *con) {
  Data *data = (Data *)(con->self);
  data->isReceiver = 1;
  return 1;
}

static void destroy(const Connection *con) {
  Data *data = (Data *)(con->self);
  close(data->theSocket);
  free(data);
  free((void *)con);
}

UDPConnection UDP = {NULL, becomeReceiver, bindClient, bindServer, listenToTraffic, sendTraffic, destroy};

const UDPConnection *create_UDPConnection(struct in_addr srcAddr, int srcPort,
                                       struct in_addr dstAddr, int dstPort) {

  Connection *con = (Connection *)malloc(sizeof(Connection));
  if (con) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      if ((data->theSocket = socket(AF_INET, SOCK_DGRAM, 0)) != -1) {
        initServerAddr(data, dstPort, dstAddr);
        initClientAddr(data, srcPort, srcAddr);
        *con = UDP;
        con->self = data;
      } else {
        free(data);
        free(con);
      }
    } else
      free(con);
  }
  return con;
}
