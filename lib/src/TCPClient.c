#include "TCPClient.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  int theSocket;
  struct sockaddr_in *client;
  struct sockaddr_in *server;
};

static int client_tcpSend(const TCPClient *client, void *msg, size_t nbytes) {
  Data *data = (Data *)(client->self);
  return send(data->theSocket, msg, nbytes, 0);
}

static int client_tcpRecv(const TCPClient *client, void *msg, size_t nbytes) {
  Data *data = (Data *)(client->self);
  return recv(data->theSocket, msg, nbytes, 0);
}

static int client_tcpIsBound(const TCPClient *client) {
  Data *data = (Data *)(client->self);
  return (data->client != NULL);
}

static int client_getClientAddr(const TCPClient *client,
                                struct sockaddr_in *ref) {
  Data *data = (Data *)(client->self);
  if (!data->client)
    return 0;

  *ref = *(data->client);
  return 1;
}

static void client_getServerAddr(const TCPClient *client,
                                 struct sockaddr_in *ref) {
  Data *data = (Data *)(client->self);
  *ref = *(data->server);
}

static void destroy(const TCPClient *client) {
  Data *data = (Data *)(client->self);
  close(data->theSocket);
  free(data);
  free((void *)client);
}

static TCPClient tcpClient = {NULL,
                              client_tcpSend,
                              client_tcpRecv,
                              client_tcpIsBound,
                              client_getClientAddr,
                              client_getServerAddr,
                              destroy};

static const TCPClient *bindClient(TCPClient *client,
                                   struct sockaddr_in *clientAddr) {
  Data *data = (Data *)(client->self);
  if (bind(data->theSocket, (struct sockaddr *)clientAddr, sizeof(struct sockaddr_in)))
    return NULL;

  memcpy(data->client, clientAddr, sizeof(struct sockaddr_in));
  return client;
}

static int captureAddress(Data *d, struct sockaddr_in *sa, struct sockaddr_in *ca) {
  struct sockaddr_in *addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
  if (addr) {
    *addr = *sa;
    d->server = addr;
    if (ca) {
      addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
      if (addr) {
        *addr = *ca;
        d->client = addr;
      } else {
        free(d->server);
        return 0;
      }
    }
  } else
    return 0;

  return 1;
}

const TCPClient *create_TCPClient(struct sockaddr_in *serverAddr,
                                  struct sockaddr_in *clientAddr) {
  if (!serverAddr)
    return NULL;

  TCPClient *client = NULL, *tmp;
  tmp = (TCPClient *)malloc(sizeof(TCPClient));
  if (tmp) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      if (captureAddress(data, serverAddr, clientAddr)) {
        if ((data->theSocket = socket(AF_INET, SOCK_STREAM, 0)) != -1) {
          if (connect(data->theSocket, (struct sockaddr *)serverAddr,
                      sizeof(struct sockaddr_in)) != -1) {
            *tmp = tcpClient;
            tmp->self = data;
            client = tmp;
          } else {
            close(data->theSocket);
            free(data);
            free(tmp);
          }
        } else {
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

  if (clientAddr)
    return bindClient(client, clientAddr);

  return client;
}
