#ifndef Display
#define Display

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#include <algorithm>

#include "NotoSans_Bold.h"
#include "OpenFontRender.h"
#include "daly-bms-uart.h"

#define TTF_FONT NotoSans_Bold
#define BMS_RX_PIN 2         // ESP32 GPIO16 (RX2) to BMS TX
#define BMS_TX_PIN 4         // ESP32 GPIO17 (TX2) to BMS RX

#define DARKER_GREY 0x18E3
#define Binghamton_Green 0x02C8
#define medium_green 0x1C07

extern TFT_eSPI tft;
extern TFT_eSprite spr;
extern OpenFontRender ofr;

// UART_Decode
void DecodeSetup();
void ReadBMS();
double GetValue(String code);

// Display_Functions
void DisplaySetup();
void UpdateDisplay();

// Arc_Meter
class Arc_Meter {
 public:
  Arc_Meter(int16_t x, int16_t y, int radius, const char *units);
  Arc_Meter(int16_t x, int16_t y, int radius, const char *units, int max_value);
  void init();
  void update(double inputVal);
  double meterValue();
  int getMax();

 private:
  int16_t x, y;     // Position
  uint16_t radius;  // Radius
  uint16_t arc_color = TFT_GREEN;
  int8_t thickness;
  double last_value = 0;  // Last displayed value
  double current_value = 0;
  double max_value = 100;
  uint16_t last_angle = 30;
  int16_t val_angle;
  float ramp = 1.0f;  // Use float and 'f' for float literal
  int8_t r;
  const char *units;  // Label
};

// Rectangle_Meter
class Rectangle_Meter {
 public:
  Rectangle_Meter(int16_t x, int16_t y, int16_t w, int16_t h, int thickness,
                  const char *units, int direction, int max_value,
                  int num_decimals);
  void init();
  void update(double inputVal);
  double meterValue();
  int getMax();

 private:
  int16_t x, y;                    // Position of the top left corner
  int16_t w, h;                    // Width and height
  int16_t thickness;               // Thickness of the rectangle
  int8_t radius = 15;              // Radius of corners
  int8_t spacing = 2 + thickness;  // Spacing between outer and inner rectangles
  uint16_t outline_color = TFT_SILVER;
  uint16_t fill_color = medium_green;
  uint16_t accent_color = Binghamton_Green;
  double last_value = 0;  // Last displayed value
  double current_value = 0;
  double max_value = 100;  // Maximum value of the meter
  int8_t num_decimals = 1;   // Number of decimals to display
  uint16_t last_height;      // Last drawn height of the bar
  uint16_t yBottom;          // Y coordinate of the bottom of the meter
  int16_t filledHeight = 0;
  int16_t newY = 0;
  int8_t direction;   // Direction the bar fills: 1 = up, -1 = down, 2 = right,
                      // -2 = left
  float ramp = 1.0f;  // Ramp speed for animation (float)
  const char *units;  // Label
};

// Meter Objects
extern Arc_Meter SOC;
extern Arc_Meter Voltage;
extern Arc_Meter Current;
extern Arc_Meter Temperature;
extern Rectangle_Meter Watts;

#endif