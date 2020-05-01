#include "UDPClient.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  int theSocket;
  struct sockaddr_in *addr; // binded address
};

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

static int client_isBound(const UDPClient *client) {
  Data *data = (Data *)(client->self);
  return (data->addr != NULL);
}

static int client_getAddr(const UDPClient *client, struct sockaddr_in *addr) {
  Data *data = (Data *)(client->self);
  if (!data->addr)
    return 0;

  memcpy(addr, data->addr, sizeof(struct sockaddr_in));
  return 1;
}

static void client_destroy(const UDPClient *client) {
  Data *data = (Data *)(client->self);
  close(data->theSocket);
  free(data->addr);
  free(data);
  free((void *)client);
}

UDPClient clientTemplate = {NULL,     client_udpSend, client_udpRecv,
                            client_isBound, client_getAddr, client_destroy};

const UDPClient *create_UDPClient(struct sockaddr_in *addr) {
  UDPClient *client = NULL, *tmp;
  Data *data = NULL;
  tmp = (UDPClient *)malloc(sizeof(UDPClient));
  if (tmp) {
    data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      if ((data->theSocket = socket(AF_INET, SOCK_DGRAM, 0)) != -1) {
        if (addr) {
          data->addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
          if (data->addr) {
            *(data->addr) = *addr;
            if (bind(data->theSocket, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1)
              goto error;
          } else
            goto error;
        }
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
error:
  free(data->addr);
  free(data);
  free(tmp);
  return NULL;
}
