#pragma once
#include "Task.h"
#include "Beep.h"
#include "Sequence.h"

class Beeper : Task {
public:
  int pin;
  
  static const int max_beeps = 10;
  Beep beeps[max_beeps];
  int beep_count = 0;
  Sequence beep_sequence;
  
  // hz of notes
  const int note_c5 = 523;
  const int note_d5 = 587;
  const int note_e5 = 659;
  const int note_f5 = 698;
  const int note_g5 = 784;
  const int note_a5 = 880;
  const int note_b5 = 988;

  // duration of notes
  const unsigned long note_ms = 100;
  const unsigned long gap_ms = 10;

  void attach(int pin);
  void beep(int note);
  void beep_ascending();
  void beep_descending();
  void beep_nbc();
  void beep_warble();
  
  void reset();
  void add_note(int note);
  
  void begin();
  void execute();
  
};
