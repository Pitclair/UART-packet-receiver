#ifndef UART_APP_H
#define UART_APP_H

// Command IDs
static const byte CMD_PULSE_LED          = 0x01;
static const byte CMD_SET_LED_STATE      = 0x02;
static const byte CMD_SET_PULSE_TIME     = 0x03;
static const byte CMD_ENTER_BLINK        = 0x04;
static const byte CMD_EXIT_BLINK         = 0x05;
static const byte CMD_SET_BLINK_INTERVAL = 0x06;
static const byte CMD_GET_STATUS         = 0x07;

// Response IDs
static const byte RSP_ACK    = 0x80;
static const byte RSP_ERROR  = 0x81;
static const byte RSP_STATUS = 0x82;

// Error codes
static const byte ERR_UNKNOWN_COMMAND = 0x01;
static const byte ERR_BAD_LENGTH      = 0x02;
static const byte ERR_INVALID_VALUE   = 0x03;
static const byte ERR_CHECKSUM        = 0x04;
static const byte ERR_TIMEOUT         = 0x05;
static const byte ERR_LEN_TOO_LARGE   = 0x06;


void appSetup();
void appLoop();

#endif