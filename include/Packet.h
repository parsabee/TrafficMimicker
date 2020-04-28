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
#include <stdint.h>

#define BUFF 64            // number of Bytes send in a packet
#define SERVER_PORT 2222   // server(TMAgents) port number

#define packed __attribute__((packed))

typedef struct packet InfoPacket;
struct packet {
  uint32_t granularity;
  struct in_addr src_ip;
  uint16_t src_port;
  struct in_addr dst_ip;
  uint16_t dst_port;
  char proto[32];
  uint8_t inferred_initiator_src;
  uint32_t vol_time_series;
} packed;

#endif // PACKET_H
