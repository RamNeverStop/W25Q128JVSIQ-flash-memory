# ESP32-S3 W25Q128JVSIQ Flash Memory Interface

A complete SPI flash memory interface using the ESP32-S3 microcontroller and W25Q128JVSIQ external flash chip. This project demonstrates reading, writing, and managing 16MB of non-volatile storage for data logging, firmware storage, or file systems.

## Hardware Requirements

- **Microcontroller**: ESP32-S3
- **Flash Memory**: W25Q128JVSIQ (Winbond 128Mbit / 16MB NOR Flash)
- **Interface**: SPI (Serial Peripheral Interface)
- **Power**: 2.7V - 3.6V (typically 3.3V)

## Features

- 💾 **Large Storage Capacity**
  - 128 Megabit (16 Megabyte) total capacity
  - Non-volatile storage (data retained without power)
  - Perfect for data logging, firmware updates, file systems

- ⚡ **High Performance**
  - Fast read/write operations via SPI
  - Up to 104 MHz SPI clock (device capability)
  - Page programming (256 bytes per page)
  - Sector and block erase operations

- 🔍 **Device Identification**
  - Automatic chip detection via JEDEC ID
  - Verification of correct chip type
  - Detailed capacity and organization info

- 📝 **Basic Operations Implemented**
  - Flash initialization and ID reading
  - Data writing (page program)
  - Data reading
  - Capacity and organization reporting

## Pin Configuration

### SPI Connections (W25Q128JVSIQ ↔ ESP32-S3)

| ESP32-S3 Pin | Function | W25Q128JVSIQ Pin | Description |
|--------------|----------|------------------|-------------|
| GPIO 17      | SCK      | CLK              | SPI Clock |
| GPIO 16      | MISO     | DO (IO1)         | Data Out (Master In) |
| GPIO 15      | MOSI     | DI (IO0)         | Data In (Master Out) |
| GPIO 9       | CS       | /CS              | Chip Select (Active Low) |
| 3.3V         | Power    | VCC              | Power Supply |
| GND          | Ground   | GND              | Ground |

**Optional Pins (for Quad SPI mode - not used in this example):**
- IO2 (/WP) - Write Protect (can be tied to VCC)
- IO3 (/HOLD) - Hold (can be tied to VCC)

## Flash Memory Organization

### W25Q128JVSIQ Structure

| Parameter | Value | Description |
|-----------|-------|-------------|
| **Total Capacity** | 16,777,216 bytes | 16 MB / 128 Mbit |
| **Page Size** | 256 bytes | Minimum write unit |
| **Sector Size** | 4,096 bytes | 4 KB erase unit |
| **Block Size (32KB)** | 32,768 bytes | Half-block erase |
| **Block Size (64KB)** | 65,536 bytes | Full block erase |
| **Total Pages** | 65,536 pages | 256 bytes each |
| **Total Sectors** | 4,096 sectors | 4 KB each |
| **Total Blocks** | 256 blocks | 64 KB each |

### Address Space
- **Address Range**: 0x000000 to 0xFFFFFF (24-bit addressing)
- **First Page**: 0x000000 - 0x0000FF
- **Last Page**: 0xFFFF00 - 0xFFFFFF

## Required Libraries

This example uses only the standard Arduino SPI library:

- **SPI** (included with Arduino IDE)

### Optional Library (Included)

A more feature-rich library `SPI_FlashMem-1.3.5.zip` is included in this repository for advanced operations:
- Install by: **Sketch** → **Include Library** → **Add .ZIP Library**
- Provides erase functions, status checking, and more

## Installation

1. **Clone this repository:**
   ```bash
   git clone https://github.com/yourusername/esp32-s3-w25q128-flash.git
   ```

2. **Open in Arduino IDE:**
   - Open `W25Q128JVSIQ_flash_example_code.ino`

3. **Select your board:**
   - **Tools** → **Board** → **ESP32 Arduino** → **ESP32S3 Dev Module**

4. **Select COM port:**
   - **Tools** → **Port** → Select your ESP32-S3 port

5. **Upload the sketch**

6. **Open Serial Monitor:**
   - Set baud rate to **115200**

## How It Works

### Initialization Sequence

1. **SPI Setup**: Initialize SPI bus with custom pins
2. **Chip Detection**: Send 0x9F command to read JEDEC ID
3. **ID Verification**: Check for manufacturer ID 0xEF (Winbond)
4. **Display Info**: Show capacity and organization details

### JEDEC ID Structure

The W25Q128JVSIQ returns a 3-byte ID:

| Byte | Value | Description |
|------|-------|-------------|
| 1    | 0xEF  | Manufacturer ID (Winbond) |
| 2    | 0x40  | Memory Type (Q-series) |
| 3    | 0x18  | Capacity (128 Mbit) |

