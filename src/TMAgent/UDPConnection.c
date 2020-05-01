//===----------------------------------------------------------------------===//
// UDPConnection.c
// ---------------------------------------------------------------------------//
// This file defines the methods of Connection.h for a UDP socket
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "Connection.h"
#include "Packet.h"
#include "UDPClient.h"
#include "UDPServer.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  int isReceiver; // boolean, 1 if is Receiver, 0 otherwise
                  // changes after every send and receive
  enum Side side; // server or client
  union Sock {
    const UDPClient *udpClient;
    const UDPServer *udpServer;
  } udpCon;
  struct sockaddr_in serverAddr;
  struct sockaddr_in clientAddr;
};

static void initServerAddr(Data *d, int port, struct in_addr ip) {
  initAddress(&(d->serverAddr), port, ip);
}

static void initClientAddr(Data *d, int port, struct in_addr ip) {
  initAddress(&(d->clientAddr), port, ip);
}

static int clientListen(Data *data, size_t nbytes) {
  char buf[nbytes];
  const UDPClient *cli = data->udpCon.udpClient;
  return cli->recv(cli, buf, nbytes);
}

static int serverListen(Data *data, size_t nbytes) {
  //same as clientListen, the recv method for UDPClient and UDPServer
  //is the same
  char buf[nbytes];
  const UDPServer *srv = data->udpCon.udpServer;
  return srv->recv(srv, buf, nbytes);
}

static int listenToTraffic(const Connection *con, size_t nbytes) {
  Data *data = (Data *)(con->self);
  if (!data->isReceiver)
    return 0;

  int status = 0;
  if (data->side == CLIENT)
    status = clientListen(data, nbytes);
  else
    status = serverListen(data, nbytes);
  
  if (status)
    data->isReceiver = 0;

  return status;
}

static int clientSend(Data *data, size_t nbytes) {
  char buf[nbytes];
  const UDPClient *cli = data->udpCon.udpClient;
  return cli->sendTo(cli, buf, nbytes, &(data->serverAddr));
}

static int serverSend(Data *data, size_t nbytes) {
  char buf[nbytes];
  const UDPServer *srv = data->udpCon.udpServer;
  return srv->send(srv, buf, nbytes);
}

static int sendTraffic(const Connection *con, size_t nbytes) {
  Data *data = (Data *)(con->self);
  if (data->isReceiver)
    return 0;

  int status = 0;
  if (data->side == CLIENT)
    status = clientSend(data, nbytes);
  else
    status = serverSend(data, nbytes);
  
  if (status)
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
  if (data->side == SERVER)
    data->udpCon.udpServer->destroy(data->udpCon.udpServer);
  else
    data->udpCon.udpClient->destroy(data->udpCon.udpClient);

  free(data);
  free((void *)con);
}

UDPConnection UDP = {NULL, becomeReceiver, listenToTraffic, sendTraffic, destroy};

const UDPConnection *create_UDPConnection(struct in_addr srcAddr, int srcPort,
                                       struct in_addr dstAddr, int dstPort, enum Side side) {

  Connection *con = NULL, *tmp;
  tmp = (Connection *)malloc(sizeof(Connection));
  if (tmp) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      initServerAddr(data, dstPort, dstAddr);
      initClientAddr(data, srcPort, srcAddr);
      int status = 0;
      if (side == SERVER)
        status = (data->udpCon.udpServer = create_UDPServer(&data->serverAddr)) ? 1 : 0;
      else {
        status = (data->udpCon.udpClient = create_UDPClient(&data->clientAddr)) ? 1 : 0;
        if (status)
          status = data->udpCon.udpClient->bind(data->udpCon.udpClient, &data->clientAddr);
      }

      if (status) {
        data->side = side;
        *tmp = UDP;
        tmp->self = data;
        con = tmp;
      } else {
        free(data);
        free(tmp);
      }
    } else
      free(tmp);
  }
  return con;
}
