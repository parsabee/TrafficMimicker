//===----------------------------------------------------------------------===//
// TMClient.h
// ---------------------------------------------------------------------------//
// This file contains the main function for TMClient module
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
#include "TMAgent/TMServices.h"
#include "TMAgent/TMServer.h"
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
extern char *prgm;
#endif

int main(int argc, char *argv[]) {

  struct packet *p;
  p = parseArgs(argc, argv, USAGE);
  if (!p)
    exit(1);

  // server
  struct sockaddr_in serv;
  initAddr(&serv, p, 0);
  
  #ifdef DEBUG
  prgm = argv[0];
  #endif

  long programError = (long)runTmServer((void *)p);
  exit(programError);
}
