#include <Arduino.h>

const unsigned long LED_ON_TIME_MS = 200;
unsigned long ledOffAt = 0;
bool ledIsOn = false;


const byte START_BYTE = 0xAA;

byte packetBuffer[3];
byte packetIndex = 0;


void pulseLed(unsigned long now) {
    // nonblocking timing pattern for the blinky
    digitalWrite(LED_BUILTIN, HIGH);
    ledIsOn = true;
    ledOffAt = now + LED_ON_TIME_MS;
}

void updateLed(unsigned long now) {
  if (ledIsOn && now >= ledOffAt) {
    digitalWrite(LED_BUILTIN, LOW);
    ledIsOn = false;
  }
}


byte computeChecksum(byte start, byte cmd) {
  return start ^ cmd;
}


void handleCommand(byte cmd, unsigned long now) {
  switch (cmd) {
    case 0x01:
      Serial.println("Command 0x01: Led pulse");
      pulseLed(now);
      break;
    case 0x02:
      Serial.println("Command 0x02: Led on");
      digitalWrite(LED_BUILTIN, HIGH);
      ledIsOn = false;
      break;
    case 0x03:
      Serial.println("Command 0x03: Led off");
      digitalWrite(LED_BUILTIN, LOW);
      ledIsOn = false;
      break;
    default:
      Serial.print("Unknown command: ");
      break;
  }
}

void processIncomingByte(byte incomingByte, unsigned long now) {

  // detect first byte
  if (packetIndex == 0) {
    if (incomingByte == START_BYTE) {
      packetBuffer[packetIndex] = incomingByte;
      packetIndex = 1;
      Serial.println("Start byte detected");
    } else {
      Serial.print("Ignored byte: 0x");
      if (incomingByte < 16) {
        Serial.print("0");
      }
      Serial.println(incomingByte, HEX);
    }
    return;
  }

  // write the command
  packetBuffer[packetIndex] = incomingByte;
  packetIndex++;

  if (packetIndex == 3) {
    byte startByte = packetBuffer[0];
    byte cmdByte = packetBuffer[1];
    byte receivedChecksum = packetBuffer[2];
    byte expectedChecksum = computeChecksum(startByte, cmdByte);  
    Serial.print("Packet received: ");
    Serial.print("0x");
    if (startByte < 16) Serial.print("0");
    Serial.print(startByte, HEX);
    Serial.print(" 0x");
    if (cmdByte < 16) Serial.print("0");
    Serial.print(cmdByte, HEX);
    Serial.print(" 0x");
    if (receivedChecksum < 16) Serial.print("0");
    Serial.println(receivedChecksum, HEX);

    if (receivedChecksum == expectedChecksum) {
      Serial.println("Checksum OK");
      handleCommand(cmdByte, now);
    } else {
      Serial.print("Checksum FAIL. Expected 0x");
      if (expectedChecksum < 16) {
        Serial.print("0");
      }
      Serial.println(expectedChecksum, HEX);
    }

    packetIndex = 0;
  }
}



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  delay(200);

  Serial.println("================================");
  Serial.println("fixed UART packets");
  Serial.println("Packet format: [0xAA][CMD][CHK]");
  Serial.println("CHK = START ^ CMD");
  Serial.println("CMD 0x01 = pulse LED");
  Serial.println("CMD 0x02 = LED ON");
  Serial.println("CMD 0x03 = LED OFF");
  Serial.println("================================");
}

void loop() {
  unsigned long now = millis();

  // this loop is done for every byte. so if i have typed a multiple letter word,
  // it will be echoed back one letter at a time. 
  if (Serial.available() > 0) {
    byte incomingByte = (byte)Serial.read();
    processIncomingByte(incomingByte, now);
  }

  updateLed(now);
}