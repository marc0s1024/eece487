// Main File
#include "Display.h"

//delete
#include <iostream>
#include <random>

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
  
  //delete
  // std::random_device rd;  // Obtain a random seed from the OS
  // std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  // std::uniform_int_distribution<> distrib(1, 100); // Define range for integers 1 to 100
  // int randomNumber = distrib(gen);

  Serial.println("Updating SOC:\n");
  UpdateDisplay("SOC", GetValue("SOC"));
  // UpdateDisplay("SOC", randomNumber);
  delay(500);
  Serial.println("Updating Voltage:\n");
  UpdateDisplay("Voltage", GetValue("Voltage"));
  delay(500);
  Serial.println("Updating Current:\n");
  UpdateDisplay("Current", GetValue("Current"));
  Serial.println("Updating Temperature:\n");
  UpdateDisplay("Temperature", GetValue("Temperature"));

  delay(500);
}