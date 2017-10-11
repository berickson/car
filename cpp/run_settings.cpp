#include "run_settings.h"
#include "file_names.h"
#include "iostream"
#include "split.h"
#include "trim.h"
#include "json.hpp"


RunSettings::RunSettings(){
  auto f = FileNames();
  auto track_names = f.get_track_names();
  if(track_names.size() == 0) {
    track_name = "";
    route_name = "";
    return;
  }
  track_name = track_names[0];
  auto route_names = f.get_route_names(track_name);
  if(route_names.size()==0) {
    route_name = "";
    return;
  }
  route_name = route_names[0];
}

void RunSettings::write_to_file_json(string path) {
  fstream file;
  file.open(path,ios_base::out);
  if(!file.is_open()) {
    throw (string) "could not open " + path;

  }

  nlohmann::json j;
  j["track_name"] = track_name;
  j["route_name"] = route_name;
  j["max_accel_lat"] = max_accel_lat;
  j["max_accel"] = max_accel;
  j["max_decel"] = max_decel;
  j["max_v"] = max_v;
  j["steering_k_p"] = steering_k_p;
  j["steering_k_i"] = steering_k_i;
  j["steering_k_d"] = steering_k_d;
  j["v_k_p"] = v_k_p;
  j["v_k_i"] = v_k_i;
  j["v_k_d"] = v_k_d;
  j["slip_rate"] = slip_rate;
  j["slip_slop"] = slip_slop;
  j["t_ahead"] = t_ahead;
  j["d_ahead"] = d_ahead;
  j["k_smooth"] = k_smooth;
  j["prune_max"] = prune_max;
  j["prune_tolerance"] = prune_tolerance;
  j["capture_video"] = capture_video;
  j["crash_recovery"] = crash_recovery;
  j["optimize_velocity"] = optimize_velocity;
  j["start_offset"] = start_offset;

  const int indent = 2;
  file << std::setw(indent) << j << std::endl;
}

void RunSettings::write_to_file_txt(string path) {

  fstream file;
  file.open(path,ios_base::out);
  if(!file.is_open()) {
    throw (string) "could not open " + path;

  }
  file << "track_name = " << track_name << endl
       << "route_name = " << route_name << endl
       << "max_accel_lat = " << max_accel_lat << endl
       << "max_accel = " << max_accel << endl
       << "max_decel = " << max_decel << endl
       << "max_v = " << max_v << endl
       << "steering_k_p = " << steering_k_p << endl
       << "steering_k_i = " << steering_k_i << endl
       << "steering_k_d = " << steering_k_d << endl

       << "v_k_p = " << v_k_p << endl
       << "v_k_i = " << v_k_i << endl
       << "v_k_d = " << v_k_d << endl

       << "slip_rate = " << slip_rate << endl
       << "slip_slop = " << slip_slop << endl

       << "t_ahead = " << t_ahead << endl
       << "d_ahead = " << d_ahead << endl
       << "k_smooth = " << k_smooth << endl

       << "prune_max = " << prune_max << endl
       << "prune_tolerance = " << prune_tolerance << endl
       << "capture_video = " << capture_video << endl
       << "crash_recovery = " << crash_recovery << endl
       << "optimize_velocity = " << optimize_velocity <<endl;

}

void RunSettings::load_from_file_txt(string path)
{
  
  ifstream file;
  file.open(path,ios_base::in);
  if(!file.is_open()) {
    throw (string) "could not open " + path;
  }

  std::string line;
  while(!file.eof()) {
    std::getline(file,line);
    vector<string> u = split(line,'=');
    if(u.size()!=2) continue;
    string name = trimmed(u[0]);
    string value = trimmed(u[1]);


    if(name == "track_name")
      this->track_name = value;
    else if (name == "route_name")
      this->route_name = value;
    else if (name == "max_accel_lat")
      this->max_accel_lat = stod(value);
    else if (name == "max_accel")
      this->max_accel = stod(value);
    else if (name == "max_decel")
      this->max_decel = stod(value);
    else if (name == "max_v")
      this->max_v = stod(value);
    else if (name == "d_ahead")
      this->d_ahead = stod(value);
    else if (name == "t_ahead")
      this->t_ahead = stod(value);
    else if (name == "k_p")
      this->steering_k_p = stod(value);
    else if (name == "k_d")
      this->steering_k_d = stod(value);
    else if (name == "steering_k_p")
      this->steering_k_p = stod(value);
    else if (name == "steering_k_i")
      this->steering_k_i = stod(value);
    else if (name == "steering_k_d")
      this->steering_k_d = stod(value);

    else if (name == "v_k_p")
      this->v_k_p = stod(value);
    else if (name == "v_k_i")
      this->v_k_i = stod(value);
    else if (name == "v_k_d")
      this->v_k_d = stod(value);
    else if (name == "prune_max")
      this->prune_max = stod(value);
    else if (name == "prune_tolerance")
      this->prune_tolerance = stod(value);
    else if (name == "k_smooth")
      this->k_smooth = stod(value);
    else if (name == "capture_video")
      this->capture_video = stod(value)>0;
    else if (name == "crash_recovery")
      this->crash_recovery = stod(value)>0;
    else if (name == "optimize_velocity")
      this->optimize_velocity= stod(value)>0;

  }
}

void RunSettings::load_from_file_json(string path) {
  std::ifstream ifs(path);
  nlohmann::json j = nlohmann::json::parse(ifs); 

  track_name = j["track_name"];
  route_name = j["route_name"].is_string() ? j["route_name"] : "";
  //route_name = j["route_name"];
  max_accel_lat = j["max_accel_lat"];
  max_accel = j["max_accel"];
  max_decel = j["max_decel"];
  max_v = j["max_v"];
  steering_k_p = j["steering_k_p"];
  steering_k_i = j["steering_k_i"];
  steering_k_d = j["steering_k_d"];
  v_k_p = j["v_k_p"];
  v_k_i = j["v_k_i"];
  v_k_d = j["v_k_d"];
  slip_rate = j["slip_rate"];
  slip_slop = j["slip_slop"];
  t_ahead = j["t_ahead"];
  d_ahead = j["d_ahead"];
  k_smooth = j["k_smooth"];
  prune_max = j["prune_max"];
  prune_tolerance = j["prune_tolerance"];
  capture_video = j["capture_video"];
  crash_recovery = j["crash_recovery"];
  optimize_velocity = j["optimize_velocity"];
  start_offset = j["start_offset"];
}
