#include "dynamics.h"
#include <string.h> // for memset of all things
#include <iostream>
#include "system.h"
#include "split.h"
#include "logger.h"

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

     << "mode:" << control_mode << endl

     << "odometer_front_left_a:" << odometer_front_left_a << endl
     << "odometer_front_left_a_us:" << odometer_front_left_a_us << endl
     << "odometer_front_left_b:" << odometer_front_left_b << endl
     << "odometer_front_left_b_us:" << odometer_front_left_b_us << endl
     << "odometer_front_left_ab_us:" << odometer_front_left_ab_us << endl

     << "odometer_front_right_a:" << odometer_front_right_a << endl
     << "odometer_front_right_a_us:" << odometer_front_right_a_us << endl
     << "odometer_front_right_b:" << odometer_front_right_b << endl
     << "odometer_front_right_b_us:" << odometer_front_right_b_us << endl
     << "odometer_front_right_ab_us:" << odometer_front_right_ab_us << endl

     << "odometer_back_left_a:" << odometer_back_left_a << endl
     << "odometer_back_left_a_us:" << odometer_back_left_a_us << endl
     << "odometer_back_left_b:" << odometer_back_left_b << endl
     << "odometer_back_left_b_us:" << odometer_back_left_b_us << endl
     << "odometer_back_left_ab_us:" << odometer_back_left_ab_us << endl

     << "odometer_back_right_a:" << odometer_back_right_a << endl
     << "odometer_back_right_a_us:" << odometer_back_right_a_us << endl
     << "odometer_back_right_b:" << odometer_back_right_b << endl
     << "odometer_back_right_b_us:" << odometer_back_right_b_us << endl
     << "odometer_back_right_ab_us:" << odometer_back_right_ab_us << endl

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
     "odo_fl_a,odo_fl_a_us,odo_fl_b,odo_fl_b_us,odo_fl_ab_us,"
     "odo_fr_a,odo_fr_a_us,odo_fr_b,odo_fr_b_us,odo_fr_ab_us,"
     "odo_bl_a,odo_bl_a_us,odo_bl_b,odo_bl_b_us,odo_bl_ab_us,"
     "odo_br_a,odo_br_a_us,odo_br_b,odo_br_b_us,odo_br_ab_us,"
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
  ss << odometer_front_left_a << ",";
  ss << odometer_front_left_a_us << ",";
  ss << odometer_front_left_b << ",";
  ss << odometer_front_left_b_us << ",";
  ss << odometer_front_left_ab_us << ",";

  // "odo_fr,odo_fr_us,"
  ss << odometer_front_right_a << ",";
  ss << odometer_front_right_a_us << ",";
  ss << odometer_front_right_b << ",";
  ss << odometer_front_right_b_us << ",";
  ss << odometer_front_right_ab_us << ",";

  // "odo_fl,odo_fl_us,"
  ss << odometer_back_left_a << ",";
  ss << odometer_back_left_a_us << ",";
  ss << odometer_back_left_b << ",";
  ss << odometer_back_left_b_us << ",";
  ss << odometer_back_left_ab_us << ",";

  // "odo_fr,odo_fr_us,"
  ss << odometer_back_right_a << ",";
  ss << odometer_back_right_a_us << ",";
  ss << odometer_back_right_b << ",";
  ss << odometer_back_right_b_us << ",";
  ss << odometer_back_right_ab_us << ",";


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


bool Dynamics::from_log_string(Dynamics & d, string &s) {
  auto fields = split(s,',');
  if(fields.size() <2) {
    return false;
  }
  if(fields[1] != "TD")
    return false;
  if(fields.size() != 53) {
    stringstream error;
    error << "wrong number of field in TD. Expected: 53, actual:" << fields.size() << ".";
    log_warning(error.str());
    //usb_error_count++;
    return false;
  }
//    self.datetime = dateutil.parser.parse(fields[0])

  try {
    d.datetime = time_from_string(fields[0]);
    d.str = stoi(fields[3]);
    d.esc = stoi(fields[5]);

    d.ax = stod(fields[7]);
    d.ay = stod(fields[8]);
    d.az = stod(fields[9]);

    d.spur_delta_us = stoul(fields[11]);
    d.spur_last_us = stoul(fields[12]);
    d.spur_odo = stoi(fields[14]);
    d.control_mode = fields[16][0];

    d.odometer_front_left_a =  stoi(fields[18]);
    d.odometer_front_left_a_us =  stoul(fields[19]);
    d.odometer_front_left_b =  stoi(fields[20]);
    d.odometer_front_left_b_us =  stoul(fields[21]);
    d.odometer_front_left_ab_us =  stoul(fields[22]);

    d.odometer_front_right_a =  stoi(fields[24]);
    d.odometer_front_right_a_us =  stoul(fields[25]);
    d.odometer_front_right_b =  stoi(fields[26]);
    d.odometer_front_right_b_us =  stoul(fields[27]);
    d.odometer_front_right_ab_us =  stoul(fields[28]);

    d.odometer_back_left_a =  stoi(fields[30]);
    d.odometer_back_left_a_us =  stoul(fields[31]);
    d.odometer_back_left_b =  stoi(fields[32]);
    d.odometer_back_left_b_us =  stoul(fields[33]);
    d.odometer_back_left_ab_us =  stoul(fields[34]);

    d.odometer_back_right_a =  stoi(fields[36]);
    d.odometer_back_right_a_us =  stoul(fields[37]);
    d.odometer_back_right_b =  stoi(fields[38]);
    d.odometer_back_right_b_us =  stoul(fields[39]);
    d.odometer_back_right_ab_us =  stoul(fields[40]);

    d.ms = stoul(fields[42]);

    d.us = stoul(fields[44]);

    d.yaw = Angle::degrees(stod(fields[46]));
    d.pitch = Angle::degrees(stod(fields[47]));
    d.roll = Angle::degrees(stod(fields[48]));

    d.battery_voltage = stod(fields[50]);
    d.go = stod(fields[52])==1;

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
  cout << "d.tostring(): " << d.to_string() << endl;
  Dynamics d2 = d;
  cout << "d2.tostring(): " << d2.to_string() << endl;

}
