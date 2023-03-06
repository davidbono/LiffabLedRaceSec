/*  
 * ____                     _      ______ _____    _____
  / __ \                   | |    |  ____|  __ \  |  __ \               
 | |  | |_ __   ___ _ __   | |    | |__  | |  | | | |__) |__ _  ___ ___ 
 | |  | | '_ \ / _ \ '_ \  | |    |  __| | |  | | |  _  // _` |/ __/ _ \
 | |__| | |_) |  __/ | | | | |____| |____| |__| | | | \ \ (_| | (_|  __/
  \____/| .__/ \___|_| |_| |______|______|_____/  |_|  \_\__,_|\___\___|
        | |                                                             
        |_|          
 Open LED Race
 An minimalist cars race for LED strip  
  
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 @author LarrxX 

 heavily modified version of the original
 by gbarbarov@singulardevices.com  for Arduino day Seville 2019 
 https://www.hackster.io/gbarbarov/open-led-race-a0331a
 https://twitter.com/openledrace
 
 
 https://gitlab.com/open-led-race
 https://openledrace.net/open-software/

*/
#include <SPI.h>
#include <Wire.h>


#include <ESP32-RGB64x32MatrixPanel-I2S-DMA.h>
RGB64x32MatrixPanel_I2S_DMA matrix;
#include "smileytongue24.h"

// import des fonts
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

// Panel Matrix doesn't fully work like Neomatrix (which I wrote this 
// demo for), so map a few calls to be compatible. The rest comes from
// Adafruit::GFX and works the same on both backends.
#define setBrightness(x) fillScreen(0) // no-op, no brightness on this board
#define clear() fillScreen(0)
#define show() drawPixel(0, 0, 0);     // no show method in this GFX implementation
#define Color(x,y,z) color444(x/16,y/16,z/16)

// Define matrix width and height.
#define LARGEUR 64
#define HAUTEUR 32


// for recap 
// X 0->64 
// Y 0
//   |
//   v
//   32

// This could also be defined as matrix.color(255,0,0) but those defines
// are meant to work for Adafruit::GFX backends that are lacking color()
#define LED_BLACK		0

#define LED_RED_VERYLOW 	(3 <<  11)
#define LED_RED_LOW 		(7 <<  11)
#define LED_RED_MEDIUM 		(15 << 11)
#define LED_RED_HIGH 		(31 << 11)

#define LED_GREEN_VERYLOW	(1 <<  5)   
#define LED_GREEN_LOW 		(15 << 5)  
#define LED_GREEN_MEDIUM 	(31 << 5)  
#define LED_GREEN_HIGH 		(63 << 5)  

#define LED_BLUE_VERYLOW	3
#define LED_BLUE_LOW 		7
#define LED_BLUE_MEDIUM 	15
#define LED_BLUE_HIGH 		31

#define LED_ORANGE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW)
#define LED_ORANGE_LOW		(LED_RED_LOW     + LED_GREEN_LOW)
#define LED_ORANGE_MEDIUM	(LED_RED_MEDIUM  + LED_GREEN_MEDIUM)
#define LED_ORANGE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH)

#define LED_PURPLE_VERYLOW	(LED_RED_VERYLOW + LED_BLUE_VERYLOW)
#define LED_PURPLE_LOW		(LED_RED_LOW     + LED_BLUE_LOW)
#define LED_PURPLE_MEDIUM	(LED_RED_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_PURPLE_HIGH		(LED_RED_HIGH    + LED_BLUE_HIGH)

#define LED_CYAN_VERYLOW	(LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_CYAN_LOW		(LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_CYAN_MEDIUM		(LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_CYAN_HIGH		(LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define LED_WHITE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_WHITE_LOW		(LED_RED_LOW     + LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_WHITE_MEDIUM	(LED_RED_MEDIUM  + LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_WHITE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH    + LED_BLUE_HIGH)

unsigned long time_now = 0;

