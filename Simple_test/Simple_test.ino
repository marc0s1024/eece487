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
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, -1);

void setup() {
  Serial.begin(115200);

  // Initialize backlight control (optional if VIN is already connected to LED)
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // Ensure backlight is on

  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_MOSI, OUTPUT);
  pinMode(TFT_SCK, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  Serial.println("Resetting display...");

  // Perform a reset sequence on the display
  digitalWrite(TFT_RST, LOW);   // Reset pin low to reset the display
  delay(150);                    // Wait 50 ms
  digitalWrite(TFT_RST, HIGH);  // Reset pin high to release display from reset
  delay(150);                   // Allow time for display to initialize

  // Begin SPI with custom pins for MOSI and SCK
  SPI.begin(TFT_SCK, -1, TFT_MOSI);  // (-1 indicates MISO is not used here)

  // Initialize the display
  Serial.println("Initializing display...");
  tft.begin();
  Serial.println("Display initialized");

  analogWrite(LED_PIN, 128);

  // Clear screen and display a message
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
  tft.fillScreen(0xF800);
  delay(1000);

  tft.drawLine(20, 20, 100, 150, ILI9341_CYAN);
  delay(1000);

  Serial.println("Filling screen with BLUE");
  tft.fillScreen(ILI9341_BLUE);
  delay(1000);

  Serial.println("Turning off backlight");
  // digitalWrite(LED_PIN, LOW);  // Turn off the backlight
  analogWrite(LED_PIN, 0);
  delay(1000);

  Serial.println("Filling screen with GREEN");
  tft.fillScreen(ILI9341_GREEN);
  delay(1000);

  Serial.println("Turning on backlight");
  //digitalWrite(LED_PIN, HIGH);  // Turn on the backlight
  analogWrite(LED_PIN, 128);
  delay(1000);

  Serial.println("Filling screen with BLACK");
  tft.fillScreen(ILI9341_BLACK);
  delay(1000);

  // Clear the screen and draw a red rectangle to test
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(20, 20, 200, 100, ILI9341_RED);
  delay(2000);
}