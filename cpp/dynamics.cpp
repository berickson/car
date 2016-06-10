#include "dynamics.h"
#include <string.h> // for memset of all things
#include <iostream>

#include "split.h"

Dynamics::Dynamics() {
   memset(this,0,sizeof(*this));
}

string Dynamics::to_string() {
  stringstream ss;

  ss << "str:" << str << endl
     << "esc:" << esc << endl

     << "ax:" << ax << endl
     << "ay:" << ay << endl
     << "az:" << az << endl

     << "spur_delta_us:" << spur_delta_us << endl
     << "spur_last_us:" << spur_last_us << endl
     << "spur_odo:" << spur_odo << endl

     << "ping_millimeters:" << ping_millimeters << endl

     << "odometer_front_left:" << odometer_front_left << endl
     << "odometer_front_left_last_us:" << odometer_front_left_last_us << endl

     << "odometer_front_right:" << odometer_front_right << endl
     << "odometer_front_right_last_us:" << odometer_front_right_last_us << endl

     << "odometer_back_left:" << odometer_back_left << endl
     << "odometer_back_left_last_us:" << odometer_back_left_last_us << endl

     << "odometer_back_right:" << odometer_back_right << endl
     << "odometer_back_right_last_us:" << odometer_back_right_last_us <<endl

     << "ms:" << ms << endl
     << "us:" << us << endl

     << "yaw:" << yaw << endl
     << "pitch:" << pitch << endl
     << "roll:" << roll << endl

     << "battery_voltage:" << battery_voltage << endl;
  return ss.str();
}

bool Dynamics::from_log_string(Dynamics & d, string &s) {
  auto fields = split(s,',');
  if(fields.size() != 39) {
    // usb_error_count++;
    return false;
  }
  if(fields[1] != "TD")
    return false;
//    self.datetime = dateutil.parser.parse(fields[0])

  try {
    d.str = stoi(fields[3]);
    d.esc = stoi(fields[5]);

    d.ax = stod(fields[7]);
    d.ay = stod(fields[8]);
    d.az = stod(fields[9]);

    d.spur_delta_us = stoi(fields[11]);
    d.spur_last_us = stoi(fields[12]);
    d.spur_odo = stoi(fields[14]);
    d.ping_millimeters = stoi(fields[16]);
    d.odometer_front_left =  stoi(fields[18]);
    d.odometer_front_left_last_us =  stoi(fields[19]);
    d.odometer_front_right = stoi(fields[21]);
    d.odometer_front_right_last_us = stoi(fields[22]);
    d.odometer_back_left = stoi(fields[24]);
    d.odometer_back_left_last_us = stoi(fields[25]);
    d.odometer_back_right = stoi(fields[27]);
    d.odometer_back_right_last_us = stoi(fields[28]);
    d.ms = stoi(fields[30]);
    d.us = stoi(fields[32]);
    d.yaw = Angle::degrees(stod(fields[34]));
    d.pitch = Angle::degrees(stod(fields[35]));
    d.roll = Angle::degrees(stod(fields[36]));
    d.battery_voltage = stod(fields[38]);
  } catch (...)
  {
    return false;
  }

  return true;

}


void test_dynamics() {
  Dynamics d;
  cout << "ax: " << d.ax << endl;

  string s = "2016-05-22 16:25:13.331951,TD,str,1478,esc,1487,aa,-0.02,0.04,0.08,spur_us,0,0,spur_odo,0,ping_mm,0,odo_fl,-3764,330458839,odo_fr,0,0,odo_bl,-87,334189373,odo_br,54,316680087,ms,334351,us,334351946,ypr,-114.40,8.73,-1.43,vbat,7.71";
  //string s = "0,TD,str,1451,esc,1492,aa,-0.07,0.07,0.09,spur_us,131800,420675479,spur_odo,687,ping_mm,0,odo_fl,-35,427389363,odo_fr,0,0,odo_bl,7TD,str,1451,esc,1491,aa,-0.08,0.05,0.08,spur_us,131800,420675479,spur_odo,687,ping_mm,0,odo_fl,-35,427389363,odo_fr,0,0,odo_bl,73070,427898122,odo_br,76178,540125596,ms,663100,us,663100483,ypr,16.33,7.23,-1.37,vbat,8.23";

  bool ok = Dynamics::from_log_string(d,s);
  cout << "ok:" << ok;
  cout << "log string: " << s << endl;
  cout << "from_log_string(s): " << d.to_string() << endl;
}
