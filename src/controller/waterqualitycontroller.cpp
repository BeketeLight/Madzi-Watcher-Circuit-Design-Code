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
