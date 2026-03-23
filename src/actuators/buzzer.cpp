#include "buzzer.h"

Buzzer::Buzzer(int pin): _pin(pin) {}

void Buzzer::begin() 
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Buzzer::beep(int durationMs, int frequency) 
{
    tone(_pin, frequency, durationMs);
}

void Buzzer::alert() 
{
    // Rapid beep pattern
    beep(100, 2500); // Short beep
    delay(400);
    beep(100, 2500); // Note: Use vTaskDelay if calling from a FreeRTOS task
    beep(100, 2500); 
    delay(300);
    beep(1000, 2500); 
}

void Buzzer::stop() 
{
    noTone(_pin);
}

