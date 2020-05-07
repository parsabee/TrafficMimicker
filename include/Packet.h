//===----------------------------------------------------------------------===//
// Packet.h
// ---------------------------------------------------------------------------//
// This file defines the packet sent to TMAgents, 
// informing them of the attributes of their connection
//
// Author: Parsa Bagheri
//===----------------------------------------------------------------------===//

#ifndef PACKET_H
#define PACKET_H

#include <arpa/inet.h>

#define packed __attribute__((packed))
#define BUFF 64            // number of Bytes send in a packet
#define SERVER_PORT 2222   // server(TMAgents) port number

// facility to determine size of packet
#define SIZEOFPACKET(P) (sizeof(struct packet) + ((P)->num_packets * sizeof(struct transfer)))

// Deep copying packet pointer `SRC' into packet pointer `DST'
#define DEEPCOPY(DST, SRC) {\
  *(DST) = *(SRC);\
  for(int i = 0; i < (SRC)->num_packets; i++) {\
    (DST)->packets[i] = (SRC)->packets[i];\
  }\
}

enum direction { incoming, outgoing };

enum control { ERR, ACK };
typedef enum control control_t;
struct control_msg {
  control_t type;
};

struct transfer {
  enum direction dir;
  int size;
} packed;

struct packet {
  int gran;                     //granularity
  struct in_addr srcip;         //src ip address(network address)
  int srcport;                  //src port
  struct in_addr dstip;         //dst ip address(network address)
  int dstport;                  //src port
  int tcp;                      //1 if connection is tcp 0 otherwise
  int num_packets;              //number of packets
  struct transfer packets[0];   //0-length array, describing size and direction of packet
} packed;

typedef struct info_packet InfoPacket;
struct info_packet {
  int init;
  struct packet thePacket;
} packed;

#endif
