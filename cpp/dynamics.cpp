#include "dynamics.h"
#include "work_queue.h"
#include <string.h> // for memset of all things
#include <iostream>
#include "system.h"
#include "string_utils.h"
#include "logger.h"
#include "../teensy/CarMessages.h"


Dynamics::Dynamics() {
   memset(this,0,sizeof(*this));
}

string Dynamics::to_string() {
  TraceDynamics td;
  td.number = 5;
  string s = td.label;

  stringstream ss;

  ss << "str:" << str << endl
     << "esc:" << esc << endl

     << "ax:" << ax << endl
     << "ay:" << ay << endl
     << "az:" << az << endl

     << "spur_last_us:" << spur_last_us << endl
     << "spur_odo:" << spur_odo << endl

     << "mode:" << control_mode << endl

     << "odo_fl_a:" << odo_fl_a << endl
     << "odo_fl_a_us:" << odo_fl_a_us << endl
     << "odo_fl_b:" << odo_fl_b << endl
     << "odo_fl_b_us:" << odo_fl_b_us << endl

     << "odo_fr_a:" << odo_fr_a << endl
     << "odo_fr_a_us:" << odo_fr_a_us << endl
     << "odo_fr_b:" << odo_fr_b << endl
     << "odo_fr_b_us:" << odo_fr_b_us << endl

     << "odo_bl_a:" << odo_bl_a << endl
     << "odo_bl_a_us:" << odo_bl_a_us << endl
     << "odo_bl_b:" << odo_bl_b << endl
     << "odo_bl_b_us:" << odo_bl_b_us << endl

     << "odo_br_a:" << odo_br_a << endl
     << "odo_br_a_us:" << odo_br_a_us << endl
     << "odo_br_b:" << odo_br_b << endl
     << "odo_br_b_us:" << odo_br_b_us << endl

     << "ms:" << ms << endl
     << "us:" << us << endl

     << "yaw:" << yaw.to_string() << endl
     << "pitch:" << pitch.to_string() << endl
     << "roll:" << roll.to_string() << endl

     << "battery_voltage:" << battery_voltage << endl;
  return ss.str();
}

std::string Dynamics::csv_field_headers() {
  return
     "timestamp,ms,us,"
     "str,esc,battery_voltage,"
     "odo_fl_a,odo_fl_a_us,odo_fl_b,odo_fl_b_us,"
     "odo_fr_a,odo_fr_a_us,odo_fr_b,odo_fr_b_us,"
     "odo_bl_a,odo_bl_a_us,odo_bl_b,odo_bl_b_us,"
     "odo_br_a,odo_br_a_us,odo_br_b,odo_br_b_us,"
     "odo_spur,odo_spur_us,"
     "yaw,pitch,roll,"
     "ax,ay,az,"
     "imu_calib";
}

std::string Dynamics::csv_fields() {
  stringstream ss;

  // "timestamp,ms,us,"
  ss << time_string(this->datetime) << ",";
  ss << ms << ",";
  ss << us << ",";

  // "str,esc,battery_voltage,"
  ss << str << ",";
  ss << esc << ",";
  ss << battery_voltage << ",";

  // "odo_fl,odo_fl_us,"
  ss << odo_fl_a << ",";
  ss << odo_fl_a_us << ",";
  ss << odo_fl_b << ",";
  ss << odo_fl_b_us << ",";

  // "odo_fr,odo_fr_us,"
  ss << odo_fr_a << ",";
  ss << odo_fr_a_us << ",";
  ss << odo_fr_b << ",";
  ss << odo_fr_b_us << ",";

  // "odo_fl,odo_fl_us,"
  ss << odo_bl_a << ",";
  ss << odo_bl_a_us << ",";
  ss << odo_bl_b << ",";
  ss << odo_bl_b_us << ",";

  // "odo_fr,odo_fr_us,"
  ss << odo_br_a << ",";
  ss << odo_br_a_us << ",";
  ss << odo_br_b << ",";
  ss << odo_br_b_us << ",";


  // "odo_spur,odo_spur_us,"
  ss << spur_odo << ",";
  ss << spur_last_us << ",";

  // "yaw,pitch,roll,"
  ss << yaw.degrees() << ",";
  ss << pitch.degrees() << ",";
  ss << roll.degrees() << ",";

  // "ax,ay,az,"
  ss << ax << ",";
  ss << ay << ",";
  ss << az << ",";

  // "imu_calib";
  ss << calibration_status;


  return ss.str();
}


bool Dynamics::from_log_string(Dynamics & d, const StampedString & s) {
  auto fields = split(s.message,',');
  if(fields.size() <2) {
    return false;
  }
  if(fields[0] != "TD")
    return false;
  size_t expected_count = 54;
  if(fields.size() != expected_count) {
    stringstream error;
    error << "wrong number of field in TD. Expected: " << expected_count << ", actual:" << fields.size() << ".";
    log_warning(error.str());
    //usb_error_count++;
    return false;
  }
  d.datetime = s.timestamp;

  try {
    //d.datetime = time_from_string(fields[0]);
    d.str = stoi(fields[2]);
    d.esc = stoi(fields[4]);

    d.ax = stod(fields[6]);
    d.ay = stod(fields[7]);
    d.az = stod(fields[8]);

    d.spur_last_us = stoul(fields[10]);
    // d.spur_delta_us = stoul(fields[11]); unused
    d.spur_odo = stoi(fields[13]);
    d.control_mode = fields[15][0];

    d.odo_fl_a =  stoi(fields[17]);
    d.odo_fl_a_us =  stoul(fields[18]);
    d.odo_fl_b =  stoi(fields[19]);
    d.odo_fl_b_us =  stoul(fields[20]);
    // d.odo_fl_ab_us =  stoul(fields[21]); // unused

    d.odo_fr_a =  stoi(fields[23]);
    d.odo_fr_a_us =  stoul(fields[24]);
    d.odo_fr_b =  stoi(fields[25]);
    d.odo_fr_b_us =  stoul(fields[26]);
    // d.odo_fr_ab_us =  stoul(fields[27]); // unused

    d.odo_bl_a =  stoi(fields[29]);
    d.odo_bl_a_us =  stoul(fields[30]);
    d.odo_bl_b =  stoi(fields[31]);
    d.odo_bl_b_us =  stoul(fields[32]);
    // d.odo_bl_ab_us =  stoul(fields[33]); // unused

    d.odo_br_a =  stoi(fields[35]);
    d.odo_br_a_us =  stoul(fields[36]);
    d.odo_br_b =  stoi(fields[37]);
    d.odo_br_b_us =  stoul(fields[38]);
    // d.odo_br_ab_us =  stoul(fields[39]); // unused

    d.ms = stoul(fields[41]);

    d.us = stoul(fields[43]);

    d.yaw = Angle::degrees(stod(fields[45]));
    d.pitch = Angle::degrees(stod(fields[46]));
    d.roll = Angle::degrees(stod(fields[47]));

    d.battery_voltage = stod(fields[49]);
    d.go = stod(fields[51])==1;
    d.mpu_temperature = stod(fields[53]);

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

  StampedString ss;
  ss.set_from_string(s);
  bool ok = Dynamics::from_log_string(d,ss);
  cout << "ok:" << ok;
  cout << "log string: " << s << endl;
  cout << "d.tostring(): " << d.to_string() << endl;
  Dynamics d2 = d;
  cout << "d2.tostring(): " << d2.to_string() << endl;

}
