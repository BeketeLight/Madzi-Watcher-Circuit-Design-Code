#include "customConfig.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

static const char *CONFIG_PATH = "/config.json";
static const char *FIRST_BOOT_FLAG = "/firstboot.flag";

ConfigManager::ConfigManager()
{
    setFactoryDefaults(); // Only sets in RAM
    Serial.println("ConfigManager initialized with factory defaults");
}

bool ConfigManager::begin()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS failed to mount");
        return false;
    }

    // === FORCE DELETE OLD CONFIG ON FIRST BOOT AFTER FLASH ===
    if (!LittleFS.exists(FIRST_BOOT_FLAG))
    {
        Serial.println(" First boot detected! Deleting old config.json...");

        if (LittleFS.exists(CONFIG_PATH))
        {
            LittleFS.remove(CONFIG_PATH);
            Serial.println(" Old config.json deleted");
        }

        // Create flag so this doesn't happen again until next flash
        File flagFile = LittleFS.open(FIRST_BOOT_FLAG, "w");
        if (flagFile)
        {
            flagFile.print("boot");
            flagFile.close();
            Serial.println(" First boot flag created");
        }

        Serial.println(" Creating fresh config with factory defaults...");
        save(); // Save new defaults
    }
    else
    {
        Serial.println("Normal boot: Loading existing config");
        load();
    }

    return true;
}
void ConfigManager::setFactoryDefaults()
{
    strlcpy(config.deviceId, "MADZI-ESP-01", sizeof(config.deviceId));
    strlcpy(config.district, "Lilongwe", sizeof(config.district));
    strlcpy(config.treatmentPlantId, "TP001", sizeof(config.treatmentPlantId));

    config.useLocalMqtt = true; // Default to local

    strlcpy(config.mqttLocalServer, MQTT_LOCAL_SERVER, sizeof(config.mqttLocalServer));
    config.mqttLocalPort = MQTT_LOCAL_PORT;

    strlcpy(config.mqttCloudServer, MQTT_CLOUD_SERVER, sizeof(config.mqttCloudServer));
    config.mqttCloudPort = MQTT_CLOUD_PORT;

    strlcpy(config.mqttUser, MQTT_USER, sizeof(config.mqttUser));
    strlcpy(config.mqttPass, MQTT_PASS, sizeof(config.mqttPass));
}

bool ConfigManager::load()
{
    if (!LittleFS.exists(CONFIG_PATH))
    {
        Serial.println("Config file not found, using defaults");
        setDefaults();
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

    config.useLocalMqtt = doc["useLocalMqtt"] | true;

    strlcpy(config.mqttLocalServer, doc["mqttLocalServer"] | "192.168.1.100", sizeof(config.mqttLocalServer));
    config.mqttLocalPort = doc["mqttLocalPort"] | 1883;

    strlcpy(config.mqttCloudServer, doc["mqttCloudServer"] | "broker.hivemq.com", sizeof(config.mqttCloudServer));
    config.mqttCloudPort = doc["mqttCloudPort"] | 1883;

    strlcpy(config.mqttUser, doc["mqttUser"] | "", sizeof(config.mqttUser));
    strlcpy(config.mqttPass, doc["mqttPass"] | "", sizeof(config.mqttPass));

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

    doc["useLocalMqtt"] = config.useLocalMqtt;
    doc["mqttLocalServer"] = config.mqttLocalServer;
    doc["mqttLocalPort"] = config.mqttLocalPort;

    doc["mqttCloudServer"] = config.mqttCloudServer;
    doc["mqttCloudPort"] = config.mqttCloudPort;

    doc["mqttUser"] = config.mqttUser;
    doc["mqttPass"] = config.mqttPass;
    serializeJson(doc, file);
    file.close();

    return true;
}
void ConfigManager::setDefaults()
{
    strlcpy(config.deviceId, "MADZI-ESP-01", sizeof(config.deviceId));
    strlcpy(config.district, "Lilongwe", sizeof(config.district));
    strlcpy(config.treatmentPlantId, "TP001", sizeof(config.treatmentPlantId));

    config.useLocalMqtt = true; // Default to local

    strlcpy(config.mqttLocalServer, MQTT_LOCAL_SERVER, sizeof(config.mqttLocalServer));
    config.mqttLocalPort = MQTT_LOCAL_PORT;

    strlcpy(config.mqttCloudServer, MQTT_CLOUD_SERVER, sizeof(config.mqttCloudServer));
    config.mqttCloudPort = MQTT_CLOUD_PORT;

    strlcpy(config.mqttUser, MQTT_USER, sizeof(config.mqttUser));
    strlcpy(config.mqttPass, MQTT_PASS, sizeof(config.mqttPass));
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

    if (doc.containsKey("useLocalMqtt"))
        config.useLocalMqtt = doc["useLocalMqtt"];

    if (doc.containsKey("mqttLocalServer"))
        strlcpy(config.mqttLocalServer, doc["mqttLocalServer"], sizeof(config.mqttLocalServer));

    if (doc.containsKey("mqttLocalPort"))
        config.mqttLocalPort = doc["mqttLocalPort"];

    if (doc.containsKey("mqttCloudServer"))
        strlcpy(config.mqttCloudServer, doc["mqttCloudServer"], sizeof(config.mqttCloudServer));

    if (doc.containsKey("mqttCloudPort"))
        config.mqttCloudPort = doc["mqttCloudPort"];

    if (doc.containsKey("mqttUser"))
        strlcpy(config.mqttUser, doc["mqttUser"], sizeof(config.mqttUser));

    if (doc.containsKey("mqttPass"))
        strlcpy(config.mqttPass, doc["mqttPass"], sizeof(config.mqttPass));

    save(); // persist immediately
}

DeviceConfig &ConfigManager::getConfig()
{
    return config;
}