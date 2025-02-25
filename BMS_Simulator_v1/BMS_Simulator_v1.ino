/*
  BMS Simulator with Command Reply and Varying Data
  --------------------------------------------------
  This sketch runs on a second ESP32 to simulate a Daly BMS.
  It listens on UART for a 13-byte command (e.g. for Voltage, Current, SOC, command 0x90)
  from the host ESP32. When it receives a valid command, it immediately sends back a
  simulated reply packet with 13 bytes.
  
  Every 10 seconds, the simulated battery values (voltage, current, SOC) change.
  
  Wiring:
    - Connect this board's TX (SIM_TX_PIN) to the host's RX.
    - Connect grounds together.
  
  The packet format for a 0x90 reply is as follows:
    Byte 0: Start byte (0xA5)
    Byte 1: BMS Address (0x01 for BMS→host)
    Byte 2: Command (0x90)
    Byte 3: Data length (0x08)
    Bytes 4-5: Pack Voltage in tenths of a volt (e.g., 57.0V → 570 → 0x023A)
    Bytes 6-7: Acquisition voltage (often 0x0000)
    Bytes 8-9: Pack Current in 0.1A units plus an offset of 30000
    Bytes 10-11: State of Charge (SOC) in 0.1% units
    Byte 12: Checksum = sum of bytes 0-11 truncated to 8 bits
*/

#include <Arduino.h>

// Choose UART pins for the simulator
#define SIM_TX_PIN 22  // Simulator TX → host RX
#define SIM_RX_PIN 23  // (Not used here, but defined for completeness)

HardwareSerial simSerial(2);

// Simulated battery values
float simVoltage = 33.0;   // Volts
float simCurrent = 10.0;    // Amps
float simSoc     = 46.0;   // Percent

// Timing for data updates
uint32_t lastChange = 0;
const uint32_t CHANGE_INTERVAL = 2000; // Change values every 10 seconds

// Function to build and send a 0x90 reply packet with the current simulated values.
void sendReplyPacket() {
  // Convert to raw data (the protocol uses 0.1 units)
  uint16_t rawVoltage = (uint16_t)(simVoltage * 10.0f);          // e.g., 57.0V -> 570
  // For current, add an offset of 30000
  uint16_t rawCurrent = (uint16_t)((simCurrent * 10.0f) + 30000);  // e.g., 0.0A -> 30000
  uint16_t rawSoc     = (uint16_t)(simSoc * 10.0f);                // e.g., 49.3% -> 493

  uint8_t packet[13];
  packet[0] = 0xA5; // Start byte
  packet[1] = 0x01; // BMS→host address
  packet[2] = 0x90; // Command ID: Voltage, Current, SOC
  packet[3] = 0x08; // Data length (fixed)

  // Bytes 4-5: Voltage
  packet[4] = (rawVoltage >> 8) & 0xFF;
  packet[5] = rawVoltage & 0xFF;

  // Bytes 6-7: Acquisition voltage (set to 0)
  packet[6] = 0x00;
  packet[7] = 0x00;

  // Bytes 8-9: Current (with offset)
  packet[8] = (rawCurrent >> 8) & 0xFF;
  packet[9] = rawCurrent & 0xFF;

  // Bytes 10-11: SOC
  packet[10] = (rawSoc >> 8) & 0xFF;
  packet[11] = rawSoc & 0xFF;

  // Compute checksum as the sum of bytes 0 through 11 (truncated to 8 bits)
  uint16_t sum = 0;
  for (int i = 0; i < 12; i++) {
    sum += packet[i];
  }
  packet[12] = (uint8_t)sum;  // Expected to match the Daly protocol

  // Send the packet over UART
  simSerial.write(packet, 13);
  simSerial.flush();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("BMS Simulator (with varying data) starting...");

  // Initialize UART2 on this simulator ESP32:
  // SIM_RX_PIN is not used in this example; we only reply.
  simSerial.begin(9600, SERIAL_8N1, SIM_RX_PIN, SIM_TX_PIN);
  Serial.println("Simulator UART initialized on UART2.");
}

void loop() {
  // Listen for an incoming command from the host.
  // The host (daly-bms-uart library) should send a command (13 bytes) asking for 0x90 data.
  if (simSerial.available() >= 13) {
    uint8_t cmdPacket[13];
    simSerial.readBytes(cmdPacket, 13);
    // Verify the command is for Voltage, Current, SOC.
    if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x90 && cmdPacket[3] == 0x08) {
      sendReplyPacket();
      Serial.println("Received command 0x90, sent simulated reply packet.");
    }
    // only to test specific get structs when not using .update()
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x91 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x91.");
    }
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x92 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x92.");
    }
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x93 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x93.");
    }
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x94 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x94.");
    }
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x95 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x95.");
    }
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x96 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x96.");
    }
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x97 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x97.");
    }
    else if (cmdPacket[0] == 0xA5 && cmdPacket[2] == 0x98 && cmdPacket[3] == 0x08) {
      Serial.println("Received command 0x98.");
    }
    else {
      Serial.println("Received an unrecognized command.");
    }
  }
  
  // Every 10 seconds, change the simulated battery values.
  if (millis() - lastChange >= CHANGE_INTERVAL) {
    lastChange = millis();
    
    // For example, increment voltage by 0.5V, increase current by 2A, and SOC by 5%.
    simVoltage += 1.0f;
    simCurrent += 2.0f;
    simSoc     += 3.0f;
    
    // Wrap or constrain values to keep them realistic.
    if (simVoltage > 70.0f)  simVoltage = 30.0f;
    if (simSoc > 100.0f)     simSoc = 40.0f;
    if (simCurrent > 50.0f)  simCurrent = 15.0f;
    
    Serial.println("Simulated battery values updated.");
  }
}
