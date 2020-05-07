#include "packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct packet *makePacket(int gran, char *srcip, int srcport, char *dstip,
                          int dstport, int tcp, int num_packets) {

  int nbytes = sizeof(struct packet) + (sizeof(struct transfer) * num_packets);
  struct packet *p = (struct packet *)malloc(nbytes);
  if (!p)
    return NULL;

  p->gran = gran;
  struct in_addr addr;
  inet_aton(srcip, &addr);
  p->srcip = addr;
  p->srcport = htons(srcport);
  inet_aton(dstip, &addr);
  p->dstip = addr;
  p->dstport = htons(dstport);
  p->tcp = tcp;
  p->num_packets = num_packets;
  return p;
}

void initAddr(struct sockaddr_in *addr, struct packet *p, int issrc) {
  memset(addr, 0, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  if (issrc) {
    addr->sin_port = p->srcport;
    addr->sin_addr = p->srcip;
  } else {
    addr->sin_port = p->dstport;
    addr->sin_addr = p->dstip;
  }
}

static int error_atoi(char *arg) {
  int ret = atoi(arg);
  if (!ret)
    fprintf(stderr, "%s: not a non-zero number\n", arg);
  return ret;
}

struct packet *parseArgs(int argc, char *argv[], char *usage) {
  if (argc < 10 || argc % 2) {
    fprintf(stderr, "%s\n", usage);
    return NULL;
  }

#ifdef DEBUG
  char *protocol = atoi(argv[6]) ? "tcp" : "udp";
  fprintf(stderr,
          "%s: connection {srcip: %s, srcport: %s, dstip: %s, dstport: %s, "
          "protocol: %s, #packets: %s}\n",
          argv[0], argv[2], argv[3], argv[4], argv[5], protocol, argv[7]);
#endif

  int gran = error_atoi(argv[1]);
  int srcport = error_atoi(argv[3]);
  int dstport = error_atoi(argv[5]);
  if (gran == 0 || srcport == 0 || dstport == 0)
    return NULL;

  struct packet *p = makePacket(gran, argv[2], srcport, argv[4], dstport,
                                atoi(argv[6]), atoi(argv[7]));
  if (!p)
    return NULL;

  int i, j;
  for (i = 8, j = 0; i < argc; i += 2, j++) {
#ifdef DEBUG
    fprintf(stderr, "%s: packet #%d: (%s, %sbytes)\n", argv[0], j + 1, argv[i],
            argv[i + 1]);
#endif
    if (strcmp(argv[i], "incoming") == 0) {
      p->packets[j].dir = incoming;
    } else if (strcmp(argv[i], "outgoing") == 0) {
      p->packets[j].dir = outgoing;
    } else {
      fprintf(stderr, "%s: unrecognized direction -- %s\n", argv[0], argv[i]);
      free(p);
      return NULL;
    }
    p->packets[j].size = error_atoi(argv[i + 1]);
    if (!p->packets[j].size) {
      free(p);
      return NULL;
    }
  }

  return p;
}
