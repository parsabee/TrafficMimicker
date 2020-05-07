#include "TCPServer.h"
#include "Connection.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#define USAGE "Usage: server hostname port"

void error(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 3)
    error(USAGE);

  struct sockaddr_in server_addr;
  struct in_addr inetaddr; 
  resolveAddress(argv[1], atoi(argv[2]), &server_addr);

  const TCPServer *server = create_TCPServer(&server_addr);
  if (!server)
    error("failed to create server");

  char buf[BUFSIZ] = {0};
  struct sockaddr_in cli_addr;

  for (;;) {
    int id;
    printf("%s: accepting requests ...\n", argv[0]);
    if ((id = server->accept(server)) == -1)
      error("failed to accept");
    
    printf("%s: connected to ", argv[0]);
    server->getAddr(server, id, &cli_addr);
    printAddr(&cli_addr, stdout);
    printf("%s: listenning ...\n", argv[0]);
    int n;
    while ((n = server->recv(server, id, buf, BUFSIZ)) > 0) {
      printf("%s: received message\n", argv[0]);
      printf("%s: sending it back ...\n", argv[0]);
      server->send(server, id, (void *)buf, strlen(buf));
      printf("%s: sent\n", argv[0]);
      memset(buf, 0, BUFSIZ);
    }
    if (n < 0)
      error("error in recv()");

    printf("%s: closing connection ...\n", argv[0]);
    server->close(server, id);
    printf("%s: connection closed\n", argv[0]);
  }

  server->destroy(server);
}
