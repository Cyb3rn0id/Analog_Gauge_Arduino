## Analog Gauge

This is an example of an Analog Gauge for TFT screens to be used on Arduino with the [UCG Library](https://github.com/olikraus/ucglib).

![gauge picture](img/gauge.jpg)

This example started from an [Instructable by Bodmer](https://www.instructables.com/Arduino-sketch-for-a-retro-analogue-meter-graphic-/). 

I've made some modifications to it for having a parametrized gauge callable as function and so having also more than one gauge on the screen (even if you must redefine functions for pointers, you'll see). I've changed some things in the code for making it more understandable and opened to modifications.

For the example I've used an Arduino Nano (or Arduino Nano Every), an 1.8" ST7735 display and 2 potentiometers attached on A0 and A1 for reading values.

### Note about used Display/Arduino

Arduino Nano (and Nano Every) works at 5V while ST7735 displays are 3.3V only. There are 2 kind of display based on the ST7735:

1) Display having the voltage regulator AND a level shifter on board 
2) Display having the voltage regulator but NOT having the level shifter on board (the most part)

Both kind of displays has a 3.3V regulator on board (the XC6206P332MR marked as '662K') so on 'Vcc' pin you must apply the 5V unless you don't want to use them with a 3.3V microcontroller: in that case the is a jumper to close for bypassing the voltage regulato so you can apply 3.3V on Vcc.

Then you can use the type 1) directly attached on Arduino Nano since has a Level shifter on board. Display type 2) can't be attached directly to Ardino but needs an external level shifter. Most of people uses in-series restistors but I not consider this a good solution since limits the current, not the voltage. I advice to use a level shifter breakout board such as the one based on the Texas Instruments TXS0108E.

Using that level shifter remember to:
- attach display to "A" port
- attach Arduino to "B" port
- connect VA of the shifter to 3.3V on Arduino
- connect Vb of the shifter to 5V on Arduino
- connect OE to 3.3V on Arduino and put on it also a 10K resistor versus GND

The "LED" pin of display has usually a 3.3Ohm resistor on-board and goes to 5V.

### Analog gauge drawing

You can paint the gauge on the screen calling the function:

<code>void drawGauge(uint8_t x, uint8_t y, uint8_t arc, uint8_t radius, uint8_t stp, uint8_t tickl, float gaugemin, float gaugemax, uint8_t decimals, float gz, float yz)</code>

where:

- <code>x</code>, <code>y</code> : are the coordinates of the center of the circle that will describe the gauge. Imagine the Gauge as a top arc of a circle. The pointer will be fixed in this center (even if not visible).

- <code>arc</code> : arc of circle in degree that will describe the gauge. Valid values up to 180.

- <code>radius</code> : radius of the circle that will describe the gauge

- <code>stp</code> : steps, number of "minor" ticks between main ticks. Main ticks are hard-wired as 5

- <code>tickl</code> : length, in pixel, of the main ticks. Minor ticks will be a fraction of this

- <code>gaugemin</code>, <code>gaugemax</code> : minimum and maximum value on the scale. Are float values so you can use decimal and negative numbers too. Only numbers on main ticks will be drawn.

- <code>decimals</code> : number of decimals to be shown on the labels of main ticks. 0 will print no decimals

- <code>gz</code>, <code>yz</code> : value for painting green zone and yellow zone respectively. A green arc will be painted from "gaugemin" value to "gz" value. A yellow arc will be painter from "gz" to "yz" value and a red arc will be painted from "yz" to "gaugemax" value.

This function will be called only once, at startup.

### Pointer drawing

The fuction for painting the pointer is defined as:

<code>drawPointer(DrawContext &ctx, float value)</code>

where:

- <code>&ctx</code> : pointer to the `DrawContext` structure defined for the pointer (see example)

- <code>value</code> : value to be pointed

You must call this function everytime you must update the value to be shown. Function itself saves the previous value so it exits quickly, without redrawing all, if passed value to function is the same of the previous call.

The pointer length is hardwired in code (<code>nl</code> variable in the function). The lower part of the pointer (pivot point) will not visible: imagine the pivot point of the pointer covered by a circle. You will see the lower part of the pointer describing an arc while moving. This happens also on most analog gauges in the real such as in Vu-meters for example.