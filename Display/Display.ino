// Main File
#include "Display.h"
#include <iostream>

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 5000; // 5 seconds
unsigned long currentTime;

void setup()
{
  Serial.begin(115200);
  DecodeSetup();
  DisplaySetup();
  lastUpdateTime = millis();
}

void loop()
{
  CheckTap();
  delay(10);

  currentTime = millis();
  if (currentTime - lastUpdateTime >= updateInterval)
  {

    lastUpdateTime = currentTime;

    ReadBMS();
    UpdateDisplay("SOC", GetValue("SOC"));
    UpdateDisplay("Voltage", GetValue("Voltage"));
    UpdateDisplay("Current", GetValue("Current"));
    UpdateDisplay("Temperature", GetValue("Temperature"));
    UpdateDisplay("Watts", GetValue("Watts"));
    UpdateDisplay("Cell1", GetValue("Cell1"));
    UpdateDisplay("Cell2", GetValue("Cell2"));
    UpdateDisplay("Cell3", GetValue("Cell3"));
    UpdateDisplay("Cell4", GetValue("Cell4"));
  }
}