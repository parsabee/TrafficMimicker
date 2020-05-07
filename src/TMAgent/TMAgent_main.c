//===----------------------------------------------------------------------===//
// TMAgent_main.c
// ---------------------------------------------------------------------------//
// This file contains main function of TMAgent module
// TMAgent's subroutines are sequentially called
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "TMAgent/TMAgent.h"
#include "packet.h"

#include <stdio.h>
#include <stdlib.h>

#define USAGE "Usage: tm-agent host"

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    error(USAGE);

  const TMAgent *tmAgent = TMAgent_create(argv[1], SERVER_PORT);
  if (!tmAgent)
    error("failed to create tmAgent\n");

  // receive info packet;
  tmAgent->receivePacket(tmAgent);

  // start the TMAgent-TMAgent connection
  tmAgent->startConnection(tmAgent);

  tmAgent->destroy(tmAgent);
}
