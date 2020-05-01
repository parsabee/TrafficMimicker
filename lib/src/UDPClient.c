#include "UDPClient.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  int theSocket;
  int bound;               // 1 if address is binded to socket, 0 otherwise
  struct sockaddr_in addr; // binded address
};

static int client_udpBind(const UDPClient *client, struct sockaddr_in *addr) {
  Data *data = (Data *)(client->self);
  if (data->bound)
    return 0;

  int status = 0;
  if (!bind(data->theSocket, (struct sockaddr *)addr,
               sizeof(struct sockaddr_in)))
    status = !connect(data->theSocket, (struct sockaddr *)addr,
               sizeof(struct sockaddr_in));
  return status;
}

static int client_udpSend(const UDPClient *client, void *msg, size_t len,
            struct sockaddr_in *addr) {
  Data *data = (Data *)(client->self);
  return sendto(data->theSocket, msg, len, 0, (struct sockaddr *)addr,
                sizeof(struct sockaddr_in));
}

static int client_udpRecv(const UDPClient *client, void *msg, size_t len) {
  Data *data = (Data *)(client->self);
  return recv(data->theSocket, msg, len, 0);
}

static int client_isBinded(const UDPClient *client) {
  Data *data = (Data *)(client->self);
  return data->bound;
}

static int client_getAddr(const UDPClient *client, struct sockaddr_in *addr) {
  Data *data = (Data *)(client->self);
  if (!data->bound)
    return 0;

  memcpy(addr, &(data->addr), sizeof(struct sockaddr_in));
  return 1;
}

static void client_destroy(const UDPClient *client) {
  Data *data = (Data *)(client->self);
  close(data->theSocket);
  free(data);
  free((void *)client);
}

UDPClient clientTemplate = {NULL,     client_udpBind, client_udpSend, client_udpRecv,
                            client_isBinded, client_getAddr, client_destroy};

const UDPClient *create_UDPClient() {
  UDPClient *client = NULL;
  UDPClient *tmp = (UDPClient *)malloc(sizeof(UDPClient));
  if (tmp) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      if ((data->theSocket = socket(AF_INET, SOCK_DGRAM, 0)) != -1) {
        *tmp = clientTemplate;
        tmp->self = (void *)data;
        client = tmp;
      } else {
        free(data);
        free(tmp);
      }
    } else
      free(tmp);
  }
  return client;
}