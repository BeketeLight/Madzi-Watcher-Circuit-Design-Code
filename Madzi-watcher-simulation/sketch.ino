const int buzzerPin = 2;
const int ecPin = 35;
const int phPin = 34;
const int tdsPin = 33;
const int turbPin = 32;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);   // Buzzer off initially
  
  Serial.println("=== Water Quality Monitoring System ===");
  Serial.println("Buzzer alerts enabled (WQI + pH violation)");
  Serial.println("-------------------------------------");
}

void loop() {
  // ==================== READ RAW VALUES ====================
  int rawTurb = analogRead(turbPin);   // Pot1
  int rawPH   = analogRead(phPin);   // Pot2
  int rawTDS  = analogRead(tdsPin);   // Pot3
  int rawEC   = analogRead(ecPin);   // Pot4

  // ==================== CONVERT TO SENSOR VALUES ====================
   float turbidity = map(rawTurb, 0, 4095, 0, 10);
  float pH        = 4.0 + (rawPH * 10.0 / 4095.0);
  float tds       = map(rawTDS, 0, 4095, 0, 1000);
  float ec        = map(rawEC, 0, 4095, 0, 1500);

  // Clamp values
  turbidity = constrain(turbidity, 0.0, 100.0);
  pH        = constrain(pH, 0.0, 14.0);
  tds       = constrain(tds, 0, 1200);
  ec        = constrain(ec, 0.0, 2000.0);

  // ==================== CALCULATE WQI ====================
  float wqi = calculateWQI(turbidity, pH, tds, ec);

  // ==================== CHECK VIOLATIONS ====================
  bool pH_Violated = (pH < 6.5 || pH > 8.5);
  bool WQI_Violated = (wqi > 50);        // Poor or Very Poor

  bool turbidity_Violated = (turbidity > 5);
  bool electricalC_Violated = (ec > 1000);
  bool tds_Violated = (tds > 600);    // Poor or Very Poor

  // ==================== PRINT RESULTS ====================
   Serial.println("\n--- Water Quality Parameters ---");
  Serial.print("Turbidity : "); Serial.print(turbidity, 1); Serial.println(" NTU");
    Serial.print("   ("); Serial.print(turbidity_Violated ? "VIOLATED" : "OK"); Serial.println(")");
  Serial.print("pH        : "); Serial.print(pH, 2);
  Serial.print("   ("); Serial.print(pH_Violated ? "VIOLATED" : "OK"); Serial.println(")");
  
  Serial.print("TDS       : "); Serial.print(tds, 2); Serial.println(" ppm");
    Serial.print("   ("); Serial.print(tds_Violated ? "VIOLATED" : "OK"); Serial.println(")");
  Serial.print("EC        : "); Serial.print(ec, 1); Serial.println(" µS/cm");
  Serial.print("   ("); Serial.print(electricalC_Violated ? "VIOLATED" : "OK"); Serial.println(")");
  Serial.print("WQI       : "); Serial.print(wqi, 2);
  
  if (wqi <= 25) {
    Serial.println("  → Excellent");
  } else if (wqi <= 50) {
    Serial.println("  → Good");
  } else if (wqi <= 70) {
    Serial.println("  → Fair");
  } else if (wqi <= 90) {
    Serial.println("  → Poor");
  } else {
    Serial.println("  → Very Poor / Unsuitable for Drinking");
  }

  // ==================== BUZZER ALERT ====================
  if (pH_Violated || WQI_Violated || turbidity_Violated || tds_Violated) {
    alertBuzzer(pH_Violated,turbidity_Violated,tds_Violated, WQI_Violated);
  } else {
    digitalWrite(buzzerPin, LOW);   // Turn off buzzer
  }

  delay(1500);
}

// ==================== BUZZER ALERT FUNCTION ====================
void alertBuzzer(bool pH_Violated,bool turbidity_Violated,bool tds_Violated, bool WQI_Violated) {
  if (pH_Violated || WQI_Violated || turbidity_Violated || tds_Violated ) {
    // Both violated → Fast continuous beep
    tone(buzzerPin, 1500, 300);
    delay(400);
    tone(buzzerPin, 1000, 300);
  } 
  else if (pH_Violated) {
    // Only pH violated → Short beeps
    tone(buzzerPin, 2000, 200);
    delay(300);
    tone(buzzerPin, 2000, 200);
  } 
  else if (WQI_Violated) {
    // Only WQI violated → Long beep
    tone(buzzerPin, 1200, 600);
  }
}


// }
float calculateWQI(float turbidity, float pH, float tds, float ec) {
  const float W_turb = 0.28;
  const float W_pH   = 0.35;
  const float W_tds  = 0.19;
  const float W_ec   = 0.18;

  // Sub-indices
  float q_turb = constrain((turbidity / 5.0) * 100.0, 0, 120);
  
  float q_pH;
  if (pH >= 6.5 && pH <= 8.5) {
    q_pH = abs(pH - 7.0) * 18.0;           // Small penalty near neutral
  } else if (pH < 6.5) {
    q_pH = 65 + (6.5 - pH) * 40;           // Heavy penalty for acidic
  } else {
    q_pH = 65 + (pH - 8.5) * 35;           // Penalty for alkaline
  }

  float q_tds = constrain((tds / 500.0) * 100.0, 0, 120);
  float q_ec  = constrain((ec / 750.0) * 100.0, 0, 120);

  // Normal WQI calculation
  float wqi = (q_turb * W_turb) + (q_pH * W_pH) + (q_tds * W_tds) + (q_ec * W_ec);

  // === IMPORTANT FIX: pH OVERRIDE ===
  if (pH < 6.0 || pH > 9.0) {
    wqi = (wqi > 75.0f)? wqi: 75.0f;    // Force at least "Poor" if pH is dangerous
  }
  else if (pH < 6.5 || pH > 8.5) {
    wqi = (wqi > 55.0f)? wqi: 55.0f;     // Force at least "Fair" for mild violation
  }

  return constrain(wqi, 0.0, 120.0);
}