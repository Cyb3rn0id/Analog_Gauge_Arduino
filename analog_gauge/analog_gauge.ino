/*
 * Analog gauge example by @cyb3rn0id and @mrloba81
 *
 * This example started from a work of BODMER
 * taken from: https://www.instructables.com/Arduino-sketch-for-a-retro-analogue-meter-graphic-/
 * 
 * Please read the repository readme for further informations and explainations.
 * https://github.com/cyb3rn0id/Analog_Gauge_Arduino
 
 * I made this example using the Arduino Nano (and Nano Every) and an 1.8" ST7735 display (128x160)
 * and 2 potentiometers attached to A0 and A1 analog inputs.
 * 
 * WARNING:
 * Arduino Nano works at 5V while the ST7735 displays are 3.3V.
 * please read the Readme in the repository for further informations 
 * on those kind of displays
 *
 */

#include <SPI.h>
#include "Ucglib.h"

// ST7735 display to Arduino Nano (or Nano Every) connections:
// CLK      : 13
// SDA      : 11
// RS or A0 :  9 (is called "CD" from the library. RS=register select, CD=command/data => is the same thing!)
// CS       : 10
// RST      :  8

// display instance using software SPI (first) or hardware SPI (second)
//Ucglib_ST7735_18x128x160_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
Ucglib_ST7735_18x128x160_HWSPI ucg(9, 10, 8);

/// constants used for the first gauge
#define G1_X 56 // X coordinates of the center of the circle
#define G1_Y 64
#define G1_ARC 120
#define G1_RADIUS 30
#define G1_SCALEMIN 0
#define G1_SCALEMAX 100

// constants used for the second gauge
#define G2_X 56
#define G2_Y 127
#define G2_ARC 120
#define G2_RADIUS 30
#define G2_SCALEMIN -50
#define G2_SCALEMAX 50

#define K 0.0174532925F // pi/180 (for grad to rad conversion)

// structure used for handling more than 1 gauge using same function
struct DrawContext 
  {
  float old_val;
  float ltx;
  uint16_t osx;
  uint16_t osy;
  bool first_start;
  uint16_t gaugeposX;
  uint16_t gaugeposY;
  uint8_t gaugeArc;
  uint8_t gaugeRadius;
  float scaleMin;
  float scaleMax;
  } pointer1, pointer2; // I've defined 2 different values for 2 different gauges

void setup(void)
  {
  // values for pointer of first gauge
  pointer1.old_val = -99999;
  pointer1.ltx = 0; 
  pointer1.osx = 0;
  pointer1.osy = 0;
  pointer1.first_start = true;
  pointer1.gaugeposX=G1_X;
  pointer1.gaugeposY=G1_Y;
  pointer1.gaugeArc=G1_ARC;
  pointer1.gaugeRadius=G1_RADIUS;
  pointer1.scaleMin=G1_SCALEMIN;
  pointer1.scaleMax=G1_SCALEMAX;
  
  pointer2.old_val = -99999;
  pointer2.ltx = 0; 
  pointer2.osx = 0;
  pointer2.osy = 0;
  pointer2.first_start = true;
  pointer2.gaugeposX=G2_X;
  pointer2.gaugeposY=G2_Y;
  pointer2.gaugeArc=G2_ARC;
  pointer2.gaugeRadius=G2_RADIUS;
  pointer2.scaleMin=G2_SCALEMIN;
  pointer2.scaleMax=G2_SCALEMAX;
  
  delay(400);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();
  ucg.setRotate90(); // display horizontal (landscape) with header on the left

  // paint screen in white
  ucg.setColor(0, 255, 255, 255);
  ucg.drawBox(0, 0, ucg.getWidth(), ucg.getHeight()); 

  // draw the two gauges
  // void drawGauge(uint8_t x, uint8_t y, uint8_t arc, uint8_t radius, uint8_t stp, uint8_t tickl, float gaugemin, float gaugemax, uint8_t decimals, float gz, float yz)
  drawGauge(G1_X, G1_Y, G1_ARC, G1_RADIUS, 5, 15, 0, 100, 0, 50, 75);
  drawGauge(G2_X, G2_Y, G2_ARC, G2_RADIUS, 5, 15, -50, 50, 0, 45, 50);
  }

