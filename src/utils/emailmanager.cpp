#include "utils/emailmanager.h"
#include "config.h" // Assuming your credentials are here

EmailManager::EmailManager() : smtp(ssl_client) {}

void EmailManager::statusCallback(SMTPStatus status)
{
    Serial.printf("Email Status: %s\n", status.text.c_str());
}

void EmailManager::begin()
{
    ssl_client.setInsecure(); // Use for testing; ideally use a root CA for production
}

bool EmailManager::sendAlert(const String &subject, const String &message)
{

    Serial.print("message");
    Serial.println(message);
    Serial.print("subject");
    Serial.println(subject);
    if (WiFi.status() != WL_CONNECTED)
        return false;

    // Connect to SMTP server
    smtp.connect(SMTP_HOST, SMTP_PORT, statusCallback);

    if (smtp.isConnected())
    {
        smtp.authenticate(SMTP_USER, SMTP_PASS, readymail_auth_password);

        SMTPMessage msg;
        msg.headers.add(rfc822_from, "Madzi Watcher <" + String(SMTP_USER) + ">");
        msg.headers.add(rfc822_to, RECIPIENT_EMAIL);
        msg.headers.add(rfc822_subject, subject);

        msg.text.body(message);

        // Required for modern SMTP: Set timestamp via NTP
        msg.timestamp = time(nullptr);

        bool result = smtp.send(msg);
        Serial.print("message sent ?");
        Serial.println(result);
        smtp.stop(); // Disconnect after sending
        return result;
    }
    return false;
}