#pragma once

class Beeper {
public:
  int pin;

  // hz of notes
  const int note_c5 = 523;
  const int note_d5 = 587;
  const int note_e5 = 659;
  const int note_f5 = 698;
  const int note_g5 = 784;
  const int note_a5 = 880;
  const int note_b5 = 988;

  // duration of notes
  const unsigned long note_ms = 500;
  const unsigned long gap_ms = 10;

  void attach(int pin);
  void play(int note);
  void beep(int note);
  void beep_ascending();
  void beep_descending();
  void beep_nbc();
  void beep_warble();
};
