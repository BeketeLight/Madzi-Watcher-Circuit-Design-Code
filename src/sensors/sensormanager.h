#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "../data/waterqualityreading.h"
#include "../config.h"
#include "../customConfig.h"

class SensorManager
{
public:
    SensorManager();
    WaterQualityReading readAll(ConfigManager &config);
    float readTurbidity(int &turbidityPin);
    float readPH(int &pHPin);
    int readTDS(int &tdsPin);
    float readEC(int &ecPin);
    float readTemperature();
    float calculateWQI(float &turbidity, float &pH, float &tds, float &ec);

    void begin();

private:
    int turbidityPin = 4; // ADC1_3
    int pHPin = 5;        // ADC1_4
    int tdsPin = 6;       // ADC1_5
    int ecPin = 15;       // ADC1_6
    int tempPinDS18B20 = 7;

    OneWire oneWire;
    DallasTemperature sensors;
};

#endif