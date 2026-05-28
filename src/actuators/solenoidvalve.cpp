#include "solenoidvalve.h"
#include <Arduino.h>

SolenoidValve::SolenoidValve(int pin) : _pin(pin), _state(false) {}

void SolenoidValve::begin()
{
    pinMode(_pin, OUTPUT);
    open();
    Serial.println("valved initialised");
}

void SolenoidValve::close()
{
    digitalWrite(_pin, HIGH);
    _state = false;
    Serial.println("Valve Closed");
}

void SolenoidValve::open()
{
    digitalWrite(_pin, LOW);
    _state = true;
    Serial.println("Valve Opened");
}

bool SolenoidValve::isOpen() const
{
    return _state;
}