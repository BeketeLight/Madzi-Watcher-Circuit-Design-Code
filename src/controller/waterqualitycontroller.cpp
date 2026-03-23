#include "waterqualitycontroller.h"

WaterQualityController::WaterQualityController(SolenoidValve &valve) : _valve(valve) {}

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

            buzzer.alert(); // Sound alarm on anomaly

            updateStatusLED(violation);
            Serial.println("ANOMALY DETECTED → Valve CLOSED");
        }
    }
    else
    {
        normalCount++;
        anomalyCount = 0;

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
