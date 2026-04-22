// mqttmanager.h  (updated interface – add these if missing)
#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <PubSubClient.h>
#include <WiFi.h>
// #include <WiFiClientSecure.h> // assuming you're using secure connection to HiveMQ Cloud
#include "data/waterqualityreading.h"

class MqttManager
{
public:
    MqttManager();
    ~MqttManager();

    bool begin();
    void loop(); // call frequently from network task
    bool publish(const WaterQualityReading &data);
    bool subscribe(const char *topic, uint8_t qos = 0);
    void callback(const char *topic, const uint8_t *payload, size_t len);

private:
    WiFiClient espClient; // secure client for port 8883
    PubSubClient client;

    unsigned long lastConnectAttemptMs = 0;
    unsigned long reconnectBackoffMs = 5000; // starts at 5 s
    int connectFailCount = 0;
    static const unsigned long MAX_BACKOFF_MS = 120000; // 2 minutes max
    static void staticCallback(char *topic, uint8_t *payload, unsigned int length);
    void setMessageCallback(void (*cb)(const char *topic, const uint8_t *payload, size_t len));
    static void (*messageCallback)(const char *topic, const uint8_t *payload, size_t len);

    bool tryConnect();
    bool isReallyConnected(); // more reliable check
};

#endif
