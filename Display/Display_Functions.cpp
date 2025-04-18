#include "Display.h"

TFT_eSPI tft =
    TFT_eSPI(); // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(
    &tft); // Declare Sprite object "spr" with pointer to "tft" object
OpenFontRender ofr;

//------------------------------------------------
// Arc_Meter Class
//------------------------------------------------

// Constructor 1
Arc_Meter::Arc_Meter(int16_t x, int16_t y, int radius, const char *units)
    : x(x),
      y(y),
      radius(radius),
      units(units),
      arc_color(TFT_GREEN),
      last_value(0),
      current_value(0),
      max_value(100),
      last_angle(30),
      ramp(1.0f)
{
  // Empty constructor body (initialization done in initializer list)
}

// Constructor 2 with max_value parameter
Arc_Meter::Arc_Meter(int16_t x, int16_t y, int radius, const char *units,
                     int max_value)
    : x(x),
      y(y),
      radius(radius),
      units(units),
      max_value(max_value),
      arc_color(TFT_GREEN),
      last_value(0),
      current_value(0),
      last_angle(30),
      ramp(1.0f)
{
  // Empty constructor body
}

// init() method implementation
void Arc_Meter::init()
{
  tft.fillCircle(x, y, radius, DARKER_GREY);
  tft.drawSmoothCircle(x, y, radius, TFT_SILVER, DARKER_GREY);
  uint16_t tmp = radius - 3;
  tft.drawArc(x, y, tmp, tmp - tmp / 5, last_angle, 330, TFT_BLACK,
              DARKER_GREY);

  // Draw the units label centered at (x, y + radius/2)
  tft.setTextColor(TFT_GOLD, DARKER_GREY);
  tft.setTextSize(std::max(radius * 0.04, 2.0));
  tft.setTextDatum(MC_DATUM);
  tft.drawString(units, x, y + radius / 2);

  // Set adjusted values for drawing the arc
  r = radius - 3;
  thickness = r / 5;
  update(last_value);
}

// update() method implementation
void Arc_Meter::update(double val)
{
  // Adjust color if value is negative
  if (val < 0)
  {
    val = abs(val);
    arc_color = TFT_RED;
  }
  else
  {
    arc_color = medium_green;
  }

  // Special handling for "SOC" meter
  if (strcmp(units, "SOC") == 0)
  {
    if (val > 60)
    {
      arc_color = medium_green;
    }
    else if (val > 30 && val <= 60)
    {
      arc_color = TFT_YELLOW;
    }
    else
    {
      arc_color = TFT_RED;
    }
  }

  // Map the current value to an angle in the range [30, 330]
  val_angle = map(val, 0, max_value, 30, 330);

  // Animate if the value has changed
  if (last_value != val)
  {
    // Calculate ramp as a fixed percentage of max_value instead of based on difference
    ramp = max_value * 0.05; // 5% of max value per animation step
    unsigned long startTime = millis();
    unsigned long animationDuration = 1000; // Duration in milliseconds

    while (millis() - startTime < animationDuration)
    {
      float progress = (millis() - startTime) / (float)animationDuration;

      // Use simple linear interpolation for smooth animation
      current_value = last_value + progress * (val - last_value);

      // Ensure we don't overshoot
      if ((val > last_value && current_value > val) ||
          (val < last_value && current_value < val))
      {
        current_value = val;
      }

      int current_angle = map(current_value, 0, max_value, 30, 330);

      // Redraw only the arc segment that changed
      if (current_angle > last_angle)
      {
        tft.drawArc(x, y, r, r - thickness, last_angle, current_angle,
                    arc_color, TFT_BLACK);
      }
      else if (current_angle < last_angle)
      {
        tft.drawArc(x, y, r, r - thickness, current_angle, last_angle,
                    TFT_BLACK, DARKER_GREY);
      }

      // Clear and update the displayed numeric value
      int textWidth = tft.textWidth(String(last_value, 1));
      int textHeight = tft.fontHeight();
      tft.fillRect(x - textWidth / 2, y - textHeight / 2, textWidth, textHeight,
                   DARKER_GREY);
      tft.setTextColor(TFT_WHITE, DARKER_GREY);
      tft.drawString(String(current_value, 1), x, y);

      last_angle = current_angle;
      delay(5);
    }
    last_value = val;
  }

  // Final redraw to ensure accuracy
  int textWidth = tft.textWidth(String(max_value, 1));
  int textHeight = tft.fontHeight();
  tft.fillRect(x - textWidth / 2, y - textHeight / 2, textWidth, textHeight,
               DARKER_GREY);
  tft.setTextColor(TFT_WHITE, DARKER_GREY);
  tft.drawString(String(last_value, 1), x, y);
  tft.drawArc(x, y, r, r - thickness, 30, 330,
              TFT_BLACK, DARKER_GREY);
  tft.drawArc(x, y, r, r - thickness, 30, val_angle, arc_color, TFT_BLACK);
}

// Get the previous value
double Arc_Meter::meterValue() { return last_value; }
// Get the max value
int Arc_Meter::getMax() { return int(max_value); }

// ---------- End Arc_Meter ---------- //

//------------------------------------------------
// Arc_Meter Class
//------------------------------------------------

// Constructor 1
Rectangle_Meter::Rectangle_Meter(int16_t x, int16_t y, int16_t w, int16_t h,
                                 int thickness, const char *units,
                                 int direction, int max_value, int num_decimals)
    : x(x),
      y(y),
      w(w),
      h(h),
      thickness(thickness),
      units(units),
      last_height(30),
      direction(direction),
      max_value(max_value),
      num_decimals(num_decimals) {}

