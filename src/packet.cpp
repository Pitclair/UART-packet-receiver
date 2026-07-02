#include "packet.h"

byte packetComputeChecksum(const Packet &packet) {
  byte checksum = 0;

  checksum ^= packet.len;
  checksum ^= packet.cmd;

  for (byte i = 0; i < packet.len; i++) {
    checksum ^= packet.payload[i];
  }

  return checksum;
}