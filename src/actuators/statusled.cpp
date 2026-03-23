#include "statusled.h"

// Global NeoPixel object
Adafruit_NeoPixel np(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastBlinkTime = 0;
bool redToggle = false;

// ====================== HELPER FUNCTIONS ======================
void setLED(uint8_t r, uint8_t g, uint8_t b)
{
    np.setPixelColor(0, np.Color(r, g, b));
    np.show();
}

void blinkRed()
{
    if (millis() - lastBlinkTime > 600)
    { // slow alert blink
        redToggle = !redToggle;
        setLED(redToggle ? 255 : 0, 0, 0); // bright red / off
        lastBlinkTime = millis();
    }
}

// ====================== INIT ======================
void initStatusLED()
{
    np.begin();
    np.setBrightness(180); // visible but not blinding
    setLED(0, 0, 255);     // start with BLUE (WiFi not ready)
    Serial.println(" Status LED initialized → BLUE");
}

// ====================== MAIN UPDATE (call every loop) ======================
void updateStatusLED(bool &newstatus)
{
    bool violation = newstatus;

    // PRIORITY: Violation > WiFi status
    if (violation)
    {
        blinkRed();
        Serial.println(" SENSOR VIOLATION → Blinking RED");
    }
    else
    {
        setLED(0, 255, 0); // Solid GREEN = everything perfect
    }
}