### Write Operation

```
1. Send Write Enable (0x06)
2. Send Page Program (0x02)
3. Send 24-bit address
4. Send data bytes (up to 256)
5. Wait for write to complete
```

**Important**: Erase before write! Flash bits can only change from 1→0, erase sets all to 1.

### Read Operation

```
1. Send Read Data (0x03)
2. Send 24-bit address
3. Read data bytes (any length)
```

## Serial Monitor Output

### Successful Operation
```
Initializing W25Q128JVSIQ...
Flash ID: EF 40 18
Flash initialized successfully.
Total Storage Capacity: 16777216 bytes
Page Size: 256 bytes
Sector Size: 4096 bytes
Block Size: 65536 bytes
Read from flash: Hello, W25Q128JVSIQ!
```

### Failed Initialization
```
Initializing W25Q128JVSIQ...
Flash ID: FF FF FF
Failed to initialize flash.
```

## SPI Commands Reference

### Basic Commands (Used in Example)

| Command | Hex Code | Description | Address | Data |
|---------|----------|-------------|---------|------|
| Read ID | 0x9F | Read JEDEC ID | No | 3 bytes |
| Read Data | 0x03 | Read memory | 24-bit | Variable |
| Write Enable | 0x06 | Enable write/erase | No | No |
| Page Program | 0x02 | Write page (256B) | 24-bit | 1-256 bytes |

### Additional Commands (Not Implemented)

| Command | Hex Code | Description |
|---------|----------|-------------|
| Sector Erase (4KB) | 0x20 | Erase 4KB sector |
| Block Erase (32KB) | 0x52 | Erase 32KB block |
| Block Erase (64KB) | 0xD8 | Erase 64KB block |
| Chip Erase | 0xC7 / 0x60 | Erase entire chip |
| Read Status Register | 0x05 | Read status |
| Write Status Register | 0x01 | Write status |
| Power Down | 0xB9 | Low power mode |
| Release Power Down | 0xAB | Wake from sleep |
| Fast Read | 0x0B | High-speed read |

## Advanced Usage

### Erase Sector Before Write

```cpp
void eraseSector(uint32_t address) {
    // Enable write
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0x06); // Write Enable
    digitalWrite(FLASH_CS_PIN, HIGH);
    
    // Erase sector
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0x20); // Sector Erase (4KB)
    SPI.transfer((address >> 16) & 0xFF);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer(address & 0xFF);
    digitalWrite(FLASH_CS_PIN, HIGH);
    
    delay(500); // Wait for erase (typical: 45-400ms)
}
```

### Check Busy Status

```cpp
bool isBusy() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0x05); // Read Status Register
    byte status = SPI.transfer(0x00);
    digitalWrite(FLASH_CS_PIN, HIGH);
    
    return (status & 0x01); // Bit 0 = BUSY
}

void waitForReady() {
    while (isBusy()) {
        delay(1);
    }
}
```

### Write Multiple Pages

```cpp
void writeMultiplePages(uint32_t startAddress, const uint8_t *data, size_t length) {
    size_t written = 0;
    
    while (written < length) {
        uint32_t pageAddress = startAddress + written;
        uint32_t pageOffset = pageAddress % 256;
        size_t bytesToWrite = min(256 - pageOffset, length - written);
        
        writeData(pageAddress, (const char*)(data + written), bytesToWrite);
        waitForReady();
        
        written += bytesToWrite;
    }
}
```

### Fast Read (Higher Speed)

```cpp
void fastReadData(uint32_t address, char *buffer, size_t length) {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0x0B); // Fast Read command
    SPI.transfer((address >> 16) & 0xFF);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer(address & 0xFF);
    SPI.transfer(0x00); // Dummy byte required for fast read
    
    for (size_t i = 0; i < length; i++) {
        buffer[i] = SPI.transfer(0x00);
    }
    
    digitalWrite(FLASH_CS_PIN, HIGH);
}
```

### Power Management

```cpp
void enterPowerDown() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0xB9); // Power Down
    digitalWrite(FLASH_CS_PIN, HIGH);
    delayMicroseconds(3); // tDP = 3μs
}

void exitPowerDown() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0xAB); // Release Power Down
    digitalWrite(FLASH_CS_PIN, HIGH);
    delayMicroseconds(3); // tRES1 = 3μs
}
```

## Wiring Diagram

```
W25Q128JVSIQ          ESP32-S3
------------          --------
   CLK       -------> GPIO 17 (SCK)
   DO        -------> GPIO 16 (MISO)
   DI        -------> GPIO 15 (MOSI)
   /CS       -------> GPIO 9  (CS)
   /WP       -------> 3.3V (or GPIO if needed)
   /HOLD     -------> 3.3V (or GPIO if needed)
   VCC       -------> 3.3V
   GND       -------> GND
```

