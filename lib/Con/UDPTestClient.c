#include "UDPClient.h"
#include "Connection.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#define USAGE "Usage: udp-echo hostname hostport [clientname clientport]"
void error(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 5 && argc != 3)
    error(USAGE);

  struct sockaddr_in clientaddr;
  struct sockaddr_in *clientAddrPtr = NULL;
  if (argc == 5) {
    resolveAddress(argv[3], atoi(argv[4]), &clientaddr);
    printf("%s: binding to %s %s\n", argv[0], argv[3], argv[4]);
    clientAddrPtr = &clientaddr;
  }

  const UDPClient *client = create_UDPClient(clientAddrPtr);
  if (!client)
    error("failed to create client");

  if (client->isBound(client)) {
    printf("%s: bound to ", argv[0]);
    client->getAddr(client, &clientaddr);
    printAddr(&clientaddr, stdout);
  }

  struct sockaddr_in server;
  resolveAddress(argv[1], atoi(argv[2]), &server);

  char buf[BUFSIZ];
  while (1) {
    memset(buf, 0, BUFSIZ);
    printf(">>> ");
    if (!fgets(buf, BUFSIZ, stdin))
      break;

    printf("%s: sending ...\n", argv[0]);
    client->sendTo(client, (void *)buf, strlen(buf), &server);
    printf("%s: sent, listenning ...\n", argv[0]);
    client->recv(client, (void *)buf, BUFSIZ);
    printf("%s: received\n", argv[0]);
    printf("%s\n", buf);
    
  }
}
