#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>

static const byte START_BYTE = 0xAA;
static const byte MAX_PAYLOAD = 8;

struct Packet {
  byte len;
  byte cmd;
  byte payload[MAX_PAYLOAD];
};

byte packetComputeChecksum(const Packet &packet);

#endif