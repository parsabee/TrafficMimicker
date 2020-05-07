#include "Con/Connection.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

void initAddress(struct sockaddr_in *addr, int portnumber,
                        struct in_addr ipAddr) {
  memset(addr, 0, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(portnumber);
  addr->sin_addr = ipAddr;
}

static int hostToNet(char *hostname, struct in_addr *addr) {
  struct hostent *host = gethostbyname(hostname);
  if (!host) {
    herror(NULL);
    return 0;
  }
  
  *addr = *((struct in_addr *)(host->h_addr));
  return 1;
}

int getInetAddr(char *name, struct in_addr *inaddr) {
  int status = 0;
#ifdef IPV4
  if (!(status = inet_aton(name, inaddr)))
    fprintf(stderr, "invalid address -- %s\n", name);
#else
  status = hostToNet(name, inaddr);
#endif //IP_ADDR
  return status;
}

int resolveAddress(char *name, int port, struct sockaddr_in *addr) {
  struct in_addr inaddr;
  if (!getInetAddr(name, &inaddr))
    return 0;

  initAddress(addr, port, inaddr);
  return 1;
}
