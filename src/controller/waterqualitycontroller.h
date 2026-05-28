#ifndef WATERQUALITYCONTROLLER_H
#define WATERQUALITYCONTROLLER_H

#include "../data/waterqualityreading.h"
#include "../actuators/solenoidvalve.h"
#include "../actuators/statusled.h"
#include "../actuators/buzzer.h"
#include "../utils/emailmanager.h"
#include "../config.h"
#include "../customConfig.h"
#include "../utils/mqttmanager.h"

enum SystemState
{
    SYSTEM_ON,
    SYSTEM_OFF,

};
enum ValveState
{
    VALVE_CLOSED_ANOMALY,
    VALVE_CLOSED_MANUAL,
    VALVE_OPEN_NORMAL,
};

class WaterQualityController
{

public:
    WaterQualityController(SolenoidValve &valve);
    void process(const WaterQualityReading &reading, Buzzer &buzzer);
    void handleCommand(const String &topic, const String &message, Buzzer &buzzer, ConfigManager &configManager, MqttManager &mqttManager);
    SystemState getState() const;
    ValveState getvalveState() const;

private:
    SolenoidValve &_valve;
    EmailManager emailManager;
    int anomalyCount = 0;
    int emailsendCount = 0;
    int normalCount = 0;
    bool valveClosed = false;
    volatile SystemState state = SYSTEM_ON;
    volatile ValveState valveState = VALVE_OPEN_NORMAL;
};

#endif