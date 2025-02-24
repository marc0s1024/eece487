#include <Arduino.h>

#define SIM_TX_PIN 23  // TX2 on ESP32 (Connect to RX2 on the receiver ESP32)
#define SIM_RX_PIN 22  // RX2 (not used for sending)

HardwareSerial BMS_Simulator(1);  // Use UART1 for transmitting simulated data

void setup() {
    Serial.begin(115200);  // USB Serial Monitor
    BMS_Simulator.begin(9600, SERIAL_8N1, -1, SIM_TX_PIN);  // TX only

    Serial.println("ESP32 Daly BMS Simulator Started...");
}

void loop() {
    sendFakeBMSPacket();
    delay(5000);  // Send every 5 seconds
}

void sendFakeBMSPacket() {
    uint8_t fakeBMSPacket[13] = {
        0xA5, 0x01, 0x90, 0x08,  // Header
        0x02, 0x3A,              // Voltage (57.0V)
        0x00, 0x00,              // Acquisition voltage (ignored)
        0x75, 0x30,              // Current (0A, offset -30,000)
        0x01, 0xED,              // SOC (49.3%)
        0x00                     // Placeholder for checksum
    };

    // Compute checksum
    uint8_t checksum = 0;
    for (int i = 0; i < 12; i++) {
        checksum += fakeBMSPacket[i];  // Sum of first 12 bytes
    }
    fakeBMSPacket[12] = checksum;  // Assign calculated checksum

    Serial.print("[BMS SIM] Sending Data: ");
    for (int i = 0; i < 13; i++) {
        Serial.printf("0x%02X ", fakeBMSPacket[i]);
    }
    Serial.println();

    BMS_Simulator.write(fakeBMSPacket, 13);  // Send packet via UART1 (TX)
}
