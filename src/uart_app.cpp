#include <Arduino.h>
#include "uart_app.h"
#include "led_status.h"
#include "packet.h"
#include "parser.h"

static LedStatus gLed;
static PacketParser gParser;


static void printHexByte(byte value) {
  if (value < 16) {
    Serial.print("0");
  }
  Serial.print(value, HEX);
}

static void printPacket(const Packet &packet) {
  Serial.print("Packet: AA ");
  printHexByte(packet.len);
  Serial.print(" ");
  printHexByte(packet.cmd);

  for (byte i = 0; i < packet.len; i++) {
    Serial.print(" ");
    printHexByte(packet.payload[i]);
  }

  Serial.print(" ");
  printHexByte(packetComputeChecksum(packet));
  Serial.println();
}

static void sendPacket(byte cmd, const byte *payload, byte len) {
  byte checksum = 0;

  checksum ^= len;
  checksum ^= cmd;
  for (byte i = 0; i < len; i++) {
    checksum ^= payload[i];
  }

  Serial.write(START_BYTE);
  Serial.write(len);
  Serial.write(cmd);

  for (byte i = 0; i < len; i++) {
    Serial.write(payload[i]);
  }

  Serial.write(checksum);
}

static void sendAck(byte originalCmd) {
  const byte payload[1] = { originalCmd };
  sendPacket(RSP_ACK, payload, 1);
}

static void sendError(byte originalCmd, byte errorCode) {
  const byte payload[2] = { originalCmd, errorCode };
  sendPacket(RSP_ERROR, payload, 2);
}

static void sendStatus(byte modeValue, byte ledValue, byte blinkIntervalUnits) {
  const byte payload[3] = { modeValue, ledValue, blinkIntervalUnits };
  sendPacket(RSP_STATUS, payload, 3);
}

static void handlePacket(const Packet &packet, unsigned long now) {
  Serial.print("Valid packet. CMD=0x");
  printHexByte(packet.cmd);
  Serial.print(" LEN=");
  Serial.println(packet.len);

  if (packet.cmd == CMD_PULSE_LED) {
    if (packet.len != 0) {
      Serial.println("Error: CMD 0x01 expects LEN=0");
      sendError(packet.cmd, ERR_BAD_LENGTH);
      return;
    }

    Serial.println("Action: pulse LED");
    ledPulse(gLed, now);
    sendAck(packet.cmd);
    return;
  }

  if (packet.cmd == CMD_SET_LED_STATE) {
    if (packet.len != 1) {
      Serial.println("Error: CMD 0x02 expects LEN=1");
      sendError(packet.cmd, ERR_BAD_LENGTH);
      return;
    }

    if (packet.payload[0] == 0x00) {
      Serial.println("Action: LED OFF");
      ledSetOff(gLed);
      sendAck(packet.cmd);
    } else if (packet.payload[0] == 0x01) {
      Serial.println("Action: LED ON");
      ledSetOn(gLed);
      sendAck(packet.cmd);
    } else {
      Serial.println("Error: LED state must be 0x00 or 0x01");
      sendError(packet.cmd, ERR_INVALID_VALUE);
    }
    return;
  }

  if (packet.cmd == CMD_SET_PULSE_TIME) {
    if (packet.len != 1) {
      Serial.println("Error: CMD 0x03 expects LEN=1");
      sendError(packet.cmd, ERR_BAD_LENGTH);
      return;
    }

    unsigned long pulseTimeMs = (unsigned long)packet.payload[0] * 10UL;
    ledSetPulseTime(gLed, pulseTimeMs);

    Serial.print("Action: LED pulse time set to ");
    Serial.print(gLed.pulseTimeMs);
    Serial.println(" ms");

    sendAck(packet.cmd);
    return;
  }

  if (packet.cmd == CMD_ENTER_BLINK) {
    if (packet.len != 0) {
      Serial.println("Error: CMD 0x04 expects LEN=0");
      sendError(packet.cmd, ERR_BAD_LENGTH);
      return;
    }

    Serial.println("Action: enter blink mode");
    ledEnterBlinkMode(gLed, now);
    sendAck(packet.cmd);
    return;
  }

  if (packet.cmd == CMD_EXIT_BLINK) {
    if (packet.len != 0) {
      Serial.println("Error: CMD 0x05 expects LEN=0");
      sendError(packet.cmd, ERR_BAD_LENGTH);
      return;
    }

    Serial.println("Action: exit blink mode");
    ledExitBlinkMode(gLed);
    sendAck(packet.cmd);
    return;
  }

  if (packet.cmd == CMD_SET_BLINK_INTERVAL) {
    if (packet.len != 1) {
      Serial.println("Error: CMD 0x06 expects LEN=1");
      sendError(packet.cmd, ERR_BAD_LENGTH);
      return;
    }

    unsigned long blinkIntervalMs = (unsigned long)packet.payload[0] * 10UL;
    ledSetBlinkInterval(gLed, blinkIntervalMs);

    Serial.print("Action: blink interval set to ");
    Serial.print(gLed.blinkIntervalMs);
    Serial.println(" ms");

    sendAck(packet.cmd);
    return;
  }

  if (packet.cmd == CMD_GET_STATUS) {
    if (packet.len != 0) {
      Serial.println("Error: CMD 0x07 expects LEN=0");
      sendError(packet.cmd, ERR_BAD_LENGTH);
      return;
    }

    byte modeValue = (byte)gLed.mode;
    byte ledValue = gLed.outputIsHigh ? 1 : 0;
    byte blinkUnits = (byte)(gLed.blinkIntervalMs / 10UL);

    Serial.println("Action: send status");
    sendStatus(modeValue, ledValue, blinkUnits);
    return;
  }

  Serial.println("Error: Unknown command");
  sendError(packet.cmd, ERR_UNKNOWN_COMMAND);
}

