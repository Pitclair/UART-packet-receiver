# UART Packet Receiver

A firmware system that receives, validates, and processes structured UART packets with support for command execution, LED control, configuration updates, blink mode, parser timeout recovery, and structured ACK / ERROR / STATUS replies. The design uses a finite-state parser, nonblocking timing with `millis()`, and a modular application structure suitable for simulation in SimulIDE or deployment on an Arduino Uno.

## Requirements

### Hardware
- Microcontroller (Arduino Uno)
- Serial terminal or USB-to-UART adapter
- LED for visible command effects
- Optional button or extra terminal-based test inputs

### Software & Tools
- **Language**: C or Arduino-style C++
- **Build System**: PlatformIO or Arduino-compatible build flow
- **Editor**: VS Code with Arduino compiler/extension
- **Simulation**: SimulIDE or Wokwi

## Project Structure

```text
├── src/
│   ├── main.cpp           # Arduino entry point: setup() and loop()
│   ├── uart_app.cpp       # Application logic, command handling, UART responses
│   ├── uart_app.h         # App-level declarations
│   ├── parser.cpp         # Packet parser finite state machine
│   ├── parser.h           # Parser states, errors, parser API
│   ├── packet.h           # Packet structure, constants, checksum helper
│   ├── led_status.cpp     # LED mode handling and nonblocking timing
│   └── led_status.h       # LED mode/state declarations
├── include/               # Optional extra headers
├── lib/                   # Optional libraries
├── test/                  # Test files
├── platformio.ini         # PlatformIO configuration
└── README.md              # Project documentation
```

## Getting Started

### 1. Setup
Ensure PlatformIO or the Arduino build flow is installed, and either connect the Arduino Uno or open the design in SimulIDE. In simulation, wire the UART terminal to the MCU and use the serial terminal to send decimal byte values to the firmware.

### 2. Build
```bash
pio run
```

### 3. Upload
```bash
pio run -t upload
```

### 4. Monitor Serial Output
```bash
pio device monitor
```

If using SimulIDE, open the serial terminal in the simulator and match the baud rate used by the firmware. The serial interface on Arduino stores received bytes in a receive buffer, so the firmware should keep the main loop responsive and avoid blocking delays while processing incoming data.

## Packet Format

The UART packet uses a variable-length framed structure:

```text
+------------+------------+------------+----------------+------------+
| START BYTE | LEN BYTE   | CMD BYTE   | PAYLOAD BYTES  | CHECKSUM   |
+------------+------------+------------+----------------+------------+
|   0xAA     |   0x??     |   0x??     |   0 to N bytes |   0x??     |
+------------+------------+------------+----------------+------------+
   byte 0       byte 1       byte 2        byte 3..N       final byte
```

- **START_BYTE** (byte 0): Frame delimiter `0xAA`
- **LEN BYTE** (byte 1): Number of payload bytes
- **CMD BYTE** (byte 2): Command code
- **PAYLOAD**: Optional command data
- **CHECKSUM**: Validation byte calculated as `LEN XOR CMD XOR all payload bytes`

### Checksum Calculation
```text
checksum = LEN ^ CMD ^ payload[0] ^ payload[1] ^ ...
```

This format is parsed by a byte-by-byte finite-state machine that waits for the start byte, then collects length, command, payload, and checksum in order before accepting or rejecting the packet.

## Parser States

The UART parser finite-state machine uses these states:

| State | Purpose |
|-------|---------|
| `WAIT_START` | Search for the start byte `0xAA` |
| `WAIT_LEN` | Read payload length |
| `WAIT_CMD` | Read command byte |
| `WAIT_PAYLOAD` | Read the expected number of payload bytes |
| `WAIT_CHECKSUM` | Validate packet checksum |

The parser also includes error recovery for bad lengths, checksum mismatches, and packet timeouts so it can resynchronize after malformed or incomplete input instead of stalling indefinitely.

## Commands & Scenarios

| Scenario | Hex Bytes | Decimal Values |
|----------|-----------|----------------|
| LED Pulse | `AA 00 01 01` | `170, 0, 1, 1` |
| LED ON | `AA 01 02 01 02` | `170, 1, 2, 1, 2` |
| LED OFF | `AA 01 02 00 03` | `170, 1, 2, 0, 3` |
| Set Pulse Time = 500 ms | `AA 01 03 32 30` | `170, 1, 3, 50, 48` |
| Enter Blink Mode | `AA 00 04 04` | `170, 0, 4, 4` |
| Exit Blink Mode | `AA 00 05 05` | `170, 0, 5, 5` |
| Set Blink Interval = 300 ms | `AA 01 06 1E 19` | `170, 1, 6, 30, 25` |
| Get Status | `AA 00 07 07` | `170, 0, 7, 7` |

