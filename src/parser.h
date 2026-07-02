#ifndef PARSER_H
#define PARSER_H

#include <Arduino.h>
#include "packet.h"

enum ParseResult {
  PARSE_NONE,
  PARSE_PACKET_READY,
  PARSE_ERROR
};

enum ParserState {
  WAIT_START,
  WAIT_LEN,
  WAIT_CMD,
  WAIT_PAYLOAD,
  WAIT_CHECKSUM
};

enum ParserError {
  PARSER_OK,
  PARSER_LEN_TOO_LARGE,
  PARSER_CHECKSUM_MISMATCH
};

struct PacketParser {
  ParserState state;
  ParserError lastError;

  byte payloadLen;
  byte cmdByte;
  byte payload[MAX_PAYLOAD];
  byte payloadIndex;
  byte runningChecksum;

  byte lastExpectedChecksum;
  byte lastReceivedChecksum;
};

void parserInit(PacketParser &parser);
ParseResult parserProcessByte(PacketParser &parser, byte incomingByte, Packet &outPacket);
const char* parserErrorToString(ParserError error);

#endif