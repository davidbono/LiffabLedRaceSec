#pragma once

#include <Arduino.h>

class Controller
{
protected:
    byte _pin;
    bool _alreadyPressed;
    unsigned long _pressedTime;

public:
    Controller(byte pin, uint8_t mode = INPUT_PULLUP);
    bool isPressed() const;
    bool isPressedThisLoop() const;
    bool isReleasedThisLoop() const;

    void Reset();
    void Update();

    bool alreadyPressed() const { return _alreadyPressed; }
    void setAlreadyPressed(bool pressed) { _alreadyPressed = pressed; }

    unsigned long getPressedTime() const { return _pressedTime; }
    byte getPin() const { return _pin; }
};