void loop(void)
  {
  static int16_t vo1=0;
  static int16_t vo2=0;
  static uint8_t i=0;

  // first trimmer mapped to 0/100
  int16_t v1=map(analogRead(A0), 0, 1023, 0 ,100);
  // second trimmer mapped to -50/50
  int16_t v2=map(analogRead(A1), 0, 1023, -50 ,50);
  vo1+=v1;
  vo2+=v2;
  i++;
  if (i==30)
    {
    v1=v1/30;
    vo1=0;
    v2=vo2/30;
    vo2=0;  
    i=0;

    // set font
    ucg.setColor(0, 0, 0, 0); // foreground color
    ucg.setColor(1, 255, 255, 255); // background color
    ucg.setFontMode(UCG_FONT_MODE_SOLID); // solid: background will painted
    // use this font if you want ONLY numbers
    //ucg.setFont(ucg_font_logisoso16_hn); // font (https://github.com/olikraus/ucglib/wiki/fontsize)
    // use this font for numbers, letters, symbols
    ucg.setFont(ucg_font_logisoso18_hr); // font (https://github.com/olikraus/ucglib/wiki/fontsize)
    
    // write values
    ucg.setPrintPos(G1_X + G1_RADIUS + 27, G1_Y - 33); // 23
    ucg.print(v1, 1); // 1 decimal, not used in this case
    ucg.print("  ");
    ucg.setPrintPos(G1_X + G1_RADIUS + 27, G1_Y - 10);
    ucg.print("V1");

    ucg.setPrintPos(G2_X + G2_RADIUS + 27, G2_Y - 33);
    ucg.print(v2); // no decimals
    ucg.print("  ");
    ucg.setPrintPos(G2_X + G2_RADIUS + 27, G2_Y - 10);
    ucg.print("V2");
    
    // drawPointer(struct DrawContest pointer, float value)
    drawPointer(pointer1, v1);
    drawPointer(pointer2, v2);
    }
 }

void drawGauge(uint8_t x, uint8_t y, uint8_t arc, uint8_t radius, uint8_t stp, uint8_t tickl, float gaugemin, float gaugemax, uint8_t decimals, float gz, float yz)
  {
  int amin = -((int)arc/2);
  int amax = (arc/2)+1;
  // Draw ticks every 'stp' degrees from -(arc/2) to (arc/2) degrees
  for (int i = amin; i < amax; i += stp) 
    {
    // Calculating coodinates of tick to draw
    // at this time will be used only for drawing the green/yellow/red zones
    float sx = cos((i - 90) * K);
    float sy = sin((i - 90) * K);
    uint16_t x0 = sx * (radius + tickl) + x;
    uint16_t y0 = sy * (radius + tickl) + y;
    uint16_t x1 = sx * radius + x;
    uint16_t y1 = sy * radius + y;
    
    // Coordinates of next tick for zone fill
    float sx2 = cos((i + stp - 90) * K);
    float sy2 = sin((i + stp - 90) * K);
    int x2 = sx2 * (radius + tickl) + x;
    int y2 = sy2 * (radius + tickl) + y;
    int x3 = sx2 * radius + x;
    int y3 = sy2 * radius + y;

    // calculate angles for green and yellow zones
    int ga=(int)lmap(gz, gaugemin, gaugemax, amin, amax); // green zone will be between gaugemin and GZ
    int ya=(int)lmap(yz, gaugemin, gaugemax, amin, amax); // yellow zone will be between GZ and YZ
    // red zone will be between YZ and gaugemax
     
    // green zone
    ucg.setColor(0, 0, 255, 0); 
    if (i >= amin && i < ga) 
      {
      ucg.drawTriangle(x0, y0, x1, y1, x2, y2);
      ucg.drawTriangle(x1, y1, x2, y2, x3, y3);
      }
    // yellow zone
    ucg.setColor(0, 255, 255, 0); 
    if (i >= ga && i <ya) 
      {
      ucg.drawTriangle(x0, y0, x1, y1, x2, y2);
      ucg.drawTriangle(x1, y1, x2, y2, x3, y3);
      }
    // red zone
    ucg.setColor(0, 255, 0, 0); 
    if (i >= ya && i < amax-1) 
      {
      ucg.drawTriangle(x0, y0, x1, y1, x2, y2);
      ucg.drawTriangle(x1, y1, x2, y2, x3, y3);
      }

    // now we can draw ticks, above colored zones
    
    // Check if this is a "Short" scale tick
    uint8_t tl=tickl;
    if (i % (arc/4) != 0) tl=(tickl/2)+1;
    
    // Recalculate coords in case tick length is changed
    x0 = sx * (radius + tl) + x;
    y0 = sy * (radius + tl) + y;
    x1 = sx * radius + x;
    y1 = sy * radius + y;
    
    // Draw tick
    ucg.setColor(0, 0, 0, 0); 
    ucg.drawLine(x0, y0, x1, y1);
    
    // Check if labels should be drawn
    if (i % (arc/4) == 0) // gauge will have 5 main ticks at 0, 25, 50, 75 and 100% of the scale
      {
      // Calculate label positions
      x0 = sx * (radius + tl + 10) + x - 5; // 10, 5 are my offset for the font I've used for labels
      y0 = sy * (radius + tl + 10) + y + 5;

      // print label positions
      ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
      ucg.setFont(ucg_font_orgv01_hf);
      ucg.setColor(0, 0, 0, 0);
      ucg.setPrintPos(x0, y0);
      float cent=(gaugemax+gaugemin)/2; // center point of the gauge
      switch (i / (arc/4)) 
        {
        case -2: ucg.print(gaugemin, decimals); break; // 0%=gaugemin
        case -1: ucg.print((cent+gaugemin)/2, decimals); break; // 25%
        case  0: ucg.print(cent, decimals); break; // central point // 50%
        case  1: ucg.print((cent+gaugemax)/2, decimals); break; // 75%
        case  2: ucg.print(gaugemax, decimals); break; // 100%=gaugemax
        }
    } // check if I must print labels
    
    // Now draw the arc of the scale
    sx = cos((i + stp - 90) * K);
    sy = sin((i + stp - 90) * K);
    x0 = sx * radius + x;
    y0 = sy * radius + y;
    ucg.setColor(0, 0, 0, 0);    
    if (i < (arc/2)) ucg.drawLine(x0, y0, x1, y1);  // don't draw the last part
  }
}

