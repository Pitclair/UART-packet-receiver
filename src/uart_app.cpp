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

static void handlePacket(const Packet &packet, unsigned long now) {
  Serial.print("Valid packet. CMD=0x");
  printHexByte(packet.cmd);
  Serial.print(" LEN=");
  Serial.println(packet.len);

  if (packet.cmd == 0x01) {
    if (packet.len != 0) {
      Serial.println("Error: CMD 0x01 expects LEN=0");
      return;
    }

    Serial.println("Action: pulse LED");
    ledPulse(gLed, now);
    return;
  }

  if (packet.cmd == 0x02) {
    if (packet.len != 1) {
      Serial.println("Error: CMD 0x02 expects LEN=1");
      return;
    }

    if (packet.payload[0] == 0x00) {
      Serial.println("Action: LED OFF");
      ledSetOff(gLed);
    } else if (packet.payload[0] == 0x01) {
      Serial.println("Action: LED ON");
      ledSetOn(gLed);
    } else {
      Serial.println("Error: LED state must be 0x00 or 0x01");
    }
    return;
  }

  if (packet.cmd == 0x03) {
    if (packet.len != 1) {
      Serial.println("Error: CMD 0x03 expects LEN=1");
      return;
    }

    unsigned long pulseTimeMs = (unsigned long)packet.payload[0] * 10UL;
    ledSetPulseTime(gLed, pulseTimeMs);

    Serial.print("Action: LED pulse time set to ");
    Serial.print(gLed.pulseTimeMs);
    Serial.println(" ms");
    return;
  }

  if (packet.cmd == CMD_ENTER_BLINK) {
    if (packet.len != 0) {
      Serial.println("Error: CMD 0x04 expects LEN=0");
      return;
    }

    Serial.println("Action: Enter blink mode");
    ledEnterBlinkMode(gLed, now);
    return;
  }

  if (packet.cmd == CMD_EXIT_BLINK) {
    if (packet.len != 0) {
      Serial.println("Error: CMD 0x05 expects LEN=0");
      return;
    }

    Serial.println("Action: Exit blink mode");
    ledExitBlinkMode(gLed);
    return;
  }

  Serial.println("Error: Unknown command");
}

void appSetup() {
  ledBegin(gLed, 200);
  parserInit(gParser);

  Serial.begin(9600);
  delay(200);

  Serial.println("================================");
  Serial.println("Format: [AA][LEN][CMD][PAYLOAD...][CHK]");
  Serial.println("CHK = LEN ^ CMD ^ all PAYLOAD bytes");
  Serial.println("CMD 01: pulse LED");
  Serial.println("CMD 02: set LED state, payload 00=OFF 01=ON");
  Serial.println("CMD 03: set pulse time, payload = value * 10 ms");
  Serial.println("CMD 04: enter blink mode");
  Serial.println("CMD 05: exit blink mode");
  Serial.println("================================");
}

void appLoop() {
  unsigned long now = millis();

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
        Serial.print("Expected checksum: 0x");
        printHexByte(gParser.lastExpectedChecksum);
        Serial.println();

        Serial.print("Received checksum: 0x");
        printHexByte(gParser.lastReceivedChecksum);
        Serial.println();
      }
    }
  }

  ledUpdate(gLed, now);
}