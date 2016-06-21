#include "driver.h"
#include "car.h"
#include "math.h"
#include "car_ui.h"

Driver::Driver(Car & car, CarUI ui)
  : car(car), ui(ui)
{

}
void Driver::play_route(Route & route) {

  // we will set error text if something goes wrong
  string error_text = "";

  WorkQueue<Dynamics> queue;
  car.add_listener(&queue);
  ui.clear();
  ui.print("press any key to abort");
  ui.refresh();
  try {
    car.set_rc_mode();
    while(ui.getkey() == -1) {
      Dynamics d;
      if(!queue.try_pop(d,1000)) {
        throw (string) "timed out waiting to read dynamics";
      }
      auto p_front = car.get_front_position();
      auto p_rear = car.get_rear_position();
      double v = car.get_velocity();
      route.set_position(p_front, p_rear, v);

      Angle steering_angle = steering_angle_by_look_ahead(route);

      unsigned str = car.steering_for_angle(steering_angle);
      unsigned esc = car.esc_for_velocity(route.get_velocity());

      if(route.done && fabs(car.get_velocity()) < 0.05)
        break;
      car.set_esc_and_str(esc, str);
    }
  } catch (string s) {
    error_text = s;
  } catch (...) {
    error_text = "unknown exception caught during play_route";
  }

  car.set_esc_and_str(1500,1500);
  car.set_manual_mode();
  car.remove_listener(&queue);
  if(error_text.size()) {
    ui.clear();
    ui.print((string) "Error during play route: \n"+error_text);
    ui.refresh();
    while(ui.getkey() == -1);
  } else {
    ui.clear();
    ui.print((string) "completed playback without error");
    ui.refresh();
    while(ui.getkey() == -1);
  }


}

Angle Driver::steering_angle_by_cte(Route &route) {
  double cte = route.cte;
  Angle segment_heading = route.heading();
  Angle car_heading = car.get_heading();
  Angle heading_fix = segment_heading - car_heading;
  double cte_fix = -80 * cte;
  if(car.get_velocity() < 0) {
    heading_fix = -heading_fix;
    cte_fix = -2*cte_fix;
  }
  Angle steering_angle = heading_fix + Angle::degrees(cte_fix);
  steering_angle.standardize();
  return steering_angle;
}

Angle Driver::steering_angle_by_look_ahead(Route &route)
{
  double v = car.get_velocity();
  if(v<0)
    return steering_angle_by_cte(route);
  Point p = route.get_position_ahead(d_ahead + v*t_ahead);
  Point car_p = car.get_front_position();
  double d = distance(car_p,p);
  Angle heading = car.get_heading();
  Angle desired = Angle::radians(atan2(p.y-car_p.y,p.x-car_p.x));
  Angle delta = desired-heading;
  double ahead = d*cos(delta.radians());
  double left = d*sin(delta.radians());
  auto arc = car.ackerman.arc_to_relative_location(ahead, left);
  return Angle::radians(arc.steer_radians);
}
