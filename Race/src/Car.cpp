#include "Car.h"

#include <Adafruit_NeoPixel.h>

#include "RaceConfig.h"

using namespace RaceConfig;

Car::Car(uint32_t color) : _color(color)
{
  Reset();
}

void Car::Reset()
{
  _speed = 0.f;
  _distance = 0.f;
  _currentLoop = 1;
  _newLoopStarted = false;
  _finishedRace = false;
}

void Car::Update()
{
  _newLoopStarted = false;

  _speed -= _speed * KF; //friction

  _distance += _speed;

  if (_distance > _currentLoop * MaxLED)
  {
    _newLoopStarted = true;
    ++_currentLoop;
    if (_currentLoop > MaxLoops)
    {
      _finishedRace = true;
    }
  }
}

void Car::Draw(Adafruit_NeoPixel *led) const
{
  for (byte i = 0; i <= _currentLoop; ++i)
  {
    led->setPixelColor(((word)_distance % MaxLED) + i, _color);
  };
}

//Get distance in current loop
float Car::getCurrentDistance() const
{
  return getTotalDistance() - ((_currentLoop - 1) * MaxLED);
}