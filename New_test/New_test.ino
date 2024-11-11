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

// Ball properties
int ballX = 120;
int ballY = 160;
int ballRadius = 20;
int ballSpeedX = 3;
int ballSpeedY = 3;
uint16_t ballColor = ILI9341_RED;

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
  // Clear screen before drawing ball (for animation effect)
  tft.fillScreen(ILI9341_BLACK);

  // Draw the ball
  tft.fillCircle(ballX, ballY, ballRadius, ballColor);

  // Update ball position
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Bounce the ball off the walls
  if (ballX >= tft.width() - ballRadius || ballX <= ballRadius) {
    ballSpeedX = -ballSpeedX;  // Reverse X direction
    changeBallColor();         // Change color on bounce
  }
  
  if (ballY >= tft.height() - ballRadius || ballY <= ballRadius) {
    ballSpeedY = -ballSpeedY;  // Reverse Y direction
    changeBallColor();         // Change color on bounce
  }

  delay(20); // Delay for smooth animation
}

// Function to randomly change the ball's color on bounce
void changeBallColor() {
  int colorChoice = random(0, 3);
  if (colorChoice == 0) {
    ballColor = ILI9341_RED;
  } else if (colorChoice == 1) {
    ballColor = ILI9341_GREEN;
  } else {
    ballColor = ILI9341_BLUE;
  }
}
