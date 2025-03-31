#include "Display.h"

#define DARKER_GREY 0x18E3
#define Binghamton_Green 0x02C8

TFT_eSPI tft = TFT_eSPI();           // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(&tft); // Declare Sprite object "spr" with pointer to "tft" object
OpenFontRender ofr;

// Each meter is an object so that you can individually control them
// Circular meter class //
class Arc_Meter
{
private:
  int16_t x, y;    // Position
  uint16_t radius; // Radius
  uint16_t arc_color = TFT_GREEN;
  int8_t thickness;
  double last_value; // Last displayed value
  uint16_t max_value = 100;
  uint16_t last_angle;
  int16_t val_angle;
  int8_t ramp = 1.0;
  int8_t r;
  const char *units; // Label

public:
  // Constructor
  Arc_Meter(int16_t x, int16_t y, int radius, const char *units)
      : x(x), y(y), radius(radius), units(units), last_angle(30), last_value(-1) {}
  Arc_Meter(int16_t x, int16_t y, int radius, const char *units, int max_value)
      : x(x), y(y), radius(radius), units(units), last_angle(30), last_value(-1), max_value(max_value) {}

  // Initialize the meter
  void init()
  {
    tft.fillCircle(x, y, radius, DARKER_GREY);
    tft.drawSmoothCircle(x, y, radius, TFT_SILVER, DARKER_GREY);
    uint16_t tmp = radius - 3;
    tft.drawArc(x, y, tmp, tmp - tmp / 5, last_angle, 330, TFT_BLACK, DARKER_GREY);

    // Draw the initial units label
    tft.setTextColor(TFT_GOLD, DARKER_GREY); // Text color with background color
    tft.setTextSize(std::max(radius * 0.04, 2.0));
    tft.setTextDatum(MC_DATUM); // Center the text
    tft.drawString(units, x, y + radius / 2);

    r = radius - 3; // Adjusted radius for drawing arcs
    thickness = r / 5;
    update(0);
  }

  // Update the meter value and display digits
  void update(double val)
  {
    // If the value is negative, change the color to red
    if (val < 0)
    {
      val = abs(val);
      arc_color = TFT_RED;
    }
    else
    {
      arc_color = TFT_GREEN;
    }

    if (units == "SOC")
    {
      if (val > 60)
      {
        arc_color = TFT_GREEN;
      }
      else if (val > 30)
      {
        arc_color = TFT_YELLOW;
      }
      else
      {
        arc_color = TFT_RED;
      }
    }

    val_angle = map(val, 0, max_value, 30, 330);

    if (last_value != val)
    {
      double ramp = std::max(std::abs(val - last_value) / 10.0, 1.0);
      unsigned long startTime = millis();
      unsigned long animationDuration = 500; // Adjust as needed

      while (millis() - startTime < animationDuration)
      {
        double progress = (millis() - startTime) / (double)animationDuration;
        double currentVal = last_value + (val - last_value) * progress;

        // Draw new number
        tft.setTextColor(TFT_WHITE, DARKER_GREY);
        tft.drawString(String(last_value, 1), x, y); // Display last_value

        if (r < 25)
        {
          thickness = r / 3;
        }

        int current_angle = map(last_value, 0, max_value, 30, 330);

        if (current_angle > last_angle)
        {
          tft.drawArc(x, y, r, r - thickness, last_angle, current_angle, arc_color, TFT_BLACK);
        }
        else
        {
          tft.drawArc(x, y, r, r - thickness, current_angle, last_angle, TFT_BLACK, DARKER_GREY);
        }

        last_angle = current_angle;

        // delay(10); // Add a small delay for smoother animation
      }
      last_value = val; // Ensure final value is exact
    }

    // Final draw of Number, to ensure accuracy.
    tft.setTextColor(DARKER_GREY, DARKER_GREY);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(String(last_value, 1), x, y);

    tft.setTextColor(TFT_WHITE, DARKER_GREY);
    tft.drawString(String(last_value, 1), x, y);
    tft.drawArc(x, y, r, r - thickness, 30, val_angle, arc_color, TFT_BLACK);
  }

  // Get the value of the meter
  double getValue()
  {
    return last_value;
  }
  int getMax()
  {
    return max_value;
  }
};
// End Arc_Meter //

// Rectangular meter class //
class Rectangle_Meter
{
private:
  int16_t x, y; // Position of the top left corner
  int16_t w, h; // Width and height

  int16_t thickness;  // Thickness of the rectangle
  int8_t radius = 15; // Radius of corners
  int8_t spacing;     // Spacing between outer and inner rectangles
  uint16_t outline_color = TFT_SILVER;
  uint16_t fill_color = TFT_GREEN;
  uint16_t accent_color = Binghamton_Green;

  double last_value;        // Last displayed value
  uint16_t max_value = 100; // Maximum value of the meter
  int8_t num_decimals = 1;  // Number of decimals to display
  uint16_t last_height;     // Last drawn height of the bar
  uint16_t yBottom;         // Y coordinate of the bottom of the meter

