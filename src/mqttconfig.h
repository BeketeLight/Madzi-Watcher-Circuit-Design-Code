#ifndef MQTTCONFIG_H
#define MQTTCONFIG_H

// ====================== MQTT ======================
extern const char *MQTT_LOCAL_SERVER;
extern const int MQTT_LOCAL_PORT;
extern const char *MQTT_CLOUD_SERVER;
extern const int MQTT_CLOUD_PORT;
extern const char *MQTT_USER;
extern const char *MQTT_PASS;
extern const char *MQTT_TOPIC_SENSOR;
extern const char *MQTT_TOPIC_COMMANDS;
extern const char *MQTT_TOPIC_STATUS;
extern const char *MQTT_TOPIC_CONFIG;
extern const int MQTT_QOS; // QoS level for publishing (0, 1, or 2)
extern bool pendingBrokerSwitch;

#endif