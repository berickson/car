#include "Beeper.h"
#include "Arduino.h"

void Beeper::attach(int pin) {
  this->pin = pin;
}

void Beeper::play(int note) {
  tone(pin,note);
}

void Beeper::beep(int note) {
#if(PLAY_SOUNDS)
  tone(pin, note, note_ms);
  delay(note_ms + gap_ms);
#endif
}

void Beeper::beep_ascending() {
  beep(note_c5);
  beep(note_e5);
  beep(note_g5);
}

void Beeper::beep_descending() {
  beep(note_g5);
  beep(note_e5);
  beep(note_c5);
}

void Beeper::beep_nbc() {
  beep(note_c5);
  beep(note_a5);
  beep(note_f5);
}

void Beeper::beep_warble() {
  beep(note_a5);
  beep(note_f5);
  beep(note_a5);
  beep(note_f5);
  beep(note_a5);
  beep(note_f5);
  beep(note_a5);
  beep(note_f5);
}
