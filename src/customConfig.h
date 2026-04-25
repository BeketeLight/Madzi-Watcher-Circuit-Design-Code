#pragma once
#include <Arduino.h>

struct DeviceConfig
{
    char deviceId[32];
    char district[32];
    char treatmentPlantId[16];
};

class ConfigManager
{
public:
    bool begin();
    bool load();
    bool save();

    void updateFromJson(const String &json);

    DeviceConfig &getConfig();

private:
    DeviceConfig config;
};