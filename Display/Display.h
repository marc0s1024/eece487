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
#define BMS_RX_PIN 2        // ESP32 GPIO16 (RX2) to BMS TX
#define BMS_TX_PIN 4        // ESP32 GPIO17 (TX2) to BMS RX
extern int8_t current_page;  // 0 = main page, 1 = cell page

// UART_Decode
void DecodeSetup();
void ReadBMS();
double GetValue(String code);

// Display_Functions
void DrawMainPage();
void DrawCellPage();
void DisplaySetup();
void UpdateDisplay(String code, double inputVal);
void SwitchPage();
void CheckTap();
void DrawGrid(int cols, int rows, uint16_t color);

/*
///////// Defined in User_Setup.h for the display library /////////
Must use the file in the arduino library folder. There is a shortcut
to it in the github. Display will not work unless you put the pins
where the source User_Setup.h file says they go.
C:\Arduino\libraries\TFT_eSPI\User_Setup.h

#define USER_SETUP_INFO "User_Setup"
#define ILI9341_DRIVER       // Generic driver for common displays

#define TFT_CS   27  // Chip select control pin
#define TFT_RST  26  // Reset pin (could connect to RST pin)
#define TFT_DC   25  // Data Command control pin
#define TFT_MOSI 33
#define TFT_SCLK 32
#define TFT_BL   14            // LED back-light control pin
#define TFT_MISO 12
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to
ESP32 board RST #define TOUCH_CS 15     // Chip select pin (T_CS) of touch
screen


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes
in FLASH #define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534
bytes in FLASH, 96 characters #define LOAD_FONT4  // Font 4. Medium 26 pixel
high font, needs ~5848 bytes in FLASH, 96 characters #define LOAD_FONT6  //
Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters
1234567890:-.apm #define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs
~2438 bytes in FLASH, only characters 1234567890:-. #define LOAD_FONT8  //
Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters
1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower,
so 3 digits fit a 160 pixel TFT #define LOAD_GFXFF  // FreeFonts. Include access
to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts #define
SMOOTH_FONT

#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
*/

#endif