#ifndef UART_APP_H
#define UART_APP_H

static const byte CMD_PULSE_LED = 0x01;
static const byte CMD_SET_LED_STATE = 0x02;
static const byte CMD_SET_PULSE_TIME = 0x03;
static const byte CMD_ENTER_BLINK = 0x04;
static const byte CMD_EXIT_BLINK  = 0x05;
static const byte CMD_SET_BLINK_INTERVAL = 0x06;


void appSetup();
void appLoop();

#endif