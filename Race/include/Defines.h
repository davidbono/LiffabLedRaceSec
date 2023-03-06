#pragma once

#define SAVE_FILE_VERSION "1.2.0"

#define LED_CIRCLE

#ifdef ESP32
    #define EEPROM_SIZE 300
    //If this is not defined, will use EEPROM instead
    #define USE_SPIFFS
#endif

#define DEFAULT_LED 600 // MAX LEDs actives on strip
#define MAXLEDCIRCLE 24 // MAX LEDs actives on circle
#define MAXLEDPLAYER 12 // MAX LEDs actives on player button

#define DEFAULT_LOOPS 3 //Number of loops in the race

#define PIN_LED 21      //Pin to which the LED track is connected
#define PIN_CIRCLE 17   //Pin to which the starting gate is connected

#define PIN_START 16    //Optional button to start race
#define PIN_EASY 15     //Optional switch for easy mode

#define PIN_AUDIO 5     //Pin to which the audio speaker is connected

#define OIL_PRESS_DELAY_DEFAULT 200

#define DEFAULT_OIL_COLOR Adafruit_NeoPixel::Color(255, 255, 0)
#define DEFAULT_RAMP_COLOR Adafruit_NeoPixel::Color(127, 0, 127)

#define INIT_OBSTACLES                                         \
    Obstacles.Add(new OilObstacle(40, 47, DEFAULT_OIL_COLOR)); \
    Obstacles.Add(new RampObstacle(90, 167, 10, DEFAULT_RAMP_COLOR, RampObstacle::RAMP_UP)); \
    Obstacles.Add(new RampObstacle(168, 220, 8, Adafruit_NeoPixel::Color(0, 127, 0), RampObstacle::RAMP_DOWN)); \
    Obstacles.Add(new RampObstacle(330, 361, 9, DEFAULT_RAMP_COLOR, RampObstacle::RAMP_UP)); \
    Obstacles.Add(new RampObstacle(385, 435, 12, DEFAULT_RAMP_COLOR, RampObstacle::RAMP_UP)); \
    Obstacles.Add(new RampObstacle(436, 550, 8, Adafruit_NeoPixel::Color(0, 127, 0), RampObstacle::RAMP_DOWN));

#define MAX_PLAYERS 4
#define MAX_NAME_LENGTH 15
#define PIN_P1 13 // switch player 1 to PIN and GND
#define PIN_P2 27 // switch player 2 to PIN and GND
#define PIN_P3 25 // switch player 3 to PIN and GND
#define PIN_P4 32 // switch player 4 to PIN and GND

#define COLOR_P1 Adafruit_NeoPixel::Color(127, 0, 0)
#define COLOR_P2 Adafruit_NeoPixel::Color(0, 127, 0)
#define COLOR_P3 Adafruit_NeoPixel::Color(0, 0, 127)
#define COLOR_P4 Adafruit_NeoPixel::Color(127, 127, 127)

#define LIGHT_PIN_P1 14 //PIN of the player 1 button lights
#define LIGHT_PIN_P2 26 //PIN of the player 2 button lights
#define LIGHT_PIN_P3 33 //PIN of the player 3 button lights
#define LIGHT_PIN_P4 19 //PIN of the player 4 button lights

#define INIT_PLAYERS                                           \
    Players.Add(Player(COLOR_P1, PIN_P1, LIGHT_PIN_P1, (char *)"Player 1")); \
    Players.Add(Player(COLOR_P2, PIN_P2, LIGHT_PIN_P2, (char *)"Player 2")); \
    Players.Add(Player(COLOR_P3, PIN_P3, LIGHT_PIN_P3, (char *)"Player 3")); \
    Players.Add(Player(COLOR_P4, PIN_P4, LIGHT_PIN_P4, (char *)"Player 4"));

#define ACEL 0.2f //Acceleration
#define KF 0.015f //friction constant
#define KG 0.003f //gravity constant

#define DYNAMIC_CHUNK_SIZE 5