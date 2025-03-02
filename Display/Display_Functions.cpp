// If the radius is > 25 then the value is drawn in the middle

// The outer ring of the meter uses the drawSmoothCircle function (which draws
// a narrow full circle smooth arc)

#include "Display.h"

#define LOOP_DELAY 0 // This controls how frequently the meter is updated \
                      // for test purposes this is set to 0
#define DARKER_GREY 0x18E3
#define Binghamton_Green 0x02C8

TFT_eSPI tft = TFT_eSPI();           // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(&tft); // Declare Sprite object "spr" with pointer to "tft" object
OpenFontRender ofr;

bool initMeter = true; // initialize meter

// each meter is an object so that you can individually control them
// Circular meter class //
class Arc_Meter
{
private:
  int16_t x, y;        // Position
  int radius;          // Radius
  const char *units;   // Label
  uint16_t last_angle; // Last drawn angle
  int last_value;      // Last displayed value

public:
  // Constructor
  Arc_Meter(int16_t x, int16_t y, int radius, const char *units)
      : x(x), y(y), radius(radius), units(units), last_angle(30), last_value(-1) {}

  // Initialize the meter
  void init()
  {
    tft.fillCircle(x, y, radius, DARKER_GREY);
    tft.drawSmoothCircle(x, y, radius, TFT_SILVER, DARKER_GREY);
    uint16_t tmp = radius - 3;
    tft.drawArc(x, y, tmp, tmp - tmp / 5, last_angle, 330, TFT_BLACK, DARKER_GREY);

    // Draw the initial units label
    tft.setTextColor(TFT_GOLD, DARKER_GREY); // Text color with background color
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM); // Center the text
    tft.drawString(units, x, y + radius / 2);
    update(0);
  }

  // Update the meter value and display digits
  void update(int val)
  {
    int r = radius - 3; // Adjusted radius for drawing arcs
    int val_angle = map(val, 0, 100, 30, 330);
    tft.setTextSize(radius * 0.08);

    // Only update if the value changes
    if (last_value != val)
    {
      // Clear previous digits by overwriting with the background color
      tft.setTextColor(DARKER_GREY, DARKER_GREY);
      tft.setTextDatum(MC_DATUM);
      tft.drawString(String(last_value), x, y); // Clear old value

      // Draw the new value
      tft.setTextColor(TFT_WHITE, DARKER_GREY);
      tft.drawString(String(val), x, y);

      last_value = val;
    }

    if (last_angle != val_angle)
    {
      uint8_t thickness = r / 5;
      if (r < 25)
        thickness = r / 3;

      if (val_angle > last_angle)
      {
        tft.drawArc(x, y, r, r - thickness, last_angle, val_angle, TFT_SKYBLUE, TFT_BLACK);
      }
      else
      {
        tft.drawArc(x, y, r, r - thickness, val_angle, last_angle, TFT_BLACK, DARKER_GREY);
      }
      last_angle = val_angle;
    }
  }

  // Get the value of the meter
  int getValue()
  {
    return last_value;
  }
};
// End Arc_Meter //

// Rectangular meter class //
class Rectangle_Meter
{
private:
  int16_t x, y;                // Position of the top left corner
  int16_t w, h;                // Width and height
  int thickness;               // Thickness of the rectangle
  const char *units;           // Label
  uint16_t last_height;        // Last drawn height of the bar
  int last_value;              // Last displayed value
  int direction;               // Direction the bar fills: 1 = up, -1 = down, 2 = right, -2 = left
  int radius = 15;             // Radius of corners
  int spacing = 2 + thickness; // Spacing between outer and inner rectangles
  int max_value = 100;
  int yBottom;

public:
  // Constructor
  Rectangle_Meter(int16_t x, int16_t y, int16_t w, int16_t h, int thickness, const char *units, int direction, int max_value)
      : x(x), y(y), w(w), h(h), thickness(thickness), units(units), last_height(30), last_value(-1), direction(direction), max_value(max_value) {}

  // Initialize the meter
  void init()
  {
    // outside shell
    tft.drawSmoothRoundRect(x, y, radius, radius - thickness, w, h, TFT_WHITE, Binghamton_Green);
    float thin = thickness / 4;
    tft.drawSmoothRoundRect(x + thin, y + thin, radius, radius-1, w - thin * 2, h - thin * 2, DARKER_GREY, DARKER_GREY);
    // inner bar
    // tft.fillSmoothRoundRect(x + spacing, y + spacing, 1 + w - 2 * spacing, 1 + h - 2 * spacing, radius - thickness, Binghamton_Green, Binghamton_Green);

    yBottom = y + h - spacing;
    update(0);
  }