static const uint16_t PROGMEM
    // These bitmaps were written for a backend that only supported
    // 4 bits per color with Blue/Green/Red ordering while neomatrix
    // uses native 565 color mapping as RGB.  
    // I'm leaving the arrays as is because it's easier to read
    // which color is what when separated on a 4bit boundary
    // The demo code will modify the arrays at runtime to be compatible
    // with the neomatrix color ordering and bit depth.
    RGB_bmp[][64] = {
      // 00: blue, blue/red, red, red/green, green, green/blue, blue, white
      {  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00, 
  0x101, 0x202, 0x303, 0x404, 0x606, 0x808, 0xA0A, 0xF0F, 
        0x001, 0x002, 0x003, 0x004, 0x006, 0x008, 0x00A, 0x00F, 
  0x011, 0x022, 0x033, 0x044, 0x066, 0x088, 0x0AA, 0x0FF, 
  0x010, 0x020, 0x030, 0x040, 0x060, 0x080, 0x0A0, 0x0F0, 
  0x110, 0x220, 0x330, 0x440, 0x660, 0x880, 0xAA0, 0xFF0, 
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00, 
  0x111, 0x222, 0x333, 0x444, 0x666, 0x888, 0xAAA, 0xFFF, },

      // 01: grey to white
      { 0x111, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF, 
  0x222, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF, 
  0x333, 0x333, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF, 
  0x555, 0x555, 0x555, 0x555, 0x777, 0x999, 0xAAA, 0xFFF, 
  0x777, 0x777, 0x777, 0x777, 0x777, 0x999, 0xAAA, 0xFFF, 
  0x999, 0x999, 0x999, 0x999, 0x999, 0x999, 0xAAA, 0xFFF, 
  0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xFFF, 
  0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, },

      // 02: low red to high red
      { 0x001, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F, 
  0x002, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F, 
  0x003, 0x003, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F, 
  0x005, 0x005, 0x005, 0x005, 0x007, 0x009, 0x00A, 0x00F, 
  0x007, 0x007, 0x007, 0x007, 0x007, 0x009, 0x00A, 0x00F, 
  0x009, 0x009, 0x009, 0x009, 0x009, 0x009, 0x00A, 0x00F, 
  0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00F, 
  0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, },

      // 03: low green to high green
      { 0x010, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0, 
  0x020, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0, 
  0x030, 0x030, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0, 
  0x050, 0x050, 0x050, 0x050, 0x070, 0x090, 0x0A0, 0x0F0, 
  0x070, 0x070, 0x070, 0x070, 0x070, 0x090, 0x0A0, 0x0F0, 
  0x090, 0x090, 0x090, 0x090, 0x090, 0x090, 0x0A0, 0x0F0, 
  0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0F0, 
  0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, },

      // 04: low blue to high blue
      { 0x100, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00, 
  0x200, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00, 
  0x300, 0x300, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00, 
  0x500, 0x500, 0x500, 0x500, 0x700, 0x900, 0xA00, 0xF00, 
  0x700, 0x700, 0x700, 0x700, 0x700, 0x900, 0xA00, 0xF00, 
  0x900, 0x900, 0x900, 0x900, 0x900, 0x900, 0xA00, 0xF00, 
  0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xF00, 
  0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, },

      // 05: 1 black, 2R, 2O, 2G, 1B with 4 blue lines rising right
      { 0x000, 0x200, 0x000, 0x400, 0x000, 0x800, 0x000, 0xF00, 
        0x000, 0x201, 0x002, 0x403, 0x004, 0x805, 0x006, 0xF07, 
  0x008, 0x209, 0x00A, 0x40B, 0x00C, 0x80D, 0x00E, 0xF0F, 
  0x000, 0x211, 0x022, 0x433, 0x044, 0x855, 0x066, 0xF77, 
  0x088, 0x299, 0x0AA, 0x4BB, 0x0CC, 0x8DD, 0x0EE, 0xFFF, 
  0x000, 0x210, 0x020, 0x430, 0x040, 0x850, 0x060, 0xF70, 
  0x080, 0x290, 0x0A0, 0x4B0, 0x0C0, 0x8D0, 0x0E0, 0xFF0,
  0x000, 0x200, 0x000, 0x500, 0x000, 0x800, 0x000, 0xF00, },

      // 06: 4 lines of increasing red and then green
      { 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003, 
  0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007, 
  0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B, 
  0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F, 
  0x000, 0x000, 0x010, 0x010, 0x020, 0x020, 0x030, 0x030, 
  0x040, 0x040, 0x050, 0x050, 0x060, 0x060, 0x070, 0x070, 
  0x080, 0x080, 0x090, 0x090, 0x0A0, 0x0A0, 0x0B0, 0x0B0, 
  0x0C0, 0x0C0, 0x0D0, 0x0D0, 0x0E0, 0x0E0, 0x0F0, 0x0F0, },

      // 07: 4 lines of increasing red and then blue
      { 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003, 
  0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007, 
  0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B, 
  0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F, 
  0x000, 0x000, 0x100, 0x100, 0x200, 0x200, 0x300, 0x300, 
  0x400, 0x400, 0x500, 0x500, 0x600, 0x600, 0x700, 0x700, 
  0x800, 0x800, 0x900, 0x900, 0xA00, 0xA00, 0xB00, 0xB00, 
  0xC00, 0xC00, 0xD00, 0xD00, 0xE00, 0xE00, 0xF00, 0xF00, },

      // 08: criss cross of green and red with diagonal blue.
      { 0xF00, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0x000, 
  0x020, 0xF21, 0x023, 0x025, 0x027, 0x02A, 0x02F, 0x020, 
  0x040, 0x041, 0xF43, 0x045, 0x047, 0x04A, 0x04F, 0x040, 
  0x060, 0x061, 0x063, 0xF65, 0x067, 0x06A, 0x06F, 0x060, 
  0x080, 0x081, 0x083, 0x085, 0xF87, 0x08A, 0x08F, 0x080, 
  0x0A0, 0x0A1, 0x0A3, 0x0A5, 0x0A7, 0xFAA, 0x0AF, 0x0A0, 
  0x0F0, 0x0F1, 0x0F3, 0x0F5, 0x0F7, 0x0FA, 0xFFF, 0x0F0, 
  0x000, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0xF00, },

      // 09: 2 lines of green, 2 red, 2 orange, 2 green
      { 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, 
  0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, 
  0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, 
  0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, 
  0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, 
  0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, 
  0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, 
  0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, },

      // 10: multicolor smiley face
      { 0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000, 
  0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000, 
  0x00F, 0x000, 0xF00, 0x000, 0x000, 0xF00, 0x000, 0x00F, 
  0x00F, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x00F, 
  0x00F, 0x000, 0x0F0, 0x000, 0x000, 0x0F0, 0x000, 0x00F, 
  0x00F, 0x000, 0x000, 0x0F4, 0x0F3, 0x000, 0x000, 0x00F, 
  0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000, 
  0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000, },
};
 

// Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    uint16_t RGB_bmp_fixed[w * h];
    for (uint16_t pixel=0; pixel<w*h; pixel++) {
	uint8_t r,g,b;
	uint16_t color = pgm_read_word(bitmap + pixel);

	//Serial.print(color, HEX);
	b = (color & 0xF00) >> 8;
	g = (color & 0x0F0) >> 4;
	r = color & 0x00F;
	// expand from 4/4/4 bits per color to 5/6/5
	b = map(b, 0, 15, 0, 31);
	g = map(g, 0, 15, 0, 63);
	r = map(r, 0, 15, 0, 31);
	RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
	  }
    matrix.drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}

uint16_t Wheel(byte WheelPos) 
{
  if(WheelPos < 8) {
     return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } 
  else if(WheelPos < 16) {
    WheelPos -= 8;
    return matrix.Color333(0, 7-WheelPos, WheelPos);
  } 
  else  {
    WheelPos -= 16;
    return matrix.Color333(0, WheelPos, 7 - WheelPos);
  }
}

void display_scrollText(String text1, String text2) {
    matrix.clear();
    matrix.setTextWrap(false);  // we don't wrap text so it scrolls nicely
    matrix.setTextSize(1);
    matrix.setRotation(0);
    for (int8_t x=0; x<120; x++) {
	    matrix.clear();
	    matrix.setCursor(-x,0);
        //matrix.setFont(&FreeSerif9pt7b);
	    matrix.setTextColor(LED_CYAN_HIGH);
	    matrix.print(text1);
	    matrix.setCursor(32-x,16);
        //matrix.setFont(&FreeMonoBoldOblique12pt7b);
	    matrix.setTextColor(LED_RED_LOW);
        matrix.setTextSize(2);
	    matrix.print(text2);
        delay(60);
	}
	matrix.show();
    delay(50);
    matrix.setTextSize(1);
    matrix.setFont();
}

