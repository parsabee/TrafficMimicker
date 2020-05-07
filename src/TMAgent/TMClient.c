//===----------------------------------------------------------------------===//
// TMClient.c
// ---------------------------------------------------------------------------//
// This file contains the definition of `runTmClinet()'
// Synopsis:
//   tm-client gran srcip srcport dstip dstport istcp num-packets
//   <direction size> [<direction size> ...]
//
// This module will be invoked by a TMAgent; after
// it has received information packet from the dispathcer.
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Con/TCPClient.h"
#include "TMAgent/TMClient.h"
#include "TMAgent/TMServices.h"
#include "Con/UDPClient.h"
#include "packet.h"

#define USAGE                                                                  \
  "Usage: tm-client gran srcip srcport dstip dstport istcp num-packets "       \
  "<direction size> [<direction size> ...]\n"                                  \
  "       tm-client -h"

#ifdef DEBUG
char *prgm;
#endif

void *runTmClient(void *args) {
  struct packet *p = (struct packet *)args;

  // client
  struct sockaddr_in cli;
  initAddr(&cli, p, 1);

  // server
  struct sockaddr_in serv;
  initAddr(&serv, p, 0);

  long programError = 0L;
  if (p->tcp)
    goto tcp_routine;
  else
    goto udp_routine;

tcp_routine:;
  const TCPClient *tcpClient = create_TCPClient(&serv, &cli);
  if (!tcpClient) {
    char *prgm = "create_TCPClient()";
    fprintf(stderr,
            "%s: failed to create tcp client with address %s:%d, and server "
            "address %s:%d\n",
            prgm, inet_ntoa(cli.sin_addr), ntohs(cli.sin_port),
            inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
    programError++;
    goto end;
  }

  char tcpBuffer[BUFSIZ] = {0};

  // iterate through the packets and send that much.
  for (int i = 0; i < p->num_packets; i++) {
    if (p->packets[i].dir == outgoing) {
#ifdef DEBUG
      fprintf(stderr, "%s: tcp -- sending %d bytes\n", prgm,
              p->packets[i].size);
#endif
      tcpClient->send(tcpClient, tcpBuffer, p->packets[i].size);
    } else {
#ifdef DEBUG
      fprintf(stderr, "%s: tcp -- receiving %d bytes\n", prgm,
              p->packets[i].size);
#endif
      tcpClient->recv(tcpClient, tcpBuffer, p->packets[i].size);
    }
  }

  tcpClient->destroy(tcpClient);
  goto end;

udp_routine:;
  const UDPClient *udpClient = create_UDPClient(&cli);
  if (!udpClient) {
    char *prgm = "create_UDPClient()";
    fprintf(stderr,
            "%s: failed to create tcp client with address %s:%d, and server "
            "address %s:%d\n",
            prgm, inet_ntoa(cli.sin_addr), ntohs(cli.sin_port),
            inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
    programError++;
    goto end;
  }

  char udpBuffer[BUFSIZ] = {0};
  for (int i = 0; i < p->num_packets; i++) {
    if (p->packets[i].dir == outgoing) {
#ifdef DEBUG
      fprintf(stderr, "%s: udp -- sending %d bytes\n", prgm,
              p->packets[i].size);
#endif
      udpClient->sendTo(udpClient, udpBuffer, p->packets[i].size, &serv);
    } else {
#ifdef DEBUG
      fprintf(stderr, "%s: udp -- receiving %d bytes\n", prgm,
              p->packets[i].size);
#endif
      udpClient->recv(udpClient, udpBuffer, p->packets[i].size);
    }
  }

  udpClient->destroy(udpClient);
  goto end;

end:
  free(p);
  return (void *)programError;
}