  int8_t direction; // Direction the bar fills: 1 = up, -1 = down, 2 = right, -2 = left
  int8_t ramp = 1;  // Ramp speed for animation

  const char *units; // Label

public:
  // Constructor
  Rectangle_Meter(int16_t x, int16_t y, int16_t w, int16_t h, int thickness, const char *units, int direction, int max_value, int num_decimals)
      : x(x), y(y), w(w), h(h), thickness(thickness), units(units), last_height(30), last_value(-1), direction(direction), max_value(max_value), num_decimals(num_decimals) {}

  // Initialize the meter
  void init()
  {
    // outside shell
    tft.fillSmoothRoundRect(x, y, w, h, radius, DARKER_GREY, Binghamton_Green);
    tft.drawSmoothRoundRect(x, y, radius, radius - thickness, w, h, outline_color, Binghamton_Green);
    double thin = thickness / 2;
    // tft.drawSmoothRoundRect(x + thin, y + thin, radius, radius - 1, w - thin * 2, h - thin * 2, accent_color, accent_color);
    tft.drawRoundRect(x + thin, y + thin, w - 1.5 * thin, h - 1.5 * thin, radius - thin, accent_color);
    // inner bar
    // tft.fillSmoothRoundRect(x + spacing, y + spacing, 1 + w - 2 * spacing, 1 + h - 2 * spacing, radius - thickness, Binghamton_Green, Binghamton_Green);

    yBottom = y + h - spacing;
    update(0);
  }

  void update(double val)
  {
    // If the value is negative, change the color to red
    if (val < 0)
    {
      val = abs(val);
      fill_color = TFT_RED;
    }
    else
    {
      fill_color = TFT_GREEN;
    }

    if (last_value != val)
    {
      double ramp = std::max(std::abs(val - last_value) / 10.0, 1.0);
      unsigned long startTime = millis();
      unsigned long animationDuration = 500;

      while (millis() - startTime < animationDuration)
      {
        double progress = (millis() - startTime) / (double)animationDuration;
        double currentVal = last_value + (val - last_value) * progress;

        // Clear previous filled area
        tft.fillSmoothRoundRect(x + spacing, y + spacing, w - 2 * spacing, h - 2 * spacing, radius - spacing, TFT_BLACK, TFT_BLACK);
        int16_t filledHeight = (last_value * (h - 2 * spacing)) / max_value;
        int16_t newY = yBottom - filledHeight;
        tft.fillSmoothRoundRect(x + spacing, newY, w - 2 * spacing, filledHeight, radius - spacing, fill_color, DARKER_GREY);

        // Draw the new value
        tft.setTextSize(2);
        tft.setTextDatum(MC_DATUM);
        // number
        tft.setTextColor(TFT_WHITE);
        tft.drawString(String(last_value, num_decimals), x + w / 2, y - 10 + h / 2);
        // units
        tft.setTextColor(TFT_GOLD);
        tft.drawString(units, x + w / 2, y + 10 + h / 2);

        delay(10);
      }
      last_value = val; // Ensure final value is exact
    }
  }

  // Get the value of the meter
  double
  getValue()
  {
    return last_value;
  }
  int getMax()
  {
    return max_value;
  }
};
// End Rectangular_Meter //

// Touchscreen variables
int8_t currentPage = 0;              // 0 = main page, 1 = cell page
const int maxTapDuration = 500;      // max duration for a tap in milliseconds
const int tapMovementThreshold = 10; // max movement for a tap in pixels

uint8_t radius1 = 60; // temporary test variables
uint8_t radius2 = 38;

// NOTE: height is the long side (320), width(240)
// 2*3 grid
int16_t row1 = (tft.width() * 1) / 3; // 240 / 3 = 80
int16_t row2 = (tft.width() * 2) / 3;
int16_t column1 = (tft.height() * 1) / 4; // 320 / 4 = 80
int16_t column2 = (tft.height() * 2) / 4;
int16_t column3 = (tft.height() * 3) / 4;

// layout 1
// columns
int16_t first_column = tft.height() * 0.2;   // wattage
int16_t second_column = tft.height() * 0.74; // voltage, temperature
int16_t third_column = tft.height() * 0.85;  // current
int16_t center_width = tft.height() * 0.5;   // SOC
// rows
int16_t first_row = tft.width() * 0.18;    // voltage
int16_t second_row = tft.width() * 0.82;   // temperature
int16_t center_height = tft.width() * 0.5; // SOC, current

//--------------------------------------------------------
// Meter objects for page 1
// (Voltage, Current, SOC, Temperature, Watts)
//--------------------------------------------------------
// (x, y, radius, units)
Arc_Meter Voltage(second_column, first_row, radius2, "V", 20);
Arc_Meter Current(third_column, center_height, radius2, "A", 20);
Arc_Meter SOC(center_width, center_height, radius1, "%");
Arc_Meter Temperature(second_column, second_row, radius2, "F", 110); // degree symbol: °C   idk if it works though so test later -> it doesnt work - Marcos

