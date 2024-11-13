#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Define the pins for the ILI9341 display
#define TFT_CS   27    // Chip Select
#define TFT_DC   25    // Data/Command
#define TFT_RST  26    // Reset
#define TFT_MOSI 19    // Master Out Slave In
#define TFT_SCK  18    // Serial Clock
#define LED_PIN  33   // Backlight control

// Create an instance of the display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Dial parameters
const int centerX = 240/2; // X coordinate of center of dial
const int centerY = 320/2; // Y coordinate of center of dial
const int radius = 80;   // Radius of the dial circle

// Angle calculation constants
const float maxAngle = 360;  // Max angle for the dial (can set to 180 degrees for half-dial)
const float angleStep = maxAngle / 100.0;  // Calculate the angle step per increment

void setup() {
  Serial.begin(115200);

  // Begin SPI with custom pins for MOSI and SCK
  SPI.begin(TFT_SCK, -1, TFT_MOSI);  // (-1 indicates MISO is not used here)

  // Initialize the backlight control pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // Turn on backlight
  
  // Initialize the display
  tft.begin();
  // tft.setRotation(3); // Adjust rotation if needed
  
  // Initial fill
  tft.fillScreen(ILI9341_BLACK);
  drawDial();
  drawNeedle(0);
}

void loop() {
  for (int value = 0; value <= 100; value++) {
    // Update the dial value and redraw it
    tft.fillScreen(ILI9341_BLACK);  // Clear the screen
    drawDial();  // Redraw the dial
    drawNeedle(value);  // Draw the needle at the current value
    
    // Display the current value on the dial
    tft.setCursor(centerX - 20, centerY + radius + 10);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.print(value);
    
    delay(100);  // Update every 100 ms (adjust for animation speed)
  }
}

void drawDial() {
  // Draw the dial circle
  tft.drawCircle(centerX, centerY, radius, ILI9341_WHITE);
  
  // Draw the scale lines (from 0 to 100)
  for (int i = 0; i <= 100; i += 10) {
    float angle = i * angleStep;  // Calculate the angle for each value
    int x1 = centerX + cos(radians(90 - angle)) * (radius - 10);
    int y1 = centerY - sin(radians(90 - angle)) * (radius - 10);
    int x2 = centerX + cos(radians(90 - angle)) * radius;
    int y2 = centerY - sin(radians(90 - angle)) * radius;
    tft.drawLine(x1, y1, x2, y2, ILI9341_WHITE);
  }
}

void drawNeedle(int value) {
  // Calculate the angle based on the value
  float angle = value * angleStep;
  
  // Calculate the needle position
  int needleX = centerX + cos(radians(90 - angle)) * (radius - 10);
  int needleY = centerY - sin(radians(90 - angle)) * (radius - 10);
  
  // Draw the needle (a line)
  tft.drawLine(centerX, centerY, needleX, needleY, ILI9341_RED);
}
