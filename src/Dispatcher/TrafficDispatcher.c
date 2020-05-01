//===----------------------------------------------------------------------===//
// TrafficDispatcher.c
// ---------------------------------------------------------------------------//
// This file contains the main function.
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dispatcher/Client.h"

#define UNUSED __attribute__((unused))

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

int main(UNUSED int argc, UNUSED char *argv[]) {
  // testing
  // constructing client object
  const Client *client = Client_create();
  if (!client) {
    fprintf(stderr, "%s: failed to create client side\n", argv[0]);
    exit(1);
  }

  // creating a dummy infopacket
  if (!client->createPacket(client, 1, "localhost", 8001,"ix.cs.uoregon.edu", 8000, 
                            "UDP", 64))
    error("failed to create packet");

  // dispatching packet to tm-agents
  if (!client->dispatch(client))
    error("failed to dispatch");

  // destroying client object
  client->destroy(client);
}
