#ifndef WATERQUALITYCONTROLLER_H
#define WATERQUALITYCONTROLLER_H

#include "../data/waterqualityreading.h"
#include "../actuators/solenoidvalve.h"
#include "../actuators/statusled.h"
#include "../actuators/buzzer.h"
#include "../config.h"

class WaterQualityController
{
public:
    WaterQualityController(SolenoidValve &valve);
    void process(const WaterQualityReading &reading, Buzzer &buzzer);

private:
    SolenoidValve &_valve;
    int anomalyCount = 0;
    int normalCount = 0;
    bool valveClosed = false;
};

#endif