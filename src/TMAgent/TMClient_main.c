//===----------------------------------------------------------------------===//
// TMClient.c
// ---------------------------------------------------------------------------//
// This file contains the main function for TMClient module
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
#include "TMAgent/TMServices.h"
#include "TMAgent/TMClient.h"
#include "Con/UDPClient.h"
#include "packet.h"

#define USAGE                                                                  \
  "Usage: tm-client gran srcip srcport dstip dstport istcp num-packets "       \
  "<direction size> [<direction size> ...]\n"                                  \
  "       tm-client -h"                                                      

#ifdef DEBUG
extern char *prgm;
#endif

int main(int argc, char *argv[]) {

  struct packet *p;
  p = parseArgs(argc, argv, USAGE);
  if (!p)
    exit(1);

#ifdef DEBUG
  prgm = argv[0];
#endif

  long programError = (long)runTmClient((void *)p);
  exit(programError);
}
