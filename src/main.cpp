#include <Arduino.h>

void setup() {
  // configuring pin mode: output
  pinMode(LED_BUILTIN, OUTPUT);

  // make sure the serial terminal replicates this rate
  Serial.begin(9600);

  delay(200);

  Serial.println("================================");
  Serial.println("Step 1 firmware booted");
  Serial.println("LED blink + UART TX test");
  Serial.println("Baud: 9600");
  Serial.println("================================");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED ON");
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED OFF");
  delay(500);
}