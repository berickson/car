#include "FollowMode.h"
#include "Esc.h"
#include "Ping.h"

extern Esc esc;
extern Servo2 speed;
extern Servo2 steering;
extern Ping ping;


Esc::eSpeedCommand speed_for_ping_inches(double inches) {
  if(inches ==0) return Esc::speed_neutral;
  // get closer if far
  if (inches > 40.)
    return Esc::speed_forward;
  // back up if too close
  if (inches < 20.)
    return Esc::speed_reverse;

  return Esc::speed_neutral;
}


void FollowMode::begin() {
  steering.writeMicroseconds(1500);
  speed.writeMicroseconds(1500);

}

void FollowMode::end() {
  steering.writeMicroseconds(1500);
  speed.writeMicroseconds(1500);
}

void FollowMode::execute() {
  double inches = ping.inches();
  steering.writeMicroseconds(1500);
  esc.set_command(speed_for_ping_inches(inches));
  esc.execute();
}