  void update(int val)
  {
    if (last_value != val)
    {
      // Clear previous filled area
      tft.fillSmoothRoundRect(x + spacing, y + spacing, w - 2 * spacing, h - 2 * spacing, radius - thickness, Binghamton_Green, Binghamton_Green);

      // Calculate filled height
      int filledHeight = (val * (h - 2 * spacing)) / max_value;

      // Calculate starting Y coordinate
      int newY = yBottom - filledHeight;

      // Draw the filled rectangle
      tft.fillSmoothRoundRect(x + spacing, newY, w - 2 * spacing, filledHeight, radius - thickness, TFT_RED, Binghamton_Green);

      // Draw the new value
      tft.setTextSize(2);
      tft.setTextDatum(MC_DATUM);
      // number
      tft.setTextColor(TFT_WHITE, DARKER_GREY);
      tft.drawString(String(val), x + w / 2, y - 10 + h / 2);
      // units
      tft.setTextColor(TFT_GOLD, DARKER_GREY);
      tft.drawString(units, x + w / 2, y + 10 + h / 2);

      last_value = val;
    }
  }

  // Get the value of the meter
  int getValue()
  {
    return last_value;
  }
};
// End Rectangular_Meter //

int radius1 = 60; // temporary test variables
int radius2 = 30;

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
int16_t second_column = tft.height() * 0.75; // voltage, temperature
int16_t third_column = tft.height() * 0.85;  // current
int16_t center_width = tft.height() * 0.5;   // SOC
// rows
int16_t first_row = tft.width() * 0.2;     // voltage
int16_t second_row = tft.width() * 0.8;    // temperature
int16_t center_height = tft.width() * 0.5; // SOC, current

// Meter Objects
// (x, y, radius, units)
Arc_Meter Voltage(second_column, first_row, radius2, "Volts");
Arc_Meter Current(third_column, center_height, radius2, "Amps");
Arc_Meter SOC(center_width, center_height, radius1, "SOC");
Arc_Meter Temperature(second_column, second_row, radius2, "C"); // degree symbol: °C   idk if it works though so test later

// (x, y, w, h, thickness, units, direction, max_value)
Rectangle_Meter Watts(10, tft.width() - 210, 80, 200, 4, "W", -1, 1200);

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
  tft.fillScreen(Binghamton_Green);
  // tft.setViewport(0, 0, 240, 320);

  // initialize meters
  Voltage.init();
  Current.init();
  SOC.init();
  Temperature.init();
  Serial.println("Watts init:");
  Watts.init();

  // for aligning
  // Draw a 3x2 grid (3 columns and 2 rows)
  // DrawGrid(4, 3, TFT_LIGHTGREY);
}

void UpdateDisplay(String code, int inputVal) // maybe change inputVal to float or something with a decimal
{
  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT)))
  {
    Serial.println("Render initialize error");
    return;
  }

  Arc_Meter *arc_meter = NULL;
  Rectangle_Meter *rect_meter = NULL;
  int8_t ramp = 1;  // value that determines the rate and direction the meter changes
  int disp_num = 0; // number thats displayed in the center of the meter
  int prev_num = 0; // previously displayed number
  int meter = 0;    // 0 for arc, 1 for rect

  // Serial.print("Received:\n");
  // Serial.println("Variable code: " + code + "\nValue: " + inputVal + "\n\n");

  if (code == "SOC")
  {
    arc_meter = &SOC;
    meter = 0;
  }
  else if (code == "Voltage")
  {
    arc_meter = &Voltage;
    meter = 0;
  }
  else if (code == "Current")
  {
    arc_meter = &Current;
    meter = 0;
  }
  else if (code == "Temperature")
  {
    arc_meter = &Temperature;
    meter = 0;
  }
  else if (code == "Watts")
  {
    rect_meter = &Watts;
    meter = 1;
  }
  else
  {
    Serial.println("Invalid code: " + code);
    ofr.unloadFont();
    return;
  }

  if (meter == 0)
  {
    // Verify that the pointer is valid
    if (arc_meter == NULL)
    {
      Serial.println("Error: Meter pointer is NULL");
      ofr.unloadFont();
      return;
    }

    prev_num = arc_meter->getValue();
    disp_num = inputVal;

    // if the previous number is less than the new number, ramp = 1  (increase meter)
    // condition ? value_if_true : value_if_false;
    ramp = (prev_num < disp_num) ? 1 : -1;

    while (prev_num != disp_num)
    {
      prev_num += ramp;
      arc_meter->update(prev_num);
      delay(30); // remove if transition should be instantanious
    }
  }
  else if (meter == 1)
  {
    // Verify that the pointer is valid
    if (rect_meter == NULL)
    {
      Serial.println("Error: Meter pointer is NULL");
      ofr.unloadFont();
      return;
    }

    prev_num = rect_meter->getValue();
    disp_num = inputVal;

    // if the previous number is less than the new number, ramp = 1  (increase meter)
    // condition ? value_if_true : value_if_false;
    ramp = (prev_num < disp_num) ? 1 : -1;

    while (prev_num != disp_num)
    {
      prev_num += ramp;
      rect_meter->update(prev_num);
      delay(30); // remove if transition should be instantanious
    }
  }
  initMeter = true; // i dont think this actually does anything
  ofr.unloadFont(); // Recover space used by font metrics etc.
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