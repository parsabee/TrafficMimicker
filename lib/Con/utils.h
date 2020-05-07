#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

// prints the ascii ipv4 and port of addr to fp
void printAddr(struct sockaddr_in *addr, FILE *fp);

#endif