// (x, y, w, h, thickness, units, direction, max_value)
Rectangle_Meter Watts(8, 20, 82, tft.width() - 40, 4, "Watts", -1, 1200, 1);

//--------------------------------------------------------
// Meter objects for page 2
// (Cell 1, Cell 2, Cell 3, Cell 4 Voltages)
//--------------------------------------------------------
Rectangle_Meter Cell1(0, 20, 78, tft.width() - 40, 4, "Cell1", -1, 4, 2);
Rectangle_Meter Cell2(80, 20, 78, tft.width() - 40, 4, "Cell2", -1, 4, 2);
Rectangle_Meter Cell3(160, 20, 78, tft.width() - 40, 4, "Cell3", -1, 4, 2);
Rectangle_Meter Cell4(240, 20, 78, tft.width() - 40, 4, "Cell4", -1, 4, 2);

void DrawMainPage()
{
  tft.fillScreen(Binghamton_Green);
  // tft.setViewport(0, 0, 240, 320);

  // initialize meters
  Voltage.init();
  Current.init();
  SOC.init();
  Temperature.init();
  Serial.println("Watts init:");
  Watts.init();
}

void DrawCellPage()
{
  tft.fillScreen(Binghamton_Green);

  // initialize meters
  Cell1.init();
  Cell2.init();
  Cell3.init();
  Cell4.init();
}

void DisplaySetup()
{
  tft.begin();

  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT)))
  {
    Serial.println("Render initialize error");
    return;
  }

  tft.begin();
  tft.setRotation(3);

  // start with main page (page 1)
  currentPage = 0;
  DrawMainPage();

  // for aligning
  // Draw a 3x2 grid (3 columns and 2 rows)
  // DrawGrid(4, 3, TFT_LIGHTGREY);
}

void UpdateDisplay(String code, double inputVal)
{
  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT)))
  {
    Serial.println("Render initialize error");
    return;
  }

  if (currentPage == 0) // check if page 1
  {
    if (code == "SOC")
    {
      SOC.update(inputVal);
    }
    else if (code == "Voltage")
    {
      Voltage.update(inputVal);
    }
    else if (code == "Current")
    {
      Current.update(inputVal);
    }
    else if (code == "Temperature")
    {
      Temperature.update(inputVal);
    }
    else if (code == "Watts")
    {
      Watts.update(inputVal);
    }
    else
    {
      Serial.println("Invalid code: " + code);
      ofr.unloadFont();
      return;
    }
  }
  else if (currentPage == 1) // check if page 2
  {
    if (code == "Cell1")
    {
      Cell1.update(inputVal);
    }
    else if (code == "Cell2")
    {
      Cell2.update(inputVal);
    }
    else if (code == "Cell3")
    {
      Cell3.update(inputVal);
    }
    else if (code == "Cell4")
    {
      Cell4.update(inputVal);
    }
    else
    {
      Serial.println("Invalid code: " + code);
      ofr.unloadFont();
      return;
    }
  }
  else
  {
    Serial.println("Invalid page: " + String(currentPage));
    ofr.unloadFont();
    return;
  }

  ofr.unloadFont(); // Recover space used by font metrics etc.
}

void SwitchPage() // to be called from Display.ino when a swipe is detected
{
  currentPage = (currentPage == 0) ? 1 : 0; // toggle between pages 1 and 2

  if (currentPage == 0)
  {
    DrawMainPage();
  }
  else if (currentPage == 1)
  {
    DrawCellPage();
  }
}

void CheckTap()
{
  static bool touchActive = false;
  static int16_t startX = 0, startY = 0;
  static int16_t lastX = 0, lastY = 0;
  static unsigned long startTime = 0;
  uint16_t touchX, touchY;

  // check if the screen is currently touched
  if (tft.getTouch(&touchX, &touchY))
  {
    if (!touchActive)
    {
      // new touch detected, record start position and time.
      startX = touchX;
      startY = touchY;
      startTime = millis();
      touchActive = true;
    }
    // update last known touch coords
    lastX = touchX;
    lastY = touchY;
  }
  else
  {
    // if touch was active and now released, determine if is tap
    if (touchActive)
    {
      unsigned long touchDuration = millis() - startTime;
      int deltaX = abs(lastX - startX);
      int deltaY = abs(lastY - startY);

      // check if the touch qualifies as a tap
      if (touchDuration < maxTapDuration && deltaX < tapMovementThreshold && deltaY < tapMovementThreshold)
      {
        Serial.println("Tap detected, switching page");
        SwitchPage();
      }
      touchActive = false;
    }
  }
}

void DrawGrid(int cols, int rows, uint16_t color)
{
  int colWidth = tft.width() / cols;   // Calculate width of each column
  int rowHeight = tft.height() / rows; // Calculate height of each row

  // Draw vertical grid lines
  for (int i = 1; i < cols; i++)
  {
    int x = i * colWidth;
    tft.drawLine(x, 0, x, tft.height(), color);
  }

  // Draw horizontal grid lines
  for (int i = 1; i < rows; i++)
  {
    int y = i * rowHeight;
    tft.drawLine(0, y, tft.width(), y, color);
  }
}