//===----------------------------------------------------------------------===//
// Dispatcher_main.c
// ---------------------------------------------------------------------------//
// This file contains the main function.
// Sequentially calls Dispatcher's subroutines
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dispatcher/Dispatcher.h"

#define USAGE(PRGM) fprintf(stderr, "Usage: %s [file]\n", PRGM);

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (argc > 2) {
    USAGE(argv[0]);
    exit(1);
  }

  // testing
  // constructing dispatcher object
  const Dispatcher *dispatcher = Dispatcher_create();
  if (!dispatcher) {
    fprintf(stderr, "%s: failed to create dispatcher side\n", argv[0]);
    USAGE(argv[0]);
    exit(1);
  }

  if (argc == 2) {
    if (!dispatcher->readFile(dispatcher, argv[1]))
      error("failed to create packet");
  } else {
    if (!dispatcher->readStdin(dispatcher))
      error("failed to create packet");
  }

  // destroying dispatcher object
  dispatcher->destroy(dispatcher);
}
