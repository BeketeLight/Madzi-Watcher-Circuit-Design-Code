#pragma once
#include <Arduino.h>
#include "mqttconfig.h"

struct DeviceConfig
{
    char deviceId[32];
    char district[32];
    char treatmentPlantId[16];
    bool useLocalMqtt;
    char mqttLocalServer[64];
    char mqttCloudServer[64];
    int mqttLocalPort;
    int mqttCloudPort;
    char mqttUser[32];
    char mqttPass[32];
};

class ConfigManager
{
public:
    ConfigManager();
    bool begin();
    bool load();
    bool save();

    void updateFromJson(const String &json);
    void setDefaults();

    DeviceConfig &getConfig();

private:
    DeviceConfig config;
    void setFactoryDefaults();
};