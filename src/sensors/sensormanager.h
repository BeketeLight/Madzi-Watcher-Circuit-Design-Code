#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include "../data/waterqualityreading.h"
#include "../config.h"

class SensorManager
{
public:
    SensorManager();
    WaterQualityReading readAll();
    float readTurbidity(int &turbidityPin);
    float readPH(int &pHPin);
    int readTDS(int &tdsPin);
    float readEC(int &ecPin);
    float calculateWQI(float &turbidity, float &pH, float &tds, float &ec);
    void begin();

private:
    int turbidityPin = 4; // ADC1_3
    int pHPin = 5;        // ADC1_4
    int tdsPin = 6;       // ADC1_5
    int ecPin = 7;        // ADC1_6
};

#endif