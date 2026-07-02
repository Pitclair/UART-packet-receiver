#ifndef UART_APP_H
#define UART_APP_H

static const byte CMD_ENTER_BLINK = 0x04;
static const byte CMD_EXIT_BLINK  = 0x05;

void appSetup();
void appLoop();

#endif