#include "Arduino.h"

// examples
//    forward: AcBaCbAcBaCbAcBaCbAcBaCbAc
//    reverse: AbCaBcAbCaBcAbCaBcAbCaBcAbC

class MotorEncoder
{

  public:
    const int pin_a;
    const int pin_b;
    const int pin_c;

    MotorEncoder(const int pin_a, int pin_b, int pin_c) : pin_a(pin_a),
                                                          pin_b(pin_b),
                                                          pin_c(pin_c)
    {
    }
    const bool trace_transitions = false;

    unsigned long a_count = 0;
    unsigned long b_count = 0;
    unsigned long c_count = 0;
    unsigned long last_change_us = 0;

    long odometer = 0;

    bool a, b, c;
    void on_a_change()
    {
        a = digitalRead(pin_a);
        last_change_us = micros();
        ++a_count;
        if (a)
        {
            if (trace_transitions)
                Serial.print('A');
            if (c)
            {
                ++odometer;
            }
            else
            {
                --odometer;
            }
        }
        else
        {
            if (b)
            {
                ++odometer;
            }
            else
            {
                --odometer;
            }
            if (trace_transitions)
                Serial.print('a');
        }
    }

    void on_b_change()
    {
        b = digitalRead(pin_b);
        last_change_us = micros();        
        ++b_count;
        if (b)
        {
            if (trace_transitions)
                Serial.print('B');
            if (a)
            {
                ++odometer;
            }
            else
            {
                --odometer;
            }
        }
        else
        {
            if (c)
            {
                ++odometer;
            }
            else
            {
                --odometer;
            }
            if (trace_transitions)
                Serial.print('b');
        }
    }

    void on_c_change()
    {
        c = digitalRead(pin_c);
        last_change_us = micros();
        ++c_count;
        if (c)
        {
            if (trace_transitions)
                Serial.print('C');
            if (b)
            {
                ++odometer;
            }
            else
            {
                --odometer;
            }
        }
        else
        {
            if (trace_transitions)
                Serial.print('c');
            if (a)
            {
                ++odometer;
            }
            else
            {
                --odometer;
            }
        }
    }
};