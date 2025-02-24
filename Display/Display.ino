// Main File
#include "Display.h"

void setup()
{
  Serial.begin(115200);

  DecodeSetup();
  DisplaySetup();
}

void loop()
{
  /*
  1) check for bms output
  2) if output recieved -> translate
  3) send number and code(SOC, current, etc) to show on display
  repeat
  */
  Serial.println("Reading BMS:\n");
  ReadBMS();
  Serial.println("Updating display:\n");
  UpdateDisplay("SOC", GetValue("SOC"));



}