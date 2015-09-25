// fake file to make compile work
#include <iostream>

void tone(int pin, int note, int duration);
void tone(int pin, int note);
void noTone(int pin);
void delay(int ms);
extern unsigned long g_millis;
unsigned long millis();
