#include "RampObstacle.h"

#include <Adafruit_NeoPixel.h>

#include "Player.h"
#include "Car.h"
#include "Defines.h"

#include "ColorUtils.h"
#include "RaceConfig.h"

const char *RampObstacle::_StyleNames[] = {"Hill", "Hole", "Up", "Down"};

RampObstacle::RampObstacle(word start, word end, byte height, uint32_t color, RampStyle style) : IObstacle(start, end, color),
                                                                                                 _style(style),
                                                                                                 _height(height)
{
    _type = IObstacle::ObstacleType::OBSTACLE_RAMP;
}

void RampObstacle::Update(Player *player)
{
    float carPos = player->car().getCurrentDistance();
    float acceleration = 0.f;

    if (carPos >= _start && carPos <= _end)
    {
        switch (_style)
        {
        case RAMP_HILL:
        {
            word center = (_start + _end) / 2;
            if (carPos < center)
            {
                acceleration = MoveUp(_start, center, carPos);
            }
            else
            {
                acceleration = MoveDown(center, _end, carPos);
            }
        }
        break;

        case RAMP_HOLE:
        {
            word center = (_start + _end) / 2;
            if (carPos < center)
            {
                acceleration = MoveDown(_start, center, carPos);
            }
            else
            {
                acceleration = MoveUp(center, _end, carPos);
            }
        }
        break;

        case RAMP_UP:
        {
            acceleration = MoveUp(_start, _end, carPos);
        }
        break;

        case RAMP_DOWN:
        {
            acceleration = MoveDown(_start, _end, carPos);
        }
        break;
        default:
            break;
        }
    }

    player->mutableCar().accelerate(acceleration);
}

void RampObstacle::Draw(Adafruit_NeoPixel *led)
{
    switch (_style)
    {
    case RAMP_HILL:
    {
        word center = (_start + _end) / 2;
        DrawUp(_start, center, led);
        DrawDown(center, _end, led);
        led->setPixelColor(center, Adafruit_NeoPixel::Color(127, 127, 127));
    }
    break;

    case RAMP_HOLE:
    {
        word center = (_start + _end) / 2;
        DrawDown(_start, center, led);
        DrawUp(center, _end, led);
        led->setPixelColor(center, 0);
    }
    break;

    case RAMP_UP:
    {
        DrawUp(_start, _end, led);
    }
    break;

    case RAMP_DOWN:
    {
        DrawDown(_start, _end, led);
    }
    break;

    default:
        break;
    }
}

void RampObstacle::DrawUp(word left, word right, Adafruit_NeoPixel *led)
{
    byte length = right - left;
    float dimming = 1;
    for (word i = left; i <= right; ++i)
    {
        dimming = 1.f - (float(right - i) / length);
        led->setPixelColor(i, ColorMultiply(_color, dimming));
    }
}

void RampObstacle::DrawDown(word left, word right, Adafruit_NeoPixel *led)
{
    byte length = right - left;
    float dimming = 1;
    for (word i = left; i <= right; ++i)
    {
        dimming = 1.f - (float(i - left)) / length;
        led->setPixelColor(i, ColorMultiply(_color, dimming));
    }
}

float RampObstacle::MoveUp(word left, word right, float position)
{
    byte length = right - left;
    float acceleration = KG * _height * ((position - left) / length);

    if( RaceConfig::EasyMode )
    {
        acceleration *= 0.5f;
    }
    
    return -acceleration;
}

float RampObstacle::MoveDown(word left, word right, float position)
{
    byte length = right - left;
    float acceleration = KG * _height * ((right - position) / length);

    return acceleration;
}

uint32_t RampObstacle::ColorMultiply(uint32_t color, float mul)
{
    uint8_t r, g, b;
    ColorUtils::SplitColor(color, r, g, b);
    return ((uint32_t)(r * mul) << 16) | ((uint32_t)(g * mul) << 8) | (uint32_t)(b * mul);
}