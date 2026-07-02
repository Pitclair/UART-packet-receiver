#include "parser.h"

static void parserResetWorkingState(PacketParser &parser) {
  parser.state = WAIT_START;
  parser.payloadLen = 0;
  parser.cmdByte = 0;
  parser.payloadIndex = 0;
  parser.runningChecksum = 0;
}

void parserInit(PacketParser &parser) {
  parser.lastError = PARSER_OK;
  parser.lastExpectedChecksum = 0;
  parser.lastReceivedChecksum = 0;
  parserResetWorkingState(parser);
}

ParseResult parserProcessByte(PacketParser &parser, byte incomingByte, Packet &outPacket) {
  switch (parser.state) {
    case WAIT_START:
      if (incomingByte == START_BYTE) {
        parser.lastError = PARSER_OK;
        parser.runningChecksum = 0;
        parser.payloadIndex = 0;
        parser.state = WAIT_LEN;
      }
      return PARSE_NONE;

    case WAIT_LEN:
      if (incomingByte > MAX_PAYLOAD) {
        parser.lastError = PARSER_LEN_TOO_LARGE;
        parserResetWorkingState(parser);
        return PARSE_ERROR;
      }

      parser.payloadLen = incomingByte;
      parser.runningChecksum ^= incomingByte;
      parser.state = WAIT_CMD;
      return PARSE_NONE;

    case WAIT_CMD:
      parser.cmdByte = incomingByte;
      parser.runningChecksum ^= incomingByte;

      if (parser.payloadLen == 0) {
        parser.state = WAIT_CHECKSUM;
      } else {
        parser.payloadIndex = 0;
        parser.state = WAIT_PAYLOAD;
      }
      return PARSE_NONE;

    case WAIT_PAYLOAD:
      parser.payload[parser.payloadIndex] = incomingByte;
      parser.runningChecksum ^= incomingByte;
      parser.payloadIndex++;

      if (parser.payloadIndex >= parser.payloadLen) {
        parser.state = WAIT_CHECKSUM;
      }
      return PARSE_NONE;

    case WAIT_CHECKSUM:
      parser.lastReceivedChecksum = incomingByte;
      parser.lastExpectedChecksum = parser.runningChecksum;

      if (incomingByte == parser.runningChecksum) {
        outPacket.len = parser.payloadLen;
        outPacket.cmd = parser.cmdByte;

        for (byte i = 0; i < parser.payloadLen; i++) {
          outPacket.payload[i] = parser.payload[i];
        }

        parser.lastError = PARSER_OK;
        parserResetWorkingState(parser);
        return PARSE_PACKET_READY;
      } else {
        parser.lastError = PARSER_CHECKSUM_MISMATCH;
        parserResetWorkingState(parser);
        return PARSE_ERROR;
      }
  }

  parser.lastError = PARSER_OK;
  parserResetWorkingState(parser);
  return PARSE_NONE;
}

const char* parserErrorToString(ParserError error) {
  switch (error) {
    case PARSER_LEN_TOO_LARGE:
      return "LEN too large";
    case PARSER_CHECKSUM_MISMATCH:
      return "Checksum mismatch";
    case PARSER_OK:
    default:
      return "No error";
  }
}