void drawPointer(DrawContext &ctx, float value)
{
  uint8_t nl = 10; // pointer "length" (0=longest)
  // quick exit if value to paint is about the same for avoiding pointer flickering
  //if ((value <= ctx.old_val + .05) && (value >= ctx.old_val - .05)) return;
  if (value==ctx.old_val) return;
  ctx.old_val = value;

  if (value < ctx.scaleMin) value = ctx.scaleMin;
  if (value > ctx.scaleMax) value = ctx.scaleMax;

  float sdeg = lmap(value, ctx.scaleMin, ctx.scaleMax, (-(ctx.gaugeArc / 2) - 90), ((ctx.gaugeArc / 2) - 90)); // Map value to angle
  
  // Calculate tip of pointer coords
  float sx = cos(sdeg * K);
  float sy = sin(sdeg * K);

  // Calculate x delta of needle start (does not start at pivot point)
  float tx = tan((sdeg + 90) * K);

  // Erase old needle image if not first time pointer is drawn
  if (!ctx.first_start)
    {
    ucg.setColor(0, 255, 255, 255);
    ucg.drawLine(ctx.gaugeposX + nl * ctx.ltx - 1, ctx.gaugeposY - nl, ctx.osx - 1, ctx.osy);
    ucg.drawLine(ctx.gaugeposX + nl * ctx.ltx, ctx.gaugeposY - nl, ctx.osx, ctx.osy);
    ucg.drawLine(ctx.gaugeposX + nl * ctx.ltx + 1, ctx.gaugeposY - nl, ctx.osx + 1, ctx.osy);
    }
  ctx.first_start = false;

  // Store new pointer end coords for next erase
  ctx.ltx = tx;
  ctx.osx = sx * (ctx.gaugeRadius - 2) + ctx.gaugeposX;
  ctx.osy = sy * (ctx.gaugeRadius - 2) + ctx.gaugeposY;

  // Draw the pointer in the new position
  // draws 3 lines to thicken needle
  ucg.setColor(0, 0, 0, 0);
  ucg.drawLine(ctx.gaugeposX + nl * ctx.ltx - 1, ctx.gaugeposY - nl, ctx.osx - 1, ctx.osy);
  ucg.drawLine(ctx.gaugeposX + nl * ctx.ltx, ctx.gaugeposY - nl, ctx.osx, ctx.osy);
  ucg.drawLine(ctx.gaugeposX + nl * ctx.ltx + 1, ctx.gaugeposY - nl, ctx.osx + 1, ctx.osy);
}

// Arduino "MAP" function but rewrited for using float numbers
float lmap(float x, float in_min, float in_max, float out_min, float out_max) 
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
