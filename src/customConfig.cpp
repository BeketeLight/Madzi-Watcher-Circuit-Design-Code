#include "customConfig.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

static const char *CONFIG_PATH = "/config.json";

bool ConfigManager::begin()
{
    return LittleFS.begin(true);
}

bool ConfigManager::load()
{
    if (!LittleFS.exists(CONFIG_PATH))
    {
        Serial.println("Config file not found, using defaults");
        return false;
    }

    File file = LittleFS.open(CONFIG_PATH, "r");
    if (!file)
        return false;

    StaticJsonDocument<256> doc;
    deserializeJson(doc, file);

    strlcpy(config.deviceId, doc["deviceId"] | "UNKNOWN", sizeof(config.deviceId));
    strlcpy(config.district, doc["district"] | "UNKNOWN", sizeof(config.district));
    strlcpy(config.treatmentPlantId, doc["treatmentPlantId"] | "TP000", sizeof(config.treatmentPlantId));

    file.close();
    return true;
}

bool ConfigManager::save()
{
    File file = LittleFS.open(CONFIG_PATH, "w");
    if (!file)
        return false;

    StaticJsonDocument<256> doc;

    doc["deviceId"] = config.deviceId;
    doc["district"] = config.district;
    doc["treatmentPlantId"] = config.treatmentPlantId;

    serializeJson(doc, file);
    file.close();

    return true;
}

void ConfigManager::updateFromJson(const String &json)
{
    StaticJsonDocument<256> doc;
    deserializeJson(doc, json);

    if (doc.containsKey("deviceId"))
        strlcpy(config.deviceId, doc["deviceId"], sizeof(config.deviceId));

    if (doc.containsKey("district"))
        strlcpy(config.district, doc["district"], sizeof(config.district));

    if (doc.containsKey("treatmentPlantId"))
        strlcpy(config.treatmentPlantId, doc["treatmentPlantId"], sizeof(config.treatmentPlantId));

    save(); // persist immediately
}

DeviceConfig &ConfigManager::getConfig()
{
    return config;
}