//===----------------------------------------------------------------------===//
// Dispatcher.c
// ---------------------------------------------------------------------------//
// This file contains the definition of Dispatcher methods
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "Dispatcher/Dispatcher.h"
#include "Con/Connection.h"
#include "Con/UDPClient.h"
#include "packet.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct data Data;
struct data {
  const UDPClient *con;
  int init; // boolean, 1 if `buff' is initialized, 0 otherwise
  int bufferSize;
  InfoPacket *buffer;
};

// private function, called by a separate thread in dispatch
// function,
// will wait for an ack from the TMAgentServer
static int listenForAck(void *d) {
  Data *data = (Data *)d;
  struct control_msg msg;
  while (1) {
    if (!data->con->recv(data->con, (void *)&msg, sizeof(struct control_msg)))
      return 0;

    if (msg.type == ACK)
      break;
  }
  return 1;
}

static void clearBuffer(Data *data) {
  if (data->init) {
    free(data->buffer);
    data->init = 0;
  }
}

static size_t sizeofInfoPacketNumPackets(int num_packets) {
  return sizeof(InfoPacket) + sizeof(struct transfer) * num_packets;
}

static size_t sizeofInfoPacket(InfoPacket *ip) {
  return sizeofInfoPacketNumPackets(ip->thePacket.num_packets);
}

// private function, sends buffer to server TMAgent
// return 1 if successful, 0 otherwise
static int sendToServer(Data *data) {
  struct sockaddr_in addr;
  initAddress(&addr, SERVER_PORT, data->buffer->thePacket.dstip);
  return data->con->sendTo(data->con, (void *)data->buffer,
                           sizeofInfoPacket(data->buffer), &addr);
}

// private function, sends buffer to client TMAgent
// return 1 if successful, 0 otherwise
static int sendToClient(Data *data) {
  struct sockaddr_in addr;
  initAddress(&addr, SERVER_PORT, data->buffer->thePacket.srcip);
  return data->con->sendTo(data->con, (void *)data->buffer,
                           sizeofInfoPacket(data->buffer), &addr);
}

static char *readLine(char *buffer, int size, FILE *fp, char *thePrompt) {
  memset(buffer, 0, size);
  if (fp == stdin)
    fprintf(stderr, "  %s:  ", thePrompt);

  char *tmp = fgets(buffer, size, fp);
  return tmp;
}

static void printError(char *error, FILE *fp) {
  if (fp == stdin)
    fprintf(stderr, "  error: %s\n", error);
}

static char *readPackets(InfoPacket *ip, char *buf, FILE *fp) {
  int n = ip->thePacket.num_packets;
  if (fp == stdin)
    fprintf(stderr, "Reading %d packets:\n", n);

  char *tmp = NULL;
  for (int i = 0; i < n; i++) {
    while (1) {
      if (!(tmp = readLine(
                buf, BUFSIZ, fp,
                "direction of packet from perspective of client [in/out]")))
        return tmp;
      if (!strcmp(buf, "in\n")) {
        ip->thePacket.packets[i].dir = incoming;
        break;
      } else if (!strcmp(buf, "out\n")) {
        ip->thePacket.packets[i].dir = outgoing;
        break;
      } else
        printError("enter [in/out]", fp);
    }
    if (!(tmp = readLine(buf, BUFSIZ, fp, "size of packet")))
      return tmp;

    if (!(ip->thePacket.packets[i].size = atoi(buf))) {
      printError("size is a positive integer", fp);
      return NULL;
    }
  }
  return tmp;
}

static char *parse(Data *d, FILE *fp) {
  if (fp == stdin)
    fprintf(stderr, "Reading packet info from stdin:\n");

  struct packet *p;
  struct in_addr addr;
  char buffer[BUFSIZ];
  char *tmp;

  tmp = readLine(buffer, BUFSIZ, fp, "number of packets");
  int n;
  if (!(n = atoi(buffer))) {
    printError("number of packets is a non-zero number", fp);
    return NULL;
  }

  InfoPacket *ip;
  if ((ip = (InfoPacket *)malloc(sizeofInfoPacketNumPackets(n))))
    ip->thePacket.num_packets = n;

  p = &(ip->thePacket);

  tmp = readLine(buffer, BUFSIZ, fp, "granularity");
  if (!(p->gran = atoi(buffer))) {
    printError("granularity is a non-zero number", fp);
    return 0;
  }

  tmp = readLine(buffer, BUFSIZ, fp, "source ip-address");
  if (!getInetAddr(buffer, &addr)) {
    printError("invalid ip-address", fp);
    return 0;
  }
  p->srcip = addr;

  tmp = readLine(buffer, BUFSIZ, fp, "source port");
  if (!(p->srcport = atoi(buffer))) {
    printError("port is a non-zero number", fp);
    return 0;
  }

  tmp = readLine(buffer, BUFSIZ, fp, "destination ip-address");
  if (!getInetAddr(buffer, &addr)) {
    printError("invalid ip-address", fp);
    return 0;
  }
  p->dstip = addr;

  tmp = readLine(buffer, BUFSIZ, fp, "destination port");
  if (!(p->srcport = atoi(buffer))) {
    printError("port is a non-zero number", fp);
    return 0;
  }

  while (1) {
    tmp = readLine(buffer, BUFSIZ, fp, "is connection tcp? [y/n]");
    if (*buffer == 'y') {
      p->tcp = 1;
      break;
    } else if (*buffer == 'n') {
      p->tcp = 0;
      break;
    } else
      printError("enter [y/n]", fp);
  }

  tmp = readPackets(ip, buffer, fp);
  d->buffer = ip;
  d->init = 1;
  return tmp;
}

static int dispatch(Data *data) {
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
    clearBuffer(data);

  return status;
}

static int readFile(const Dispatcher *self, char *file) {
  Data *d = (Data *)self->self;
  FILE *fp = fopen(file, "r");
  if (fp) {
    while (parse(d, fp))
      dispatch(d);

    fclose(fp);
  } else
    fprintf(stderr, "dispatcher: %s: No such file\n", file);

  // if successful, the buffer should have been dispatched
  return !d->init;
}

static int readStdin(const Dispatcher *self) {
  Data *d = (Data *)self->self;
  while (parse(d, stdin))
    dispatch(d);

  // if successful, the buffer should have been dispatched
  return !d->init;
}

static void destroy(const Dispatcher *self) {
  Data *data = (Data *)self->self;
  clearBuffer(data);
  data->con->destroy(data->con);
  free(data);
  free((void *)self);
}

static Dispatcher template = {NULL, readStdin, readFile, destroy};

const Dispatcher *Dispatcher_create() {
  Dispatcher *dispatcher = (Dispatcher *)malloc(sizeof(Dispatcher));
  if (dispatcher) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      const UDPClient *con = create_UDPClient(NULL);
      if (con != NULL) {
        data->con = con;
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
