#pragma once
#include "RxEvent.h"

class EventQueue {
public:
  static const int size = 10;

  RxEvent events[size];

  void add(RxEvent new_event);
  bool matches(const RxEvent * pattern, int count);
};
