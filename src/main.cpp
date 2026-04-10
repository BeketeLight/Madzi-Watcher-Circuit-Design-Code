#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "config.h"
#include "actuators/statusled.h"
#include "actuators/solenoidvalve.h"
#include "actuators/buzzer.h"
#include "data/waterqualityreading.h"
#include "sensors/sensormanager.h"
#include "controller/waterqualitycontroller.h"
#include "utils/mqttmanager.h"
#include "utils/wifimanager.h"

// Global objects
static SensorManager sensors;
static SolenoidValve valve(SOLENOID_PIN);
static WaterQualityController controller(valve);
static WiFiManager wifiManager;
static MqttManager mqttManager;
static Buzzer buzzer(BUZZER_PIN); // Buzzer on pin 8

// Queue: sensor task → network task
static QueueHandle_t readingQueue = nullptr;

// Forward declarations
static void sensorTask(void *pvParameters);
static void networkTask(void *pvParameters);

void setup()
{
    Serial.begin(115200);
    delay(1000); // give serial time to settle

    Serial.println("\n=== Madzi Watcher starting ===\n");

    // Initialize hardware & classes
    initStatusLED();
    buzzer.begin();
    buzzer.beep(200, 1500); // startup beep
    valve.begin();
    sensors.begin();

    // Create queue (buffer up to 5 readings if network is slow/offline)
    readingQueue = xQueueCreate(5, sizeof(WaterQualityReading));
    if (readingQueue == nullptr)
    {
        Serial.println("ERROR: Failed to create reading queue!");
        setLED(255, 0, 0); // red = critical failure
        while (true)
            vTaskDelay(1000);
    }

    // Start WiFi (non-blocking — it begins the connection attempt)
    wifiManager.begin();
    wifiManager.connect(); // start first attempt

    // Initialize MQTT (sets server, etc. — actual connect happens in network task)
    mqttManager.begin();

    // ────────────────────────────────────────────────
    // Create two tasks
    // ────────────────────────────────────────────────

    // Sensor + actuation task – high priority, pinned to core 1
    xTaskCreatePinnedToCore(
        sensorTask,   // function
        "SensorTask", // name
        8192,         // stack size (words)
        nullptr,      // parameters
        2,            // priority (higher = more important)
        nullptr,      // task handle (not used)
        1             // core 1
    );

    // Network task – lower priority, pinned to core 0
    xTaskCreatePinnedToCore(
        networkTask,
        "NetworkTask",
        12288, // bigger stack for JSON & network
        nullptr,
        1,
        nullptr,
        0 // core 0
    );

    Serial.println("Tasks created – system running");
    // Arduino loop task is no longer needed
}

void loop()
{
    // Empty – everything is in FreeRTOS tasks
    vTaskDelete(NULL); // self-delete the default loop task
}

// ────────────────────────────────────────────────
// Sensor + Actuation Task (never blocked by network)
// ────────────────────────────────────────────────
static void sensorTask(void *pvParameters)
{
    (void)pvParameters;

    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(PUBLISH_INTERVAL_MS); // e.g. 30000 ms = 30s

    while (true)
    {
        Serial.println("\nReading sensors...");

        // Read all sensors
        WaterQualityReading reading = sensors.readAll();

        // Process logic → valve, alarms, status LED, etc.
        controller.process(reading, buzzer);

        // Send latest reading to network task (non-blocking)
        BaseType_t sent = xQueueSend(readingQueue, &reading, 0);
        if (sent != pdTRUE)
        {
            // Queue full → oldest reading dropped (we keep newest)
            Serial.println("Warning: reading queue full → dropping oldest");
        }

        // Wait for next cycle (precise timing)
        vTaskDelayUntil(&lastWakeTime, frequency);
    }
}

// ────────────────────────────────────────────────
// Network Task (handles WiFi + MQTT + publishing)
// ────────────────────────────────────────────────
static void networkTask(void *pvParameters)
{
    (void)pvParameters;

    while (true)
    {
        // Keep WiFi alive / reconnecting
        wifiManager.update();

        // Keep MQTT alive / reconnecting / process incoming
        mqttManager.loop();

        // If we have a new reading in queue → try to publish
        WaterQualityReading reading;
        if (xQueueReceive(readingQueue, &reading, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            // Only attempt publish if WiFi is up (MQTT will check connection too)
            if (wifiManager.isConnected())
            {
                bool success = mqttManager.publish(reading);

                if (success)
                {
                    Serial.println("→ Published reading to MQTT");
                }
                else

                {
                    Serial.println("Publish failed (will retry next cycle)");
                    // Optional: re-queue if you want guaranteed delivery (but be careful with memory)
                    // xQueueSendToFront(readingQueue, &reading, 0);
                }
            }
            else
            {
                Serial.println("WiFi down → cannot publish yet");
                // reading is dropped here — or re-queue if critical
            }
        }

        // Give CPU back – run network logic ~10–20 times per second
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
