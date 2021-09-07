/*
 * Analog gauge example by @cyb3rn0id
 *
 * This example started from a work of BODMER
 * taken from: https://www.instructables.com/Arduino-sketch-for-a-retro-analogue-meter-graphic-/
 * 
 * Please read the repository readme for further informations and explainations.
 * https://github.com/cyb3rn0id/Analog_Gauge_Arduino
 
 * I made this example using the Arduino Every and an ST7735 display (128x160) 
 *
 */

#include <SPI.h>
#include "Ucglib.h"

// ST7735 display to Arduino Every connections:
// CLK : 13
// SDA : 11
// RS  :  9 (is called "CD" from the library. RS=register select, CD=command/data => is the same thing!)
// CS  : 10
// RST :  8

// display instance using software SPI (first) or hardware SPI (second)
//Ucglib_ST7735_18x128x160_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
Ucglib_ST7735_18x128x160_HWSPI ucg(9, 10, 8);

// constants used for the first gauge
#define G1_X 60
#define G1_Y 62
#define G1_ARC 120
#define G1_RADIUS 30

// constants used for the second gauge
#define G2_X 60
#define G2_Y 124
#define G2_ARC 120
#define G2_RADIUS 30

#define c 0.0174532925F // pi/180 (for grad to rad conversion)

void setup(void)
  {
  delay(1000);
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
  static int16_t vo0=0;
  static int16_t vo1=0;
  static uint8_t i=0;

  // first trimmer mapped to 0/100
  int16_t v0=map(analogRead(A0), 0, 1023, 0 ,100);
  // second trimmer mapped to -50/50
  int16_t v1=map(analogRead(A1), 0, 1023, -50 ,50);
  vo0+=v0;
  vo1+=v1;
  i++;
  if (i==30)
    {
    v0=vo0/30;
    vo0=0;
    v1=vo1/30;
    vo1=0;  
    i=0;
    
    // drawPointer(uint8_t x, uint8_t y, uint8_t arc, uint8_t radius, int value, int minval, int maxval)
	// since the drawpointer function uses static variables for keeping in memory where was the last pointer
    // position for deleting it after a refresh, I've copied an equal second function for the second gauge
    drawPointer(G1_X, G1_Y, G1_ARC, G1_RADIUS, v0, 0, 100);
    drawPointer2(G2_X, G2_Y, G2_ARC, G2_RADIUS, v1, -50, 50);
    
    // set font
    ucg.setColor(0, 0, 0, 0); // foreground color
    ucg.setColor(1, 255, 255, 255); // background color
    ucg.setFontMode(UCG_FONT_MODE_SOLID); // solid: background will painted
    ucg.setFont(ucg_font_logisoso16_hn); // font (https://github.com/olikraus/ucglib/wiki/fontsize)

    // write values
    ucg.setPrintPos(G1_X+G1_RADIUS+35, G1_Y-23);
    ucg.print(v0);
    ucg.print("  ");

    ucg.setPrintPos(G2_X+G2_RADIUS+35, G2_Y-23);
    ucg.print(v1);
    ucg.print("  ");
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
    float sx = cos((i - 90) * c);
    float sy = sin((i - 90) * c);
    uint16_t x0 = sx * (radius + tickl) + x;
    uint16_t y0 = sy * (radius + tickl) + y;
    uint16_t x1 = sx * radius + x;
    uint16_t y1 = sy * radius + y;
    
    // Coordinates of next tick for zone fill
    float sx2 = cos((i + stp - 90) * c);
    float sy2 = sin((i + stp - 90) * c);
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
    sx = cos((i + stp - 90) * c);
    sy = sin((i + stp - 90) * c);
    x0 = sx * radius + x;
    y0 = sy * radius + y;
    ucg.setColor(0, 0, 0, 0);    
    if (i < (arc/2)) ucg.drawLine(x0, y0, x1, y1);  // don't draw the last part
  }
}

