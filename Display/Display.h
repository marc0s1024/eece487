#ifndef Display
#define Display

#define BMS_RX_PIN 23 // ESP32 GPIO16 (RX2) to BMS TX
#define BMS_TX_PIN 22 // ESP32 GPIO17 (TX2) to BMS RX

#include <Arduino.h>
#include "daly-bms-uart.h"
#include "NotoSans_Bold.h"
#include "OpenFontRender.h"
#define TTF_FONT NotoSans_Bold

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

// UART_Decode
void DecodeSetup();
void ReadBMS();
int GetValue(String code);

// Display_Functions
void DisplaySetup();
void UpdateDisplay(String code, int inputVal);
void DrawGrid(int cols, int rows, uint16_t color);

/*
///////// Defined in User_Setup.h for the display library /////////
#define USER_SETUP_INFO "User_Setup"
#define ILI9341_DRIVER       // Generic driver for common displays

#define TFT_CS   27  // Chip select control pin
#define TFT_RST  26  // Reset pin (could connect to RST pin)
#define TFT_DC   25  // Data Command control pin
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_BL   33            // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT

#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
*/

#endif