//===----------------------------------------------------------------------===//
// TMAgent.c
// ---------------------------------------------------------------------------//
// This file contains main function of TMAgent module
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#include "TMAgent/Server.h"

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

  const Server *server = Server_create(argv[1]);
  if (!server)
    error("failed to create server\n");

  // receive info packet;
  server->receivePacket(server);

  // start the TMAgent-TMAgent connection
  server->startConnection(server);

  server->destroy(server);
}
