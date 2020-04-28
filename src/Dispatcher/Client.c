//===----------------------------------------------------------------------===//
// Client.c
// ---------------------------------------------------------------------------//
// This file contains the definition of Client methods
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "Dispatcher/Client.h"
#include "Packet.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  int theSocket;
  int init; // boolean, 1 if `buff' is initialized, 0 otherwise 
  InfoPacket buff;
};

// private function, initializing the sockaddr_in
// with the port and ip-address provided
static void initAddress(struct sockaddr_in *addr, int portnumber,
                        struct in_addr ipAddr) {
  memset(addr, 0, sizeof(struct in_addr));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(portnumber);
  addr->sin_addr = ipAddr;
}

// private function, called by a separate thread in dispatch
// function,
// will wait for an ack from the TMAgentServer
static int listenForAck(void *d) {
  Data *data = (Data *)d;
  char ack[16]; //FIXME use uint8_t
  while (1) {
    struct sockaddr_in addr;
    socklen_t len;
    if (!recvfrom(data->theSocket, (void *)ack, 16, 0,
                  (struct sockaddr *)&addr, &len))
      return 0;

    if (!strcmp(ack, "ack"))
      break;
  }
  return 1;
}

// private function, sends buffer to server TMAgent
// return 1 if successful, 0 otherwise
static int sendToServer(Data *data) {
  struct sockaddr_in addr;
  initAddress(&addr, SERVER_PORT, data->buff.dst_ip);
  data->buff.inferred_initiator_src = 0;
  return sendto(data->theSocket, (void *)&(data->buff), BUFF, 0,
                (struct sockaddr *)&addr, (socklen_t)sizeof(addr));
}

// private function, sends buffer to client TMAgent
// return 1 if successful, 0 otherwise
static int sendToClient(Data *data) {
  struct sockaddr_in addr;
  initAddress(&addr, SERVER_PORT, data->buff.src_ip);
  data->buff.inferred_initiator_src = 1;
  return sendto(data->theSocket, (void *)&(data->buff), BUFF, 0,
                (struct sockaddr *)&addr, (socklen_t)sizeof(addr));
}

static int dispatch(const Client *self) {
  Data *data = (Data *)self->self;
  if (!data->init)
    return 0;

  // sending the packet to destination(TMAgentServer)
  if (!sendToServer(data))
    return 0;

  // wait until get ack from destination(TMAgentServer)
  if (!listenForAck(data))
    return 0;

  // got ack, now send packet to source(TMAgentClient)
  int status = sendToClient(data);
  if (status)
    data->init = 0;

  return status;
}

static int createPacket(const Client *self, int gran, char *srcip, int srcprt,
                        char *dstip, int dstprt, char *protocol, int vol) {

  Data *data = (Data *)self->self;
  InfoPacket packet;

  packet.granularity = gran;
  packet.src_port = (uint16_t)srcprt;
  struct in_addr addr;
  if (!inet_aton(srcip, &addr))
    return 0;

  packet.src_ip = addr;
  packet.dst_port = (uint16_t)dstprt;
  if (!inet_aton(dstip, &addr))
    return 0;

  packet.dst_ip = addr;
  packet.vol_time_series = (uint32_t)vol;
  strcpy(packet.proto, protocol);
  memcpy(&(data->buff), &packet, sizeof(InfoPacket));
  data->init = 1;
  return 1;
}

static void destroy(const Client *self) {
  Data *data = (Data *)self->self;
  close(data->theSocket);
  free(data);
  free((void *)self);
}

static Client template = {NULL, createPacket, dispatch, destroy};

const Client *Client_create() {
  Client *dispatcher = (Client *)malloc(sizeof(Client));
  if (dispatcher) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      int theSocket = socket(AF_INET, SOCK_DGRAM, 0);
      if (theSocket > 0) {
        data->theSocket = theSocket;
        *dispatcher = template;
        dispatcher->self = (void *)data;
      } else {
        free(data);
        free(dispatcher);
      }
    } else
      free(dispatcher);
  }
  return dispatcher;
}

