// If the radius is > 25 then the value is drawn in the middle

// The outer ring of the meter uses the drawSmoothCircle function (which draws
// a narrow full circle smooth arc)

// Uncomment to draw meter digits and label text
#define DRAW_DIGITS

// If DRAW_DIGITS is defined the OpenFontRender library must be loaded since
// the sketch uses a scaleable TrueType font for the text and numerals.
// https://github.com/Bodmer/OpenFontRender

#define LOOP_DELAY 0 // This controls how frequently the meter is updated
                     // for test purposes this is set to 0


#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

#ifdef DRAW_DIGITS
  #include "NotoSans_Bold.h"
  #include "OpenFontRender.h"
  #define TTF_FONT NotoSans_Bold
#endif

TFT_eSPI tft = TFT_eSPI();            // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object

#ifdef DRAW_DIGITS
OpenFontRender ofr;
#endif

#define DARKER_GREY 0x18E3


bool initMeter = true;  // initialize meter
int8_t ramp = 1;        // value that determines the rate and direction the meter changes
int disp_num = 0;    // number thats displayed in the center of the meter
int prev_num = 0;           // previously displayed number

int radius1 = 60;       // temporary test variables
int radius2 = 30;
int16_t row1 = (tft.width() * 1) / 3;
int16_t row2 = (tft.width() * 2) / 3;
int16_t column1 = (tft.height() * 1) / 4;
int16_t column2 = (tft.height() * 2) / 4;
int16_t column3 = (tft.height() * 3) / 4;


// each meter is an object so that you can individually control them
class Meter {
private:
  int16_t x, y;         // Position
  int radius;           // Radius
  const char *units;    // Label
  uint16_t last_angle;  // Last drawn angle
  int last_value;       // Last displayed value

public:
  // Constructor
  Meter(int16_t x, int16_t y, int radius, const char *units)
    : x(x), y(y), radius(radius), units(units), last_angle(30), last_value(-1) {}

  // Initialize the meter
  void init() {
    tft.fillCircle(x, y, radius, DARKER_GREY);
    tft.drawSmoothCircle(x, y, radius, TFT_SILVER, DARKER_GREY);
    uint16_t tmp = radius - 3;
    tft.drawArc(x, y, tmp, tmp - tmp / 5, last_angle, 330, TFT_BLACK, DARKER_GREY);

    // Draw the initial units label
    tft.setTextColor(TFT_GOLD, DARKER_GREY); // Text color with background color
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM); // Center the text
    tft.drawString(units, x, y + radius / 2);
    update(disp_num);
  }

  // Update the meter value and display digits
  void update(int val) {
    int r = radius - 3;  // Adjusted radius for drawing arcs
    int val_angle = map(val, 0, 100, 30, 330);
    tft.setTextSize(radius*0.08);

    // Only update if the value changes
    if (last_value != val) {
      // Clear previous digits by overwriting with the background color
      tft.setTextColor(DARKER_GREY, DARKER_GREY);
      tft.setTextDatum(MC_DATUM);
      tft.drawString(String(last_value), x, y); // Clear old value

      // Draw the new value
      tft.setTextColor(TFT_WHITE, DARKER_GREY);
      tft.drawString(String(val), x, y);

      last_value = val;
    }

    if (last_angle != val_angle) {
      uint8_t thickness = r / 5;
      if (r < 25) thickness = r / 3;

      if (val_angle > last_angle) {
        tft.drawArc(x, y, r, r - thickness, last_angle, val_angle, TFT_SKYBLUE, TFT_BLACK);
      } else {
        tft.drawArc(x, y, r, r - thickness, val_angle, last_angle, TFT_BLACK, DARKER_GREY);
      }
      last_angle = val_angle;
    }
  }
};

// create meter objects
Meter meter1(column3, row2, radius1, "Percent");
Meter meter2(column1, row1, radius2, "Volts");


void setup(void) {
  // this is from the Bodmer library, used for fonts, currently not used but could be implimented later
  #ifdef DRAW_DIGITS
    // Loading a font takes a few milliseconds, so for test purposes it is done outside the test loop
    if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
      Serial.println("Render initialize error");
      return;
    }
  #endif

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_NAVY);
  //tft.setViewport(0, 0, 240, 320);

  // initialize meters
  meter1.init();
  meter2.init();

  // for aligning
  // Draw a 3x2 grid (3 columns and 2 rows)
  drawGrid(4, 3, TFT_LIGHTGREY);
}


void loop() {
  // same as in setup
  #ifdef DRAW_DIGITS
    // Loading a font takes a few milliseconds, so for test purposes it is done outside the test loop
    if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
      Serial.println("Render initialize error");
      return;
    }
  #endif

  // for debugging; if serial available then update the meter with the value and print to console
  if (Serial.available() > 0) {
    // Read the incoming message
    String incomingMessage = Serial.readStringUntil('\n');
    incomingMessage.trim(); // Remove any trailing newlines or spaces

    if (incomingMessage) {
      Serial.print("Received: ");
      Serial.println(incomingMessage);

      prev_num = disp_num;
      disp_num = incomingMessage.toInt();
      
      // if the previous number is less than the new number, ramp = 1  (increase meter)
      // condition ? value_if_true : value_if_false;
      ramp = (prev_num < disp_num) ? 1 : -1;

      while (prev_num != disp_num) {
        prev_num += ramp;
        meter1.update(prev_num);
        meter2.update(prev_num);
        delay(30);  // remove if transition should be instantanious
      }
    } 
  }

  initMeter = true;
  
  #ifdef DRAW_DIGITS
    ofr.unloadFont(); // Recover space used by font metrics etc.
  #endif
}

void drawGrid(int cols, int rows, uint16_t color) {
  int colWidth = tft.width() / cols;  // Calculate width of each column
  int rowHeight = tft.height() / rows; // Calculate height of each row

  // Draw vertical grid lines
  for (int i = 1; i < cols; i++) {
    int x = i * colWidth;
    tft.drawLine(x, 0, x, tft.height(), color);
  }

  // Draw horizontal grid lines
  for (int i = 1; i < rows; i++) {
    int y = i * rowHeight;
    tft.drawLine(0, y, tft.width(), y, color);
  }
}