// Initialize the meter
void Rectangle_Meter::init()
{
  // outside shell
  tft.fillSmoothRoundRect(x, y, w, h, radius, DARKER_GREY, Binghamton_Green);
  tft.drawSmoothRoundRect(x, y, radius, radius - thickness, w, h, outline_color,
                          Binghamton_Green);
  double thin = thickness / 2;
  tft.drawRoundRect(x + thin, y + thin, w - 1.5 * thin, h - 1.5 * thin,
                    radius - thin, accent_color);

  yBottom = y + h - spacing;
  update(last_value);
}

void Rectangle_Meter::update(double val)
{
  // If the value is negative, change the color to red
  if (val < 0)
  {
    val = abs(val);
    fill_color = TFT_RED;
  }
  else
  {
    fill_color = medium_green;
  }

  if (last_value != val)
  {
    // Calculate ramp as a fixed percentage of max_value instead of based on difference
    ramp = max_value * 0.05; // 5% of max value per animation step
    unsigned long startTime = millis();
    unsigned long animationDuration = 500; // Adjust as needed

    current_value = last_value;

    while (millis() - startTime < animationDuration)
    {
      float progress = (millis() - startTime) / (float)animationDuration;

      // Use simple linear interpolation for smooth animation
      current_value = last_value + progress * (val - last_value);

      // Ensure we don't overshoot
      if ((val > last_value && current_value > val) ||
          (val < last_value && current_value < val))
      {
        current_value = val;
      }

      // Calculate filled height and newY based on current_value
      filledHeight = (current_value * (h - 2 * spacing)) / max_value;
      newY = yBottom - filledHeight;

      // Clear the previous filled area
      tft.fillSmoothRoundRect(x + spacing, y + spacing, w - 2 * spacing,
                              h - 2 * spacing, radius - spacing, TFT_BLACK,
                              TFT_BLACK);
      // Draw the new filled area
      tft.fillSmoothRoundRect(x + spacing, newY, w - 2 * spacing, filledHeight,
                              radius - spacing, fill_color, DARKER_GREY);

      // Draw the new value
      tft.setTextSize(2);
      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE);
      tft.drawString(String(current_value, num_decimals), x + w / 2,
                     y - 10 + h / 2);
      // units
      tft.setTextColor(TFT_GOLD);
      tft.drawString(units, x + w / 2, y + 10 + h / 2);

      delay(100);
    }
  }
  last_value = val;
  // Clear the previous filled area
  tft.fillSmoothRoundRect(x + spacing, y + spacing, w - 2 * spacing,
                          h - 2 * spacing, radius - spacing, TFT_BLACK,
                          TFT_BLACK);
  // Draw the new filled area
  tft.fillSmoothRoundRect(x + spacing, newY, w - 2 * spacing, filledHeight,
                          radius - spacing, fill_color, DARKER_GREY);
  // number
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(String(last_value, num_decimals), x + w / 2, y - 10 + h / 2);
  // units
  tft.setTextColor(TFT_GOLD);
  tft.drawString(units, x + w / 2, y + 10 + h / 2);
}

// Get the previous value
double Rectangle_Meter::meterValue() { return last_value; }
// Get the max value
int Rectangle_Meter::getMax() { return int(max_value); }

// ---------- End Rectangular_Meter ---------- //

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

// Size of Arc Meters
uint8_t radius1 = 60;
uint8_t radius2 = 38;

// (x, y, radius, units)
Arc_Meter Voltage(second_column, first_row, radius2, "V", 15);
Arc_Meter Current(third_column, center_height, radius2, "A", 100);
Arc_Meter SOC(center_width, center_height, radius1, "%");
Arc_Meter Temperature(second_column, second_row, radius2, "F",
                      110); // degree symbol: °C   idk if it works though so
                            // test later -> it doesnt work - Marcos

// (x, y, w, h, thickness, units, direction, max_value)
Rectangle_Meter Watts(8, 20, 82, tft.width() - 40, 4, "Watts", -1, 1200, 1);

//--------------------------------------------------------
// Meter objects for page 2
// (Cell 1, Cell 2, Cell 3, Cell 4 Voltages)
//--------------------------------------------------------
Rectangle_Meter Cell1(4, 20, 72, tft.width() - 40, 4, "Cell1", -1, 4, 2);
Rectangle_Meter Cell2(84, 20, 72, tft.width() - 40, 4, "Cell2", -1, 4, 2);
Rectangle_Meter Cell3(164, 20, 72, tft.width() - 40, 4, "Cell3", -1, 4, 2);
Rectangle_Meter Cell4(244, 20, 72, tft.width() - 40, 4, "Cell4", -1, 4, 2);

void DisplaySetup()
{
  tft.begin();

  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT)))
  {
    Serial.println("Render initialize error");
    return;
  }

  tft.setRotation(1);
  tft.fillScreen(Binghamton_Green);

  // initialize meters
  Voltage.init();
  Current.init();
  SOC.init();
  Temperature.init();
  Watts.init();
}

void UpdateDisplay()
{
  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT)))
  {
    Serial.println("Render initialize error");
    return;
  }

  if (abs(SOC.meterValue() - GetValue("SOC")) > 0.1)
  {
    SOC.update(GetValue("SOC"));
  }
  else if (abs(Voltage.meterValue() - GetValue("Voltage")) > 0.1)
  {
    Voltage.update(GetValue("Voltage"));
  }
  else if (abs(Current.meterValue() - GetValue("Current")) > 0.1)
  {
    Current.update(GetValue("Current"));
  }
  else if (abs(Temperature.meterValue() - GetValue("Temperature")) > 0.1)
  {
    Temperature.update(GetValue("Temperature"));
  }
  else if (abs(Watts.meterValue() - GetValue("Watts")) > 0.1)
  {
    Watts.update(GetValue("Watts"));
  }

  ofr.unloadFont(); // Recover space used by font metrics etc.
}