// wificlient.cpp   (or wifimanager.cpp)
#include "wifimanager.h"
#include "config.h" // ← should contain #define WIFI_SSID  "yourssid"  and WIFI_PASS

WiFiManager::WiFiManager()
{
    // Optional: WiFi.mode(WIFI_STA);  // usually already default
}

WiFiManager::~WiFiManager()
{
    disconnect();
}

void WiFiManager::begin()
{
    initStatusLED(); // your function
    WiFi.mode(WIFI_STA);
    // Optional: force good DNS servers right away
    // WiFi.config(localIP, gateway, subnet, IPAddress(8,8,8,8), IPAddress(1,1,1,1));
}

bool WiFiManager::connect()
{
    if (isConnected())
    {
        setLED(0, 255, 0); // green = already good
        return true;
    }

    if (retryCount >= MAX_RETRIES)
    {
        Serial.println("WiFi max retries reached → giving up until reset or manual call");
        setLED(255, 0, 0); // red = permanent failure
        return false;
    }

    attemptConnection();
    return true; // attempt started (not necessarily connected yet)
}

void WiFiManager::attemptConnection()
{
    unsigned long now = millis();

    if ((now - lastAttemptMs) < backoffMs)
    {
        return; // still in backoff
    }

    lastAttemptMs = now;

    Serial.print("WiFi connecting... attempt #");
    Serial.print(retryCount + 1);
    Serial.print(" (backoff ");
    Serial.print(backoffMs / 1000);
    Serial.println("s)");

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Visual feedback: blinking yellow/orange while trying
    setLED(255, 165, 0); // orange = connecting

    retryCount++;
    increaseBackoff();
}

void WiFiManager::update()
{
    if (isConnected())
    {
        if (retryCount > 0)
        { // just reconnected
            Serial.println("\nWiFi connected ✓  IP: " + WiFi.localIP().toString());
            setLED(0, 255, 0); // solid green
            resetBackoff();
        }
        return;
    }

    // Not connected → keep trying via backoff
    attemptConnection();
}

bool WiFiManager::isConnected() const
{
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::disconnect()
{
    WiFi.disconnect(true); // true = power off radio to save energy
    WiFi.mode(WIFI_OFF);
    setLED(255, 0, 0); // red = disconnected
    resetBackoff();
}

void WiFiManager::resetBackoff()
{
    retryCount = 0;
    backoffMs = 2000; // reset to fast retry
}

void WiFiManager::increaseBackoff()
{
    // Exponential: 2s → 4s → 8s → 16s → 32s → 60s → cap at 120s
    unsigned long next = backoffMs * 2;
    if (next > MAX_BACKOFF_MS)
        next = MAX_BACKOFF_MS;
    backoffMs = next;
}