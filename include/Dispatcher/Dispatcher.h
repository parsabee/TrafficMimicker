//===----------------------------------------------------------------------===//
// Dispatcher.h
// ---------------------------------------------------------------------------//
// This file contains the method table of Dispatcher object
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

typedef struct dispatcher Dispatcher;

// Dispatcher constructor
const Dispatcher *Dispatcher_create();

struct dispatcher {
  void *self; // private data

  // reads stdin for InfoPacket
  // return 1 if successful, 0 otherwise
  int (*readStdin)(const Dispatcher *self);

  // reads file named `filename' for InfoPacket
  // return 1 if successful, 0 otherwise
  int (*readFile)(const Dispatcher *self, char *filename);

  // Dispatcher destructor
  void (*destroy)(const Dispatcher *self);
};
