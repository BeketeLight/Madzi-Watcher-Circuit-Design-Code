// wificlient.h   (or rename to wifimanager.h)
#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include "../actuators/statusled.h" // your LED functions: initStatusLED(), setLED(r,g,b)

class WiFiManager
{ // ← better name, but you can keep WifiClient
public:
    WiFiManager();
    ~WiFiManager();

    void begin();   // call once in setup()
    bool connect(); // try once (non-blocking), returns if attempt started
    bool isConnected() const;
    void disconnect();
    void update(); // call frequently (in network task / loop) → handles retries + backoff

    // Optional: getters
    String getIP() const { return WiFi.localIP().toString(); }
    String getMAC() const { return WiFi.macAddress(); }

private:
    unsigned long lastAttemptMs = 0;
    unsigned long backoffMs = 2000; // start with 2 seconds
    int retryCount = 0;
    static const int MAX_RETRIES = 15;
    static const unsigned long MAX_BACKOFF_MS = 120000; // 2 minutes cap

    void attemptConnection();
    void resetBackoff();
    void increaseBackoff();

    
};

#endif