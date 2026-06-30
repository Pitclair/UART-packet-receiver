# UART Packet Receiver

A robust firmware system that receives, validates, and processes structured packets over UART with support for command execution including LED control, mode changes, and configuration updates.  

## Requirements

### Hardware
- Microcontroller (e.g., Arduino Uno, STM32, ESP32)
- Serial terminal or USB-to-UART adapter
- LED and/or button for testing command effects

### Software & Tools
- **Language**: C or Arduino-style C++
- **Build System**: PlatformIO
- **Simulation** (optional): SimulIDE or Wokwi

## Project Structure

```
├── src/
│   └── main.cpp          # Main firmware code
├── include/              # Header files
├── lib/                  # Library files
├── test/                 # Test files
├── platformio.ini        # PlatformIO configuration
└── README.md            # This file
```

## Getting Started

### 1. Setup
Ensure PlatformIO is installed and your MCU is connected or simulator is running.

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

## Packet Format

A typical packet structure:
```
[START_BYTE] [LENGTH] [CMD] [PAYLOAD...] [CHECKSUM]
```

- **START_BYTE**: Frame delimiter (e.g., 0xAA)
- **LENGTH**: Payload length
- **CMD**: Command code (e.g., LED_ON, LED_OFF, MODE_CHANGE)
- **PAYLOAD**: Command arguments (variable length)
- **CHECKSUM**: Validation (CRC or simple sum)

## Example Commands

| Command | Code | Effect |
|---------|------|--------|
| LED On  | 0x01 | Turn on LED |
| LED Off | 0x02 | Turn off LED |
| Mode Change | 0x03 | Switch firmware mode |
| Config Update | 0x04 | Update settings |

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

## Troubleshooting

- **Serial connection issues**: Check baud rate in `platformio.ini`
- **Packet not recognized**: Verify frame format and checksum calculation
- **Buffer overflow**: Increase buffer size or process packets more frequently
- **Timing issues**: Check interrupt priorities and serial baud rate

## Resources

- [UART Protocol Guide](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Circular Buffer Implementation](https://en.wikipedia.org/wiki/Circular_buffer)