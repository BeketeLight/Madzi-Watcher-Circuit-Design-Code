#include "sensormanager.h"

SensorManager::SensorManager()
{
    randomSeed(millis());
}

// void SensorManager::begin()
// {
//     // Replace analogReadResolution/Attenuation with this:
//     pinMode(turbidityPin, INPUT);
// }

WaterQualityReading SensorManager::readAll()
{
    WaterQualityReading r;

    // Use strncpy for fixed-size buffers
    strncpy(r.deviceId, "MADZI_ESP32_T01", sizeof(r.deviceId));
    strncpy(r.district, "Zomba", sizeof(r.district));
    strncpy(r.treatmentPlantId, "TP001", sizeof(r.treatmentPlantId));

    r.turbidity = readTurbidity(turbidityPin);
    r.pH = readPH(pHPin);
    r.tds = readTDS(tdsPin);
    r.electricalConductivity = readEC(ecPin);
    r.waterQualityIndex = calculateWQI(r.turbidity, r.pH, r.tds, r.electricalConductivity);
    r.anomalyDetected = (r.turbidity > 5.0);
    r.electricalConductivityVioleted = (r.electricalConductivity > 750.0);
    r.pHVioleted = (r.pH < 6.5 || r.pH > 8.5);
    r.tdsVioleted = (r.tds > 500);
    r.turbidityVioleted = (r.turbidity > 5.0);

    return r;
}

float SensorManager::readTurbidity(int &turbidityPin)
{
    // Set ADC resolution to 12 bits (0–4095)
    analogReadResolution(12);

    // Set attenuation for ~0–3.3V full range (recommended for most sensors)
    // Options: ADC_0db (~1.1V), ADC_2_5db, ADC_6db, ADC_11db (~3.3V)
    analogSetAttenuation(ADC_11db);

    // Read raw ADC value
    int raw = analogRead(turbidityPin);

    // Convert to voltage (assuming 3.1V reference and 12-bit)
    float rawVoltage = raw * (3.1 / 4095.0);

    // Correct scaling (important!)
    float voltage = rawVoltage * (3.7 / 3.1);
    Serial.print("Corrected Voltage: ");
    Serial.println(voltage);

    // Convert to NTU
    float turbidity = (-54.7 * voltage) + 203.0; // Example linear conversion (calibrate with your sensor)
    Serial.print("Equivalent turbidity: ");
    Serial.println(turbidity);

    // Clamp values
    if (turbidity < 0)
        turbidity = 0;

    return turbidity;
}
float SensorManager::readPH(int &pHPin)
{
    return analogRead(pHPin) / 100.0 * 14.0; // Map ADC to pH range (0-14)
}
int SensorManager::readTDS(int &tdsPin)
{
    return analogRead(tdsPin) / 2;
}
float SensorManager::readEC(int &ecPin)
{
    return analogRead(ecPin) / 10.0;
}

// ==================== WQI CALCULATION BASED ON BIS 10500:2012 METHODOLOGY ====================
float SensorManager::calculateWQI(float &turbidity, float &pH, float &tds, float &ec)
{
    // ==================== STANDARD PERMISSIBLE VALUES (S_n) ====================
    // From Table 7.2 + BIS 10500:2012 (consistent with the document's methodology)
    const float S_turbidity = 5.0f; // NTU (permissible limit for drinking water)
    // const float S_pH = 8.5f;        // from Table 7.2 (ISI)
    // const float S_tds = 500.0f;     // from Table 7.2 (IS)
    // const float S_ec = 750.0f;      // µS/cm (desirable limit used in Indian WQI studies)

    // const float V_id_pH = 7.0f;    // from document (pH ideal value)
    const float V_id_other = 0.0f; // from document (all other parameters)

    // ==================== STEP 1: Calculate constant k (Equation 7.4) ====================
    float sum_inv_S = (1.0f / S_turbidity); // +
                                            //   (1.0f / S_pH) +
                                            //   (1.0f / S_tds) +
                                            //   (1.0f / S_ec);

    float k = 1.0f / sum_inv_S; // k ≈ 0.0018 with these 4 parameters (document uses k=0.286 for 8 parameters)

    // ==================== STEP 2: Unit weights W_n = k / S_n (Equation 7.3) ====================
    float W_turb = k / S_turbidity;
    // float W_pH = k / S_pH;
    // float W_tds = k / S_tds;
    // float W_ec = k / S_ec;

    // ==================== STEP 3: Quality ratings q_n (Equation 7.2) ====================
    float q_turb = ((turbidity - V_id_other) / (S_turbidity - V_id_other)) * 100.0f;
    // float q_pH = ((pH - V_id_pH) / (S_pH - V_id_pH)) * 100.0f;
    // float q_tds = ((static_cast<float>(tds) - V_id_other) / (S_tds - V_id_other)) * 100.0f;
    // float q_ec = ((ec - V_id_other) / (S_ec - V_id_other)) * 100.0f;

    // ==================== STEP 4: Water Quality Index (Equation 7.1) ====================
    // WQI = Σ (q_n × W_n) / Σ W_n
    // (Σ W_n is always exactly 1.0 with this method)
    float wqi = (q_turb * W_turb); // +
                                   // (q_pH * W_pH) +
                                   // (q_tds * W_tds) +
                                   // (q_ec * W_ec);

    // WQI can be < 0 or > 100 (exactly as in the document and Table 7.1)
    Serial.println("Calculated WQI: " + String(wqi));
    return wqi;
}