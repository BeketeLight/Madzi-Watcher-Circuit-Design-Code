#include "waterqualitycontroller.h"

WaterQualityController::WaterQualityController(SolenoidValve &valve) : _valve(valve)
{
    emailManager.begin();
}

void WaterQualityController::process(const WaterQualityReading &reading, Buzzer &buzzer)
{
    bool violation = reading.anomalyDetected;

    if (violation)
    {
        anomalyCount++;
        normalCount = 0;

        Serial.print("Anomaly detected: WQI=");
        Serial.print(anomalyCount);

        // Close only after 5 consecutive anomalies
        if ((anomalyCount >= 5 && !valveClosed) || (valveClosed && anomalyCount >= 5))
        {
            _valve.close();
            valveClosed = true;
            if (emailsendCount == 0 || emailsendCount == 10)
            {
                emailsendCount++;
                emailManager.sendAlert("Water Quality Alert", "Warning: Anomaly detected in water quality readings. Immediate attention required.   Device ID: " + String(reading.deviceId) + "\nDistrict: " + String(reading.district) + "\nTreatment Plant: " + String(reading.treatmentPlantId) + "\nTurbidity: " + String(reading.turbidity) + "\npH: " + String(reading.pH) + "\nTDS: " + String(reading.tds) + "\nEC: " + String(reading.electricalConductivity) + "\nWQI: " + String(reading.waterQualityIndex));
            }
            emailsendCount++;

            // buzzer.alert(); // Sound alarm on anomaly

            updateStatusLED(violation);
            Serial.println("ANOMALY DETECTED → Valve CLOSED");
        }
    }
    else
    {
        normalCount++;
        anomalyCount = 0;
        emailsendCount = 0;

        // Reopen only after 5 consecutive NORMAL readings
        if (normalCount >= 5 && valveClosed)
        {
            _valve.open();
            valveClosed = false;

            updateStatusLED(violation);
            Serial.println("SYSTEM NORMAL → Valve OPENED");
        }
    }
}

void WaterQualityController::handleCommand(const String &topic, const String &message, Buzzer &buzzer, ConfigManager &configManager)
{
    if (topic == "waterquality/commands")
    {
        if (message == "poweroff")
        {
            Serial.println("Entering deep sleep in 10 seconds...");

            buzzer.alert(); // optional feedback

            delay(500); // let serial + buzzer finish

            //  SET WAKE-UP FIRST
            esp_sleep_enable_timer_wakeup(10 * 1000000ULL); // 10 seconds

            Serial.println("Going to sleep now...");
            delay(100);

            esp_deep_sleep_start(); //  LAST CALL (never returns)
        }

        if (message == "turnon")
        {
            Serial.println("Turning ON system");
            state = SYSTEM_ON;
            buzzer.beep(); // Sound alarm on manual startup
            _valve.open();
        }
        else if (message == "turnoff")
        {
            Serial.println("Turning OFF system");
            state = SYSTEM_OFF;
            buzzer.beep(); // Sound alarm on manual shutdown
            _valve.close();
        }
    }
    if (topic == "waterquality/config")
    {
        Serial.println("Updating config from MQTT...");
        configManager.updateFromJson(message);
    }
}

SystemState WaterQualityController::getState() const
{
    return state;
}