void drawPointer(uint8_t x, uint8_t y, uint8_t arc, uint8_t radius, float value, float minval, float maxval)
  {
  uint8_t nl=10; // pointer "length" (0=longest)
  static float old_val=-999;
  // quick exit if value to paint is the same
  if (value==old_val) return;
  old_val=value;
  
  static float ltx = 0; // Saved x coord of bottom of pointer
  static uint16_t osx=0;
  static uint16_t osy=0; 
  static bool first_start=true;
      
  if (value<minval) value=minval; 
  if (value>maxval) value=maxval;

  float sdeg = lmap(value, minval, maxval, (-(arc/2)-90), ((arc/2)-90)); // Map value to angle 
  // Calculate tip of pointer coords
  float sx = cos(sdeg * c);
  float sy = sin(sdeg * c);

  // Calculate x delta of needle start (does not start at pivot point)
  float tx = tan((sdeg+90) * c);
    
  // Erase old needle image if not first time pointer is drawn
  if (!first_start)
      {
      ucg.setColor(0, 255, 255, 255);
      ucg.drawLine(x + nl * ltx - 1, y - nl, osx - 1, osy);
      ucg.drawLine(x + nl * ltx, y - nl, osx, osy);
      ucg.drawLine(x + nl * ltx + 1, y - nl, osx + 1, osy);
      }
  first_start=false;
   
  // Store new pointer end coords for next erase
  ltx = tx;
  osx = sx * (radius-2) + x;
  osy = sy * (radius-2) + y;
    
  // Draw the pointer in the new position
  // draws 3 lines to thicken needle
  ucg.setColor(0, 0, 0, 0);
  ucg.drawLine(x + nl * ltx - 1, y - nl, osx - 1, osy);
  ucg.drawLine(x + nl * ltx, y - nl, osx, osy);
  ucg.drawLine(x + nl * ltx + 1, y - nl, osx + 1, osy);
  }

void drawPointer2(uint8_t x, uint8_t y, uint8_t arc, uint8_t radius, float value, float minval, float maxval)
  {
  uint8_t nl=10; // pointer "length" (0=longest)
  static float old_val=-999;
  // quick exit if value to paint is the same
  if (value==old_val) return;
  old_val=value;
  
  static float ltx = 0; // Saved x coord of bottom of pointer
  static uint16_t osx=0;
  static uint16_t osy=0; 
  static bool first_start=true;
      
  if (value<minval) value=minval; 
  if (value>maxval) value=maxval;

  float sdeg = lmap(value, minval, maxval, (-(arc/2)-90), ((arc/2)-90)); // Map value to angle 
  // Calculate tip of pointer coords
  float sx = cos(sdeg * c);
  float sy = sin(sdeg * c);

  // Calculate x delta of needle start (does not start at pivot point)
  float tx = tan((sdeg+90) * c);
    
  // Erase old needle image if not first time pointer is drawn
  if (!first_start)
      {
      ucg.setColor(0, 255, 255, 255);
      ucg.drawLine(x + nl * ltx - 1, y - nl, osx - 1, osy);
      ucg.drawLine(x + nl * ltx, y - nl, osx, osy);
      ucg.drawLine(x + nl * ltx + 1, y - nl, osx + 1, osy);
      }
  first_start=false;
   
  // Store new pointer end coords for next erase
  ltx = tx;
  osx = sx * (radius-2) + x;
  osy = sy * (radius-2) + y;
    
  // Draw the pointer in the new position
  // draws 3 lines to thicken needle
  ucg.setColor(0, 0, 0, 0);
  ucg.drawLine(x + nl * ltx - 1, y - nl, osx - 1, osy);
  ucg.drawLine(x + nl * ltx, y - nl, osx, osy);
  ucg.drawLine(x + nl * ltx + 1, y - nl, osx + 1, osy);
  }

// Arduino "MAP" function but rewrited for using float numbers
float lmap(float x, float in_min, float in_max, float out_min, float out_max) 
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
