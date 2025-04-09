/*
  Pins set in User_Setup.h for TFT eSPI lib

  TFT_MOSI 19
  TFT_SCLK 18
  TFT_CS   27  // Chip select control pin
  TFT_DC   25  // Data Command control pin
  TFT_RST  26  // Reset pin (could connect to RST pin)
  TFT_BL   33            // LED back-light control pin
  TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)
*/

#include <Arduino.h>
//#include <TFT_eSPI.h>     // Hardware-specific display library
//#include <TFT_eWidget.h>  // Widget library for analogue meters
#include <daly-bms-uart.h> // Daly BMS UART library

// Pins set for BMS UART RX/TX
#define BMS_RX_PIN 22  // ESP32 GPIO16 (RX2) to BMS TX
#define BMS_TX_PIN 23  // ESP32 GPIO17 (TX2) to BMS RX

// Create an instance of the TFT display
//TFT_eSPI tft = TFT_eSPI();

// Create an instance of the Daly BMS driver using a hardware serial port
HardwareSerial BMS_Serial(2);  // use UART2 for BMS RX TX
Daly_BMS_UART bms(BMS_Serial);

// Create meter widgets for SOC, Voltage, and Current
//MeterWidget socMeter = MeterWidget(&tft);
//MeterWidget voltageMeter = MeterWidget(&tft);
//MeterWidget currentMeter = MeterWidget(&tft);

unsigned long lastUpdateTime = 0;  // timer var for serial mon update

void setup() {
  // Begin serial communication for debugging
  Serial.begin(115200);
  BMS_Serial.begin(9600, SERIAL_8N1, BMS_RX_PIN, BMS_TX_PIN);  // init UART2

  // Initialize the TFT display and set the rotation as desired
  //tft.init();
  //tft.setRotation(0);
  
  // Initialize the Daly BMS driver (sets up the UART interface)
  bms.Init();
  
  // Configure the SOC meter widget:
  // Parameters: x, y position, full scale value, units string,
  // and labels for scale markers (e.g., "0", "25", "50", "75", "100")
  //socMeter.analogMeter(10, 10, 100.0, "%", "0", "25", "50", "75", "100");
  
  // Configure the Voltage meter widget:
  // Adjust the full scale value and labels according to your battery specs.
  //voltageMeter.analogMeter(10, 130, 60.0, "V", "0", "15", "30", "45", "60");
  
  // Configure the Current meter widget:
  //currentMeter.analogMeter(10, 250, 200.0, "A", "0", "50", "100", "150", "200");
  
  // Optionally, you can set colour zones on each meter widget by calling setZones().
}

void loop() {
  // Update battery data from the BMS every 5 seconds
  if (millis() - lastUpdateTime >= 5000) {
    lastUpdateTime = millis();  // reset timer

    // Right now there's a bug where if you send more than one character it will read multiple times
    Serial.read(); // Discard the character sent
    Serial.read(); // Discard the new line
    
    // This .update() call populates the entire get struct. If you only need certain values (like
    // SOC & Voltage) you could use other public APIs, like getPackMeasurements(), which only query
    // specific values from the BMS instead of all.
    bms.update();

    // Update the meter needles with new readings from the bms.get structure.
    // The second parameter (0) can be used to control smoothing of the needle movement.
    //socMeter.updateNeedle(bms.get.packSOC, 0);
    //voltageMeter.updateNeedle(bms.get.packVoltage, 0);
    //currentMeter.updateNeedle(bms.get.packCurrent, 0);
    
    // Print the battery readings to the serial monitor for debugging.
    Serial.printf("Voltage: %.1f V, Current: %.1f A, SOC: %.1f%%\n",
                  bms.get.packVoltage, bms.get.packCurrent, bms.get.packSOC);
  }
  /*
  else {
    Serial.println("bms.update() returned false.");
  }
  */

  // Update at approximately one-second intervals (adjust delay as needed)
  delay(1000);
}
