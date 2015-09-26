// fake file to make compile work
#include <iostream>

void tone(int pin, int note, int duration) {
  std::cout << "tone(" << pin << ", " << note << ", " << duration << ");" << std::endl;
  //throw("This command should not be used");
}


void tone(int pin, int note) {
  std::cout << "tone(" << pin << ", " << note <<");" << std::endl;
}

void noTone(int pin) {
  std::cout << "notone(" << pin << ");" << std::endl;
}

void delay(int ms) {
  std::cout << "delay(" << ms << ");" << std::endl;
}
unsigned long g_millis = 0;
unsigned long millis() {
  return g_millis;
}
