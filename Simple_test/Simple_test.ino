#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Define the pins for the ILI9341 display
#define TFT_CS   27    // Chip Select
#define TFT_DC   25    // Data/Command
#define TFT_RST  26    // Reset
#define TFT_MOSI 23    // Master Out Slave In
#define TFT_SCK  18    // Serial Clock
#define LED_PIN  35    // Backlight control

// Create an instance of the display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);

  // Initialize the backlight control pin
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Initializing display...");

  // Begin SPI with custom pins for MOSI and SCK
  SPI.begin(TFT_SCK, -1, TFT_MOSI);  // (-1 indicates MISO is not used here)

  // Toggle the backlight (LED_PIN) to check if the display powers on
  Serial.println("Turning on the backlight");
  digitalWrite(LED_PIN, HIGH);  // Turn on backlight
  delay(1000);                  // Wait for 1 second

  // Initialize the display
  tft.begin();
  Serial.println("Display initialized");

  // Basic welcome screen to verify functionality
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 120);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("ESP32 Display Test");

  delay(2000); // Display the welcome message for 2 seconds
}

void loop() {
  // Cycle through colors to test the display
  Serial.println("Filling screen with RED");
  tft.fillScreen(ILI9341_RED);
  delay(1000);

  Serial.println("Filling screen with GREEN");
  tft.fillScreen(ILI9341_GREEN);
  delay(1000);

  Serial.println("Filling screen with BLUE");
  tft.fillScreen(ILI9341_BLUE);
  delay(1000);

  Serial.println("Turning off backlight");
  digitalWrite(LED_PIN, LOW);  // Turn off the backlight
  delay(1000);

  Serial.println("Turning on backlight");
  digitalWrite(LED_PIN, HIGH);  // Turn on the backlight
  delay(1000);

  Serial.println("Filling screen with BLACK");
  tft.fillScreen(ILI9341_BLACK);
  delay(1000);
}