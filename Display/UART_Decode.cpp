#include "Display.h"

// #define BMS_RX_PIN 16  // ESP32 GPIO16 (RX2) to BMS TX
// #define BMS_TX_PIN 17  // ESP32 GPIO17 (TX2) to BMS RX

HardwareSerial BMS_Serial(2);  // use UART2 for BMS RX TX
Daly_BMS_UART bms(BMS_Serial); // constuct Daly_BMS_UART object

unsigned long lastUpdateTime = 0; // timer var for serial mon update

int serialRead = 0;

void DecodeSetup()
{
  BMS_Serial.begin(9600, SERIAL_8N1, BMS_RX_PIN, BMS_TX_PIN); // init UART2
  bms.Init();                                                 // set up driver
}

void ReadBMS()
{
  while (serialRead == 0)
  {
    // update every 5 secs
    if (millis() - lastUpdateTime >= 2000)
    {
      lastUpdateTime = millis(); // reset timer

      // TODO: Could these both be reduced to a single flush()?
      // Right now there's a bug where if you send more than one character it will read multiple times
      Serial.read(); // Discard the character sent
      Serial.read(); // Discard the new line

      /* this could fix ^
      // Read the incoming message
      String incomingMessage = Serial.readStringUntil('\n');
      incomingMessage.trim(); // Remove any trailing newlines or spaces
      */

      // This .update() call populates the entire get struct. If you only need certain values (like
      // SOC & Voltage) you could use other public APIs, like getPackMeasurements(), which only query
      // specific values from the BMS instead of all.
      bms.getPackMeasurements();
      bms.getPackTemp();
      bms.getCellVoltages();

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

      Serial.println("_________________________________________________");
      serialRead = 1;
    }
  } // end while, this is just for debugging so remove later
  serialRead = 0;
}

double GetValue(String code)
{
  double fahrenheitTempAvg = 0; // def var for conversion Celsius to Fahrenheit
  double totalPower = 0; // def var for total power (Watts)

  if (code == "SOC")
  {
    return double(bms.get.packSOC);
  }
  else if (code == "Voltage")
  {
    return double(bms.get.packVoltage);
  }
  else if (code == "Current")
  {
    return double(bms.get.packCurrent);
  }
  else if (code == "Temperature")
  {
    fahrenheitTempAvg = (double(bms.get.tempAverage) * 1.8) + 32; // C to F conversion
    return fahrenheitTempAvg;
  }
  else if (code == "Watts")
  {
    totalPower = double(bms.get.packVoltage) * double(bms.get.packCurrent); // calculate total power (P=VI)
    return totalPower;
  }
  else if (code == "Cell1")
  {
    return double(bms.get.cellVmV[0]) / 1000; // cell voltage in V
  }
  else if (code == "Cell2")
  {
    return double(bms.get.cellVmV[1]) / 1000;
  }
  else if (code == "Cell3")
  {
    return double(bms.get.cellVmV[2]) / 1000;
  }
  else if (code == "Cell4")
  {
    return double(bms.get.cellVmV[3]) / 1000;
  }
  else
  {
    Serial.println("Invalid code: " + code);
    return -1;
  }
}