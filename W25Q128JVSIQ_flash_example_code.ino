#include <SPI.h>

#define FLASH_CS_PIN 9  // Chip select pin

void setup() {
    Serial.begin(115200);
    while (!Serial) { }

    // Initialize SPI with your pin configuration
    SPI.begin(17, 16, 15, FLASH_CS_PIN); // SCK, MISO, MOSI, CS
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // Set CS high

    // Check if the flash is connected
    Serial.println("Initializing W25Q128JVSIQ...");
    if (initializeFlash()) {
        Serial.println("Flash initialized successfully.");
        printFlashDetails();
     } else {
        Serial.println("Failed to initialize flash.");
        return;
     }

    // Example: Write data to flash
    const char *dataToWrite = "Hello, W25Q128JVSIQ!";
    writeData(0, dataToWrite, strlen(dataToWrite) + 1); // +1 for null terminator

    // Example: Read data from flash
    char buffer[32]; // Make sure the buffer is large enough
    readData(0, buffer, sizeof(buffer));
    Serial.print("Read from flash: ");
    Serial.println(buffer);
}

void loop() {
    // Your main code here
}

bool initializeFlash() {
    // Send a command to read the identification of the flash
    digitalWrite(FLASH_CS_PIN, LOW); // Select the chip
    SPI.transfer(0x9F); // Read ID command
    byte id1 = SPI.transfer(0x00); // Dummy byte
    byte id2 = SPI.transfer(0x00); // Dummy byte
    byte id3 = SPI.transfer(0x00); // Dummy byte
    digitalWrite(FLASH_CS_PIN, HIGH); // Deselect the chip

    Serial.print("Flash ID: ");
    Serial.print(id1, HEX); // Manufacturer ID
    Serial.print(" ");
    Serial.print(id2, HEX); // Memory Type
    Serial.print(" ");
    Serial.println(id3, HEX); // Capacity

    // Check if the ID corresponds to W25Q128JVSIQ
    return (id1 == 0xEF && id2 == 0x40 && id3 == 0x18);
}

void printFlashDetails() {
    const uint32_t totalCapacity = 16 * 1024 * 1024; // W25Q128JVSIQ capacity: 128Mb = 16MB
    const uint32_t pageSize = 256;  // Page size: 256 bytes
    const uint32_t sectorSize = 4096; // Sector size: 4KB
    const uint32_t blockSize = 65536; // Block size: 64KB

    Serial.print("Total Storage Capacity: ");
    Serial.print(totalCapacity);
    Serial.println(" bytes");

    Serial.print("Page Size: ");
    Serial.print(pageSize);
    Serial.println(" bytes");

    Serial.print("Sector Size: ");
    Serial.print(sectorSize);
    Serial.println(" bytes");

    Serial.print("Block Size: ");
    Serial.print(blockSize);
    Serial.println(" bytes");
}

void writeData(uint32_t address, const char *data, size_t length) {
    digitalWrite(FLASH_CS_PIN, LOW); // Select the chip
    SPI.transfer(0x06); // Write Enable command
    digitalWrite(FLASH_CS_PIN, HIGH);

    digitalWrite(FLASH_CS_PIN, LOW); // Select the chip
    SPI.transfer(0x02); // Page Program command
    SPI.transfer((address >> 16) & 0xFF); // Address high byte
    SPI.transfer((address >> 8) & 0xFF);  // Address mid byte
    SPI.transfer(address & 0xFF);         // Address low byte

    for (size_t i = 0; i < length; i++) {
        SPI.transfer(data[i]); // Write data
    }

    digitalWrite(FLASH_CS_PIN, HIGH); // Deselect the chip
    delay(10); // Wait for write to complete
}

void readData(uint32_t address, char *buffer, size_t length) {
    digitalWrite(FLASH_CS_PIN, LOW); // Select the chip
    SPI.transfer(0x03); // Read Data command
    SPI.transfer((address >> 16) & 0xFF); // Address high byte
    SPI.transfer((address >> 8) & 0xFF);  // Address mid byte
    SPI.transfer(address & 0xFF);         // Address low byte

    for (size_t i = 0; i < length; i++) {
        buffer[i] = SPI.transfer(0x00); // Read data
    }

    digitalWrite(FLASH_CS_PIN, HIGH); // Deselect the chip
}