**Note**: /WP and /HOLD can be tied to VCC for normal operation.

## Troubleshooting

### Flash ID Returns 0xFF 0xFF 0xFF
- **Check wiring**: Verify all SPI connections
- **Check power**: Ensure 3.3V stable power supply
- **Check CS pin**: Must be correct GPIO and properly configured
- **Try lower SPI speed**: Some boards need slower clock
  ```cpp
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  ```

### Flash ID Returns 0x00 0x00 0x00
- **Check connections**: Likely MISO not connected
- **Check pin definitions**: Verify GPIO numbers match hardware
- **Test with multimeter**: Check for shorts or opens

### Data Not Writing
- **Erase first**: Must erase sector/block before writing
- **Check write enable**: Must send 0x06 before each write
- **Wait for completion**: Use status register or delay
- **Check page boundaries**: Don't cross 256-byte pages
- **Verify not write-protected**: Check status register bits

### Data Corruption
- **Power supply**: Ensure stable 3.3V during write/erase
- **Timing**: Add proper delays after erase operations
- **Address alignment**: Keep track of sector/page boundaries
- **Wear leveling**: Implement for frequently updated data

### Slow Performance
- **Use Fast Read**: 0x0B command instead of 0x03
- **Increase SPI clock**: Up to 80-104 MHz if stable
- **Batch operations**: Write/read multiple bytes per transaction
- **Optimize transfers**: Minimize CS toggling

## Performance Specifications

### Timing (Typical)

| Operation | Typical Time | Maximum Time |
|-----------|--------------|--------------|
| Page Program | 0.7 ms | 3 ms |
| Sector Erase (4KB) | 45 ms | 400 ms |
| Block Erase (32KB) | 120 ms | 1600 ms |
| Block Erase (64KB) | 150 ms | 2000 ms |
| Chip Erase | 40 s | 200 s |

### Endurance & Retention

- **Program/Erase Cycles**: 100,000 typical
- **Data Retention**: 20 years typical @ 25°C
- **Operating Temperature**: -40°C to +85°C

## Use Cases

### Data Logging
```cpp
// Store sensor readings with timestamps
struct LogEntry {
    uint32_t timestamp;
    float temperature;
    float humidity;
};

void logData(LogEntry entry) {
    static uint32_t logAddress = 0x010000; // Start at 64KB
    
    // Check if sector needs erasing (every 16 entries)
    if (logAddress % 4096 == 0) {
        eraseSector(logAddress);
    }
    
    writeData(logAddress, (const char*)&entry, sizeof(LogEntry));
    logAddress += sizeof(LogEntry);
}
```

### Configuration Storage
```cpp
struct Config {
    char deviceName[32];
    uint32_t baudRate;
    bool enableLogging;
};

void saveConfig(Config cfg) {
    eraseSector(0); // Erase first sector
    writeData(0, (const char*)&cfg, sizeof(Config));
}

void loadConfig(Config &cfg) {
    readData(0, (char*)&cfg, sizeof(Config));
}
```

### Firmware Storage
- Store firmware updates before flashing
- Implement A/B partition scheme for recovery
- Store calibration data separate from code

## Safety Warnings

⚠️ **IMPORTANT**

- **Voltage**: Only use 3.3V, NOT 5V! Will damage chip.
- **Write Protection**: Implement in production code to prevent accidental erasure
- **Wear Leveling**: Spread writes across memory to extend life
- **Backup Critical Data**: Flash memory can fail; maintain backups
- **ESD Protection**: Handle with anti-static precautions

## Comparison with ESP32 Internal Flash

| Feature | W25Q128JVSIQ External | ESP32-S3 Internal |
|---------|----------------------|-------------------|
| Capacity | 16 MB | 8-32 MB (varies) |
| Speed | Up to 104 MHz SPI | Native bus speed |
| Removable | Yes (external chip) | No (integrated) |
| Cost | Additional component | Included |
| Use Case | Extra storage | Program + data |

## License

This project is open source and available under the MIT License.

## Contributing

Pull requests are welcome! Areas for improvement:
- Implement erase functions
- Add wear leveling algorithm
- Create file system support
- Add error checking and recovery

## Author

Your Name

## Resources

- [W25Q128JV Datasheet](https://www.winbond.com/resource-files/w25q128jv%20revf%2003272018%20plus.pdf)
- [SPI Protocol Specification](https://www.sparkfun.com/tutorials/16)
- [ESP32-S3 SPI Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/spi_master.html)
- [SPI_FlashMem Library](https://github.com/Marzogh/SPIMemory) (included)

## Acknowledgments

- Winbond for the W25Q128JV flash memory
- Espressif for ESP32-S3 platform
- Arduino community for SPI library
- SPI_FlashMem library contributors
