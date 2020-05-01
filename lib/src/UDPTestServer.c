#include "UDPServer.h"
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

  const UDPServer *server = create_UDPServer(&server_addr);
  if (!server)
    error("failed to create server");
  
  char buf[BUFSIZ];
  struct sockaddr_in cli_addr;

  for (;;) {
    memset(buf, 0, BUFSIZ);
    printf("%s: listenning ...\n", argv[0]);
    server->recv(server, (void *)buf, BUFSIZ);
    if (!server->getClientAddr(server, &cli_addr))
      error("server->getClientAddr()\n");

    printf("%s: received message from ", argv[0]);
    printAddr(&cli_addr, stdout);
    printf("%s: sending it back ...\n", argv[0]);
    server->send(server, (void *)buf, strlen(buf));
    printf("%s: sent\n", argv[0]);
  }
}
