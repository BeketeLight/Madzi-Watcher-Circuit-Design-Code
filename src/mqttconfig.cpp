#include "mqttconfig.h"

// ====================== MQTT ======================
const char *MQTT_LOCAL_SERVER = "192.168.43.82"; //"c73a854202c347a9bda6c26b8cc5a443.s1.eu.hivemq.cloud";
const int MQTT_LOCAL_PORT = 1883;                // 8883;
const char *MQTT_CLOUD_SERVER = "c73a854202c347a9bda6c26b8cc5a443.s1.eu.hivemq.cloud";
const int MQTT_CLOUD_PORT = 8883;
const char *MQTT_USER = "madziwatcher";
const char *MQTT_PASS = "madziWatcher001";
const char *MQTT_TOPIC_SENSOR = "waterquality/sensor";
const char *MQTT_TOPIC_COMMANDS = "waterquality/commands";
const char *MQTT_TOPIC_STATUS = "waterquality/status";
const char *MQTT_TOPIC_CONFIG = "waterquality/config";
const int MQTT_QOS = 0;           // QoS level for publishing (0, 1, or 2)
bool pendingBrokerSwitch = false; // Flag to indicate pending MQTT broker switch
