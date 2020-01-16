#ifndef RUN_SETTINGS_H
#define RUN_SETTINGS_H

#include <string>

using namespace std;

struct RunSettings{
  RunSettings();

  string track_name = "";
  string route_name;// = FileNames().get_route_names(track_name)[0];
  double max_accel_lat = 0.25;
  double max_accel = 0.25;
  double max_decel = 0.25;
  double max_v = 1.0;
  double t_ahead = 0.3;
  double d_ahead = 0.05;
  double k_smooth = 0.4;

  double steering_k_p = 40.0;
  double steering_k_i = 0.0;
  double steering_k_d = 20.0;

  double v_k_p = 0.5;
  double v_k_i = 0.0;
  double v_k_d = 0.5;

  double slip_rate = 0.2;
  double slip_slop = 0.5;

  double prune_max = 0.5;
  double prune_tolerance = 0.02;


  bool capture_video = true;
  bool crash_recovery = false;
  bool optimize_velocity = true;
  double start_offset = 0.0;
  void write_to_file_json(string path);
  void write_to_file_txt(string path);
  void load_from_file_txt(string path);
  void load_from_file_json(string path);
};


#endif // RUN_SETTINGS_H
