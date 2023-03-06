#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "Defines.h"

class Car;
class Controller;
class IObstacle;
class LedLightingCoroutine;

template <class T>
class DynamicPointerArray;

class Player
{
protected:
    Car *_car;
    Controller *_controller;
    Adafruit_NeoPixel *_controllerLED;

    int16_t _lightingPin;
    byte _currentObstacle;
    char _name[MAX_NAME_LENGTH];

public:
    Player(uint32_t carColor, byte controllerPin, int16_t lightingPin, char *name);
    ~Player();
    void Destroy();

    void Update(DynamicPointerArray<IObstacle *> &obstacles);
    void Reset();
    
    const Controller &controller() const { return *_controller; }
    
    const Car &car() const { return *_car; }
    Car &mutableCar() { return *_car; }

    int16_t getLightingPin() const { return _lightingPin;}
    const char *getName() const { return _name; }
    uint32_t getColor() const;

    void setName(char *name);
    void setColor(uint32_t color);

    bool operator<(const Player &other) const;
    bool operator==(const Player &other) const;
    bool operator!=(const Player &other) const;

private:
    Player();
};