**Example transmission**: To enter blink mode, send bytes `0xAA 0x00 0x04 0x04`.

### Command Notes
- `CMD 0x01`: Trigger a one-shot LED pulse
- `CMD 0x02`: Set LED steady state, payload `0x00 = OFF`, `0x01 = ON`
- `CMD 0x03`: Set pulse duration in units of 10 ms
- `CMD 0x04`: Enter continuous blink mode
- `CMD 0x05`: Exit blink mode and return LED to OFF
- `CMD 0x06`: Set blink interval in units of 10 ms
- `CMD 0x07`: Request current status packet

## Response Packets

The firmware also transmits structured response packets using the same framing format:

| Response Type | CMD | Purpose |
|---------------|-----|---------|
| ACK | `0x80` | Command accepted and executed |
| ERROR | `0x81` | Command rejected or parser-level error occurred |
| STATUS | `0x82` | Report current LED mode and timing information |

### Response Payloads
- **ACK**: `[original_cmd]`
- **ERROR**: `[original_cmd_or_0x00, error_code]`
- **STATUS**: `[mode, led_output, blink_interval_units]`

### Error Codes

| Error Code | Meaning |
|------------|---------|
| `0x01` | Unknown command |
| `0x02` | Wrong payload length |
| `0x03` | Invalid value |
| `0x04` | Checksum failure |
| `0x05` | Parser timeout |
| `0x06` | Payload length too large |

Structured ACK / ERROR replies are useful because binary command/response protocols let the sender detect success or failure without depending on free-form text logs.

## LED Modes

The LED subsystem is handled separately from the UART parser and supports multiple modes:

| Mode | Description |
|------|-------------|
| `LED_MODE_OFF` | LED stays OFF |
| `LED_MODE_ON` | LED stays ON |
| `LED_MODE_PULSE` | LED turns ON briefly, then returns to OFF |
| `LED_MODE_BLINK` | LED toggles continuously at a configured interval |

Blinking and pulse timing are implemented with nonblocking `millis()` checks rather than `delay()`, which allows UART reception and LED updates to run together without freezing the main loop.

## Reliability Features

- **Finite-state packet parser** for robust byte-by-byte decoding
- **Checksum validation** to reject corrupted packets
- **Length checking** to reject oversized payloads before buffer misuse
- **Parser timeout recovery** to abandon incomplete packets and resynchronize 
- **Structured error replies** for machine-readable diagnostics 
- **Nonblocking LED timing** so UART processing remains responsive 

## Building & Testing

### Local Build
```bash
pio run
```

### Build with Upload
```bash
pio run -t upload
```

### Run Tests
```bash
pio test
```

### Suggested Manual Tests
- Send a valid LED ON packet and verify steady output.
- Send a valid pulse command and verify one-shot timing.
- Send enter-blink and exit-blink commands and verify nonblocking blinking.
- Change the blink interval while blinking and verify the rate changes.
- Send a packet with a bad checksum and verify parser rejection.
- Send only part of a packet, wait past the timeout, and verify timeout recovery.

## Troubleshooting

- **Serial connection issues**: Check baud rate in `platformio.ini` or the simulator terminal settings.
- **Packet not recognized**: Verify frame format, payload length, and checksum calculation.
- **Unexpected parser errors**: Confirm that decimal values sent from SimulIDE match the intended hex bytes.
- **Timeout errors**: Ensure all bytes of a packet are sent without long gaps.
- **No LED blinking**: Confirm blink mode was entered and the blink interval was set to a valid nonzero value.
- **Binary response looks garbled**: This is normal if the serial terminal displays raw bytes as text; structured replies use `Serial.write()` and are meant for machine parsing rather than pretty printing.


## Resources

- [Arduino Blink Without Delay](https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay)
- [Arduino Serial.available() Reference](https://arduinogetstarted.com/reference/serial-available)
- [UART Protocol Guide](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
- [PlatformIO Documentation](https://docs.platformio.org/)