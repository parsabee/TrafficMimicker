#include "utils.h"

#include <arpa/inet.h>

void sprintAddr(struct sockaddr_in *addr, char *s) {
  char *buf = inet_ntoa(addr->sin_addr);
  int port = ntohs(addr->sin_port);
  sprintf(s, "%s:%d\n", buf, port);
}

void printAddr(struct sockaddr_in *addr, FILE *fp) {
  char *buf = inet_ntoa(addr->sin_addr);
  int port = ntohs(addr->sin_port);
  fprintf(fp, "%s:%d\n", buf, port);
}
