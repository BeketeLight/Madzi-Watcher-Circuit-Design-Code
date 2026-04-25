#include "config.h"

// ====================== WiFi ======================
const char *WIFI_SSID = "Light";    // ← CHANGE THIS
const char *WIFI_PASS = "99997014"; // ← CHANGE THIS

// ====================== MQTT ======================
const char *MQTT_SERVER = "192.168.43.82"; //"c73a854202c347a9bda6c26b8cc5a443.s1.eu.hivemq.cloud";
const int MQTT_PORT = 1883;                // 8883;
const char *MQTT_USER = "madziwatcher";
const char *MQTT_PASS = "madziWatcher001";
const char *MQTT_TOPIC_SENSOR = "waterquality/sensor";
const char *MQTT_TOPIC_COMMANDS = "waterquality/commands";
const char *MQTT_TOPIC_STATUS = "waterquality/status";
const char *MQTT_TOPIC_CONFIG = "waterquality/config";
const int MQTT_QOS = 1; // QoS level for publishing (0, 1, or 2)

// ====================== Timing ======================
const unsigned long PUBLISH_INTERVAL_MS = 5000;

// ====================== Hardware ======================
const int SOLENOID_PIN = 12;   // ← change if needed
const int BUZZER_PIN = 8;      // ← change if needed
const int STATUS_LED_PIN = 48; // ← change if needed (must support NeoPixel)
int TURBIDITY_SENSOR_PIN = 4;  // ADC1_3
const int PH_SENSOR_PIN = 5;   // ADC1_4
const int TDS_SENSOR_PIN = 6;  // ADC1_5
const int TEMP_SENSOR_PIN = 7; // ADC1_6

// =========EMAILS =========
const char *SMTP_HOST = "smtp.gmail.com";
const int SMTP_PORT = 465;                                    // 465 for SSL, 587 for STARTTLS
const char *SMTP_USER = "beketelight15@gmail.com";            // Your email address
const char *SMTP_PASS = "kyiu maiw tofl udxr";                // Your 16-character App Password
const char *RECIPIENT_EMAIL = "bsc-com-ne-07-21@unima.ac.mw"; // Recipient email address

// ====================== Thresholds ======================
const float PH_MIN = 6.5;
const float PH_MAX = 8.5;
const float TURBIDITY_MAX = 50.0;
const int TDS_MAX = 500;
const int EC_MAX = 20;
const float DIVIDER_SCALE = 1;         // To recover real sensor voltage from ADC reading (0-3.3V) to actual sensor output
const float TURBIDITY_K = 2714.426208; // Calibration constant for turbidity sensor (to be determined experimentally)