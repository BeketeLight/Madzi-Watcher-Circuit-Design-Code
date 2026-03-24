#ifndef STATUSLED_H
#define STATUSLED_H

#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "../config.h"

extern Adafruit_NeoPixel np;

void initStatusLED();
void updateStatusLED(bool &newStatus);
void blinkRed();
void setLED(uint8_t r, uint8_t g, uint8_t b);

#endif