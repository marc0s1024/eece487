#include "Display.h"

// #define BMS_RX_PIN 16  // ESP32 GPIO16 (RX2) to BMS TX
// #define BMS_TX_PIN 17  // ESP32 GPIO17 (TX2) to BMS RX

HardwareSerial BMS_Serial(2);   // use UART2 for BMS RX TX
Daly_BMS_UART bms(BMS_Serial);  // constuct Daly_BMS_UART object

void DecodeSetup() {
  BMS_Serial.begin(9600, SERIAL_8N1, BMS_RX_PIN, BMS_TX_PIN);  // init UART2
  bms.Init();                                                  // set up driver
}

void ReadBMS() {
  // TODO: Could these both be reduced to a single flush()?
  // Right now there's a bug where if you send more than one character it will
  // read multiple times
  Serial.read();  // Discard the character sent
  Serial.read();  // Discard the new line

  /* this could fix ^
  // Read the incoming message
  String incomingMessage = Serial.readStringUntil('\n');
  incomingMessage.trim(); // Remove any trailing newlines or spaces
  */

  // This .update() call populates the entire get struct. If you only need
  // certain values (like SOC & Voltage) you could use other public APIs, like
  // getPackMeasurements(), which only query specific values from the BMS
  // instead of all. bms.update();
  bms.getPackMeasurements();
  bms.getPackTemp();
  bms.getCellVoltages();
}

double GetValue(String code) {
  double fahrenheitTempAvg = 0;  // def var for conversion Celsius to Fahrenheit
  double totalPower = 0;         // def var for total power (Watts)

  if (code == "SOC") {
    if (bms.get.packSOC > 100) {
      return 100;
    }
    else if (bms.get.packSOC < 0) {
      return 0;
    }
    else {
      return double(bms.get.packSOC);
    }
  } else if (code == "Voltage") {
    return double(bms.get.packVoltage);
  } else if (code == "Current") {
    if (bms.get.packCurrent < -1000) {
      return Current.getValue();
    }
    else {
      return double(bms.get.packCurrent);
    }
  } else if (code == "Temperature") {
    // Sometimes the BMS will return -40, so only update when not -40
    if (((double(bms.get.tempAverage) * 1.8) + 32) > -40) {
      fahrenheitTempAvg =
          (double(bms.get.tempAverage) * 1.8) + 32;  // C to F conversion
    }
    return fahrenheitTempAvg;
  } else if (code == "Watts") {
    totalPower = double(bms.get.packVoltage) *
                 double(bms.get.packCurrent);  // calculate total power (P=VI)
    return totalPower;
  } else if (code == "Cell1") {
    return double(bms.get.cellVmV[0]) / 1000;  // cell voltage in V
  } else if (code == "Cell2") {
    return double(bms.get.cellVmV[1]) / 1000;
  } else if (code == "Cell3") {
    return double(bms.get.cellVmV[2]) / 1000;
  } else if (code == "Cell4") {
    return double(bms.get.cellVmV[3]) / 1000;
  } else {
    Serial.println("Invalid code: " + code);
    return -1;
  }
}