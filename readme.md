# UART Packet Receiver

A firmware system that receives, validates, and processes structured packets over UART with support for command execution including LED control, mode changes, and configuration updates.  

## Requirements

### Hardware
- Microcontroller (Arduino Uno)
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

The UART packet uses a 3-byte structure:

```
+------------+------------+------------+
| START BYTE | CMD BYTE   | CHECKSUM   |
+------------+------------+------------+
|   0xAA     |   0x??     |   0x??     |
+------------+------------+------------+
   byte 0       byte 1       byte 2
```

- **START_BYTE** (byte 0): Frame delimiter (0xAA)
- **CMD BYTE** (byte 1): Command code
- **CHECKSUM** (byte 2): Validation byte calculated as `START_BYTE XOR CMD_BYTE`

### Checksum Calculation
```
checksum = START ^ CMD
```

## Commands & Scenarios

| Scenario | Hex Bytes | Decimal Values |
|----------|-----------|-----------------|
| LED Pulse | AA 01 AB | 170, 1, 171 |
| LED ON | AA 02 A8 | 170, 2, 168 |
| LED OFF | AA 03 A9 | 170, 3, 169 |

**Example transmission**: To turn on LED, send bytes: `0xAA 0x02 0xA8`

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