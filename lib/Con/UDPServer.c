#include "Con/UDPServer.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  int theSocket;
  int hasClient;
  struct sockaddr_in client;
  struct sockaddr_in addr;
};

static int server_udpSend(const UDPServer *server, void *msg, size_t len) {
  Data *data = (Data *)(server->self);
  if (!data->hasClient)
    return 0;

  return sendto(data->theSocket, msg, len, 0,
                (struct sockaddr *)&(data->client), sizeof(struct sockaddr_in));
}

static int server_udpSendTo(const UDPServer *server, void *msg, size_t len,
                            struct sockaddr_in *addr) {
  Data *data = (Data *)(server->self);
  return sendto(data->theSocket, msg, len, 0, (struct sockaddr *)addr,
                sizeof(struct sockaddr_in));
}

static int server_udpRecv(const UDPServer *server, void *msg, size_t len) {
  Data *data = (Data *)(server->self);

  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  if (!recvfrom(data->theSocket, msg, len, 0, (struct sockaddr *)&addr,
                &addrlen))
    return 0;

  data->client = addr;
  data->hasClient = 1;

  return 1;
}

static int server_getClientAddr(const UDPServer *server,
                                struct sockaddr_in *addr) {
  Data *data = (Data *)(server->self);
  if (!data->hasClient)
    return 0;

  memcpy(addr, &(data->client), sizeof(struct sockaddr_in));
  return 1;
}

static void server_destroy(const UDPServer *server) {
  Data *data = (Data *)(server->self);
  close(data->theSocket);
  free(data);
  free((void *)server);
}

UDPServer serverTemplate = {
    NULL,           server_udpSend,       server_udpSendTo,
    server_udpRecv, server_getClientAddr, server_destroy};

const UDPServer *create_UDPServer(struct sockaddr_in *addr) {
  UDPServer *server = NULL;
  UDPServer *tmp = (UDPServer *)malloc(sizeof(UDPServer));
  if (tmp) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      if ((data->theSocket = socket(AF_INET, SOCK_DGRAM, 0)) != -1) {
        if (!bind(data->theSocket, (struct sockaddr *)addr,
                  sizeof(struct sockaddr_in))) {
          *tmp = serverTemplate;
          tmp->self = (void *)data;
          server = tmp;
        } else {
          close(data->theSocket);
          free(data);
          free(tmp);
        }
      } else {
        free(data);
        free(tmp);
      }
    } else
      free(tmp);
  }
  return server;
}
