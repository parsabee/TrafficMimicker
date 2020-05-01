#include "TCPClient.h"
#include "Connection.h"


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

  /*
  struct sockaddr_in clientaddr;
  resolveAddress(argv[1], atoi(argv[2]), &clientaddr);
  printf("%s: binding to %s %s ...\n", argv[0], argv[1], argv[2]);
  if (!client->bind(client, &clientaddr))
    error("failed to bind");
  printf("%s: done\n", argv[0]);*/

  struct sockaddr_in server;
  resolveAddress(argv[1], atoi(argv[2]), &server);

  const TCPClient *client = create_TCPClient(&server, NULL);
  if (!client)
    error("failed to create client");

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
