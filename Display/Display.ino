// Main File
#include "Display.h"

//delete
#include <iostream>
#include <random>

// vars for testing display
double SOCTEST = 85;
double VoltageTEST = 12;
double CurrentTEST = 5;
double TemperatureTEST = 85;
double WattsTEST = 0;
double Cell1TEST = 3.30;
double Cell2TEST = 3.30;
double Cell3TEST = 3.30;
double Cell4TEST = 3.30;

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
  // Serial.println("Reading BMS:\n");
  // ReadBMS(); temporarily dissabled
  
   
  // update with random numbers
  std::random_device rd;  // Obtain a random seed from the OS
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib(1, 100); // Define range for integers 1 to 100
  int randomNumber = distrib(gen);

  // call tap function to handle page switching
  //Serial.println("Checking for tap:\n");
  CheckTap();

  delay(10);

  // display testing
  SOCTEST++;
  if (SOCTEST > 100)
    SOCTEST = 85;

  VoltageTEST += 0.1;
  if (VoltageTEST > 13.5)
    VoltageTEST = 12;
    
  CurrentTEST++;
  if (CurrentTEST > 20)
    CurrentTEST = 5;

  TemperatureTEST++;
  if (TemperatureTEST > 100)
    TemperatureTEST = 85;
  
  WattsTEST = VoltageTEST * CurrentTEST;
  
  Cell1TEST += 0.01;
  if (Cell1TEST > 3.45)
    Cell1TEST = 3.30;

  Cell2TEST += 0.01;
  if (Cell2TEST > 3.45)
    Cell2TEST = 3.30;
  
  Cell3TEST += 0.01;
  if (Cell3TEST > 3.45)
    Cell3TEST = 3.30;
  
  Cell4TEST += 0.01;
  if (Cell4TEST > 3.45)
    Cell4TEST = 3.30;
  
  UpdateDisplay("SOC", SOCTEST);
  //delay(500);
  UpdateDisplay("Voltage", VoltageTEST);
  //delay(500);
  UpdateDisplay("Current", CurrentTEST);
  //delay(500);
  UpdateDisplay("Temperature", TemperatureTEST);
  //delay(500);
  UpdateDisplay("Watts", WattsTEST);
  //delay(500);
  UpdateDisplay("Cell1", Cell1TEST);
  //delay(500);
  UpdateDisplay("Cell2", Cell2TEST);
  //delay(500);
  UpdateDisplay("Cell3", Cell3TEST);
  //delay(500);
  UpdateDisplay("Cell4", Cell4TEST);
  //delay(500);

/*
  // Read from BMS
  Serial.println("Updating SOC:\n");
  UpdateDisplay("SOC", GetValue("SOC"));
  //delay(500);
  Serial.println("Updating Voltage:\n");
  UpdateDisplay("Voltage", GetValue("Voltage"));
  //delay(500);
  Serial.println("Updating Current:\n");
  UpdateDisplay("Current", GetValue("Current"));
  //delay(500);
  Serial.println("Updating Temperature:\n");
  UpdateDisplay("Temperature", GetValue("Temperature"));
  //delay(500);
  Serial.println("Updating Total Power Draw (Watts):\n");
  UpdateDisplay("Watts", GetValue("Watts"));
  //delay(500);
  Serial.println("Updating Cell 1 Voltage:\n");
  UpdateDisplay("Cell1", GetValue("Cell1"));
  //delay(500);
  Serial.println("Updating Cell 2 Voltage:\n");
  UpdateDisplay("Cell2", GetValue("Cell2"));
  //delay(500);
  Serial.println("Updating Cell 3 Voltage:\n");
  UpdateDisplay("Cell3", GetValue("Cell3"));
  //delay(500);
  Serial.println("Updating Cell 4 Voltage:\n");
  UpdateDisplay("Cell4", GetValue("Cell4"));
  //delay(500);

*/

  delay(100);
}