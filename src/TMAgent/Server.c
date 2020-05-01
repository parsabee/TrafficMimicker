//===----------------------------------------------------------------------===//
// Server.h
// ---------------------------------------------------------------------------//
// This file contains the method table of Server object
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "TMAgent/Server.h"
#include "UDPServer.h"
#include "Connection.h"
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
  const UDPServer *dispCon; // server always listens to this socket to get traffic
                            // info from TrafficDispatcher
  const Connection *con;    // the connection between TMAgents;
  InfoPacket packet;        // the packet sent by the TrafficDispatcher
};

// Private function,
// If TMAgent is a server, it needs to send an ACK to
// TrafficDispatcher, so it can notify TMAgent clients
// returns 1 if successful, 0 otherwise
static int sendAck(Data *data) {
  char ack[16] = "ack";
  return data->dispCon->send(data->dispCon, (void *)ack, 16);
}

static int startConnection(const Server *server) {
  Data *data = (Data *)(server->self);
  int status = 0;
  if (data->packet.inferred_initiator_src) {
    // generate/send packet
    status = data->con->sendTraffic(data->con, data->packet.vol_time_series);

  } else {
    // if TMAgent is not initializing, then it's Server
    // sending ack to TrafficDispatcher
    (void)data->con->becomeReceiver(data->con);
    
  #ifndef TEST_TMAGENT_S2S
    (void)sendAck(data);
  #endif

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

#ifdef TEST_TMAGENT_S2S // testing tmagent server2server communication
//private function for testing
static void initializeDummyPacket(Data *data) {
  InfoPacket *packet = &(data->packet);
  packet->granularity = 1;
  packet->src_port = (uint16_t)8080;
  struct in_addr addr;
  (void)getInetAddr("127.0.0.1", &addr);
  packet->src_ip = addr;
  packet->dst_port = (uint16_t)8081;
  (void)getInetAddr("127.0.0.1", &addr);
  packet->dst_ip = addr;
  packet->vol_time_series = (uint32_t)64;
  strcpy(packet->proto, "UDP");
}
#endif

static int receivePacket(const Server *server) {
  Data *data = (Data *)(server->self);

#ifdef TEST_TMAGENT_S2S // testing tmagent server2server communication
  initializeDummyPacket(data);
  char buf[8];
  printf("client[1/0] >>> ");
  fgets(buf, 8, stdin);
  data->packet.inferred_initiator_src = atoi(buf);
#else
  if (!data->dispCon->recv(data->dispCon, (void *)&(data->packet), sizeof(InfoPacket)))
    return 0;
#endif

  InfoPacket *packet = &(data->packet);
  
  enum Side side = (data->packet.inferred_initiator_src) ? CLIENT : SERVER;
  const Connection *con = NULL;
  if (!strcmp(toLower(data->packet.proto), "tcp"))
    con = create_TCPConnection(&data->packet);
  else
    con = create_UDPConnection(packet->src_ip, packet->src_port, packet->dst_ip, packet->dst_port, side);
  
  // return 1 if connection not NULL, 0 otherwise
  return ((data->con = con) != NULL);
}

static void destroy(const Server *server) {
  Data *data = (Data *)(server->self);
  if(data->con)
    data->con->destroy(data->con);
  if(data->dispCon)
    data->dispCon->destroy(data->dispCon);
  free(data);
  free((void *)server);
}

const Server template = {NULL, receivePacket, startConnection, destroy};

#ifdef TEST_TMAGENT_S2S // testing tmagent server2server communication
#define UNUSED __attribute__((unused)) 
const Server *Server_create(UNUSED char *hostname, UNUSED int port) {
  Server *server = NULL, *self; 
  self = (Server *)malloc(sizeof(Server));
  if (self) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      data->dispCon = NULL;
      *self = template;
      self->self = data;
      server = self;
    } else
      free(server);
  }
  return server;
}

#else
const Server *Server_create(char *hostname, int port) {
  Server *server = NULL, *self; 
  self = (Server *)malloc(sizeof(Server));
  if (self) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      struct sockaddr_in addr;
      const UDPServer *tmp = NULL;
      if (resolveAddress(hostname, port, &addr))
        tmp = create_UDPServer(&addr);
      
      if (tmp) {
        data->dispCon = tmp;
        *self = template;
        self->self = data;
        server = self;
      } else {
        free(data);
        free(server);
      }
    } else
      free(server);
  }
  return server;
}

#endif
