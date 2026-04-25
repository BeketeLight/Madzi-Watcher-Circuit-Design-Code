// mqttmanager.cpp

#include "mqttmanager.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include "config.h"

MqttManager::MessageHandler MqttManager::messageHandler = nullptr;

void MqttManager::setMessageHandler(MessageHandler handler)
{
    messageHandler = handler;
}

MqttManager::MqttManager()
    : client(espClient)
{
}

MqttManager::~MqttManager()
{
    if (client.connected())
    {
        client.disconnect();
    }
}

bool MqttManager::begin()
{
    // espClient.setInsecure(); // ← only if you don't want to verify cert (not recommended long-term)
    // espClient.setCACert( hivemq_ca_cert );   // ← better: add proper CA cert from HiveMQ docs

    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setKeepAlive(60);
    client.setBufferSize(512); // increase if your JSON is large

    client.setBufferSize(512);
    client.setCallback(MqttManager::staticCallback);

    return tryConnect();
}

bool MqttManager::subscribe(const char *topic, uint8_t qos)
{
    if (!client.connected())
    {
        Serial.println("Cannot subscribe – MQTT not connected");
        return false;
    }

    bool success = (qos == 0) ? client.subscribe(topic) : client.subscribe(topic, qos);

    if (success)
    {
        Serial.printf("Subscribed to topic '%s' (QoS %d) ✓\n", topic, qos);
        return true;
    }
    else
    {
        Serial.printf("Failed to subscribe to topic '%s' → rc=%d\n", topic, client.state());
        return false;
    }
}

void (*MqttManager::messageCallback)(const char *topic, const uint8_t *payload, size_t len) = nullptr;

// Static forwarding callback (required because PubSubClient expects a free function / matching signature)
void MqttManager::staticCallback(char *topic, uint8_t *payload, unsigned int length)
{
    String msg;

    for (unsigned int i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }

    String topicStr = String(topic);

    Serial.println("MQTT Message: " + msg);

    if (messageHandler)
    {
        messageHandler(topicStr, msg);
    }
}

void MqttManager::setMessageCallback(void (*cb)(const char *topic, const uint8_t *payload, size_t len))
{
    messageCallback = cb;
}

void MqttManager::loop()
{
    unsigned long now = millis();

    if (client.connected())
    {
        client.loop();
        connectFailCount = 0;
        reconnectBackoffMs = 2000; // Start smaller (2s)
        return;
    }

    //  Don't even try if WiFi is down.
    // This prevents the backoff from exploding while the router reboots.
    if (WiFi.status() != WL_CONNECTED)
    {
        lastConnectAttemptMs = now; // Keep sliding the window
        return;
    }

    if (now - lastConnectAttemptMs < reconnectBackoffMs)
        return;

    lastConnectAttemptMs = now;

    if (tryConnect())
    {
        Serial.println("MQTT Connected ✓");
        connectFailCount = 0;
        reconnectBackoffMs = 2000;
    }
    else
    {
        connectFailCount++;

        // 2. Linear/Soft Backoff: 2s, 4s, 8s, 15s, 15s...
        // 15 seconds is plenty for a local network.
        reconnectBackoffMs = min(2000UL * (1UL << min(connectFailCount, 3)), 15000UL);

        Serial.printf("MQTT Fail (rc=%d) | Retry #%d in %lus\n",
                      client.state(), connectFailCount, reconnectBackoffMs / 1000);
    }
}

bool MqttManager::tryConnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected → cannot attempt MQTT");
        return false;
    }

    // Make client ID unique (helps with quick reconnects / broker side)
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    String clientId = "MADZI_WATCHER_TREATMENT_01_" + mac.substring(8); // last 3 bytes usually enough

    Serial.print("Attempting MQTT connection as ");
    Serial.println(clientId);

    bool ok = client.connect(
        clientId.c_str(),
        MQTT_USER,
        MQTT_PASS
        // , "will/topic", 0, true, "offline"   // ← optional last will
    );

    if (!ok)
    {
        int rc = client.state();
        if (rc == MQTT_CONNECT_FAILED)
        { // -2
            Serial.println("→ Network failure (DNS / TCP / no route) – check WiFi/DNS");
        }
        else if (rc == MQTT_CONNECTION_TIMEOUT)
        { // -4
            Serial.println("→ Connection timeout – broker unreachable / firewall?");
        }
        else if (rc == MQTT_CONNECT_BAD_CREDENTIALS)
        {
            Serial.println("→ Bad username/password");
        }
        else
        {
            Serial.print("→ Other error rc=");
            Serial.println(rc);
        }
    }
    else
    {
        subscribe(MQTT_TOPIC_COMMANDS, MQTT_QOS); // re-subscribe on reconnect (if needed)
        subscribe(MQTT_TOPIC_CONFIG, MQTT_QOS);
        ;
    }

    return ok;
}

bool MqttManager::publish(const WaterQualityReading &data)
{
    if (!client.connected())
    {
        // Don't even try publish if disconnected – let loop() handle reconnection
        return false;
    }

    JsonDocument doc; // adjust size if needed (test with serializeJsonPretty)

    doc["deviceId"] = data.deviceId;
    doc["turbidity"] = data.turbidity;
    doc["pH"] = data.pH;
    doc["tds"] = data.tds;
    doc["electricalConductivity"] = data.electricalConductivity;
    doc["waterQualityIndex"] = data.waterQualityIndex;

    // anomaly object
    JsonObject anomaly = doc["anomaly"].to<JsonObject>();
    anomaly["detected"] = data.anomalyDetected;
    anomaly["pH"] = data.pHVioleted;
    anomaly["tds"] = data.tdsVioleted;
    anomaly["turbidity"] = data.turbidityVioleted;
    anomaly["electricalConductivity"] = data.electricalConductivityVioleted;

    // location object
    JsonObject loc = doc["location"].to<JsonObject>();
    loc["district"] = data.district;
    loc["treatmentPlantId"] = data.treatmentPlantId;

    char jsonBuffer[512];
    size_t len = serializeJson(doc, jsonBuffer);

    if (len >= sizeof(jsonBuffer) - 1)
    {
        Serial.println("ERROR: JSON buffer too small! Increase to 768 or more.");
        return false;
    }

    bool success = client.publish(MQTT_TOPIC_SENSOR, jsonBuffer, len);

    // if (!success)
    // {
    //     Serial.print("Publish failed → rc=");
    //     Serial.println(client.state());
    //     // Optional: force disconnect so loop() will reconnect properly
    //     if (client.connected())
    //         client.disconnect();
    // }

    return success;
}
