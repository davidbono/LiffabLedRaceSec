#include "Controller.h"

Controller::Controller(byte pin, uint8_t mode) : _pin(pin)
{
    pinMode(_pin, mode);
    Reset();
}

void Controller::Reset()
{
    _alreadyPressed = true;
    _pressedTime = 0;
}

void Controller::Update()
{
    if (isPressedThisLoop())
    {
        _pressedTime = millis();
        _alreadyPressed = true;
    };
    
    if (isReleasedThisLoop())
    {
        _alreadyPressed = false;
    };
}

bool Controller::isPressed() const
{
    return (digitalRead(_pin) == 0);
}

bool Controller::isPressedThisLoop() const
{
    return isPressed() && !_alreadyPressed;
}
    
bool Controller::isReleasedThisLoop() const
{
    return _alreadyPressed && !isPressed();
}