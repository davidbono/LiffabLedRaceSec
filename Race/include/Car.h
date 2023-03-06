#pragma once

#include <Arduino.h>

class Adafruit_NeoPixel;

class Car
{
    protected:
    uint32_t _color;
    float _speed;
    float _distance;
    byte _currentLoop;
    bool _newLoopStarted;
    bool _finishedRace;

    public:
    Car(uint32_t color);

    void Update();
    void Reset();
    void Draw(Adafruit_NeoPixel* led) const;

    void setSpeed( float speed) { _speed = speed; }
    void accelerate( float inc ) { _speed += inc; }

    bool isStartingNewLoop() const { return _newLoopStarted; }
    bool isFinishedRace() const { return _finishedRace; }

    float getTotalDistance() const { return _distance; }
    float getCurrentDistance() const;
    void increaseDistance(float inc) { _distance += inc; }
    uint32_t getColor() const { return _color; }
    void setColor(uint32_t color) { _color = color; }
    
};
