#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>

static const unsigned long BLINK_INTERVAL_MS = 300;

enum LedMode {
  LED_MODE_OFF,
  LED_MODE_ON,
  LED_MODE_PULSE,
  LED_MODE_BLINK
};

struct LedStatus {
  LedMode mode;
  bool pulseActive;
  bool outputIsHigh;

  unsigned long offAt;
  unsigned long pulseTimeMs;

  unsigned long blinkIntervalMs;
  unsigned long nextBlinkToggleAt;
};

void ledBegin(LedStatus &led, unsigned long defaultPulseTimeMs);
void ledUpdate(LedStatus &led, unsigned long now);
void ledPulse(LedStatus &led, unsigned long now);
void ledSetOn(LedStatus &led);
void ledSetOff(LedStatus &led);
void ledSetPulseTime(LedStatus &led, unsigned long pulseTimeMs);
void ledEnterBlinkMode(LedStatus &led, unsigned long now);
void ledExitBlinkMode(LedStatus &led);

void ledSetBlinkInterval(LedStatus &led, unsigned long blinkIntervalMs);

#endif