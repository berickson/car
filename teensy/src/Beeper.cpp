#include "Beeper.h"
#include <Arduino.h>

void Beeper::attach(int pin) {
  this->pin = pin;
}

void Beeper::beep(int note) {
  reset();
  add_note(note);
  enter();
}

void Beeper::beep_ascending() {
  reset();
  add_note(note_c5);
  add_note(note_e5);
  add_note(note_g5);
  enter();
}

void Beeper::beep_descending() {
  reset();
  add_note(note_g5);
  add_note(note_e5);
  add_note(note_c5);
  enter();
}

void Beeper::beep_nbc() {
  reset();
  add_note(note_c5);
  add_note(note_a5);
  add_note(note_f5);
  enter();
}

void Beeper::enter() {
  beep_sequence.enter();
}

void Beeper::beep_warble() {
  reset();
  add_note(note_a5);
  add_note(note_f5);
  add_note(note_a5);
  add_note(note_f5);
  add_note(note_a5);
  add_note(note_f5);
  add_note(note_a5);
  add_note(note_f5);
  enter();
}

void Beeper::add_note(int note) {

  // fail silently if we can't add more
  if(beep_count + 1 >= max_beeps) {
    return;
  }
  beeps[beep_count].init(pin, note, note_ms);
  beep_sequence.add_task(&beeps[beep_count]);
  beep_count++;
}

void Beeper::reset() {
  beep_sequence.init();
  beep_count = 0;
}

void Beeper::execute() {
  beep_sequence.execute();
}
