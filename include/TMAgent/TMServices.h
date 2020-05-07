#ifndef TMSERVICES_H
#define TMSERVICES_H

#include "packet.h"

struct packet *makePacket(int gran, char *srcip, int srcport, char *dstip,
                          int dstport, int tcp, int num_packets);

void initAddr(struct sockaddr_in *addr, struct packet *p, int issrc);

struct packet *parseArgs(int argc, char *argv[], char *usage);

#endif