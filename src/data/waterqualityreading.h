#ifndef WATERQUALITYREADING_H
#define WATERQUALITYREADING_H

#include <Arduino.h>

struct WaterQualityReading
{
    char deviceId[32]; // Fixed size instead of String
    char district[32];
    char treatmentPlantId[16];

    float turbidity;
    float pH;
    float tds;
    float electricalConductivity;
    float waterQualityIndex;

    bool anomalyDetected;
    bool pHVioleted;
    bool tdsVioleted;
    bool turbidityVioleted;
    bool electricalConductivityVioleted;
};

#endif