// Affichage du Liffab et de l'année 2022.
void matrix_LiffabSplashScreen(void) {
  uint8_t Pos_X = 0;
  uint8_t Pos_Y = 1; 
  uint8_t w;

  matrix.setTextSize(2);     // size 1 == 8 pixels high

  for (uint16_t Tour=0; Tour<32; Tour++) {
    Pos_Y = 32-Tour;
    w = 0;
    //  Logo LIFFAB
    matrix.clear();
    
    matrix.setCursor(1, Pos_Y);   matrix.setTextColor(Wheel(w));  matrix.print("L");  w = w+1;
    matrix.setCursor(11, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print("I");  w = w+1;
    matrix.setCursor(21, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print("F");  w = w+1;
    matrix.setCursor(32, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print("F");  w = w+1;
    matrix.setCursor(42, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print("A");  w = w+1;
    matrix.setCursor(54, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print("B");
   
    matrix.show();
    delay(70);
    }
  
  Pos_Y = 16; 
  matrix.setCursor(9, Pos_Y);  
  w = 5;
  //  Annee
  uint8_t w_init = 4;
  char *str = "2022";
  for (w=0; w<4; w++) {
    matrix.setTextColor(Wheel(w_init-w));
    matrix.print(str[w]);
  }
  delay(2500);
 
}

// Affichage de DGA et de l'année 2022.
void matrix_DGASplashScreen(void) {
  uint8_t Pos_X = 0;
  uint8_t Pos_Y = 1; 
  uint8_t w;

  matrix.setTextSize(2);     // size 1 == 8 pixels high

  for (uint16_t Tour=0; Tour<32; Tour++) {
    Pos_Y = 32-Tour;
    w = 0;
    //  Logo DGA
    matrix.clear();
    
    matrix.setCursor(1, Pos_Y);   matrix.setTextColor(Wheel(w));  matrix.print("D");  w = w+1;
    matrix.setCursor(11, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print(" ");  w = w+1;
    matrix.setCursor(21, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print("G");  w = w+1;
    matrix.setCursor(32, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print(" ");  w = w+1;
    matrix.setCursor(42, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print("A");  w = w+1;
    matrix.setCursor(54, Pos_Y);  matrix.setTextColor(Wheel(w));  matrix.print(" ");
   
    matrix.show();
    delay(70);
    }
  
  Pos_Y = 16; 
  matrix.setCursor(9, Pos_Y);  
  w = 5;
  //  Annee
  uint8_t w_init = 4;
  char *str = "2022";
  for (w=0; w<4; w++) {
    matrix.setTextColor(Wheel(w_init-w));
    matrix.print(str[w]);
  }
  delay(2500);
 
}

void loop() {

  // Affichage du Liffab et de l'année 2022.
  //matrix_LiffabSplashScreen();
  matrix_DGASplashScreen();
  display_scrollText("Agile Tour", "Rennes!");
  // Affichage du démarrage : "OpenLedRace, Soyez prets !"
  display_scrollText("OpenLedRace", "Soyez prets !");

}

void setup() {

    //Serial.begin(115200);
    //WiFi.mode(WIFI_STA);
    //while(WiFi.status() != WL_CONNECTED) {
      //Serial.println(readconf.ssid);
      //WiFi.begin("openLedRace","1234led56");
      //delay(2000);
      //Serial.print(".");
    //}
    //Serial.println(F("\r\nWiFi connecté"));

    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(BRIGHTNESS);
    // Test full bright of all LEDs. If brightness is too high
    // for your current limit (i.e. USB), decrease it.
    matrix.fillScreen(LED_WHITE_HIGH);
    matrix.show();
    delay(1000);
    matrix.clear();
}