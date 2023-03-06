#include "Coroutines/LedLightingCoroutine.h"

#include <Adafruit_NeoPixel.h>

LedLightingCoroutine::LedLightingCoroutine(Adafruit_NeoPixel *ledStrip) : _ledStrip(ledStrip),
                                                                          _color(0),
                                                                          _delay(0),
                                                                          _type(COLOR_WIPE)
{
}

void LedLightingCoroutine::setParameters(LightingType type, uint32_t color, uint32_t delay)
{
    reset();
    _type = type;
    _color = color;
    _delay = delay;
    _index = _numChases = 0;
}

byte LedLightingCoroutine::getPin() const
{
    return (byte)_ledStrip->getPin();
}

int LedLightingCoroutine::runCoroutine()
{
    COROUTINE_BEGIN();

    switch (_type)
    {
    // Fill the LEDs one after the other with a color
    case COLOR_WIPE:
    {
        while( _index < _ledStrip->numPixels())
        {
            _ledStrip->setPixelColor(_index, _color);
            _ledStrip->show();
            ++_index;
            BLOCKING_COROUTINE_DELAY(_delay);
        }
    }
    break;

    //Cycle through all colors of the rainbow
    case RAINBOW:
    {
        while(_index < 256)
        {
            for (int i = 0; i < _ledStrip->numPixels(); ++i)
            {
                _ledStrip->setPixelColor(i, Wheel((_index + i) & 255));
            }
            _ledStrip->show();
            ++_index;
            BLOCKING_COROUTINE_DELAY(_delay);
        }
    }
    break;

    //Theatre-style crawling lights.
    case THEATER_CHASE:
    {
        while( _numChases < 10)
        { 
            _index = 0;
            //do 10 cycles of chasing
            while( _index < 3)
            {
                for (int i = 0; i < _ledStrip->numPixels(); i += 3)
                {
                    //turn every third pixel on
                    _ledStrip->setPixelColor(_index + i, _color);
                }
                _ledStrip->show();
                BLOCKING_COROUTINE_DELAY(_delay);
                _ledStrip->clear();

                ++_index;
            }
            ++_numChases;
        }
    }
    break;

    case SOLID_COLOR:
    {
        _ledStrip->fill(_color);
        _ledStrip->show();
        BLOCKING_COROUTINE_DELAY(_delay);
    }
    break;
    }

    COROUTINE_END();
}

uint32_t LedLightingCoroutine::Wheel(byte WheelPos)
{
    if (WheelPos < 85)
    {
        return Adafruit_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        return Adafruit_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return Adafruit_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}
