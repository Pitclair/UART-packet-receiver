#include <Arduino.h>

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
  // this loop is done for every byte. so if i have typed a multiple letter word,
  // it will be echoed back one letter at a time. 
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();

    digitalWrite(LED_BUILTIN, HIGH);

    delay(500);

    Serial.print("RX: ");
    Serial.print((char)incomingByte);
    Serial.print("   ASCII: ");
    Serial.println(incomingByte);

    digitalWrite(LED_BUILTIN, LOW);
  }
}