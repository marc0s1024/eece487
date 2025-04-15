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
    // Gets necesary data from the BMS
    ReadBMS();
    // Updates the display with new BMS data
    UpdateDisplay();
    currentTime = millis();
    lastUpdateTime = currentTime;
  }
}