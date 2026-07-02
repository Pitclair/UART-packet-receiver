#include "led_status.h"

void ledBegin(LedStatus &led, unsigned long defaultPulseTimeMs) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  led.pulseActive = false;
  led.offAt = 0;
  led.pulseTimeMs = defaultPulseTimeMs;
}

void ledUpdate(LedStatus &led, unsigned long now) {
  if (led.pulseActive && now >= led.offAt) {
    digitalWrite(LED_BUILTIN, LOW);
    led.pulseActive = false;
  }
}

void ledPulse(LedStatus &led, unsigned long now) {
  digitalWrite(LED_BUILTIN, HIGH);
  led.pulseActive = true;
  led.offAt = now + led.pulseTimeMs;
}

void ledSetOn(LedStatus &led) {
  digitalWrite(LED_BUILTIN, HIGH);
  led.pulseActive = false;
}

void ledSetOff(LedStatus &led) {
  digitalWrite(LED_BUILTIN, LOW);
  led.pulseActive = false;
}

void ledSetPulseTime(LedStatus &led, unsigned long pulseTimeMs) {
  if (pulseTimeMs == 0) {
    pulseTimeMs = 10;
  }
  led.pulseTimeMs = pulseTimeMs;
}