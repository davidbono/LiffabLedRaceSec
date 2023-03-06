#pragma once

#include <Arduino.h>
#include "Coroutines/IBlockingCoroutine.h"

class Adafruit_NeoPixel;

class LedLightingCoroutine : public IBlockingCoroutine
{
public:
    enum LightingType
    {
        COLOR_WIPE,
        RAINBOW,
        THEATER_CHASE,
        SOLID_COLOR
    };

protected:
    Adafruit_NeoPixel *_ledStrip;
    uint32_t _color;
    uint32_t _delay;
    LightingType _type;
    word _index;
    word _numChases;

public:
    LedLightingCoroutine(Adafruit_NeoPixel *ledStrip);
    void setParameters(LightingType type, uint32_t color, uint32_t delay);
    byte getPin() const;
    
    int runCoroutine() override;

    virtual ~LedLightingCoroutine(){};

private:
    static uint32_t Wheel(byte WheelPos);
};