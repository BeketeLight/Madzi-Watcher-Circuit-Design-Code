#ifndef CONFIG_H
#define CONFIG_H

// ====================== STATUS LED & THRESHOLDS ======================
#define NEOPIXEL_PIN 48
#define NUM_PIXELS 1

// ====================== WiFi ======================
extern const char *WIFI_SSID;
extern const char *WIFI_PASS;

// ====================== MQTT ======================
extern const char *MQTT_SERVER;
extern const int MQTT_PORT;
extern const char *MQTT_USER;
extern const char *MQTT_PASS;
extern const char *MQTT_TOPIC_SENSOR;

// ====================== Timing ======================
extern const unsigned long PUBLISH_INTERVAL_MS;

// ====================== Hardware ======================
extern const int SOLENOID_PIN;
extern const int BUZZER_PIN;
extern const int STATUS_LED_PIN;
extern int TURBIDITY_SENSOR_PIN;
extern const int PH_SENSOR_PIN;
extern const int TDS_SENSOR_PIN;
extern const int TEMP_SENSOR_PIN;

// ====================== Thresholds ======================
extern const float PH_MIN;
extern const float PH_MAX;
extern const float TURBIDITY_MAX;
extern const int TDS_MAX;
extern const int EC_MAX;
extern const float DIVIDER_SCALE;
extern const float TURBIDITY_K; // Calibration constant for turbidity sensor (to be determined experimentally)

#endif