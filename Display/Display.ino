// Main File
#include <iostream>

#include "Display.h"

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 5000;
unsigned long currentTime;

void setup() {
  Serial.begin(115200);
  DecodeSetup();
  DisplaySetup();
  lastUpdateTime = millis();
}

void loop() {
  CheckTap();
  delay(10);

  currentTime = millis();
  // Serial.println(current_page);
  if (currentTime - lastUpdateTime >= updateInterval) {
    Serial.println("Reading BMS");
    ReadBMS();
    if (current_page == 0) {
      UpdateDisplay("SOC", GetValue("SOC"));
      UpdateDisplay("Voltage", GetValue("Voltage"));
      UpdateDisplay("Current", GetValue("Current"));
      UpdateDisplay("Temperature", GetValue("Temperature"));
      UpdateDisplay("Watts", GetValue("Watts"));
    } else if (current_page == 1) {
      UpdateDisplay("Cell1", GetValue("Cell1"));
      UpdateDisplay("Cell2", GetValue("Cell2"));
      UpdateDisplay("Cell3", GetValue("Cell3"));
      UpdateDisplay("Cell4", GetValue("Cell4"));
    }
    currentTime = millis();
    lastUpdateTime = currentTime;
  }
}