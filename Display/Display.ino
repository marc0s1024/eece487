// Main File
#include "Display.h"
#include <iostream>

void setup()
{
  Serial.begin(115200);
  DecodeSetup();
  DisplaySetup();
}

void loop()
{
  delay(500);

  // Gets necesary data from the BMS
  ReadBMS();
  // Updates the display with new BMS data
  UpdateDisplay();
}