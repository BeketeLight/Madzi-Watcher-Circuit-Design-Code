#ifndef SOLENOIDVALVE_H
#define SOLENOIDVALVE_H

class SolenoidValve
{

public:
    SolenoidValve(int pin);
    void begin();
    void open();
    void close();
    bool isOpen() const;

private:
    int _pin = 12;
    bool _state;
};
#endif // SOLENOIDVALVE_H
