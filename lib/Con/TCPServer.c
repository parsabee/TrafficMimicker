#include "Con/TCPServer.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CONS 100
#define LISTENQ 100

typedef struct con Con;
struct con {
  int id;
  int sockFD;
  struct sockaddr_in addr;
  socklen_t len;
};

typedef struct data Data;
struct data {
  int listenFD;
  Con **cons;
  int index;
};

static int bindAndListen(Data *data, struct sockaddr_in *addr) {
  int status = 0;
  if (bind(data->listenFD, (struct sockaddr *)addr,
           sizeof(struct sockaddr_in)) != -1) {
    if (listen(data->listenFD, LISTENQ) != -1)
      status = 1;
  }
  return status;
}

// private function, returns the index of open spot
// in const, if no open spot found, returns -1
static int searchForOpenSpot(Con **cons, int size) {
  for (int i = 0; i < size; i++)
    if (cons[i] == NULL)
      return i;

  return -1;
}

Con *create_Con(int id, int sock, struct sockaddr_in *addr, socklen_t len) {
  Con *con = (Con *)malloc(sizeof(Con));
  if (con) {
    con->id = id;
    con->sockFD = sock;
    con->addr = *addr;
    con->len = len;
  }
  return con;
}

static void destroy_Con(Con **cons, int id) {
  if (!cons[id])
    return;

  close(cons[id]->sockFD);
  free(cons[id]);
  cons[id] = NULL;
}

static int server_tcpAccept(const TCPServer *server) {
  Data *data = (Data *)(server->self);
  int index = 0;
  if ((index = searchForOpenSpot(data->cons, MAX_CONS)) == -1)
    return -1;

  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  int sock = accept(data->listenFD, (struct sockaddr *)&addr, &len);
  if (sock == -1)
    return -1;

  Con *con = create_Con(index, sock, &addr, len);
  if (!con)
    return -1;

  data->cons[index] = con;
  return index;
}

static int server_tcpSend(const TCPServer *server, int id, void *msg,
                          size_t nbytes) {
  if (id < 0 || MAX_CONS <= id)
    return 0;

  Data *data = (Data *)(server->self);
  Con *con;
  if ((con = data->cons[id]) == NULL)
    return 0;

  int sock = con->sockFD;
  return send(sock, msg, nbytes, 0);
}

static int server_tcpRecv(const TCPServer *server, int id, void *msg,
                          size_t nbytes) {
  if (id < 0 || MAX_CONS <= id)
    return 0;

  Data *data = (Data *)(server->self);
  Con *con;
  if ((con = data->cons[id]) == NULL)
    return 0;

  int sock = con->sockFD;
  return recv(sock, msg, nbytes, 0);
}

static int server_tcpCloseCon(const TCPServer *server, int id) {
  if (id < 0 || MAX_CONS <= id)
    return 0;

  Data *data = (Data *)(server->self);
  destroy_Con(data->cons, id);
  return 1;
}

static int server_tcpGetAddr(const TCPServer *server, int id,
                             struct sockaddr_in *addr) {
  if (id < 0 || MAX_CONS <= id)
    return 0;

  Data *data = (Data *)(server->self);
  if (!data->cons[id])
    return 0;

  *addr = data->cons[id]->addr;
  return 1;
}

static void server_tcpDestroy(const TCPServer *server) {
  Data *data = (Data *)(server->self);
  close(data->listenFD);
  for (int i = 0; i < MAX_CONS; i++)
    destroy_Con(data->cons, i);

  free(data->cons);
  free(data);
  free((void *)server);
}

TCPServer tcpServer = {NULL,
                       server_tcpAccept,
                       server_tcpSend,
                       server_tcpRecv,
                       server_tcpCloseCon,
                       server_tcpGetAddr,
                       server_tcpDestroy};

const TCPServer *create_TCPServer(struct sockaddr_in *addr) {
  TCPServer *server = NULL, *tmp;
  tmp = (TCPServer *)malloc(sizeof(TCPServer));
  if (tmp) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      Con **cons = (Con **)malloc(sizeof(Con *) * MAX_CONS);
      if (cons) {
        memset(cons, 0, sizeof(Con *) * MAX_CONS);
        if ((data->listenFD = socket(AF_INET, SOCK_STREAM, 0)) != -1) {
          if (bindAndListen(data, addr)) {
            *tmp = tcpServer;
            data->cons = cons;
            data->index = 0;
            tmp->self = data;
            server = tmp;
          } else {
            close(data->listenFD);
            free(cons);
            free(data);
            free(tmp);
          }
        } else {
          free(cons);
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
