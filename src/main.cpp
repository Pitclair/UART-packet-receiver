#include <Arduino.h>

const unsigned long LED_ON_TIME_MS = 200;
unsigned long ledOffAt = 0;
bool ledIsOn = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  delay(200);

  Serial.println("================================");
  Serial.println("Type characters in the Serial Terminal.");
  Serial.println("The Arduino will echo each byte back.");
  Serial.println("================================");
}

void loop() {
  unsigned long now = millis();

  // this loop is done for every byte. so if i have typed a multiple letter word,
  // it will be echoed back one letter at a time. 
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();

    // nonblocking timing pattern for the blinky
    digitalWrite(LED_BUILTIN, HIGH);
    ledIsOn = true;
    ledOffAt = now + LED_ON_TIME_MS;

    Serial.print("RX: ");
    Serial.print((char)incomingByte);
    Serial.print("   ASCII: ");
    Serial.println(incomingByte);
  }

  if (ledIsOn && now >= ledOffAt) {
    digitalWrite(LED_BUILTIN, LOW);
    ledIsOn = false;
  }
}