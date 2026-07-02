#ifndef LED_STATUS_H
#define LED_STATUS_H

#include <Arduino.h>

struct LedStatus {
  bool pulseActive;
  unsigned long offAt;
  unsigned long pulseTimeMs;
};

void ledBegin(LedStatus &led, unsigned long defaultPulseTimeMs);
void ledUpdate(LedStatus &led, unsigned long now);
void ledPulse(LedStatus &led, unsigned long now);
void ledSetOn(LedStatus &led);
void ledSetOff(LedStatus &led);
void ledSetPulseTime(LedStatus &led, unsigned long pulseTimeMs);

#endif