void appSetup() {
  ledBegin(gLed, 200);
  parserInit(gParser);

  Serial.begin(9600);
  delay(200);

  Serial.println("================================");
  Serial.println("Step 6 ready: command/response UART");
  Serial.println("Format: [AA][LEN][CMD][PAYLOAD...][CHK]");
  Serial.println("CHK = LEN ^ CMD ^ all PAYLOAD bytes");
  Serial.println("CMD 01: pulse LED");
  Serial.println("CMD 02: set LED state, payload 00=OFF 01=ON");
  Serial.println("CMD 03: set pulse time, payload = value * 10 ms");
  Serial.println("CMD 04: enter blink mode");
  Serial.println("CMD 05: exit blink mode");
  Serial.println("CMD 06: set blink interval, payload = value * 10 ms");
  Serial.println("CMD 07: get status");
  Serial.println("RSP 80: ACK");
  Serial.println("RSP 81: ERROR");
  Serial.println("RSP 82: STATUS");
  Serial.println("================================");
}

void appLoop() {
  unsigned long now = millis();

  ParseResult timeoutResult = parserCheckTimeout(gParser, now);
  if (timeoutResult == PARSE_ERROR) {
    Serial.print("Parser error: ");
    Serial.println(parserErrorToString(gParser.lastError));

    if (gParser.lastError == PARSER_TIMEOUT) {
      sendError(0x00, ERR_TIMEOUT);
    }
  }

  while (Serial.available() > 0) {
    Packet packet;
    byte incomingByte = (byte)Serial.read();

    ParseResult result = parserProcessByte(gParser, incomingByte, packet);

    if (result == PARSE_PACKET_READY) {
      printPacket(packet);
      Serial.println("Checksum OK");
      handlePacket(packet, now);
    } else if (result == PARSE_ERROR) {
      Serial.print("Parser error: ");
      Serial.println(parserErrorToString(gParser.lastError));

      if (gParser.lastError == PARSER_CHECKSUM_MISMATCH) {
        sendError(0x00, ERR_CHECKSUM);

        Serial.print("Expected checksum: 0x");
        printHexByte(gParser.lastExpectedChecksum);
        Serial.println();

        Serial.print("Received checksum: 0x");
        printHexByte(gParser.lastReceivedChecksum);
        Serial.println();
      } else if (gParser.lastError == PARSER_LEN_TOO_LARGE) {
        sendError(0x00, ERR_LEN_TOO_LARGE);
      } else if (gParser.lastError == PARSER_TIMEOUT) {
        sendError(0x00, ERR_TIMEOUT);
      }
    }
  }

  ledUpdate(gLed, now);
}