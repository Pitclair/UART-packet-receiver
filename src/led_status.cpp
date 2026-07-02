#include "led_status.h"

void ledBegin(LedStatus &led, unsigned long defaultPulseTimeMs) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  led.mode = LED_MODE_OFF;
  led.pulseActive = false;
  led.outputIsHigh = false;

  led.offAt = 0;
  led.pulseTimeMs = defaultPulseTimeMs;

  led.blinkIntervalMs = BLINK_INTERVAL_MS;
  led.nextBlinkToggleAt = 0;
}

void ledUpdate(LedStatus &led, unsigned long now) {
  if (led.mode == LED_MODE_PULSE) {
    if (led.pulseActive && now >= led.offAt) {
      digitalWrite(LED_BUILTIN, LOW);
      led.outputIsHigh = false;
      led.pulseActive = false;
      led.mode = LED_MODE_OFF;
    }
    return;
  }

  if (led.mode == LED_MODE_BLINK) {
    if (now >= led.nextBlinkToggleAt) {
      led.outputIsHigh = !led.outputIsHigh;
      digitalWrite(LED_BUILTIN, led.outputIsHigh ? HIGH : LOW);
      led.nextBlinkToggleAt = now + led.blinkIntervalMs;
    }
    return;
  }
}

void ledPulse(LedStatus &led, unsigned long now) {
  led.mode = LED_MODE_PULSE;
  led.pulseActive = true;
  led.offAt = now + led.pulseTimeMs;

  digitalWrite(LED_BUILTIN, HIGH);
  led.outputIsHigh = true;
}

void ledSetOn(LedStatus &led) {
  led.mode = LED_MODE_ON;
  led.pulseActive = false;

  digitalWrite(LED_BUILTIN, HIGH);
  led.outputIsHigh = true;
}

void ledSetOff(LedStatus &led) {
  led.mode = LED_MODE_OFF;
  led.pulseActive = false;

  digitalWrite(LED_BUILTIN, LOW);
  led.outputIsHigh = false;
}

void ledSetPulseTime(LedStatus &led, unsigned long pulseTimeMs) {
  if (pulseTimeMs == 0) {
    pulseTimeMs = 10;
  }
  led.pulseTimeMs = pulseTimeMs;
}

void ledEnterBlinkMode(LedStatus &led, unsigned long now) {
  led.mode = LED_MODE_BLINK;
  led.pulseActive = false;

  digitalWrite(LED_BUILTIN, HIGH);
  led.outputIsHigh = true;
  led.nextBlinkToggleAt = now + led.blinkIntervalMs;
}

void ledExitBlinkMode(LedStatus &led) {
  led.mode = LED_MODE_OFF;
  led.pulseActive = false;

  digitalWrite(LED_BUILTIN, LOW);
  led.outputIsHigh = false;
}

void ledSetBlinkInterval(LedStatus &led, unsigned long blinkIntervalMs) {
  if (blinkIntervalMs == 0) {
    blinkIntervalMs = 10;
  }
  led.blinkIntervalMs = blinkIntervalMs;
}