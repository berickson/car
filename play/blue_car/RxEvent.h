#pragma once

class RxEvent {
public:
  char speed;
  char steer;

  RxEvent():RxEvent('0','0'){}
  RxEvent(char steer, char speed);
  bool equals(RxEvent other);
  bool is_bad();
};
