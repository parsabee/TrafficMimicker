//===----------------------------------------------------------------------===//
// TMServer.h
// ---------------------------------------------------------------------------//
// This file contains the definition of `runTmServer()'
// Synopsis:
//   tm-server gran srcip srcport dstip dstport istcp num-packets 
//   <direction size> [<direction size> ...]
//
// This module will be invoked by a TMAgent; after 
// it has received information packet from the dispathcer.
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "Con/TCPServer.h"
#include "TMAgent/TMServer.h"
#include "TMAgent/TMServices.h"
#include "Con/UDPServer.h"
#include "packet.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USAGE                                                                  \
  "Usage: tm-server gran srcip srcport dstip dstport istcp num-packets "       \
  "<direction size> [<direction size> ...]\n"                                  \
  "       tm-server -h"

#ifdef DEBUG
char *prgm;
#endif

void *runTmServer(void *args) {
  struct packet *p = (struct packet *)args;

  // server
  struct sockaddr_in serv;
  initAddr(&serv, p, 0);

  long programError = 0L;
  if (p->tcp)
    goto tcp_routine;
  else
    goto udp_routine;

tcp_routine:
  ;
  const TCPServer *tcpServer = create_TCPServer(&serv);
  if (!tcpServer) {
    #ifndef DEBUG
    char *prgm = "create_TCPServer()";
    #endif
    fprintf(stderr,
            "%s: failed to create tcp server with address %s:%d\n",
            prgm, inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));
    programError++;
    goto end;
  }

  char tcpBuffer[BUFSIZ] = {0};

#ifdef DEBUG
  fprintf(stderr, "%s: tcp -- accepting connections\n", prgm);
#endif

  //accepting connections, storing connection id
  int id = tcpServer->accept(tcpServer);
  if (id == -1) {
    programError++;
    goto end;
  }

  for (int i = 0; i < p->num_packets; i++) {
    if (p->packets[i].dir == incoming) {
#ifdef DEBUG
      fprintf(stderr, "%s: tcp -- sending %d bytes\n", prgm,
              p->packets[i].size);
#endif
      tcpServer->send(tcpServer, id, tcpBuffer, p->packets[i].size);
    } else {
#ifdef DEBUG
      fprintf(stderr, "%s: tcp -- receiving %d bytes\n", prgm,
              p->packets[i].size);
#endif
      tcpServer->recv(tcpServer, id, tcpBuffer, p->packets[i].size);
    }
  }

  tcpServer->close(tcpServer, id);
  tcpServer->destroy(tcpServer);
  goto end;

udp_routine:
  ;
  const UDPServer *udpServer = create_UDPServer(&serv);
  if (!udpServer) {
    #ifndef DEBUG
    char *prgm = "create_TCPServer()";
    #endif
    fprintf(stderr,
            "%s: failed to create udp server with address %s:%d\n",
            prgm, inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));
    programError++;
    goto end;
  }

  char udpBuffer[BUFSIZ] = {0};

  for (int i = 0; i < p->num_packets; i++) {
    if (p->packets[i].dir == incoming) {
#ifdef DEBUG
      fprintf(stderr, "%s: udp -- sending %d bytes\n", prgm,
              p->packets[i].size);
#endif
      udpServer->send(udpServer, udpBuffer, p->packets[i].size);
    } else {
#ifdef DEBUG
      fprintf(stderr, "%s: udp -- receiving %d bytes\n", prgm,
              p->packets[i].size);
#endif
      udpServer->recv(udpServer, udpBuffer, p->packets[i].size);
    }
  }

  udpServer->destroy(udpServer);
  goto end;

end:
  free(p);
  return (void *)programError;
}
