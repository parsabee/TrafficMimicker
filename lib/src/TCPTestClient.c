#include "TCPClient.h"
#include "Connection.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#define USAGE "Usage: tcp-echo hostname hostport [clientname clientport]"
void error(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 5)
    error(USAGE);
  
  struct sockaddr_in clientaddr;
  struct sockaddr_in *addrptr = NULL;
  if (argc == 5) {
    resolveAddress(argv[3], atoi(argv[4]), &clientaddr);
    printf("%s: binding to %s %s\n", argv[0], argv[3], argv[4]);
    addrptr = &clientaddr;
  }

  struct sockaddr_in server;
  resolveAddress(argv[1], atoi(argv[2]), &server);

  const TCPClient *client = create_TCPClient(&server, addrptr);
  if (!client)
    error("failed to create client");

  if (client->isBound(client)) {
    client->getClientAddr(client, &clientaddr);
    printf("%s: bound to ", argv[0]);
    printAddr(&clientaddr, stdout);
  }

  char buf[BUFSIZ];
  while (1) {
    memset(buf, 0, BUFSIZ);
    printf(">>> ");
    if (!fgets(buf, BUFSIZ, stdin))
      break;

    printf("%s: sending ...\n", argv[0]);
    client->send(client, (void *)buf, strlen(buf));
    printf("%s: sent, listenning ...\n", argv[0]);
    if (!client->recv(client, (void *)buf, BUFSIZ))
      error("error in recv()");

    printf("%s: received\n", argv[0]);
    printf("%s\n", buf);
  }
  client->destroy(client);
}
