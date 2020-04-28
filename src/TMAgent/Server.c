//===----------------------------------------------------------------------===//
// Server.h
// ---------------------------------------------------------------------------//
// This file contains the method table of Server object
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//


#include "TMAgent/Server.h"
#include "TMAgent/Connection.h"
#include "Packet.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

typedef struct data Data;
struct data {
  int serverUDPSocket; // server always listens to this socket to get traffic
                       // info from TrafficDispatcher
  struct sockaddr_in addr;       // the address to which server is bounded
  struct sockaddr_in dispatcher; // the address of TrafficDispatcher
  socklen_t dispatcherLen;
  InfoPacket packet;     // the packet sent by the TrafficDispatcher
  const Connection *con; // the connection between TMAgents;
};

// private function,
// initialize sockaddr_in struct pointed to by addr
// return 1 if successful, 0 otherwise
static int initAddress(char *hostname, struct sockaddr_in *addr) {
  struct hostent *host = gethostbyname(hostname);
  if (!host)
    return 0;
    
  memset(addr, 0, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(SERVER_PORT);
  addr->sin_addr = *((struct in_addr *)(host->h_addr));
  return 1;
}

// Private function,
// If TMAgent is a server, it needs to send an ACK to
// TrafficDispatcher, so it can notify TMAgent clients
// returns 1 if successful, 0 otherwise
static int sendAck(Data *data) {
  char ack[16] = "ack";
  return sendto(data->serverUDPSocket, (void *)ack, 16, 0, (struct sockaddr *)&(data->dispatcher),
                data->dispatcherLen);
}

static int startConnection(const Server *server) {
  Data *data = (Data *)(server->self);
  int status = 0;
  if (data->packet.inferred_initiator_src) {
    // generate/send packet
    if (data->con->bindClient(data->con))
      status = data->con->sendTraffic(data->con, data->packet.vol_time_series);

  } else {
    // if TMAgent is not initializing, then it's Server
    // sending ack to TrafficDispatcher
    if (!data->con->bindServer(data->con))
      return 0;

    (void)data->con->becomeReceiver(data->con);
    (void)sendAck(data);

    // start listenning to traffic
    status = data->con->listenToTraffic(data->con, data->packet.vol_time_series);
  }
  return status;
}

// private function
// make a string lower case
static char *toLower(char *s) {
  char *ret = s;
  for (; *s != '\0'; s++)
    *s = tolower(*s);
  return ret;
}

static int receivePacket(const Server *server) {
  Data *data = (Data *)(server->self);
  struct sockaddr_in clientAddr;
  socklen_t len;
  if (!recvfrom(data->serverUDPSocket, (void *)&(data->packet),
                sizeof(InfoPacket), MSG_WAITALL,
                (struct sockaddr *)&(clientAddr), &len))
    return 0;

  data->dispatcher = clientAddr;
  data->dispatcherLen = len;
  InfoPacket *packet = &(data->packet);
  
  const Connection *con = NULL;
  if (!strcmp(toLower(data->packet.proto), "tcp"))
    con = create_TCPConnection(&data->packet);
  else
    con = create_UDPConnection(packet->src_ip, packet->src_port, packet->dst_ip, packet->dst_port);
  
  // return 1 if connection not NULL, 0 otherwise
  return ((data->con = con) != NULL);
}

static void destroy(const Server *server) {
  Data *data = (Data *)(server->self);
  close(data->serverUDPSocket);
  data->con->destroy(data->con);
  free(data);
  free((void *)server);
}

const Server template = {NULL, receivePacket, startConnection, destroy};

const Server *Server_create(char *hostname) {
  Server *self = NULL;
  int server;
  if ((server = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
    struct sockaddr_in addr;
    if (initAddress(hostname, &addr)) {
      if (bind(server, (struct sockaddr *)&addr, (socklen_t)sizeof(addr)) !=
          -1) {
        Data *data = (Data *)malloc(sizeof(Data));
        if (data) {
          self = (Server *)malloc(sizeof(Server));
          if (self) {
            data->serverUDPSocket = server;
            data->addr = addr;
            *self = template;
            self->self = data;
          } else {
            free(data);
            close(server);
          }
        } else
          close(server);
      } else
        close(server);
    } else
      close(server);
  }
  return self;
}
