#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
    public:
        Buzzer(int pin);
        void begin();
        void beep(int durationMs = 100,int frequency = 2000);
        void alert(); //for recurring alarm patterns
        void stop();

    private:
        int _pin;
};

#endif
