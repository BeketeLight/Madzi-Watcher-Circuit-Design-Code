#include "solenoidvalve.h"
#include <Arduino.h>

SolenoidValve::SolenoidValve(int pin) : _pin(pin), _state(false) {}

void SolenoidValve::begin() {
    pinMode(_pin, OUTPUT);
    open(); 
}

void SolenoidValve::open() {
    digitalWrite(_pin, HIGH);
    _state = true;
}

void SolenoidValve::close() {
    digitalWrite(_pin, LOW);
    _state = false;
}

bool SolenoidValve::isOpen() const {
    return _state;
}