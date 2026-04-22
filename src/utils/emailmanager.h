#ifndef EMAIL_MANAGER_H
#define EMAIL_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#define ENABLE_SMTP
#include <ReadyMail.h>

class EmailManager
{
public:
    EmailManager();
    void begin();
    bool sendAlert(const String &subject, const String &message);

private:
    WiFiClientSecure ssl_client;
    SMTPClient smtp;
    static void statusCallback(SMTPStatus status);
};

#endif