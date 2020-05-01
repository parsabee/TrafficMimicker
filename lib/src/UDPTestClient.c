#include "UDPClient.h"
#include "Connection.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#define USAGE "Usage: udp-echo clientname clientport hostname hostport"
void error(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 5)
    error(USAGE);

  const UDPClient *client = create_UDPClient();
  if (!client)
    error("failed to create client");

  struct sockaddr_in clientaddr;
  resolveAddress(argv[1], atoi(argv[2]), &clientaddr);
  
#ifdef BIND
  printf("%s: binding to %s %s ...\n", argv[0], argv[1], argv[2]);
  if (!client->bind(client, &clientaddr))
    error("failed to bind");
  printf("%s: done\n", argv[0]);
#endif

  struct sockaddr_in server;
  resolveAddress(argv[3], atoi(argv[4]), &server);

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
