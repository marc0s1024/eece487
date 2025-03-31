#include <Arduino.h>
#include "daly-bms-uart.h"

#define BMS_RX_PIN 16  // ESP32 GPIO16 (RX2) to BMS TX
#define BMS_TX_PIN 17  // ESP32 GPIO17 (TX2) to BMS RX

HardwareSerial BMS_Serial(2);  // use UART2 for BMS RX TX
Daly_BMS_UART bms(BMS_Serial); // constuct Daly_BMS_UART object

unsigned long lastUpdateTime = 0;  // timer var for serial mon update

void setup() {
    Serial.begin(115200); // start serial mon
    BMS_Serial.begin(9600, SERIAL_8N1, BMS_RX_PIN, BMS_TX_PIN);  // init UART2

    bms.Init(); // set up driver
}

void loop() {
    // update every 5 secs
    if (millis() - lastUpdateTime >= 5000) {
        lastUpdateTime = millis();  // reset timer

        // TODO: Could these both be reduced to a single flush()?
        // Right now there's a bug where if you send more than one character it will read multiple times
        Serial.read(); // Discard the character sent
        Serial.read(); // Discard the new line

        // This .update() call populates the entire get struct. If you only need certain values (like
        // SOC & Voltage) you could use other public APIs, like getPackMeasurements(), which only query
        // specific values from the BMS instead of all.
        bms.getPackMeasurements();

        /*
        // Print received BMS packet before updating values
        Serial.println("\n[BMS READER] Received Packet:");

        if (BMS_Serial.available() >= 13) {  // Ensure a full packet is available
            uint8_t receivedPacket[13];
            BMS_Serial.readBytes(receivedPacket, 13);  // Read full packet

            // Print the received packet in HEX format
            for (int i = 0; i < 13; i++) {
                Serial.printf("0x%02X ", receivedPacket[i]);
            }
            Serial.println();
        }
        else {
            Serial.println("❌ No valid packet received.\n");
        }
        */

        // serial print baud 115200
        Serial.println("Basic BMS Data:              " + (String)bms.get.packVoltage + "V " + (String)bms.get.packCurrent + "I " + (String)bms.get.packSOC + "\% ");
        Serial.println("Package Temperature (C):     " + (String)bms.get.tempAverage);
        Serial.println("Highest Cell Voltage:        #" + (String)bms.get.maxCellVNum + " with voltage " + (String)(bms.get.maxCellmV / 1000));
        Serial.println("Lowest Cell Voltage:         #" + (String)bms.get.minCellVNum + " with voltage " + (String)(bms.get.minCellmV / 1000));
        Serial.println("Number of Cells:             " + (String)bms.get.numberOfCells);
        Serial.println("Number of Temp Sensors:      " + (String)bms.get.numOfTempSensors);
        Serial.println("BMS Chrg / Dischrg Cycles:   " + (String)bms.get.bmsCycles);
        Serial.println("BMS Heartbeat:               " + (String)bms.get.bmsHeartBeat); // cycle 0-255
        Serial.println("Discharge MOSFet Status:     " + (String)bms.get.disChargeFetState);
        Serial.println("Charge MOSFet Status:        " + (String)bms.get.chargeFetState);
        Serial.println("Remaining Capacity mAh:      " + (String)bms.get.resCapacitymAh);
        //... any many many more data

        for (size_t i = 0; i < size_t(bms.get.numberOfCells); i++)
        {
          Serial.println("Remaining Capacity mAh:      " + (String)bms.get.cellVmV[i]);
        }

        // Alarm flags
        // These are boolean flags that the BMS will set to indicate various issues.
        // For all flags see the alarm struct in daly-bms-uart.h and refer to the datasheet
        Serial.println("Level one Cell V too High:   " + (String)bms.alarm.levelOneCellVoltageTooHigh);

        /**
        * Advanced functions:
        * bms.setBmsReset(); //Reseting the BMS, after reboot the MOS Gates are enabled!
        * bms.setDischargeMOS(true); Switches on the discharge Gate
        * bms.setDischargeMOS(false); Switches off thedischarge Gate
        * bms.setChargeMOS(true); Switches on the charge Gate
        * bms.setChargeMOS(false); Switches off the charge Gate
        */

        Serial.println("_________________________________________________");
    }
}
