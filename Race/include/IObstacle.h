#pragma once

#include <Arduino.h>

class Player;
class Adafruit_NeoPixel;

class IObstacle
{
public:
    enum ObstacleType
    {
        OBSTACLE_OIL,
        OBSTACLE_RAMP
    };

protected:
    word _start;
    word _end;
    uint32_t _color;
    ObstacleType _type;

public:
    IObstacle(word start, word end, uint32_t color);
    virtual void Update(Player *player) = 0;
    virtual void Draw(Adafruit_NeoPixel *led) = 0;

    word getStart() const { return _start; }
    void setStart(word start) { _start = start; }
    word getEnd() const { return _end; }
    void setEnd(word end) { _end = end; }
    uint32_t getColor() const { return _color; }
    void setColor(uint32_t color) { _color = color; }
    ObstacleType getType() const { return _type; }

    bool operator<(const IObstacle &other)
    {
        if (_start == other._start)
        {
            return _end <= other._end;
        }

        return _start < other._start;
    }
};
