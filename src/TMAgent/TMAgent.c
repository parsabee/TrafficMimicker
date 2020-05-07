//===----------------------------------------------------------------------===//
// TMAgent.h
// ---------------------------------------------------------------------------//
// This file contains the method table of TMAgent object
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "TMAgent/TMAgent.h"
#include "Con/UDPServer.h"
#include "Con/Connection.h"
#include "TMAgent/TMClient.h"
#include "TMAgent/TMServer.h"
#include "ADTs/arraylist.h"
#include "packet.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

typedef struct data Data;
struct data {
  const UDPServer *dispCon; // server always listens to this socket to get traffic
                            // info from TrafficDispatcher

  const ArrayList *threadIDs;

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

// private function
// make a string lower case
// static char *toLower(char *s) {
//   char *ret = s;
//   for (; *s != '\0'; s++)
//     *s = tolower(*s);
//   return ret;
// }

// private function,
// creates and stores a thread responsible for carrying out `fxn' with `p' args
static int createAndStoreThread(Data *d, void *(*fxn)(void *), struct packet *p) {
  int status = 0;
  pthread_t *tid = NULL;

  struct packet *packet = (struct packet *)malloc(SIZEOFPACKET(p));
  if (packet) {
    DEEPCOPY(packet, p);
    status = 1;
  }

  if (status) {
    tid = malloc(sizeof(pthread_t));
    if (tid) {
      if (pthread_create(tid, NULL, fxn, (void *)packet)) {
        free(tid);
        status = 0;
      }
    } else {
      status = 0;
    }
  }
  if (status) {
    status = d->threadIDs->add(d->threadIDs, (void *)tid);
  }
  return status;
}

static int startConnection(const TMAgent *server) {
  Data *data = (Data *)(server->self);
  int status = 0;
  if (data->packet.init) //initialize TMClient
    status = createAndStoreThread(data, runTmClient, &(data->packet.thePacket)); 
  else {
    if ((status = createAndStoreThread(data, runTmServer, &(data->packet.thePacket))))  //initialize TMServer
      (void)sendAck(data);
  }
  
  return status;
}

static int receivePacket(const TMAgent *server) {
  Data *data = (Data *)(server->self);
  return data->dispCon->recv(data->dispCon, (void *)&(data->packet), BUFSIZ);
}

static int destroy(const TMAgent *server) {
  Data *data = (Data *)(server->self);
  if(data->dispCon)
    data->dispCon->destroy(data->dispCon);
  
  long status = 1;
  for(long i = 0; i < data->threadIDs->size(data->threadIDs); i++) {
    pthread_t *tid;
    (void)data->threadIDs->get(data->threadIDs, i, (void **)&tid);
    long exitStatus;
    pthread_join(*tid, (void **)&exitStatus);
    if (!exitStatus)
      status = 0;
  }
  data->threadIDs->destroy(data->threadIDs, free);
  free(data);
  free((void *)server);
  return status;
}

const TMAgent template = {NULL, receivePacket, startConnection, destroy};

const TMAgent *TMAgent_create(char *hostname, int port) {
  TMAgent *server = NULL, *self; 
  self = (TMAgent *)malloc(sizeof(TMAgent));
  if (self) {
    Data *data = (Data *)malloc(sizeof(Data));
    if (data) {
      memset(data, 0, sizeof(Data));
      const ArrayList *al = ArrayList_create(0L);
      if (al) {
        struct sockaddr_in addr;
        const UDPServer *tmp = NULL;
        if (resolveAddress(hostname, port, &addr))
          tmp = create_UDPServer(&addr);
        
        if (tmp) {
          data->dispCon = tmp;
          data->threadIDs = al;
          *self = template;
          self->self = data;
          server = self;
        } else {
          free(data);
          free(server);
        }
      } else {
        free(data);
        free(server);
      }
    } else
      free(server);
  }
  return server;
}
